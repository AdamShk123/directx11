find_package(fmt CONFIG REQUIRED)

add_executable (${PROJECT_NAME} "directx11.cpp" "directx11.hpp")

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
