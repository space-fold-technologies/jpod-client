find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(asio REQUIRED CONFIG)
find_package(indicators REQUIRED CONFIG)
find_package(lyra REQUIRED CONFIG)
find_package(range-v3 REQUIRED CONFIG)
find_package(tabulate REQUIRED CONFIG)
find_package(msgpack REQUIRED CONFIG)


# Generic test that uses conan libs

add_executable(client
                main.cc
                core/commands/command_registry.cc
                core/connections/connection.cc
                core/operations/operation.cc
                # images
                domain/images/image_command.cc
                domain/images/list_command.cc
                domain/images/build_command.cc
                domain/images/push_command.cc
                domain/images/pull_command.cc
                domain/images/remove_command.cc
                domain/images/details_command.cc
                domain/images/import_command.cc
                # containers
                domain/containers/container_commands.cc
                domain/containers/create_command.cc
                domain/containers/start_command.cc
                domain/containers/stop_command.cc
                domain/containers/list_command.cc
                domain/containers/remove_command.cc
                domain/containers/log_command.cc
                domain/containers/shell_command.cc
                domain/containers/shell/terminal.cc
                )

target_link_libraries(client
                       PRIVATE
                       fmt::fmt 
                       spdlog::spdlog_header_only
                       asio::asio
                       indicators::indicators
                       bfg::lyra
                       tabulate::tabulate
                       range-v3::range-v3
                       msgpack-cxx
                       )
                   

target_include_directories(client
                            PUBLIC
                            ${PROJECT_HDRS_FOLDER}
                            PRIVATE "${CMAKE_BINARY_DIR}"
                            )
install(TARGETS client DESTINATION bin)
