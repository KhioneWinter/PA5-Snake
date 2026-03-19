#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.hpp"

using namespace std; 

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Snake Game", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- INITIALIZE GAME ---
    Game game;
    game.start();

    // Timing variables


    // --- GAME LOOP ---
    while(!glfwWindowShouldClose(window))
    {
        // get input
        game.handleInput(window);

        // update logic
        game.update();

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black screen border
        glClear(GL_COLOR_BUFFER_BIT);
        game.render();

        glfwSwapBuffers(window);
        glfwPollEvents();    

        // slow down framerate
        // TODO fix delta time movement instead of fixed sleep
        glfwWaitEventsTimeout(0.25); // wait 500ms between frames
    }

    glfwTerminate();
    return 0;
}
