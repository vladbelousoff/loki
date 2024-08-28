cmake_minimum_required(VERSION 3.20)

# Project definition
project(Loki LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required libs
find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(
  libassert
  GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
  GIT_TAG        v2.1.0
)

FetchContent_MakeAvailable(libassert)

FetchContent_Declare(
  boost_pfr
  GIT_REPOSITORY https://github.com/boostorg/pfr.git
  GIT_TAG        2.2.0
)

FetchContent_MakeAvailable(boost_pfr)

FetchContent_Declare(
  glfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(glfw3)

FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_TAG        glew-cmake-2.2.0
)

FetchContent_MakeAvailable(glew)

FetchContent_Declare(
    imgui_external
	URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0-docking.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(imgui_external)

add_library(
    imgui
	${imgui_external_SOURCE_DIR}/imgui.cpp
	${imgui_external_SOURCE_DIR}/imgui_draw.cpp
	${imgui_external_SOURCE_DIR}/imgui_tables.cpp
	${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

target_include_directories(
    imgui PUBLIC
    ${imgui_external_SOURCE_DIR}
)

# Link GLFW to ImGui so that it can find GLFW headers
target_link_libraries(imgui PUBLIC glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.1
)

set(GLM_ENABLE_CXX_20 ON)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.14.1
)

set(SPD_LOG_USE_STD_FORMAT ON)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.2
)

FetchContent_MakeAvailable(cli11)

FetchContent_Declare(
  mimalloc
  GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
  GIT_TAG        v2.1.7
)

set(MI_BUILD_SHARED OFF)
set(MI_BUILD_TESTS OFF)

FetchContent_MakeAvailable(mimalloc)

FetchContent_Declare(
  StormLib
  GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib.git
  GIT_TAG        v9.26
)

FetchContent_MakeAvailable(StormLib)

# Assuming StormLib does not provide a target, manually specify the include directory
add_library(StormLib INTERFACE)
target_include_directories(StormLib INTERFACE ${StormLib_SOURCE_DIR}/src)

FetchContent_Declare(
  sockpp
  GIT_REPOSITORY https://github.com/fpagliughi/sockpp.git
  GIT_TAG        v1.0.0
)

set(SOCKPP_BUILD_SHARED OFF)
set(SOCKPP_BUILD_STATIC ON)

FetchContent_MakeAvailable(sockpp)

FetchContent_Declare(
  lib_glob
  GIT_REPOSITORY https://github.com/p-ranav/glob.git
  GIT_TAG        v0.0.1
)

FetchContent_MakeAvailable(lib_glob)

FetchContent_Declare(
  BLPConverter
  GIT_REPOSITORY https://github.com/vladbelousoff/BLPConverter.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(BLPConverter)

# This library is PITA
target_include_directories(blp INTERFACE ${blp_SOURCE_DIR})

FetchContent_Declare(
  OpenSSL
  GIT_REPOSITORY https://github.com/viaduck/openssl-cmake.git
  GIT_TAG        v3
)

FetchContent_MakeAvailable(OpenSSL)

# Source files
set(SOURCES
    main.cpp
    engine/engine_app.cpp
    engine/utils/string_manager.cpp
    engine/utils/big_num.cpp
    engine/utils/byte_buffer.cpp
    engine/crypto/crypto_random.cpp
    engine/crypto/arc_4.cpp
    engine/crypto/srp_6.cpp
    engine/network/auth_session.cpp
    engine/network/auth_crypt.cpp
    engine/network/world_session.cpp
    engine/render/shader.cpp
    engine/datasource/mpq/mpq_archive.cpp
    engine/datasource/mpq/mpq_chain.cpp
    engine/datasource/mpq/mpq_file.cpp
    engine/datasource/mpq/mpq_file_manager.cpp
    engine/asset/asset.cpp
    engine/model/m_2_model.cpp
    engine/model/m_2_model_view.cpp
    engine/texture/blp_texture.cpp
    engine/mt/main_thread_queue.cpp
    game/game_app.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
	# Add any necessary compile definitions
	target_compile_definitions(${PROJECT_NAME} PRIVATE -DWIN32_LEAN_AND_MEAN)
endif()

# Link dependencies to the executable
target_link_libraries(${PROJECT_NAME}
    libassert::assert
	boost_pfr
	glfw
	libglew_static
	imgui
	glm
	spdlog
	CLI11::CLI11
	mimalloc-static
	StormLib::storm
	sockpp-static
	Glob
	blp
	ssl
	crypto
)

target_include_directories(${PROJECT_NAME} PRIVATE .)

# On windows copy libassert.dll to the same directory as the executable for ${PROJECT_NAME}
# if(WIN32)
#   add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     $<TARGET_FILE:libassert::assert>
#     $<TARGET_FILE_DIR:${PROJECT_NAME}>
#   )
# endif()
