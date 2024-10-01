cmake_minimum_required(VERSION 3.20)

# Project definition
project(Loki LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
include(cmake/CPM.cmake)

FetchContent_Declare(
        libassert
        GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
        GIT_TAG v2.1.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libassert)

FetchContent_Declare(
        boost_pfr
        GIT_REPOSITORY https://github.com/boostorg/pfr.git
        GIT_TAG 2.2.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(boost_pfr)

CPMAddPackage(
        NAME glfw3
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.4
        OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_INSTALL OFF"
        "GLFW_BUILD_SHARED_LIBS OFF"
)

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
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
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
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG v1.14.1
        OPTIONS
        "SPDLOG_USE_STD_FORMAT OFF"
        "SPDLOG_ENABLE_PCH ON"
)

FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.4.2
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(cli11)

CPMAddPackage(
        NAME mimalloc
        GITHUB_REPOSITORY microsoft/mimalloc
        GIT_TAG v2.1.7
        OPTIONS
        "MI_BUILD_SHARED OFF"
        "MI_BUILD_TESTS OFF"
)

FetchContent_Declare(
        StormLib
        GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib.git
        GIT_TAG v9.26
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(StormLib)

# Assuming StormLib does not provide a target, manually specify the include directory
add_library(StormLib INTERFACE)
target_include_directories(StormLib INTERFACE ${StormLib_SOURCE_DIR}/src)

CPMAddPackage(
        NAME sockpp
        GITHUB_REPOSITORY fpagliughi/sockpp
        GIT_TAG v1.0.0
        OPTIONS
        "SOCKPP_BUILD_SHARED OFF"
        "SOCKPP_BUILD_STATIC ON"
)

FetchContent_Declare(
        lib_glob
        GIT_REPOSITORY https://github.com/p-ranav/glob.git
        GIT_TAG v0.0.1
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(lib_glob)

FetchContent_Declare(
        BLPConverter
        GIT_REPOSITORY https://github.com/vladbelousoff/BLPConverter.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(BLPConverter)

# This library is PITA
target_include_directories(blp INTERFACE ${blp_SOURCE_DIR})

FetchContent_Declare(
        OpenSSL
        GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
        GIT_TAG 1.1.1w-20231130
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(OpenSSL)

FetchContent_Declare(
        gl3w
        GIT_REPOSITORY https://github.com/vladbelousoff/gl3w.git
        GIT_TAG 1.0.0
)
FetchContent_MakeAvailable(gl3w)

# Source files
set(SOURCES
        main.cpp
        engine/config.h
        engine/engine_app.h
        engine/engine_app.cpp
        engine/time/scope_timer.h
        engine/utils/strings.h
        engine/utils/string_manager.h
        engine/utils/string_manager.cpp
        engine/utils/big_num.h
        engine/utils/big_num.cpp
        engine/utils/byte_buffer.h
        engine/utils/byte_buffer.cpp
        engine/crypto/crypto_random.h
        engine/crypto/crypto_random.cpp
        engine/crypto/arc_4.h
        engine/crypto/arc_4.cpp
        engine/crypto/srp_6.h
        engine/crypto/srp_6.cpp
        engine/network/auth_session.h
        engine/network/auth_session.cpp
        engine/network/auth_crypt.h
        engine/network/auth_crypt.cpp
        engine/network/world_session.h
        engine/network/world_session.cpp
        engine/render/shader.h
        engine/render/shader.cpp
        engine/datasource/mpq/mpq_archive.h
        engine/datasource/mpq/mpq_archive.cpp
        engine/datasource/mpq/mpq_chain.h
        engine/datasource/mpq/mpq_chain.cpp
        engine/datasource/mpq/mpq_file.h
        engine/datasource/mpq/mpq_file.cpp
        engine/datasource/mpq/mpq_file_manager.h
        engine/datasource/mpq/mpq_file_manager.cpp
        engine/asset/asset.h
        engine/asset/asset.cpp
        engine/model/m_2_model.h
        engine/model/m_2_model.cpp
        engine/model/m_2_model_view.h
        engine/model/m_2_model_view.cpp
        engine/texture/blp_texture.h
        engine/texture/blp_texture.cpp
        engine/mt/main_thread_queue.h
        engine/mt/main_thread_queue.cpp
        game/game_app.h
        game/game_app.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

if (WIN32)
    # Add any necessary compile definitions
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DWIN32_LEAN_AND_MEAN)

    # Needed for OpenSSL
    target_link_libraries(${PROJECT_NAME} ws2_32.lib crypt32.lib)
endif ()

# Link dependencies to the executable
target_link_libraries(${PROJECT_NAME}
        gl3w
        libassert::assert
        boost_pfr
        glfw
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
