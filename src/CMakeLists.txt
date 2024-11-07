find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

add_executable (${PROJECT_NAME} "directx11.cpp" "directx11.hpp")

target_link_libraries(${PROJECT_NAME} PRIVATE 
	fmt::fmt 
	SDL2::SDL2main
	SDL2::SDL2
	d3d11
	Microsoft::DirectXTK
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
