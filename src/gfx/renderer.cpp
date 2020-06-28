#include <GL/glew.h>
#include <vector>
#include <array>
#include <mapbox/earcut.hpp>
#include "renderer.hpp"
#include "graphics_util.hpp"

#include <iostream>

using Coord = float;
using N = unsigned int;
using Point = std::array<Coord, 2>;

void clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) {
    shader.bind();
    va.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, ib.count, GL_UNSIGNED_INT, nullptr);
}

void draw(const GraphicsObject& obj, const Shader& shader) {
    draw(*obj.va, *obj.ib, shader);
}

GraphicsObject environment_graphics_object(Environment* environment, int screen_width, int screen_height, float padding) {
    std::vector<float> positions;
    double x_scale = (1 - 2 * padding) * screen_width / (*environment).width;
    double y_scale = (1 - 2 * padding) * screen_height / (*environment).height;
    double scale = x_scale < y_scale ? x_scale : y_scale;


    std::vector<std::vector<Point>> environment_polygon;
    std::vector<Point> border_vertices;
    std::vector<Point> obstacles_vertices[(*environment).obstacles_s];

    #define PUSH_X positions.push_back(x / 1000)//positions.push_back((scale * x + screen_width) / screen_width - 1)
    #define PUSH_Y positions.push_back(y / 1000)//positions.push_back((scale * y + screen_height) / screen_height - 1)

    for (size_t i = 0; i < (*((*environment).border)).vertices_s; i++) {
        float x = (*((*environment).border)).vertices[i].x;
        float y = (*((*environment).border)).vertices[i].y;
        border_vertices.push_back((Point){x, y});
        PUSH_X; PUSH_Y;
    }
    
    for (size_t i = 0; i < (*environment).obstacles_s; i++) {
        for (size_t j = 0; j < (*((*environment).obstacles[i])).vertices_s; j++) {
            float x = (*((*environment).obstacles[i])).vertices[j].x;
            float y = (*((*environment).obstacles[i])).vertices[j].y;
            obstacles_vertices[i].push_back((Point){x, y});
            PUSH_X; PUSH_Y;
        }
    }

    environment_polygon.push_back(border_vertices);
    for (size_t i = 0; i < (*environment).obstacles_s; i++) {
        environment_polygon.push_back(obstacles_vertices[i]);
    }

    std::vector<N> indices = mapbox::earcut<N>(environment_polygon);

    /* std::cout << "positions:\n[";
    for (auto i = positions.begin(); i != positions.end(); ++i)
        std::cout << *i << ", ";

    std::cout << "]\nindices:\n[";
    for (auto i = indices.begin(); i != indices.end(); ++i)
        std::cout << *i << ", ";
    std::cout << "]" << std::endl; */

    size_t positions_s = positions.size();
    size_t indices_s = indices.size();

    float* positions_heap = new float[positions_s];
    unsigned int* indices_heap = new unsigned int[indices_s];

    std::copy(positions.begin(), positions.end(), positions_heap);
    std::copy(indices.begin(), indices.end(), indices_heap);
    
    VertexArray* va = new VertexArray();
    VertexBuffer* vb = new VertexBuffer(positions_heap, positions_s * sizeof(float));

    VertexBufferLayout* vbl = new VertexBufferLayout();
    (*vbl).push_f(2);
    (*va).add_buffer(*vb, *vbl);

    IndexBuffer* ib = new IndexBuffer(indices_heap, indices_s);

    return GraphicsObject(va, vb, vbl, ib, positions_heap, indices_heap, positions_s, indices_s);
}