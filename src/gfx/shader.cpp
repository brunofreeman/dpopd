#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "shader.hpp"
#include "renderer.hpp"

Shader::Shader(const std::string& filepath) : filepath(filepath), id(0) {
    this->filepath = filepath;
    ShaderSource shader_src = this->parse(filepath);
    this->id = this->create(shader_src.vertex_src, shader_src.fragment_src);
}

Shader::~Shader() {
    glDeleteProgram(this->id);
}

void Shader::bind() const {
    glUseProgram(this->id);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::set_uniform_4f(const std::string& name, float f0, float f1, float f2, float f3) {
    glUniform4f(this->get_uniform_location(name), f0, f1, f2, f3);
}

unsigned int Shader::create(const std::string& vertex_shader_src, const std::string& fragment_shader_src) {
    unsigned int program_id = glCreateProgram();
    unsigned int vertex_shader_id = this->compile(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader_id = this->compile(GL_FRAGMENT_SHADER, fragment_shader_src);

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    glValidateProgram(program_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

unsigned int Shader::compile(unsigned int type, const std::string& shader_src) {
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

ShaderSource Shader::parse(const std::string& filepath) {
    std::ifstream file_stream(this->filepath);

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

unsigned int Shader::get_uniform_location(const std::string& name) {
    if (this->uniform_location_cache.find(name) == this->uniform_location_cache.end()) {
        int location = glGetUniformLocation(this->id, name.c_str());
        uniform_location_cache[name] = location;
    }
    return this->uniform_location_cache[name];
}
