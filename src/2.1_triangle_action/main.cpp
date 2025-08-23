#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

GLfloat vertices[] =
    {
        -0.25f, 0.25f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        0.25f, 0.0f, 0.0f};

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

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

void UpdateVertexData(GLuint VAO, GLuint VBO)
{
  // Update Vertex Data
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, GLuint VAO, GLuint VBO)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  // Move up
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    vertices[1] += 0.025f;
    vertices[4] += 0.025f;
    vertices[7] += 0.025f;

    UpdateVertexData(VAO, VBO);
  }

  // Move down
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    vertices[1] -= 0.025f;
    vertices[4] -= 0.025f;
    vertices[7] -= 0.025f;

    UpdateVertexData(VAO, VBO);
  }

  // Move left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    vertices[0] -= 0.025f;
    vertices[3] -= 0.025f;
    vertices[6] -= 0.025f;

    UpdateVertexData(VAO, VBO);
  }

  // Move right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    vertices[0] += 0.025f;
    vertices[3] += 0.025f;
    vertices[6] += 0.025f;

    UpdateVertexData(VAO, VBO);
  }

  static bool rKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
  {
    if (!rKeyPressed)
    {
      float cx = (vertices[0] + vertices[3] + vertices[6]) / 3.0f;
      float cy = (vertices[1] + vertices[4] + vertices[7]) / 3.0f;

      float angle = -3.14159265f / 2.0f;
      float cosA = cos(angle);
      float sinA = sin(angle);

      for (int i = 0; i < 9; i += 3)
      {
        float x = vertices[i] - cx;
        float y = vertices[i + 1] - cy;
        float xr = x * cosA - y * sinA;
        float yr = x * sinA + y * cosA;
        vertices[i] = xr + cx;
        vertices[i + 1] = yr + cy;
      }
      UpdateVertexData(VAO, VBO);
      rKeyPressed = true;
    }
  }
  else
  {
    rKeyPressed = false;
  }

  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
  {
    vertices[0] = -0.25f;
    vertices[1] = 0.25f;
    vertices[2] = 0.0f;
    vertices[3] = -0.25f;
    vertices[4] = -0.25f;
    vertices[5] = 0.0f;
    vertices[6] = 0.25f;
    vertices[7] = 0.0f;
    vertices[8] = 0.0f;

    UpdateVertexData(VAO, VBO);
  }

  // Mouse click: center triangle at mouse position
  static bool mousePressed = false;
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  {
    if (!mousePressed)
    {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);

      int width, height;
      glfwGetWindowSize(window, &width, &height);

      // Convert to normalized device coordinates (-1 to 1)
      float x_ndc = (float)((xpos / width) * 2.0 - 1.0);
      float y_ndc = (float)(1.0 - (ypos / height) * 2.0);

      // Calculate current centroid
      float cx = (vertices[0] + vertices[3] + vertices[6]) / 3.0f;
      float cy = (vertices[1] + vertices[4] + vertices[7]) / 3.0f;

      float dx = x_ndc - cx;
      float dy = y_ndc - cy;

      // Move all vertices by (dx, dy)
      for (int i = 0; i < 9; i += 3)
      {
        vertices[i] += dx;
        vertices[i + 1] += dy;
      }

      UpdateVertexData(VAO, VBO);
      mousePressed = true;
    }
  }
  else
  {
    mousePressed = false;
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

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // render loop
  while (!glfwWindowShouldClose(window))
  {
    // input
    processInput(window, VAO, VBO);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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