find_package(fmt CONFIG REQUIRED)
find_package(asio REQUIRED CONFIG)
find_package(indicators REQUIRED CONFIG)
find_package(lyra REQUIRED CONFIG)
find_package(range-v3 REQUIRED CONFIG)
find_package(tabulate REQUIRED CONFIG)
find_package(msgpack REQUIRED CONFIG)

add_executable(jc
               main.cc
               #CORE::SESSIONS
               core/sessions/session.cc
               core/sessions/interactive_session.cc
               core/sessions/staged_session.cc
               core/sessions/rpc_session.cc
               #CORE::COMMANDS
               core/commands/command_registry.cc
               #DOMAIN::IMAGES
               domain/images/import_command.cc
               domain/images/list_command.cc
               domain/images/build_command.cc
               domain/images/pull_command.cc
               #DOMAIN::CONTAINERS
               domain/containers/list_command.cc
               domain/containers/create_command.cc
               domain/containers/start_command.cc
               domain/containers/stop_command.cc
               domain/containers/shell_command.cc
               domain/containers/remove_command.cc
               domain/containers/log_command.cc
               domain/containers/terminal.cc
               )

target_link_libraries(jc
                       PRIVATE
                       fmt::fmt
                       asio::asio
                       indicators::indicators
                       bfg::lyra
                       tabulate::tabulate
                       range-v3::range-v3
                       msgpack-cxx
                       )


target_include_directories(jc
                            PUBLIC
                            ${PROJECT_HDRS_FOLDER}
                            PRIVATE "${CMAKE_BINARY_DIR}"
                            )
install(TARGETS jc DESTINATION bin)


