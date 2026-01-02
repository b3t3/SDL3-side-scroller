find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
add_executable (RPG "RPG.cpp" "RPG.h" "animation.h" "gameobject.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RPG PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(RPG PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_include_directories(RPG PRIVATE "ext/")
