#include <asio/io_context.hpp>
#include <core/sessions/interactive_session.h>
#include <core/sessions/interactive_session_listener.h>
#include <core/sessions/internal.h>
#include <fmt/format.h>
namespace core::operations {
interactive_session::interactive_session(interactive_session_listener &listener) : session(), listener(listener) {}
void interactive_session::on_data(response::operation operation, const std::vector<uint8_t> &content)
{
  switch (operation) {
  case response::operation::failure: {
    auto error = unpack_error_payload(content);
    listener.on_finish(true, fmt::format("remote failure: {}", error.message));
    disconnect();
    break;
  };
  case response::operation::success: {
    listener.on_start(true);
    break;
  };
  case response::operation::frame: {
    listener.on_data_received(content);
    break;
  };
  default:
    break;
  }
}
void interactive_session::on_error(const std::error_code &error)
{
  listener.on_finish(true, fmt::format("internal failure: {}", error.message()));
}
void interactive_session::on_open() { listener.on_start(false); }
asio::io_context &interactive_session::context() { return this->_context; }
interactive_session::~interactive_session() {}
}// namespace core::operations