#include <GL/glew.h>
#include <vector>
#include <array>
#include <mapbox/earcut.hpp>
#include "renderer.hpp"
#include "graphics_util.hpp"

#define SET_X go->positions[index++] = scale * (2 * x - environment->width) / screen_width
#define SET_Y go->positions[index++] = scale * (2 * y - environment->height) / screen_height

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

void scale_environment_positions(GraphicsObject* go, Environment* environment, int screen_width, int screen_height, float padding) {
    double x_scale = (1 - 2 * padding) * screen_width / environment->width;
    double y_scale = (1 - 2 * padding) * screen_height / environment->height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    size_t index = 0;
    
    Polygon* border = environment->border;
    for (size_t i = 0; i < border->vertices_s; i++) {
        float x = border->vertices[i].x;
        float y = border->vertices[i].y;
        SET_X; SET_Y;
    }
    
    for (size_t i = 0; i < environment->obstacles_s; i++) {
        Polygon* obstacle = environment->obstacles[i];
        for (size_t j = 0; j < obstacle->vertices_s; j++) {
            float x = obstacle->vertices[j].x;
            float y = obstacle->vertices[j].y;
            SET_X; SET_Y;
        }
    }
}

void scale_polygon_positions(GraphicsObject* go, Environment* environment, int screen_width, int screen_height, float padding) {
    double x_scale = (1 - 2 * padding) * screen_width / environment->width;
    double y_scale = (1 - 2 * padding) * screen_height / environment->height;
    double scale = x_scale < y_scale ? x_scale : y_scale;

    size_t index = 0;

    Polygon* polygon = (Polygon*)go->obj;
    for (size_t i = 0; i < polygon->vertices_s; i++) {
        float x = polygon->vertices[i].x;
        float y = polygon->vertices[i].y;
        SET_X; SET_Y;
    }
}

GraphicsObject* environment_graphics_object(Environment* environment, int screen_width, int screen_height, float padding) {
    std::vector<std::vector<Point>> environment_polygon;
    std::vector<Point> border_vertices;
    std::vector<Point> obstacles_vertices[environment->obstacles_s];

    size_t positions_s = 0;

    #define NEW_VERTEX(src, dest) dest.push_back((Point){(float)src->vertices[i].x, (float)src->vertices[i].y}); positions_s += 2;

    for (size_t i = 0; i < environment->border->vertices_s; i++) {
        NEW_VERTEX(environment->border, border_vertices);
    }
    
    for (size_t j = 0; j < environment->obstacles_s; j++) {
        for (size_t i = 0; i < environment->obstacles[j]->vertices_s; i++) {
            NEW_VERTEX(environment->obstacles[j], obstacles_vertices[j])
        }
    }

    environment_polygon.push_back(border_vertices);
    for (size_t i = 0; i < environment->obstacles_s; i++) {
        environment_polygon.push_back(obstacles_vertices[i]);
    }

    std::vector<N> indices = mapbox::earcut<N>(environment_polygon);

    size_t indices_s = indices.size();

    float* positions_heap = new float[positions_s];
    unsigned int* indices_heap = new unsigned int[indices_s];

    std::copy(indices.begin(), indices.end(), indices_heap);
    
    VertexArray* va = new VertexArray();
    VertexBuffer* vb = new VertexBuffer(positions_heap, positions_s * sizeof(float));

    VertexBufferLayout* vbl = new VertexBufferLayout();
    vbl->push_f(2); // two floats per vertex: (x, y)
    va->add_buffer(vb, vbl);

    IndexBuffer* ib = new IndexBuffer(indices_heap, indices_s);

    GraphicsObject* ego = new GraphicsObject(environment, va, vb, vbl, ib, positions_heap, indices_heap, positions_s, indices_s);
    scale_environment_positions(ego, environment, screen_width, screen_height, padding);
    return ego;
}

GraphicsObject* agent_graphics_object(Agent* agent, Environment* environment, int screen_width, int screen_height, float padding) {
    std::vector<std::vector<Point>> agent_polygon;
    std::vector<Point> agent_vertices;

    for (size_t i = 0; i < agent->shape_sides; i++) {
        Vector vertex = agent->shape->vertices[i];
        agent_vertices.push_back((Point){(float)vertex.x, (float)vertex.y});
    }

    agent_polygon.push_back(agent_vertices);

    std::vector<N> indices = mapbox::earcut<N>(agent_polygon);

    size_t positions_s = 2 * agent->shape_sides;
    size_t indices_s = indices.size();

    float* positions_heap = new float[positions_s];
    unsigned int* indices_heap = new unsigned int[indices_s];

    std::copy(indices.begin(), indices.end(), indices_heap);
    
    VertexArray* va = new VertexArray();
    VertexBuffer* vb = new VertexBuffer(positions_heap, positions_s * sizeof(float));

    VertexBufferLayout* vbl = new VertexBufferLayout();
    vbl->push_f(2); // two floats per vertex: (x, y)
    va->add_buffer(vb, vbl);

    IndexBuffer* ib = new IndexBuffer(indices_heap, indices_s);

    GraphicsObject* ago = new GraphicsObject(agent->shape, va, vb, vbl, ib, positions_heap, indices_heap, positions_s, indices_s);
    scale_polygon_positions(ago, environment, screen_width, screen_height, padding);
    return ago;
}