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
    Polygon** obstacles = new Polygon*[obstacles_s];
    for (size_t i = 0; i < obstacles_s; i++) {
        Polygon* obstacle = json_polygon(json_env[JSON_ID_OBSTACLES][i]);
        obstacles[i] = obstacle;
    }

    return new Environment(border, obstacles, obstacles_s);
}