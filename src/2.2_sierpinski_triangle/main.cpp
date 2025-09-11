#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <learnopengl/shader.h>

using namespace std;

// settings screen
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// App data
// Triangle base points
int state = 1;
float A1[2] = {-1.0f, 1.0f};
float B1[2] = {-1.0f, -1.0f};
float C1[2] = {1.0f, 0.0f};

float A2[2] = {-1.0f, 1.0f};
float B2[2] = {1.0f, 1.0f};
float C2[2] = {0.0f, -1.0f};

float A3[2] = {1.0f, 1.0f};
float B3[2] = {1.0f, -1.0f};
float C3[2] = {-1.0f, 0.0f};

float A4[2] = {0.0f, 1.0f};
float B4[2] = {-1.0f, -1.0f};
float C4[2] = {1.0f, -1.0f};
int depth = 0; // recursion depth
GLuint VAO, VBO;
// vertices all
vector<float> vertices;

/*
A, B, C -> [2] -> (x, y)
*/
void generateVertices(vector<float> &verts, float *a, float *b, float *c, int d)
{
  if (d == 0)
  {
    float r, g, bl;
    // insert A pos
    r = (rand() % 1024) / 1023.0f;
    g = (rand() % 1024) / 1023.0f;
    bl = (rand() % 1024) / 1023.0f;
    verts.insert(verts.end(), {a[0], a[1], 0.0f, r, g, bl});

    // insert B pos
    r = (rand() % 1024) / 1023.0f;
    g = (rand() % 1024) / 1023.0f;
    bl = (rand() % 1024) / 1023.0f;
    verts.insert(verts.end(), {b[0], b[1], 0.0f, r, g, bl});

    // insert C pos
    r = (rand() % 1024) / 1023.0f;
    g = (rand() % 1024) / 1023.0f;
    bl = (rand() % 1024) / 1023.0f;
    verts.insert(verts.end(), {c[0], c[1], 0.0f, r, g, bl});
    return;
  }

  /*
                  A

              AB     AC

          B      BC       C
  */

  float AB[2] = {(a[0] + b[0]) / 2.0f, (a[1] + b[1]) / 2.0f};
  float AC[2] = {(a[0] + c[0]) / 2.0f, (a[1] + c[1]) / 2.0f};
  float BC[2] = {(b[0] + c[0]) / 2.0f, (b[1] + c[1]) / 2.0f};

  // Triangle A
  //      AB    AC
  generateVertices(verts, a, AB, AC, d - 1);
  // Triangle AB
  //      B     BC
  generateVertices(verts, AB, b, BC, d - 1);
  // Triangle AC
  //      BC      C
  generateVertices(verts, AC, BC, c, d - 1);
}

void updateVertices()
{
  vertices.clear();
  if (state == 1)
  {
    generateVertices(vertices, A1, B1, C1, depth);
  }
  else if (state == 2)
  {
    generateVertices(vertices, A2, B2, C2, depth);
  }
  else if (state == 3)
  {
    generateVertices(vertices, A3, B3, C3, depth);
  }
  else if (state == 4)
  {
    generateVertices(vertices, A4, B4, C4, depth);
  }
  else
  {
    state = 1;
    updateVertices();
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  state++;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  static bool aPressed = false, dPressed = false;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    if (!aPressed) // only trigger once per press
    {
      depth++;
      cout << "Depth increased: " << depth << endl;
      updateVertices();
    }
    aPressed = true;
  }
  else
  {
    aPressed = false;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    if (!dPressed && depth > 0)
    {
      depth--;
      cout << "Depth decreased: " << depth << endl;
      updateVertices();
    }
    dPressed = true;
  }
  else
  {
    dPressed = false;
  }
}

int main()
{
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // glfw window creation
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  Shader ourShader("model.vs", "model.fs");

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Clear bind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // first generate
  updateVertices();

  // render loop
  while (!glfwWindowShouldClose(window))
  {
    // input
    processInput(window);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(ourShader.ID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
