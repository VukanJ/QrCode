cmake_minimum_required(VERSION 3.1)

project(QRgen)

add_compile_options(-Ofast -Wall -Wpedantic -Wextra -Wunused-value -Wno-sign-compare)

# BOOST
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options)
# SFML
#set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "/usr/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)


if(SFML_FOUND AND Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  
  add_executable(QRgen MaskInfo.cpp Printer.cpp gf256poly.cpp QR.cpp Main.cpp Encoder.cpp Module.cpp gf256.cpp EC.cpp)
  set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
  target_link_libraries(QRgen ${Boost_LIBRARIES} sfml-graphics sfml-window sfml-system)
  
  target_compile_features(QRgen PRIVATE cxx_constexpr)
  set_property(TARGET QRgen PROPERTY CXX_STANDARD 17)
endif()