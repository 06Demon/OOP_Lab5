cmake_minimum_required(VERSION 3.14)
project(lab5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
  TLS_VERIFY false
)

if(MINGW)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_compile_definitions(
        _WIN32_WINNT=0x0601
        __USE_MINGW_ANSI_STDIO=1
    )
endif()

FetchContent_MakeAvailable(googletest)

# Основная программа
add_executable(main
    main.cpp
    src/ChunkMemoryResource.cpp  # Добавляем исходный файл
)

# Указываем include директории
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Тесты
add_executable(runTests
    tests/tests.cpp
    src/ChunkMemoryResource.cpp  # Добавляем исходный файл
)

target_include_directories(runTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MINGW)
    target_link_libraries(runTests gtest_main pthread)
else()
    target_link_libraries(runTests gtest_main)
endif()

enable_testing()
add_test(NAME FiguresTests COMMAND runTests)