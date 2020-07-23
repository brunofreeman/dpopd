cmake_minimum_required(VERSION 3.16)
project(dpo_pdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LDFLAGS "-framework OpenGL")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

set(VIS_TARGET visualizer)
set(GEN_TARGET data_generator)

set(INC_DIR inc)
set(SRC_DIR src)
set(GFX_DIR gfx)
set(SIM_DIR sim)
set(NET_DIR net)

set(SRC_EXT .cpp)
set(INC_EXT .hpp)

list(APPEND GFX_FILES color graphics_object graphics_util index_buffer renderer shader vertex_array vertex_buffer vertex_buffer_layout)
list(TRANSFORM GFX_FILES PREPEND ${GFX_DIR}/)
list(APPEND SIM_FILES agent environment polygon segment to_string vector wall)
list(TRANSFORM SIM_FILES PREPEND ${SIM_DIR}/)
list(APPEND NET_FILES model occupancy_grid)
list(TRANSFORM NET_FILES PREPEND ${NET_DIR}/)
list(APPEND GLOB_FILES random_wrapper json_wrapper move_model)

list(APPEND FILES ${GFX_FILES} ${SIM_FILES} ${NET_FILES} ${GLOB_FILES})

set(VIS_INC ${FILES})
list(TRANSFORM VIS_INC PREPEND ${INC_DIR}/)
list(TRANSFORM VIS_INC APPEND  ${INC_EXT})

set(VIS_SRC ${FILES})
list(TRANSFORM VIS_SRC PREPEND ${SRC_DIR}/)
list(TRANSFORM VIS_SRC APPEND  ${SRC_EXT})

set(GEN_INC ${GLOB_FILES} ${SIM_FILES} ${NET_FILES})
list(TRANSFORM GEN_INC PREPEND ${INC_DIR}/)
list(TRANSFORM GEN_INC APPEND  ${INC_EXT})

set(GEN_SRC ${GLOB_FILES} ${SIM_FILES})
list(TRANSFORM GEN_SRC PREPEND ${SRC_DIR}/)
list(TRANSFORM GEN_SRC APPEND  ${SRC_EXT})

add_executable(${VIS_TARGET} ${SRC_DIR}/${SIM_DIR}/${VIS_TARGET}${SRC_EXT} ${VIS_INC} ${VIS_SRC})
add_executable(${GEN_TARGET} ${SRC_DIR}/${NET_DIR}/${GEN_TARGET}${SRC_EXT} ${GEN_INC} ${GEN_SRC})

set(LIB_INC_DIR /usr/local/include)
set(LIB_DIR /usr/local/lib)
set(LIB_EXT .dylib)

list(APPEND LIB_PATHS_VIS GLEW glfw dijkstrapolygon)
list(TRANSFORM LIB_PATHS_VIS PREPEND ${LIB_DIR}/lib)
list(TRANSFORM LIB_PATHS_VIS APPEND ${LIB_EXT})
target_include_directories(${VIS_TARGET} PRIVATE ${INC_DIR} ${INC_DIR}/${GFX_DIR} ${INC_DIR}/${SIM_DIR} ${INC_DIR}/${NET_DIR} ${LIB_INC_DIR})
target_link_libraries(${VIS_TARGET} ${LIB_PATHS_VIS})

list(APPEND LIB_PATHS_GEN dijkstrapolygon)
list(TRANSFORM LIB_PATHS_GEN PREPEND ${LIB_DIR}/lib)
list(TRANSFORM LIB_PATHS_GEN APPEND ${LIB_EXT})
target_include_directories(${GEN_TARGET} PRIVATE ${INC_DIR} ${INC_DIR}/${GFX_DIR} ${INC_DIR}/${SIM_DIR} ${INC_DIR}/${NET_DIR} ${LIB_INC_DIR})
target_link_libraries(${GEN_TARGET} ${LIB_PATHS_GEN})