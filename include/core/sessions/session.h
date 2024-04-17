#ifndef __JC_CORE_SESSIONS_SESSION__
#define __JC_CORE_SESSIONS_SESSION__

#include <asio/local/stream_protocol.hpp>
#include <asio/steady_timer.hpp>
#include <asio/strand.hpp>
#include <asio/streambuf.hpp>
#include <core/sessions/details.h>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#if (defined(ASIO_STANDALONE) && ASIO_VERSION >= 101800)
#include <asio/any_io_executor.hpp>
using strand = asio::strand<asio::any_io_executor>;
#else
#include <asio/executor.hpp>
using strand = asio::strand<asio::executor>;
#endif

namespace core::operations {
static constexpr uint8_t _header_operation_mask = 0b00011111;
static constexpr uint8_t _header_target_mask = 0b11100000;
class session
{
public:
  session();
  virtual ~session();
  virtual void on_data(response::operation operation, const std::vector<uint8_t> &content) = 0;
  virtual void on_error(const std::error_code &error) = 0;
  virtual void on_open() = 0;
  void connect();
  void disconnect();
  void write(request::operation operation, request::target target, const std::vector<uint8_t> &data);

private:
  void read_header(std::size_t left_over_bytes);
  void read_payload(uint8_t operation, std::size_t payload_length, std::size_t left_over_bytes);
  void read_medium_payload(uint8_t operation, std::size_t left_over_bytes);
  void read_large_payload(uint8_t operation, std::size_t left_over_bytes);
  void set_timeout(long seconds = -1) noexcept;
  void cancel_timeout() noexcept;
  void send_from_queue();
  void run();

protected:
  asio::io_context _context;


private:
  asio::local::stream_protocol::socket socket;
  std::unique_ptr<std::thread> operation_thread;
  bool is_running;
  std::mutex lock;
  long timeout_idle;
  std::unique_ptr<asio::steady_timer> timer;
  std::deque<std::vector<uint8_t>> sending_queue;
  asio::streambuf stream_buffer;
};
}// namespace core::operations

#endif//__JC_CORE_SESSIONS_SESSION__