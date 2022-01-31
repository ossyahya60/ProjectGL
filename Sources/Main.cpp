#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Graphics/Shader.h"
#include <filesystem>
#include "Graphics/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Graphics/Texture.h"
#include <entt/entt.hpp>
#include "Graphics/SpriteBatch.h"

GLFWwindow* window = NULL;

//camera parameters
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 0;
float deltaTime = 0.02f;
double prevWindowTime = 0;

float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;
double prevMousePosX = 0.0;
double prevMousePosY = 0.0;
float pitch = 0.0f;
float yaw = -90.0f;
float roll = 0.0f;
float sensetivity = 0.1f;
bool firstMouse = true;
float fov = 45;
glm::vec3 sideWaysMatrix;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //On Resize function callback
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) //called if mouse pos changed
{
    if (firstMouse)
    {
        prevMousePosX = xpos;
        prevMousePosY = ypos;
        firstMouse = false;
    }

    mouseDeltaX = (float)(xpos - prevMousePosX);
    mouseDeltaY = (float)(ypos - prevMousePosY);

    prevMousePosX = xpos;
    prevMousePosY = ypos;

    pitch += sensetivity * mouseDeltaY;
    yaw += sensetivity * mouseDeltaX;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= sideWaysMatrix * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += sideWaysMatrix * cameraSpeed;

}

void init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Uncomment this line if you are using MacOS

    window = glfwCreateWindow(800, 600, "Graphics Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  //Initialize GLAD (Call this before using any OpenGL function)
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //register function

    glfwSetCursorPosCallback(window, mouse_callback); //register mouse movement callback

    glfwSetScrollCallback(window, scroll_callback); //mouse wheel movement callback

    //disable cursor (cursor will not leave the window and will be hidden)
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void cleanUp()
{
    glfwTerminate(); //Clean up
}

int main()
{
    init();

    //enable depth testing
    glEnable(GL_DEPTH_TEST);

    sideWaysMatrix = glm::normalize(glm::cross(cameraFront, cameraUp));

    std::string projectPath = std::filesystem::current_path().string();
    std::replace(projectPath.begin(), projectPath.end(), '\\', '/');

    //I should provide a relative path to the project
    Shader mainShader(projectPath + "/Assets/Shaders/Vertex.vert", projectPath + "/Assets/Shaders/Fragment.frag");

    ///////////////textures/////////////////////////
    Texture mamdouh = Texture("/Assets/Textures/Mamdouh.png", true);
    Texture container = Texture("/Assets/Textures/container.jpg", false);

    int samplers[16];
    for (int i = 0; i < 16; i++)
        samplers[i] = i;
    mainShader.use();
    mainShader.setVeci("textures", 16, samplers);

    SpriteBatch::init();

    while (!glfwWindowShouldClose(window))
    {
        deltaTime = (float)(glfwGetTime() - prevWindowTime);
        prevWindowTime = glfwGetTime();
        cameraSpeed = deltaTime;
        // process input
        processInput(window);

        //render stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //we clear both buffers: color and depth

        // 2. use our shader program when we want to render an object
        float timeValue = (float)glfwGetTime();

        mainShader.use();
        
        //model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, 0.0f, glm::vec3(0.5f, 1.0f, 0.0f));

        //Camera stuff
        //cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); //cos pitch * sin yaw
        //cameraFront.y = sin(glm::radians(-pitch)); //sin pitch (yaw must be initialized to -90 to face -z axis)
        //cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); //cos pitch * cos yaw
        //cameraFront = glm::normalize(cameraFront);

        //view matrix
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //camera pos, target, global up direction

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        //projection matrix
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 100.0f);

        mainShader.setMat4("model", 1, glm::value_ptr(model));
        mainShader.setMat4("view", 1, glm::value_ptr(view));
        mainShader.setMat4("projection", 1, glm::value_ptr(projection));
        
        //wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        SpriteBatch::resetStats();
        SpriteBatch::begin();

        SpriteBatch::drawQuad(glm::vec2(0.0f, 0.0f), glm::vec2(0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), &mamdouh);

        SpriteBatch::end();

        mainShader.use();
        SpriteBatch::flush();

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SpriteBatch::shutDown();

    container.deleteTexture();
    mamdouh.deleteTexture();
    mainShader.deleteShader();

    cleanUp();

    return 0;
}