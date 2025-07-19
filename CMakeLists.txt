cmake_minimum_required(VERSION 3.10)
project(nmeaairplane)

set(CMAKE_CXX_STANDARD 17)

# Find Paho MQTT C++ from vcpkg
find_package(PahoMqttCpp CONFIG REQUIRED)

# Find CLI11 from vcpkg
find_package(CLI11 CONFIG REQUIRED)

add_executable(nmeaairplane main.cpp PlaneSimulator.cpp NMEAGenerator.cpp)

target_link_libraries(nmeaairplane PRIVATE
        PahoMqttCpp::paho-mqttpp3-shared
        CLI11::CLI11
        )
