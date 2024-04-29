#include <asio/connect.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <core/sessions/session.h>
#include <cstdlib>
#include <fmt/color.h>
#include <thread>


namespace core::operations {
session::session()
  : _context(), socket(_context), is_running(false), operation_thread(nullptr), timeout_idle(10), timer(nullptr),
    stream_buffer(1024)
{}
void session::connect()
{
  // if (const char *env_var = std::getenv("JPOD_UNIX_DOMAIN_FILE")) {
  //   asio::local::stream_protocol::endpoint endpoint(env_var);
  //   // you will need to create the endpoint with a unique uid.

  //   socket.async_connect(endpoint, [this](const std::error_code &err) {
  //     if (!err) {
  //       this->on_open();
  //       read_payload();
  //     } else {
  //       this->on_error(err);
  //     }
  //   });

  // } else {
  //   this->on_error(std::make_error_code(std::errc::bad_file_descriptor));
  // }

  asio::local::stream_protocol::endpoint endpoint("/home/william/Projects/updates/unix.socket.jpod");
  // you will need to create the endpoint with a unique uid.

  socket.async_connect(endpoint, [this](const std::error_code &err) {
    if (!err) {
      this->on_open();
      read_header(0);
    } else {
      this->on_error(err);
    }
  });
  operation_thread = std::make_unique<std::thread>(&session::run, this);
  operation_thread->join();
}

void session::run()
{
  if (!is_running) {
    is_running = true;
    while (is_running) { _context.run(); }
  }
}
void session::disconnect()
{
  if (socket.is_open()) {
    is_running = false;
    std::error_code err;
    socket.close(err);
    _context.stop();
    if (err) { this->on_error(err); }
  }
}

void session::write(request::operation operation, request::target target, const std::vector<uint8_t> &data)
{
  std::vector<uint8_t> header;
  std::size_t length = data.size();
  header.assign(2 + (length >= 126 ? 2 : 0) + (length >= 65536 ? 6 : 0), 0);
  header[0] =
    (static_cast<uint8_t>(operation) & _header_operation_mask) | (static_cast<uint8_t>(target) & _header_target_mask);
  if (length < 126) {
    header[1] = (length & 0xff) | 0;
  } else if (length < 65536) {
    header[1] = 126 | 0;
    header[2] = (length >> 8) & 0xff;
    header[3] = (length >> 0) & 0xff;
  } else {
    header[1] = 127 | 0;
    header[2] = (length >> 56) & 0xff;
    header[3] = (length >> 48) & 0xff;
    header[4] = (length >> 40) & 0xff;
    header[5] = (length >> 32) & 0xff;
    header[6] = (length >> 24) & 0xff;
    header[7] = (length >> 16) & 0xff;
    header[8] = (length >> 8) & 0xff;
    header[9] = (length >> 0) & 0xff;
  }

  std::vector<uint8_t> frame(header.begin(), header.end());
  frame.insert(frame.end(), data.begin(), data.end());
  sending_queue.emplace_back(frame);
  if (sending_queue.size() == 1) { send_from_queue(); }
}

void session::send_from_queue()
{
  auto content = sending_queue.front();
  asio::async_write(socket, asio::buffer(content), [this](const std::error_code &err, std::size_t bytes_transferred) {
    if (!err) {
      this->sending_queue.pop_front();
      if (!this->sending_queue.empty()) { this->send_from_queue(); }
    } else {
      this->on_error(err);
    }
  });
}

void session::read_header(std::size_t left_over_bytes)
{
  std::size_t bytes_to_fetch = left_over_bytes > 2 ? 0 : 2 - left_over_bytes;
  set_timeout();
  asio::async_read(socket,
    stream_buffer,
    asio::transfer_exactly(bytes_to_fetch),
    [this, left_over_bytes](const std::error_code &error, std::size_t bytes_transferred) {
      this->cancel_timeout();
      if (!error) {
        if (bytes_transferred == 0 && this->stream_buffer.size() == 0) {
          this->read_header(0);
        } else {
          std::array<uint8_t, 2> first_bytes;
          std::istream istream(&this->stream_buffer);
          istream.read((char *)&first_bytes[0], 2);
          auto target = (first_bytes[0] & _header_target_mask);
          auto operation = (first_bytes[0] & _header_operation_mask);
          std::size_t boundary = (first_bytes[1] & 0x7F);
          std::size_t updated_left_over_bytes = left_over_bytes > 2 ? left_over_bytes - 2 : 0;
          if (boundary <= 125) {
            this->read_payload(operation, boundary, updated_left_over_bytes);
          } else if (boundary == 126) {
            this->read_medium_payload(operation, updated_left_over_bytes);
          } else if (boundary == 127) {
            this->read_large_payload(operation, updated_left_over_bytes);
          }
        }
      } else {
        this->on_error(error);
      }
    });
}
void session::read_payload(uint8_t operation, std::size_t payload_length, std::size_t left_over_bytes)
{
  std::size_t bytes_to_fetch = (left_over_bytes > payload_length ? 0 : payload_length - left_over_bytes);
  asio::async_read(socket,
    stream_buffer,
    asio::transfer_exactly(bytes_to_fetch),
    [this, operation, payload_length, left_over_bytes](const std::error_code &error, std::size_t bytes_transferred) {
      this->cancel_timeout();
      if (!error) {
        auto updated_left_over_bytes =
          left_over_bytes > this->stream_buffer.size() ? left_over_bytes - this->stream_buffer.size() : 0;
        std::istream input_stream(&this->stream_buffer);
        std::vector<uint8_t> content;
        content.reserve(payload_length);
        for (std::size_t c = 0; c < static_cast<std::size_t>(payload_length); c++) {
          content.push_back(input_stream.get());
        }
        this->on_data(static_cast<response::operation>(operation), content);
        this->stream_buffer.consume(this->stream_buffer.size());
        this->read_header(updated_left_over_bytes);
      }
    });
}
void session::read_medium_payload(uint8_t operation, std::size_t left_over_bytes)
{
  set_timeout();
  asio::async_read(socket,
    stream_buffer,
    asio::transfer_exactly(left_over_bytes > 2 ? 0 : 2 - left_over_bytes),
    [this, operation, left_over_bytes](const std::error_code &error, std::size_t bytes_transferred) {
      this->cancel_timeout();
      if (!error) {
        std::istream istream(&this->stream_buffer);
        std::size_t length = 0;
        std::size_t num_bytes = 2;
        std::array<uint8_t, 2> length_bytes;
        istream.read((char *)&length_bytes[0], 2);
        for (std::size_t c = 0; c < num_bytes; c++) {
          length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));
        }
        this->read_payload(operation, length, left_over_bytes > 2 ? left_over_bytes - 2 : 0);
      } else {
        this->on_error(error);
      }
    });
}
void session::read_large_payload(uint8_t operation, std::size_t left_over_bytes)
{
  set_timeout();
  asio::async_read(socket,
    stream_buffer,
    asio::transfer_exactly(left_over_bytes > 8 ? 0 : 8 - left_over_bytes),
    [this, operation, left_over_bytes](const std::error_code &error, std::size_t bytes_transferred) {
      this->cancel_timeout();
      if (!error) {
        std::istream istream(&this->stream_buffer);
        std::size_t length = 0;
        std::size_t num_bytes = 8;
        std::array<uint8_t, 8> length_bytes;
        istream.read((char *)&length_bytes[0], 8);
        for (std::size_t c = 0; c < num_bytes; c++) {
          length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));
        }
        this->read_payload(operation, length, left_over_bytes > 8 ? left_over_bytes - 8 : 0);
      } else {
        this->on_error(error);
      }
    });
}

void session::set_timeout(long seconds) noexcept
{
  if (seconds == -1) {
    seconds = timeout_idle;
  } else if (seconds == 0) {
    timer = nullptr;
  } else if (lock.try_lock()) {
    timer =
      std::unique_ptr<asio::steady_timer>(new asio::steady_timer(socket.get_executor(), std::chrono::seconds(seconds)));
    timer->async_wait([this](const std::error_code &err) {
      if (err) { this->on_error(err); }
      this->disconnect();
    });
  }
}
void session::cancel_timeout() noexcept
{
  if (lock.try_lock()) {
    if (timer) {
      std::error_code error;
      timer->cancel(error);
      if (error) { this->on_error(error); }
    }
  }
}
session::~session()
{
  if (socket.is_open()) { socket.close(); }
  if (!_context.stopped()) { _context.stop(); }
}
}// namespace core::operations