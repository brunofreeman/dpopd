#include <GL/glew.h>
#include <vector>
#include <array>
#include <mapbox/earcut.hpp>
#include "renderer.hpp"

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

void draw(GraphicsObject* go, const Shader& shader) {
    shader.bind();
    go->bind();
    set_vb_data(go->positions, go->positions_s * sizeof(float));
    glDrawElements(GL_TRIANGLES, go->ib->count, GL_UNSIGNED_INT, nullptr);
}

void set_color(Shader& shader, const Color& color) {
    shader.bind();
    shader.set_uniform_4f("u_Color", color.r, color.g, color.b, color.a);
}

float window_scale(const double xy, const double scale, const double environment_width_height,
                   const int screen_width_height) {
    return scale * (2 * xy - environment_width_height) / screen_width_height;
}

void refresh_environment_positions(GraphicsObject* go, Environment* environment, const int screen_width,
                                   const int screen_height,
                                   const float padding) {
    double x_scale = (1 - 2 * padding) * screen_width / environment->width;
    double y_scale = (1 - 2 * padding) * screen_height / environment->height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    size_t index = 0;

    Polygon* border = environment->border;
    for (auto& vertex : border->vertices) {
        float x = vertex.x;
        float y = vertex.y;
        go->positions[index++] = window_scale(x, scale, environment->width, screen_width);
        go->positions[index++] = window_scale(y, scale, environment->height, screen_height);
    }

    for (auto& obstacle : environment->obstacles) {
        for (auto& vertex : obstacle->vertices) {
            float x = vertex.x;
            float y = vertex.y;
            go->positions[index++] = window_scale(x, scale, environment->width, screen_width);
            go->positions[index++] = window_scale(y, scale, environment->height, screen_height);
        }
    }
}

void
refresh_polygon_positions(GraphicsObject* go, Environment* environment, const int screen_width, const int screen_height,
                          const float padding) {
    double x_scale = (1 - 2 * padding) * screen_width / environment->width;
    double y_scale = (1 - 2 * padding) * screen_height / environment->height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    size_t index = 0;

    auto* polygon = (Polygon*) go->obj;
    for (auto& vertex : polygon->vertices) {
        float x = vertex.x;
        float y = vertex.y;
        go->positions[index++] = window_scale(x, scale, environment->width, screen_width);
        go->positions[index++] = window_scale(y, scale, environment->height, screen_height);
    }
}

GraphicsObject*
environment_graphics_object(Environment* environment, const int screen_width, const int screen_height,
                            const float padding) {
    std::vector<std::vector<Point>> environment_polygon;
    std::vector<Point> border_vertices;
    std::vector<Point> obstacles_vertices[environment->obstacles.size()];

    size_t positions_s = 0;

    auto new_vertex = [](const Polygon* src, std::vector<Point>& dest, const size_t i, size_t& positions_s) {
        dest.push_back((Point) {(float) src->vertices[i].x, (float) src->vertices[i].y});
        positions_s += 2;
    };

    for (size_t i = 0; i < environment->border->vertices.size(); i++) {
        new_vertex(environment->border, border_vertices, i, positions_s);
    }

    for (size_t i = 0; i < environment->obstacles.size(); i++) {
        for (size_t j = 0; j < environment->obstacles[i]->vertices.size(); j++) {
            new_vertex(environment->obstacles[i], obstacles_vertices[i], j, positions_s);
        }
    }

    environment_polygon.push_back(border_vertices);
    for (size_t i = 0; i < environment->obstacles.size(); i++) {
        environment_polygon.push_back(obstacles_vertices[i]);
    }

    // triangulates the polygon with external library
    std::vector<N> indices = mapbox::earcut<N>(environment_polygon);

    size_t indices_s = indices.size();

    auto* positions_heap = new float[positions_s];
    auto* indices_heap = new unsigned int[indices_s];

    std::copy(indices.begin(), indices.end(), indices_heap);

    auto* va = new VertexArray();
    auto* vb = new VertexBuffer(positions_heap, positions_s * sizeof(float));

    auto* vbl = new VertexBufferLayout();
    vbl->push_f(2); // two floats per vertex: (x, y)
    va->add_buffer(vb, vbl);

    auto* ib = new IndexBuffer(indices_heap, indices_s);

    auto* ego = new GraphicsObject(environment, va, vb, vbl, ib, positions_heap, indices_heap, positions_s,
                                   indices_s);
    refresh_environment_positions(ego, environment, screen_width, screen_height, padding);
    return ego;
}

GraphicsObject*
agent_graphics_object(Agent* agent, Environment* environment, const int screen_width, const int screen_height,
                      const float padding) {
    std::vector<std::vector<Point>> agent_polygon;
    std::vector<Point> agent_vertices;

    for (size_t i = 0; i < Agent::shape_sides; i++) {
        Vector vertex = agent->shape->vertices[i];
        agent_vertices.push_back((Point) {(float) vertex.x, (float) vertex.y});
    }

    agent_polygon.push_back(agent_vertices);

    // triangulates the polygon with external library
    std::vector<N> indices = mapbox::earcut<N>(agent_polygon);

    size_t positions_s = 2 * Agent::shape_sides;
    size_t indices_s = indices.size();

    auto* positions_heap = new float[positions_s];
    auto* indices_heap = new unsigned int[indices_s];

    std::copy(indices.begin(), indices.end(), indices_heap);

    auto* va = new VertexArray();
    auto* vb = new VertexBuffer(positions_heap, positions_s * sizeof(float));

    auto* vbl = new VertexBufferLayout();
    vbl->push_f(2); // two floats per vertex: (x, y)
    va->add_buffer(vb, vbl);

    auto* ib = new IndexBuffer(indices_heap, indices_s);

    auto* ago = new GraphicsObject(agent->shape, va, vb, vbl, ib, positions_heap, indices_heap, positions_s,
                                   indices_s);
    refresh_polygon_positions(ago, environment, screen_width, screen_height, padding);
    return ago;
}