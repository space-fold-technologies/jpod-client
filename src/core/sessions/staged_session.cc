#include <core/sessions/internal.h>
#include <core/sessions/staged_session.h>
#include <core/sessions/staged_session_listener.h>
#include <fmt/format.h>

namespace core::operations {
staged_session::staged_session(staged_session_listener &listener) : session(), listener(listener) {}
void staged_session::on_data(response::operation operation, const std::vector<uint8_t> &content)
{
  switch (operation) {
  case response::operation::progress: {
    auto progress = unpack_progress_frame(content);
    listener.on_progress(progress.feed, progress.percentage);
    break;
  }
  case response::operation::success: {
    auto payload = unpack_success_payload(content);
    listener.on_finish(false, payload.message);
    break;
  }
  case response::operation::failure: {
    auto error = unpack_error_payload(content);
    listener.on_finish(true, fmt::format("remote failure: {}", error.message));
    break;
  }

  case response::operation::close_order: {
    std::string message(content.begin(), content.end());
    disconnect();
    listener.on_finish(!message.empty(), message);
    break;
  }

  default:
    break;
  }
}
void staged_session::on_error(const std::error_code &error) {
  listener.on_finish(true, error.message());
}
void staged_session::on_open() {
  listener.on_start();
}

staged_session::~staged_session() {}
}// namespace core::operations