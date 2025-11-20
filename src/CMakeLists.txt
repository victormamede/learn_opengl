set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})

add_executable(${EXECUTABLE_NAME} 
  main.cpp 
  shader.cpp
  cube.cpp
)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw glad stb_image glm::glm imgui)
