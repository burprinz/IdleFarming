cmake_minimum_required(VERSION 3.16)
include(FetchContent)

# propagate variables set in this project to fetched dependencies
cmake_policy(SET CMP0077 NEW)

project(IdleFarming VERSION 0.0 LANGUAGES CXX C)

# Set the build type to Debug if no other build type was selected.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Fetch some basic dependencies for creating windows (glfw), vector math (glm),
# audio (miniaudio) and loading OpenGL profiles (gl3w)
FetchContent_Declare(
	glfw # Licensed under Zlib
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	glm # Licensed under MIT (or the Happy Bunny License [modified MIT])
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	miniaudio # Public Domain (or licensed under MIT)
	GIT_REPOSITORY https://github.com/mackron/miniaudio.git
	GIT_TAG 0.11.22
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	gl3w # Public Domain
	GIT_REPOSITORY https://github.com/skaslev/gl3w.git
	GIT_TAG        master
	GIT_SHALLOW    TRUE
	GIT_PROGRESS   TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
		entt
		GIT_REPOSITORY https://github.com/skypjack/entt.git
		GIT_TAG v3.13.0
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
)
FetchContent_Declare(
		freetype # FreeType Library
		GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
		GIT_TAG VER-2-14-1
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
		EXCLUDE_FROM_ALL
)

# You can fetch additional libraries here, e.g. `fmt`
FetchContent_Declare(
	fmt # Licensed under MIT
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG 11.2.0
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	tinyobj # Licensed under MIT
	GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
	GIT_TAG release
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)
FetchContent_Declare(
	stb # Public Domain
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG master
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	EXCLUDE_FROM_ALL
)

# Adding nlohmann JSON for working with json files.
FetchContent_Declare(
		json
		GIT_REPOSITORY https://github.com/nlohmann/json.git
		GIT_TAG v3.11.3
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
		EXCLUDE_FROM_ALL
)

# Discord
FetchContent_Declare(
		discord-rpc
		GIT_REPOSITORY https://github.com/discord/discord-rpc.git
		GIT_TAG        v3.4.0 # Latest stable release
		GIT_SHALLOW    TRUE
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(glfw glm miniaudio gl3w entt freetype fmt tinyobj stb json discord-rpc)


# Configure compiler flags as an interface library
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)
target_compile_definitions(compiler_flags INTERFACE $<$<CONFIG:Debug>:DEBUG>)

if (MSVC)
	target_compile_definitions(compiler_flags INTERFACE _USE_MATH_DEFINES)
	target_compile_options(compiler_flags INTERFACE /W4 /wd4244 /wd4267 /wd4838)
else()
	target_compile_options(compiler_flags INTERFACE -Wall -Wextra -Wpedantic)
endif()

# Configure header containing project metadata
configure_file(${PROJECT_SOURCE_DIR}/src/project_config.h.in project_config.h)

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/src/main.cpp
	${PROJECT_SOURCE_DIR}/src/common.cpp
	${PROJECT_SOURCE_DIR}/src/assets.cpp
        src/core/application.cpp
	${PROJECT_SOURCE_DIR}/src/core/registry.cpp
	${PROJECT_SOURCE_DIR}/src/window.cpp

	${PROJECT_SOURCE_DIR}/src/systems/world.cpp
		${PROJECT_SOURCE_DIR}/src/systems/render.cpp
	${PROJECT_SOURCE_DIR}/src/systems/audio.cpp

	${PROJECT_SOURCE_DIR}/src/utils/mesh.cpp
	${PROJECT_SOURCE_DIR}/src/utils/shader.cpp
	${PROJECT_SOURCE_DIR}/src/utils/framebuffer.cpp
	${PROJECT_SOURCE_DIR}/src/utils/texture.cpp
        src/core/util.h
		src/core/draw.cpp
		src/core/draw.h
		src/utils/geometry.h
		src/discord.cpp
		src/discord.h
		src/utils/entity_factory.cpp
		src/utils/entity_factory.h
		src/utils/BigNumber.h
		src/systems/field.cpp
		src/systems/field.h
		src/core/colors.h
		src/systems/ui.cpp
		src/systems/ui.h
		src/core/sizes.h
		src/core/paths.h
		src/systems/camera.cpp
		src/systems/camera.h
)


set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
	EXPORT_COMPILE_COMMANDS ON
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_BINARY_DIR} # location of the configured header
	${PROJECT_SOURCE_DIR}/src # location of our own headers
	${stb_SOURCE_DIR} # location of the `stb_image` header
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	compiler_flags glfw glm::glm miniaudio tinyobjloader gl3w fmt nlohmann_json::nlohmann_json freetype EnTT::EnTT discord-rpc
)
