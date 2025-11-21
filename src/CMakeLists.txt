set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})

find_package(X11 REQUIRED)

add_executable(${EXECUTABLE_NAME} 
  ${IMGUI_SOURCES} 
  ${IMGUI_BACKENDS_PATH}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_PATH}/imgui_impl_opengl3.cpp

  main.cpp 
  shader.cpp
  cube.cpp
  transform.cpp
  camera.cpp
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw glad stb_image glm::glm ${X11_LIBRARIES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE 
  ${IMGUI_PATH}
  ${IMGUI_BACKENDS_PATH}
)
