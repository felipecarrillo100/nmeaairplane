cmake_minimum_required(VERSION 3.10)
project(nmeaairplane)

set(CMAKE_CXX_STANDARD 17)

# Use the Paho MQTT C++ package from vcpkg
find_package(PahoMqttCpp CONFIG REQUIRED)

add_executable(nmeaairplane main.cpp PlaneSimulator.cpp NMEAGenerator.cpp)

# ✅ Use only the shared variant target provided by vcpkg
target_link_libraries(nmeaairplane PRIVATE
    PahoMqttCpp::paho-mqttpp3-shared
)
