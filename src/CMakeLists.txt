﻿find_package(SDL2 CONFIG REQUIRED)
find_package(Stb REQUIRED)

if(WIN32)
	find_package(directxtk CONFIG REQUIRED)
else()
	find_package(glm CONFIG REQUIRED)
endif()

file(COPY "vertex.hlsl" "pixel.hlsl" DESTINATION ${CMAKE_BINARY_DIR}/src)
file(COPY "../assets/awesomeface.png" "../assets/EagleBackground.jpg" "../assets/container.jpg" DESTINATION ${CMAKE_BINARY_DIR}/assets)

add_executable (${PROJECT_NAME} "main.cpp" "renderer.cpp" "dx.cpp" "window.cpp" "game.cpp" "logger.cpp" "input.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE "directx11.hpp" "game.hpp" "dx.hpp")

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE 
		SDL2::SDL2main
		SDL2::SDL2
		d3d11
		dxgi
		d3dcompiler
		Microsoft::DirectXTK
	)
endif()

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
