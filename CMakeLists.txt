cmake_minimum_required(VERSION 3.18)
include(ExternalProject)
set(CMAKE_CXX_STANDARD 17)

project(max31865spi)
add_executable(max31865spi src/main.cpp src/Thermometer.cpp)

ExternalProject_Add(mqttmsg SOURCE_DIR /home/pi/picomqqt/
  INSTALL_COMMAND ""
  )

ExternalProject_Get_Property(mqttmsg SOURCE_DIR)

ExternalProject_Get_Property(mqttmsg BINARY_DIR)

add_library(mqttmsglib UNKNOWN IMPORTED)
set_target_properties(mqttmsglib PROPERTIES
  IMPORTED_LOCATION ${BINARY_DIR}/libmqttmsg.a
  )
              
target_link_libraries(max31865spi PUBLIC
    mqttmsglib
    )
