#include "GroundMesh.hpp"
#include <glad/glad.h>

// Flat quad facing +Y (up), centered at origin
// Two triangles form a single square
static float groundVertices[] = {
    // positions          // texCoords
    -0.5f, 0.0f, -0.5f, 0.0f, 0.0f,
    0.5f, 0.0f, -0.5f, 10.0f, 0.0f,
    0.5f, 0.0f, 0.5f, 10.0f, 10.0f,

    0.5f, 0.0f, 0.5f, 10.0f, 10.0f,
    -0.5f, 0.0f, 0.5f, 0.0f, 10.0f,
    -0.5f, 0.0f, -0.5f, 0.0f, 0.0f};

void GroundMesh::build()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

  // position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  // texCoord attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

  glBindVertexArray(0);
}

void GroundMesh::draw() const
{
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
