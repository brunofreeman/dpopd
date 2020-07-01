#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>
#include <unordered_map>

struct ShaderSource {
    std::string vertex_src;
    std::string fragment_src;
};

class Shader {
    public:
        unsigned int id;
        std::unordered_map<std::string, int> uniform_location_cache;
        Shader(const std::string& filepath);
        ~Shader();
        void bind() const;
        void unbind() const;
        void set_uniform_4f(const std::string& name, float f0, float f1, float f2, float f3);
    private:
        ShaderSource parse(const std::string& filepath);
        unsigned int create(const std::string& vertex_shader_src, const std::string& fragment_shader_src);
        unsigned int compile(unsigned int type, const std::string& shader_src);
        unsigned int get_uniform_location(const std::string& name);
};

#endif  // __SHADER_HPP__