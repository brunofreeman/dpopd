#include <iostream>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"

int make_window();

int main(int argc, char **argv) {
    Environment *environment = json_environment("two_sqr_in_sqr");

    std::cout << (*environment).to_string() << "\n";

    delete environment;

    make_window();

    return 0;
}

int make_window() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}