cmake_minimum_required(VERSION 3.20)

# Windows では vcpkg のツールチェーンを使う
# vcpkg の場所は環境変数 VCPKG_ROOT で指定する
if(CMAKE_HOST_WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(NOT DEFINED ENV{VCPKG_ROOT})
        message(FATAL_ERROR
            "VCPKG_ROOT is not set. Set it to your vcpkg directory "
            "(e.g. setx VCPKG_ROOT C:\\tools\\vcpkg) and open a new terminal.")
    endif()

    file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}" VCPKG_ROOT)

    # 開発者用のシェル (Developer PowerShell for VS など) は VCPKG_ROOT を
    # Visual Studio 付属の vcpkg に書き換えるが、付属の vcpkg は
    # vcpkg install で入れたライブラリ (クラシックモード) を扱えない
    if(VCPKG_ROOT MATCHES "Microsoft Visual Studio")
        message(FATAL_ERROR
            "VCPKG_ROOT points to the vcpkg bundled with Visual Studio "
            "(${VCPKG_ROOT}). Run CMake from a normal PowerShell, "
            "not the Developer PowerShell / Command Prompt for VS.")
    endif()

    if(NOT EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        message(FATAL_ERROR "vcpkg not found in VCPKG_ROOT: ${VCPKG_ROOT}")
    endif()

    set(CMAKE_TOOLCHAIN_FILE
        "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "vcpkg toolchain file")
endif()

# アプリの名前 (実行ファイルの名前になる。変えるときは .vscode/launch.json と tasks.json も直す)
set(APP_NAME ImGuiApp)

project(${APP_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Dear ImGui (CMakeLists.txt を持たないのでソースを直接ビルド)
include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.9b
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}/misc/cpp
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

add_executable(${APP_NAME}
    src/main.cpp
    src/app/application.cpp
    src/app/renderer.cpp
    src/common/font.cpp
    src/common/ime.cpp
    src/common/wake.cpp
    src/common/widgets.cpp
    src/sample/basic_tab.cpp
    src/sample/worker_tab.cpp
)

# #include "common/tab.h" のように src/ からの相対パスで書く
target_include_directories(${APP_NAME} PRIVATE src)

# macOS ではタイトルバーの設定に AppKit を使う (Objective-C++。透明なウィンドウのときだけ使う)
if(APPLE)
    enable_language(OBJCXX)
    target_sources(${APP_NAME} PRIVATE src/common/macos_window.mm)
    set_source_files_properties(src/common/macos_window.mm
        PROPERTIES COMPILE_OPTIONS -fobjc-arc)
    target_link_libraries(${APP_NAME} PRIVATE "-framework AppKit")
endif()

# ソースは UTF-8 (日本語の文字列リテラルを含む)
if(MSVC)
    target_compile_options(imgui PRIVATE /utf-8)
    target_compile_options(${APP_NAME} PRIVATE /utf-8)
endif()

target_link_libraries(${APP_NAME} PRIVATE
    imgui
    Threads::Threads
)

# フォントなどのアセットを実行ファイルの隣にコピー
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Asset
        $<TARGET_FILE_DIR:${APP_NAME}>/Asset
    VERBATIM
)
