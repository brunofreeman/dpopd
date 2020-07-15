cmake_minimum_required(VERSION 3.16)
project(dpo_pdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LDFLAGS "-framework OpenGL")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

set(TARGET visualizer)

set(INC_DIR inc)
set(SRC_DIR src)
set(GFX_DIR gfx)

set(SRC_EXT .cpp)
set(INC_EXT .hpp)

list(APPEND FILES color graphics_object graphics_util index_buffer renderer shader vertex_array vertex_buffer vertex_buffer_layout)
list(TRANSFORM FILES PREPEND ${GFX_DIR}/)
list(APPEND FILES agent environment json_wrapper move_model polygon random_wrapper to_string vector wall)

set(INC ${FILES})
list(TRANSFORM INC PREPEND ${INC_DIR}/)
list(TRANSFORM INC APPEND  ${INC_EXT})

set(SRC ${FILES})
list(TRANSFORM SRC PREPEND ${SRC_DIR}/)
list(TRANSFORM SRC APPEND  ${SRC_EXT})

add_executable(${TARGET} ${SRC_DIR}/${TARGET}${SRC_EXT} ${INC} ${SRC})

set(LIB_INC_DIR /usr/local/include)
set(LIB_DIR /usr/local/lib)
set(LIB_EXT .dylib)

list(APPEND LIB_PATHS GLEW glfw dijkstrapolygon)
list(TRANSFORM LIB_PATHS PREPEND ${LIB_DIR}/lib)
list(TRANSFORM LIB_PATHS APPEND ${LIB_EXT})
target_include_directories(${TARGET} PRIVATE ${INC_DIR} ${INC_DIR}/${GFX_DIR} ${LIB_INC_DIR})
target_link_libraries(${TARGET} ${LIB_PATHS})