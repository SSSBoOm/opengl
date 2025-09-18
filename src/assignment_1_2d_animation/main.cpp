#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void getTimeAngle(float *hourAngle, float *minAngle, float *secAngle)
{
  time_t now = time(0);
  tm *ltm = localtime(&now);

  int hours = ltm->tm_hour % 12;
  int minutes = ltm->tm_min;
  int seconds = ltm->tm_sec;
  // std::cout << hours << " : " << minutes << " : " << seconds << " : " << std::endl;

  *secAngle = glm::radians(-6.0f * seconds);
  *minAngle = glm::radians(-6.0f * minutes - 0.1f * seconds);
  *hourAngle = glm::radians(-30.0f * hours - 0.5f * minutes);
  // std::cout << hourAngle << " : " << minAngle << " : " << secAngle << " : " << std::endl;
}

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  Shader clockShader("clock.vs", "clock.fs");
  Shader clockHandShader("clock_hand.vs", "clock_hand.fs");

  // set up clock
  // ------------------------------------------------------------------
  // positions (x, y, z)
  // texture coords
  GLfloat clockVertices[] = {
      // top right
      0.9f, 0.9f, 0.0f,
      1.0f, 1.0f,
      // bottom right
      0.9f, -0.9f, 0.0f,
      1.0f, 0.0f,
      // bottom left
      -0.9f, -0.9f, 0.0f,
      0.0f, 0.0f,
      // top left
      -0.9f, 0.9f, 0.0f,
      0.0f, 1.0f};
  GLuint clockIndices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  GLuint clockVBO, clockVAO, clockEBO;
  glGenVertexArrays(1, &clockVAO);
  glGenBuffers(1, &clockVBO);
  glGenBuffers(1, &clockEBO);

  glBindVertexArray(clockVAO);
  glBindBuffer(GL_ARRAY_BUFFER, clockVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(clockVertices), clockVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clockEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(clockIndices), clockIndices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint texture1;
  // texture 1
  // ---------
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/clock.jpg").c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture1" << std::endl;
  }
  stbi_image_free(data);

  // Setup clock hand
  GLfloat clockHandVertices[] = {
      // top right
      0.1f, 0.6f, 0.0f,
      // bottom right
      0.1f, -0.05f, 0.0f,
      // bottom left
      -0.1f, -0.05f, 0.0f,
      // top left
      -0.1f, 0.6f, 0.0f};
  GLuint clockHandIndices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  GLuint clockHandVBO, clockHandVAO, clockHandEBO;
  glGenVertexArrays(1, &clockHandVAO);
  glGenBuffers(1, &clockHandVBO);
  glGenBuffers(1, &clockHandEBO);

  glBindVertexArray(clockHandVAO);
  glBindBuffer(GL_ARRAY_BUFFER, clockHandVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(clockHandVertices), clockHandVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clockHandEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(clockHandIndices), clockHandIndices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window))
  {
    // processInput
    processInput(window);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // clock
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    clockShader.use();
    glBindVertexArray(clockVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // clock hand
    clockHandShader.use();
    float hourAngle, minAngle, secAngle;
    getTimeAngle(&hourAngle, &minAngle, &secAngle);
    unsigned int transformLoc = glGetUniformLocation(clockHandShader.ID, "transform");
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, secAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(0.075f, 0.9f, 1.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(clockHandVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, minAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(0.075f, 0.6f, 1.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(clockHandVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, hourAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(0.075f, 0.3f, 1.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(clockHandVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &clockVAO);
  glDeleteBuffers(1, &clockVBO);
  glDeleteBuffers(1, &clockEBO);

  glDeleteVertexArrays(1, &clockHandVAO);
  glDeleteBuffers(1, &clockHandVBO);
  glDeleteBuffers(1, &clockHandEBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}