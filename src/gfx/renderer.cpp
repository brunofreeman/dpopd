#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <array>
#include <mapbox/earcut.hpp>
#include "renderer.hpp"

using Coord = float;
using N = uint16_t;
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

RenderingBundle environment_rb(const Environment& environment) {
    std::vector<float> positions;
    std::vector<std::vector<Point>> environment_polygon;
    std::vector<Point> border_vertices;
    std::vector<std::vector<Point>> obstacles_vertices;

    for (size_t i = 0; i < (*(environment.border)).vertices_s; i++) {
        border_vertices.push_back((Point){
            (float)(*(environment.border)).vertices[i].x,
            (float)(*(environment.border)).vertices[i].y
        });
    }

    for (size_t i = 0; i < environment.obstacles_s; i++) {
        for (size_t j = 0; j < (*environment.obstacles)[i].vertices_s; j++) {
            obstacles_vertices[i].push_back((Point){
                (float)(*environment.obstacles)[i].vertices[j].x,
                (float)(*environment.obstacles)[i].vertices[j].y
            });
        }
    }

    environment_polygon.push_back(border_vertices);
    for (size_t i = 0; i < environment.obstacles_s; i++) {
        environment_polygon.push_back(obstacles_vertices[i]);
    }
    std::vector<N> indices = mapbox::earcut<N>(environment_polygon);
    // &indices[0];
}