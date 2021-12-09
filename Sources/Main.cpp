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

GLFWwindow* window = NULL;

struct Vector2
{
    float x;
    float y;
};

struct Vector3
{
    float x;
    float y;
    float z;
};

struct Vector4
{
    float x;
    float y;
    float z;
    float w;
};

struct Vertex //keep this order!!
{
    Vector3 position;
    Vector2 texCoord;
    Vector4 color;
    float texID;
};

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void cleanUp()
{
    glfwTerminate(); //Clean up
}

static Vertex* createQuad(Vertex* target, float x, float y, float texID)
{
    float size = 1.0f;

    target->position = { x, y, 0.0f };
    target->texCoord = { 0.0f, 0.0f };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texID = texID;
    target++;

    target->position = { x + size,  y, 0.0f };
    target->texCoord = { 1.0f, 0.0f };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texID = texID;
    target++;

    target->position = { x + size,  y + size, 0.0f };
    target->texCoord = { 1.0f, 1.0f };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texID = texID;
    target++;

    target->position = { x,  y + size, 0.0f };
    target->texCoord = { 0.0f, 1.0f };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texID = texID;
    target++;

    return target;
}

int main()
{
    init();

    sideWaysMatrix = glm::normalize(glm::cross(cameraFront, cameraUp));

    std::string projectPath = std::filesystem::current_path().string();
    std::replace(projectPath.begin(), projectPath.end(), '\\', '/');

    //I should provide a relative path to the project
    Shader mainShader(projectPath + "/Assets/Shaders/Vertex.vert", projectPath + "/Assets/Shaders/Fragment.frag");

    //render a cube without indices
    /*float vertices[] = {
        -1.5f,  0.5f, 0.0f,  0.0f, 1.0f,  1.0, 0.0, 0.0, 1.0,  0.0,  //pos, texCoord, color, texID
        -1.5f, -0.5f, 0.0f,  0.0f, 0.0f,  1.0, 0.0, 0.0, 1.0,  0.0,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  1.0, 0.0, 0.0, 1.0,  0.0,
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  1.0, 0.0, 0.0, 1.0,  0.0,

         0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
         1.5f,  0.5f, 0.0f,  1.0f, 1.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
         1.5f, -0.5f, 0.0f,  1.0f, 0.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
    };*/

    const size_t maxQuadCount = 1000;
    const size_t maxVerticesCount = maxQuadCount * 4;
    const size_t maxIndexCount = maxQuadCount * 6;

    /*unsigned int indices[] = {
        0, 1, 2, // first triangle
        2, 3, 0,  // second triangle

        4, 5, 6, // third triangle
        6, 7, 4  // fourth triangle
    };*/

    unsigned int indices[maxIndexCount];

    int offset = 0;
    for (size_t i = 0; i < maxIndexCount; i += 6)
    {
        indices[i] = offset;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    unsigned int VAO; //vertex array object
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * maxVerticesCount, nullptr, GL_DYNAMIC_DRAW); //capable of storing 1000 vertices (250 quads)

    //using buffer element for complex shapes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    // texture id attribute
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texID));
    glEnableVertexAttribArray(3);

    ///////////////textures/////////////////////////
    Texture mamdouh = Texture("/Assets/Textures/Mamdouh.png", true);
    Texture container = Texture("/Assets/Textures/container.jpg", false);

    int samplers[2] = { 0, 1 };
    mainShader.use();
    mainShader.setVeci("textures", 2, samplers);

    //enable depth testing
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        deltaTime = (float)(glfwGetTime() - prevWindowTime);
        prevWindowTime = glfwGetTime();
        cameraSpeed = deltaTime;
        // process input
        processInput(window);

        // set dynamic vertex buffer data
        /*float vertices[] = {
            -1.5f,  0.5f, 0.0f,  0.0f, 1.0f,  1.0, 0.0, 0.0, 1.0,  0.0,  //pos, texCoord, color, texID
            -1.5f, -0.5f, 0.0f,  0.0f, 0.0f,  1.0, 0.0, 0.0, 1.0,  0.0,
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  1.0, 0.0, 0.0, 1.0,  0.0,
            -0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  1.0, 0.0, 0.0, 1.0,  0.0,

             0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
             0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
             1.5f,  0.5f, 0.0f,  1.0f, 1.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
             1.5f, -0.5f, 0.0f,  1.0f, 0.0f,  0.0, 1.0, 0.0, 1.0,  1.0,
        };*/

        unsigned int indexCount = 0;
        std::array<Vertex, 1000> vertices;
        Vertex* buffer = vertices.data();
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                buffer = createQuad(buffer, j, i, (i + j) % 2);
                indexCount += 6;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());

        //render stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //we clear both buffers: color and depth

        // 2. use our shader program when we want to render an object
        float timeValue = (float)glfwGetTime();

        mainShader.use();
        glActiveTexture(GL_TEXTURE0);
        mamdouh.bindTexture();

        glActiveTexture(GL_TEXTURE1);
        container.bindTexture();
        
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

        glBindVertexArray(VAO);
        // 3. now draw the object 
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        
        //wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(0);
        //

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    container.deleteTexture();
    mamdouh.deleteTexture();
    mainShader.deleteShader();

    cleanUp();

    return 0;
}