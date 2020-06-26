#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "json_wrapper.hpp"
#include "renderer.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

struct ShaderSource {
    std::string vertex_src;
    std::string fragment_src;
};

static int make_window();
static unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src);
static unsigned int compile_shader(unsigned int type, const std::string& shader_src);
static ShaderSource parse_shaders(const std::string& filepath);

int main(int argc, char** argv) {
    Environment* environment = json_environment("two_sqr_in_sqr");

    std::cout << (*environment).to_string() << std::endl;

    delete environment;

    make_window();

    return 0;
}

static ShaderSource parse_shaders(const std::string& filepath) {
    std::ifstream file_stream(filepath);

    enum class ShaderType {
        NONE     = -1,
        VERTEX   =  0,
        FRAGMENT =  1
    };

    std::string line;
    std::stringstream str_streams[2];
    ShaderType type = ShaderType::NONE;

    while (getline(file_stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            str_streams[(int)type] << line << '\n';
        }
    }

    return (ShaderSource){str_streams[0].str(), str_streams[1].str()};
}

static unsigned int compile_shader(unsigned int type, const std::string& shader_src) {
    unsigned int id = glCreateShader(type);
    const char* src = shader_src.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char message[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int create_shader(const std::string& vertex_shader_src, const std::string& fragment_shader_src) {
    unsigned int program_id = glCreateProgram();
    unsigned int vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    glValidateProgram(program_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

static int make_window() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    // 4.1 INTEL-14.6.18, could use lower #version if neccesary
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "dpo_pdf", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    print_gl_version();

    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f  // 3
    };

    unsigned int indices[] {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vertex_arr_id;
    glGenVertexArrays(1, &vertex_arr_id);
    glBindVertexArray(vertex_arr_id);

    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    IndexBuffer ib(indices, 6);

    ShaderSource shader_src = parse_shaders("src/gfx/_default.shader");

    unsigned int shader_id = create_shader(shader_src.vertex_src, shader_src.fragment_src);
    glUseProgram(shader_id);

    int u_color_location = glGetUniformLocation(shader_id, "u_Color");
    // ASSERT(location != -1);
    glUniform4f(u_color_location, 0.2f, 0.3f, 0.8f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_id);
        glUniform4f(u_color_location, 0.2f, 0.3f, 0.8f, 1.0f);


        glBindVertexArray(vertex_arr_id);
        ib.bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader_id);

    glfwTerminate();

    return 0;
}