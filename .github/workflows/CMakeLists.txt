cmake_minimum_required(VERSION 3.20)
project(AimTrainer LANGUAGES CXX)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY
      "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(AimTrainer WIN32 src/main.cpp)

target_compile_features(AimTrainer PRIVATE cxx_std_17)

target_compile_definitions(AimTrainer PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MSVC)
  target_compile_options(AimTrainer PRIVATE /W4 /permissive-)
endif()
