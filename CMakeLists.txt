cmake_minimum_required(VERSION 3.10)
project(mqtt_hello)

set(CMAKE_CXX_STANDARD 17)

# Use the Paho MQTT C++ package from vcpkg
find_package(PahoMqttCpp CONFIG REQUIRED)

add_executable(mqtt_hello main.cpp PlaneSimulator.cpp)

# ✅ Use only the shared variant target provided by vcpkg
target_link_libraries(mqtt_hello PRIVATE
    PahoMqttCpp::paho-mqttpp3-shared
)
