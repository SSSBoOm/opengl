#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Maze.hpp"

class MazeMesh
{
public:
  MazeMesh();
  ~MazeMesh();

  // non-copyable
  MazeMesh(const MazeMesh &) = delete;
  MazeMesh &operator=(const MazeMesh &) = delete;

  // moveable (optional, safe)
  MazeMesh(MazeMesh &&other) noexcept;
  MazeMesh &operator=(MazeMesh &&other) noexcept;

  void build(const Maze &maze, float wallHeight = 1.0f);
  void draw() const;

private:
  GLuint VAO{0}, VBO{0}, EBO{0};
  GLsizei indexCount{0};
};
