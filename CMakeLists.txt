cmake_minimum_required(VERSION 2.8.4)

set(PROJECT LOCAL_PACKAGE)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set(CMAKE_CXX_FLAGS "-std=c++14 -g")
else()
  set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -DNDEBUG")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 1)
endif()

set(LOCAL_PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(BIN_DIR ${LOCAL_PACKAGE_DIR}/build)
set(LIB_DIR ${LOCAL_PACKAGE_DIR}/lib)

file(MAKE_DIRECTORY ${BIN_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

add_definitions(-fPIC)

file(GLOB_RECURSE SOURCES "./strategies/*.cpp")
file(GLOB_RECURSE HEADERS "./include/*.h")
set (STRATEGY_SOURCES "")
set (ADDITIONAL_SOURCES "")
foreach(SOURCE ${SOURCES})
  if(${SOURCE} MATCHES "strategies/(.*)\\.cpp")
    LIST(APPEND STRATEGY_SOURCES ${SOURCE})
  else()
    LIST(APPEND ADDITIONAL_SOURCES ${SOURCE})
  endif()
endforeach()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

foreach(str_source ${STRATEGY_SOURCES})
  get_filename_component(STRATEGY_NAME ${str_source} NAME_WE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/${STRATEGY_NAME}/)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/${STRATEGY_NAME}/)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}/${STRATEGY_NAME}/)
  add_library(${STRATEGY_NAME} SHARED ${str_source} ${ADDITIONAL_SOURCES} ${HEADERS})
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${STRATEGY_NAME} ${LIB_DIR}/libsimulator_lib.so)
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(${STRATEGY_NAME} ${LIB_DIR}/libsimulator_lib.dylib)
  else()
    target_link_libraries(${STRATEGY_NAME} ${LOCAL_PACKAGE_DIR}/libsimulator_lib.dll)
  endif()
endforeach()
