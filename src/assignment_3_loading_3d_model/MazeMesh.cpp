#include "MazeMesh.hpp"
#include <iostream>
#include <GLFW/glfw3.h>

MazeMesh::MazeMesh() = default;

MazeMesh::~MazeMesh()
{
  // Safe cleanup only if GL context is still active
  if (glfwGetCurrentContext())
  {
    if (EBO)
      glDeleteBuffers(1, &EBO);
    if (VBO)
      glDeleteBuffers(1, &VBO);
    if (VAO)
      glDeleteVertexArrays(1, &VAO);
  }
}

// Move constructor
MazeMesh::MazeMesh(MazeMesh &&other) noexcept
{
  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  indexCount = other.indexCount;

  other.VAO = 0;
  other.VBO = 0;
  other.EBO = 0;
  other.indexCount = 0;
}

// Move assignment
MazeMesh &MazeMesh::operator=(MazeMesh &&other) noexcept
{
  if (this != &other)
  {
    if (glfwGetCurrentContext())
    {
      if (EBO)
        glDeleteBuffers(1, &EBO);
      if (VBO)
        glDeleteBuffers(1, &VBO);
      if (VAO)
        glDeleteVertexArrays(1, &VAO);
    }

    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    indexCount = other.indexCount;

    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.indexCount = 0;
  }
  return *this;
}

void MazeMesh::build(const Maze &maze, float wallHeight)
{
  struct Vertex
  {
    glm::vec3 pos;
    glm::vec2 uv;
  };

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  int w = maze.width();
  int h = maze.height();

  float wallThickness = 0.05f;
  float cellSize = 2.0f;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      const auto &cell = maze.getCell(x, y);
      float bx = x * cellSize;
      float bz = y * cellSize;

      // Add 4 walls per cell if they exist
      auto addWall = [&](float x1, float z1, float x2, float z2)
      {
        glm::vec3 v0(bx + x1, 0.0f, bz + z1);
        glm::vec3 v1(bx + x2, 0.0f, bz + z2);
        glm::vec3 v2(bx + x2, wallHeight, bz + z2);
        glm::vec3 v3(bx + x1, wallHeight, bz + z1);
        unsigned int base = vertices.size();
        vertices.push_back({v0, {0.0f, 0.0f}});
        vertices.push_back({v1, {1.0f, 0.0f}});
        vertices.push_back({v2, {1.0f, 1.0f}});
        vertices.push_back({v3, {0.0f, 1.0f}});
        indices.insert(indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
      };

      if (cell.up)
        addWall(0, 0, cellSize, 0);
      if (cell.down)
        addWall(0, cellSize, cellSize, cellSize);
      if (cell.left)
        addWall(0, 0, 0, cellSize);
      if (cell.right)
        addWall(cellSize, 0, cellSize, cellSize);
    }
  }

  indexCount = static_cast<GLsizei>(indices.size());

  // --- Create VAO/VBO/EBO ---
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  // vertex positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  std::cout << "MazeMesh built: " << vertices.size() << " vertices, "
            << indices.size() / 3 << " triangles.\n";
}

void MazeMesh::draw() const
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
