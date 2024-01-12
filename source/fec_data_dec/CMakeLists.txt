# CMakeList.txt : CMake project for fec_data_dec, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("fec_data_dec")

find_package(ROOT REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/inc ${PROJECT_SOURCE_DIR}/../common/inc ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/../../common/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.hh  ${PROJECT_SOURCE_DIR}/../common/inc/*.hh)


# Add source to this project's executable.
add_executable (fec_data_dec "fec_data_dec.cc" "fec_data_dec.hh"  ${sources} ${headers})
target_link_libraries(fec_data_dec ${ROOT_LIBRARIES})
# TODO: Add tests and install targets if needed.
