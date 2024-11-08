find_package(SDL2 CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

add_executable (${PROJECT_NAME} "directx11.cpp" "game.cpp" "dx.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE 
	SDL2::SDL2main
	SDL2::SDL2
	d3d11
	dxgi
	Microsoft::DirectXTK
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
