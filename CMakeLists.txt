cmake_minimum_required(VERSION 3.16)

project(socket-tests)

add_executable(server "server.cpp" "crib-socket.cpp" "chat.cpp")
add_executable(client "client.cpp" "crib-socket.cpp" "chat.cpp")

target_compile_features(server PRIVATE cxx_std_20)
target_compile_features(client PRIVATE cxx_std_20)
