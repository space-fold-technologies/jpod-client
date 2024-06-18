#include <core/sessions/internal.h>
#include <core/sessions/rpc_session.h>
#include <core/sessions/rpc_session_listener.h>
#include <fmt/format.h>
namespace core::operations {
rpc_session::rpc_session(rpc_session_listener &listener) : session(), listener(listener) {}
void rpc_session::on_data(response::operation operation, const std::vector<uint8_t> &content)
{
  switch (operation) {
  case response::operation::frame: {
    listener.on_response(content);
    break;
  }
  case response::operation::success: {
    auto payload = unpack_success_payload(content);
    listener.on_response(std::vector<uint8_t>(payload.message.begin(), payload.message.end()));
    break;
  }

  case response::operation::failure: {
    auto error = unpack_error_payload(content);
    listener.on_failure(fmt::format("remote failure: {}", error.message));
    break;
  }
  default:
    break;
  }
}
void rpc_session::on_error(const std::error_code &error)
{
  listener.on_failure(fmt::format("internal failure: {}", error.message()));
}
void rpc_session::on_open() { this->listener.on_start(); }
rpc_session::~rpc_session() {}
}// namespace core::operations