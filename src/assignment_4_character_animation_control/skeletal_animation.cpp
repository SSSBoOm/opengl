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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// model movement
glm::vec3 characterPosition(0.0f, 0.0f, 0.0f);
float characterYaw = 0.0f;
float walkSpeed = 3.0f;
float runSpeed = 6.0f;

// camera follow parameters
float followDistance = 3.5f;
float followHeight = 1.2f;
float cameraSmoothness = 0.18f;
bool frontView = false;
float frontFollowDistance = 2.0f;
float originalFollowDistance = followDistance;
float originalFollowHeight = followHeight;

enum AnimState
{
  IDLE = 1,
  IDLE_WALK,
  WALK_IDLE,
  WALK,
  WALK_RUN,
  RUN_WALK,
  RUN,
  RUN_IDLE,
  IDLE_JUMPIDLE,
  JUMPIDLE_IDLE,
};

float SmoothStep(float t) { return t * t * (3 - 2 * t); }

int main()
{
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
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_DEPTH_TEST);

  Shader ourShader("anim_model.vs", "anim_model.fs");

  Model ourModel(FileSystem::getPath("resources/objects/assignment_4/Ch42_nonPBR.dae"));
  Animation idleAnimation(FileSystem::getPath("resources/objects/assignment_4/animation/Idle.dae"), &ourModel);
  Animation walkAnimation(FileSystem::getPath("resources/objects/assignment_4/animation/Walk.dae"), &ourModel);
  Animation runAnimation(FileSystem::getPath("resources/objects/assignment_4/animation/Running.dae"), &ourModel);
  Animation idleJumpAnimation(FileSystem::getPath("resources/objects/assignment_4/animation/IdleJump.dae"), &ourModel);

  Animator animator(&idleAnimation);
  AnimState charState = IDLE;
  float blendAmount = 0.0f;
  float blendRate = 0.0075f;

  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);
    glfwPollEvents();

    // Character rotation
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      characterYaw -= 120.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      characterYaw += 120.0f * deltaTime;

    // ---------------- Animation State Machine (Your Original) ---------------- //
    animator.UpdateAnimation(deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
      frontView = true;
    }
    else
    {
      frontView = false;
    }

    switch (charState)
    {
    case IDLE:
    {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleAnimation, &walkAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
        charState = IDLE_WALK;
      }
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleAnimation, &idleJumpAnimation, animator.m_CurrentTime, 0.0f, blendAmount);
        charState = IDLE_JUMPIDLE;
      }
      break;
    }
    case IDLE_WALK:
    {
      blendAmount += blendRate;
      animator.PlayAnimation(&idleAnimation, &walkAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, SmoothStep(blendAmount));
      if (blendAmount > 0.9f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&walkAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = WALK;
      }
      break;
    }
    case WALK:
    {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        characterPosition += glm::vec3(
            sin(glm::radians(characterYaw)) * walkSpeed * deltaTime,
            0.0f,
            cos(glm::radians(characterYaw)) * walkSpeed * deltaTime);
      }

      animator.PlayAnimation(&walkAnimation, NULL, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);

      if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS)
        charState = WALK_IDLE;
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        charState = WALK_RUN;
      break;
    }
    case WALK_IDLE:
    {
      blendAmount += blendRate;
      animator.PlayAnimation(&walkAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, SmoothStep(blendAmount));
      if (blendAmount > 0.9f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = IDLE;
      }
      break;
    }
    case WALK_RUN:
    {
      blendAmount += blendRate;
      animator.PlayAnimation(&walkAnimation, &runAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, SmoothStep(blendAmount));
      if (blendAmount > 0.9f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&runAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = RUN;
      }
      break;
    }

    case RUN:
    {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        characterPosition += glm::vec3(
            sin(glm::radians(characterYaw)) * runSpeed * deltaTime,
            0.0f,
            cos(glm::radians(characterYaw)) * runSpeed * deltaTime);
      }

      animator.PlayAnimation(&runAnimation, NULL, animator.m_CurrentTime, animator.m_CurrentTime2, blendAmount);

      if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS)
        charState = RUN_IDLE;
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
        charState = RUN_WALK;
      break;
    }
    case RUN_WALK:
    {
      blendAmount += blendRate;
      animator.PlayAnimation(&runAnimation, &walkAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, SmoothStep(blendAmount));
      if (blendAmount > 0.9f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&walkAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = WALK;
      }
      break;
    }
    case RUN_IDLE:
    {
      blendAmount += blendRate;
      animator.PlayAnimation(&runAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, SmoothStep(blendAmount));
      if (blendAmount > 0.9f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = IDLE;
      }
      break;
    }
    case IDLE_JUMPIDLE:
    {
      blendAmount += blendRate;
      float smoothBlend = SmoothStep(glm::clamp(blendAmount, 0.0f, 1.0f));
      animator.PlayAnimation(&idleAnimation, &idleJumpAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, smoothBlend);
      if (smoothBlend >= 1.0f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleJumpAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = JUMPIDLE_IDLE;
      }
      break;
    }

    case JUMPIDLE_IDLE:
    {
      blendAmount += blendRate;
      float smoothBlend = SmoothStep(glm::clamp(blendAmount, 0.0f, 1.0f));
      animator.PlayAnimation(&idleJumpAnimation, &idleAnimation, animator.m_CurrentTime, animator.m_CurrentTime2, smoothBlend);
      if (smoothBlend >= 1.0f)
      {
        blendAmount = 0.0f;
        animator.PlayAnimation(&idleAnimation, NULL, animator.m_CurrentTime2, 0.0f, blendAmount);
        charState = IDLE;
      }
      break;
    }
    }

    glm::vec3 idealCamPos;

    if (!frontView)
    {
      idealCamPos = characterPosition - glm::vec3(sin(glm::radians(characterYaw)) * followDistance, -followHeight, cos(glm::radians(characterYaw)) * followDistance);
    }
    else
    {
      idealCamPos = characterPosition + glm::vec3(sin(glm::radians(characterYaw)) * frontFollowDistance, followHeight + 0.5f, cos(glm::radians(characterYaw)) * frontFollowDistance);
    }
    camera.Position = glm::mix(camera.Position, idealCamPos, cameraSmoothness);

    if (frontView)
    {
      camera.Front = glm::normalize((characterPosition + glm::vec3(0, 1.7f, 0)) - camera.Position);
    }
    else
    {
      camera.Front = glm::normalize((characterPosition + glm::vec3(0, 1.2f, 0)) - camera.Position);
    }

    camera.Position = glm::mix(camera.Position, idealCamPos, cameraSmoothness);
    camera.Front = glm::normalize((characterPosition + glm::vec3(0, 0.5f, 0)) - camera.Position);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    auto transforms = animator.GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
      ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, characterPosition);
    model = glm::rotate(model, glm::radians(characterYaw), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}