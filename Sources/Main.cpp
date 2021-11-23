#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <filesystem>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"

GLFWwindow* window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //On Resize function callback
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
}

void cleanUp()
{
    glfwTerminate(); //Clean up
}

int main()
{
    init();

    std::string projectPath = std::filesystem::current_path().string();
    std::replace(projectPath.begin(), projectPath.end(), '\\', '/');

    //I should provide a relative path to the project
    Shader mainShader(projectPath + "/Assets/Shaders/Vertex.vert", projectPath + "/Assets/Shaders/Fragment.frag");

    //render a rectangle
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    float vertices2[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        1.0f, -0.5f, 0.0f  // right most
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VAO[2]; //vertex array object
    glGenVertexArrays(2, VAO);

    unsigned int VBO[2];
    glGenBuffers(2, VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //using buffer element for complex shapes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);*/

    ///////////////textures/////////////////////////
    Texture mamdouh = Texture("/Assets/Textures/Mamdouh.png", true);
    Texture container = Texture("/Assets/Textures/container.jpg", false);

    float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
    };

    mainShader.use();
    mainShader.setInt("texture1", 0);
    mainShader.setInt("texture2", 1);

    while (!glfwWindowShouldClose(window))
    {
        // process input
        processInput(window);

        //render stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 2. use our shader program when we want to render an object
        float timeValue = (float)glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(mainShader.ID, "color");

        glActiveTexture(GL_TEXTURE0);
        container.bindTexture();

        mainShader.use();
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        mainShader.setMat4("transform", 1, glm::value_ptr(trans));

        glBindVertexArray(VAO[0]);
        // 3. now draw the object 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        mamdouh.bindTexture();

        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
        trans = glm::scale(trans, glm::vec3(sin((float)glfwGetTime()), sin((float)glfwGetTime()), 1));

        mainShader.setMat4("transform", 1, glm::value_ptr(trans));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //glUseProgram(shaderProgram2);
        //glBindVertexArray(VAO[1]);
        // 3. now draw the object 
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        //

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mainShader.deleteShader();

    cleanUp();

    return 0;
}