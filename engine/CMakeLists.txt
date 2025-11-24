find_package(X11 REQUIRED)

add_library(engine STATIC
  ${IMGUI_SOURCES} 
  ${IMGUI_BACKENDS_PATH}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_PATH}/imgui_impl_opengl3.cpp

  shader.cpp
  object.cpp
  transform.cpp
  camera.cpp
  light.cpp
  mesh.cpp
  game.cpp
  material.cpp
  texture.cpp
)

target_link_libraries(engine PRIVATE stb_image glm::glm ${X11_LIBRARIES})
target_link_libraries(engine PUBLIC glfw glad glm::glm)
target_include_directories(engine PRIVATE 
  ${IMGUI_BACKENDS_PATH}
)
target_include_directories(engine PUBLIC ${IMGUI_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/include")
