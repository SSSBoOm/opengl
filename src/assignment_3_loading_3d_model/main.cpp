#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>

#include <iostream>

#include "Maze.hpp"
#include "MazeMesh.hpp"

// -------------------- settings --------------------
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 2.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// player
glm::vec3 playerPos(1.0f, 0.0f, 1.0f);
float playerYaw = 0.0f;

// animation toggle
bool playAnimation = true;
bool qPressed = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, const Maze &maze);

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Maze with Floor", NULL, NULL);
  if (!window)
  {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD\n";
    return -1;
  }

  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_DEPTH_TEST);

  // ---- load textures ----
  auto loadTexture = [](const std::string &path) -> unsigned int
  {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
      GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
      std::cout << "Failed to load texture at path: " << path << "\n";
    }
    stbi_image_free(data);
    return textureID;
  };

  unsigned int wallTexture = loadTexture(FileSystem::getPath("resources/textures/bricks2.jpg"));
  unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/bricks2_disp.jpg"));

  // ---- maze + shader ----
  Maze maze(10, 10);
  maze.generate();

  MazeMesh mazeMesh;
  mazeMesh.build(maze, 3.0f);

  Shader mazeShader("maze.vs", "maze.fs");
  Shader animShader("anim_model.vs", "anim_model.fs");
  Shader playerShader("model_loading.vs", "model_loading.fs");

  Model playerModel(FileSystem::getPath("resources/objects/mixamo/YakuJIgnite.dae"));
  Animation walkAnimation(FileSystem::getPath("resources/objects/mixamo/Walk.dae"), &playerModel);
  Animator walkAnimator(&walkAnimation);

  // ---- floor setup ----
  float floorVertices[] = {
      // positions            // texcoords
      -20.0f, 0.0f, -20.0f, 0.0f, 0.0f,
      20.0f, 0.0f, -20.0f, 10.0f, 0.0f,
      20.0f, 0.0f, 20.0f, 10.0f, 10.0f,
      -20.0f, 0.0f, 20.0f, 0.0f, 10.0f};
  unsigned int floorIndices[] = {0, 1, 2, 0, 2, 3};

  unsigned int floorVAO, floorVBO, floorEBO;
  glGenVertexArrays(1, &floorVAO);
  glGenBuffers(1, &floorVBO);
  glGenBuffers(1, &floorEBO);

  glBindVertexArray(floorVAO);
  glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  Shader floorShader("maze.vs", "maze.fs");

  // render loop
  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // update animation only when playing
    if (playAnimation)
      walkAnimator.UpdateAnimation(deltaTime);

    processInput(window, maze);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float distanceBehind = 5.0f;
    float heightAbove = 10.0f;

    glm::vec3 camOffset = glm::vec3(
        sin(glm::radians(playerYaw)) * -distanceBehind,
        heightAbove,
        cos(glm::radians(playerYaw)) * -distanceBehind);

    camera.Position = playerPos + camOffset;

    glm::vec3 target = playerPos + glm::vec3(0.0f, -2.5f, 0.0f);
    camera.Front = glm::normalize(target - camera.Position);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                            0.05f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    // ---- draw floor ----
    floorShader.use();
    floorShader.setMat4("projection", projection);
    floorShader.setMat4("view", view);
    glm::mat4 floorModel = glm::mat4(1.0f);
    floorShader.setMat4("model", floorModel);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ---- draw maze ----
    mazeShader.use();
    mazeShader.setMat4("projection", projection);
    mazeShader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    mazeShader.setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    mazeMesh.draw();

    // ---- draw player ----
    animShader.use();
    animShader.setMat4("projection", projection);
    animShader.setMat4("view", view);

    auto transforms = walkAnimator.GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
      animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

    glm::mat4 playerModelMat = glm::mat4(1.0f);
    playerModelMat = glm::translate(playerModelMat, playerPos);
    playerModelMat = glm::rotate(playerModelMat, glm::radians(-playerYaw), glm::vec3(0.0f, 1.0f, 0.0f));
    playerModelMat = glm::scale(playerModelMat, glm::vec3(1.0f));
    animShader.setMat4("model", playerModelMat);
    playerModel.Draw(animShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

// Input / callbacks
void processInput(GLFWwindow *window, const Maze &maze)
{
  const float speed = 3.0f * deltaTime;
  glm::vec3 forward(sin(glm::radians(playerYaw)), 0.0f, cos(glm::radians(playerYaw)));
  glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

  // Toggle animation with Q (edge-detect)
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
  {
    if (!qPressed)
    {
      playAnimation = !playAnimation;
      qPressed = true;
      std::cout << "Animation " << (playAnimation ? "resumed\n" : "paused\n");
    }
  }
  else
  {
    qPressed = false;
  }

  glm::vec3 nextPos = playerPos;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    nextPos += forward * speed;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    nextPos -= forward * speed;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    nextPos -= right * speed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    nextPos += right * speed;
  nextPos.y = 0.0f;

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    playerYaw += 100.0f * deltaTime; // rotate left
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    playerYaw -= 100.0f * deltaTime; // rotate right

  float px = nextPos.x / 2.0f;
  float pz = nextPos.z / 2.0f;
  int cellX = static_cast<int>(px);
  int cellY = static_cast<int>(pz);

  if (cellX >= 0 && cellY >= 0 && cellX < maze.width() && cellY < maze.height())
  {
    const auto &cell = maze.getCell(cellX, cellY);
    float localX = px - cellX;
    float localZ = pz - cellY;
    float margin = 0.1f;

    bool blocked = false;
    if (cell.up && localZ < margin)
      blocked = true;
    if (cell.down && localZ > 1.0f - margin)
      blocked = true;
    if (cell.left && localX < margin)
      blocked = true;
    if (cell.right && localX > 1.0f - margin)
      blocked = true;

    if (!blocked)
      playerPos = nextPos;
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}