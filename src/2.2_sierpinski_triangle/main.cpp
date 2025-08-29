#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

// Vertex Shader source code
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
// Fragment Shader source code
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
                                   "}\n\0";

// settings screen
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// App data
// Triangle base points
float A[2] = {0.0f, 1.0f};
float B[2] = {-1.0f, -1.0f};
float C[2] = {1.0f, -1.0f};
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
    // insert A pos
    verts.insert(verts.end(), {a[0], a[1], 0.0f});
    // insert B pos
    verts.insert(verts.end(), {b[0], b[1], 0.0f});
    // insert C pos
    verts.insert(verts.end(), {c[0], c[1], 0.0f});
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
  generateVertices(vertices, A, B, C, depth);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
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

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
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
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

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

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
