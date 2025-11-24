add_executable(game
  main.cpp
  freelookCamera.cpp
)

target_link_libraries(game PRIVATE engine)