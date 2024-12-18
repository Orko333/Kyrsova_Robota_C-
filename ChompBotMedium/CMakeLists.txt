cmake_minimum_required(VERSION 3.10)
project(ChompBotEasy LANGUAGES CXX)

# Встановити стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


# Указати, що ми створюємо спільну бібліотеку
add_library(chomp_ai SHARED chomp_ai.cpp)

# Оптимізації та видимість
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(chomp_ai PRIVATE -fvisibility=hidden)
endif()

# Якщо потрібно додати інші флаги компіляції, зробіть це тут
if(MSVC)
    target_compile_options(chomp_ai PRIVATE /W4)
else()
    target_compile_options(chomp_ai PRIVATE -Wall -Wextra -pedantic)
endif()

# Встановлення бібліотеки
install(TARGETS chomp_ai DESTINATION lib)
