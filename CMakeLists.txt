# CMakeLists.txt
# 9-Apr-2025
# 2025 (c) MVK
#
# > cmake -H. -B_builds 
# > cmake --build _builds
#
# > readelf -d EXE_PATH | grep 'NEEDED'
# > objdump -p LIB_PATH | grep 'SONAME'
# > ldd EXE_PATH
# > patchelf --set-soname NEW_SONAME LIB_PATH
# > patchelf --replace-needed OLD_NAME NEW_NAME EXE_PATH
# > lsof -d `pidof <exe>`
# > LD_DEBUG=libs EXE_PATH

cmake_minimum_required(VERSION 3.10)
project(EPD
		VERSION 1.0
		DESCRIPTION "SPI e-Paper Display"
    	LANGUAGES C)
		
add_compile_definitions(PROJECT_VERSION=${PROJECT_VERSION})
add_compile_definitions(CMAKE_PROJECT_VERSION=${CMAKE_PROJECT_VERSION})
add_compile_definitions(RPI)
add_compile_definitions(USE_DEV_LIB)
add_compile_definitions(USE_LGPIO_LIB)
add_compile_definitions(USELIB_RPI USE_LGPIO_LIB)
# add_compile_definitions(EPD "epd1in54V2")
# add_compile_definitions(epd1in54V2)

file(GLOB_RECURSE SOURCES 
		${CMAKE_SOURCE_DIR}/Config/*.c
		${CMAKE_SOURCE_DIR}/e-Paper/*.c
		${CMAKE_SOURCE_DIR}/lib/*.c
		${CMAKE_SOURCE_DIR}/pic/*.c
		${CMAKE_SOURCE_DIR}/src/EPD_1in54_V2_test.c
		${CMAKE_SOURCE_DIR}/src/ImageData.c
		${CMAKE_SOURCE_DIR}/src/ImageData2.c
		${CMAKE_SOURCE_DIR}/src/utils.c
		${CMAKE_SOURCE_DIR}/src/i2c_utils.c
		${CMAKE_SOURCE_DIR}/src/main.c
		)

add_executable(EPD ${SOURCES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

# add_compile_definitions(USE_DEBUG)

target_include_directories(EPD PRIVATE ./src)
target_include_directories(EPD PRIVATE ./lib)
target_include_directories(EPD PRIVATE ./lib/Config)
target_include_directories(EPD PRIVATE ./lib/Fonts)
target_include_directories(EPD PRIVATE ./lib/GUI)
target_include_directories(EPD PRIVATE ./lib/e-Paper)
target_include_directories(EPD PRIVATE ./src)
target_include_directories(EPD PRIVATE ./pic)

find_library(LIB_M m)
message(STATUS "LIB_M: " ${LIB_M})
target_link_libraries(EPD PUBLIC ${LIB_M})

find_library(LIB_GPIOD gpiod)
message(STATUS "LIB_GPIOD: " ${LIB_GPIOD})
target_link_libraries(EPD PUBLIC ${LIB_GPIOD})

find_library(LIB_GPIO lgpio)
message(STATUS "LIB_GPIO: " ${LIB_GPIO})
target_link_libraries(EPD PUBLIC ${LIB_GPIO})

find_library(LIB_SD systemd)
message(STATUS "LIB_SD: " ${LIB_SD})
target_link_libraries(EPD PUBLIC ${LIB_SD})
