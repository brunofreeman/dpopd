#include <fstream>
#include <nlohmann/json.hpp>
#include "json_wrapper.hpp"

nlohmann::json open_json(std::string filepath) {
    std::ifstream in(filepath);
    nlohmann::json json;
    in >> json;
    return json;
}

Polygon* json_polygon(nlohmann::json json_polygon) {
    size_t vertices_s = json_polygon.size();
    Vector* vertices = new Vector[vertices_s];

    for (size_t i = 0; i < vertices_s; i++) {
        Vector vertex(json_polygon[i][0], json_polygon[i][1]);
        vertices[i] = vertex;
    }

    return new Polygon(vertices, vertices_s);
}

Environment* json_environment(std::string filename) {
    nlohmann::json json_env = open_json(ENV_PATH + filename + ".json");

    Polygon* border = json_polygon(json_env[JSON_ID_BORDER]);

    size_t obstacles_s = json_env[JSON_ID_OBSTACLES].size();
    Polygon** obstacles = new Polygon* [obstacles_s];
    for (size_t i = 0; i < obstacles_s; i++) {
        Polygon* obstacle = json_polygon(json_env[JSON_ID_OBSTACLES][i]);
        obstacles[i] = obstacle;
    }

    double min_x = border->vertices[0].x;
    double min_y = border->vertices[0].y;

    for (size_t i = 1; i < border->vertices_s; i++) {
        float x = border->vertices[i].x;
        float y = border->vertices[i].y;
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
    }

    double scale = json_env[JSON_ID_SCALE];

#define NORM_AND_SCALE(src) src.x -= min_x; src.y -= min_y; src.x *= scale; src.y *= scale;

    for (size_t i = 0; i < border->vertices_s; i++) {
        NORM_AND_SCALE(border->vertices[i]);
    }

    for (size_t i = 0; i < obstacles_s; i++) {
        for (size_t j = 0; j < obstacles[i]->vertices_s; j++) {
            NORM_AND_SCALE(obstacles[i]->vertices[j]);
        }
    }

    return new Environment(border, obstacles, obstacles_s);
}