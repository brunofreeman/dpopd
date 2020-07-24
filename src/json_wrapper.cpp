#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "json_wrapper.hpp"

nlohmann::json open_json(std::string filepath) {
    std::ifstream in(filepath);
    nlohmann::json json;
    in >> json;
    return json;
}

Polygon* json_polygon(nlohmann::json json_polygon) {
    std::vector<Vector> vertices(json_polygon.size());

    for (size_t i = 0; i < vertices.size(); i++) {
        Vector vertex(json_polygon[i][0], json_polygon[i][1]);
        vertices[i] = vertex;
    }

    return new Polygon(vertices);
}

Environment* json_environment(const std::string& filename) {
    nlohmann::json json_env = open_json(ENV_PATH + filename + ".json");

    Polygon* border = json_polygon(json_env[ENV_ID_BORDER]);

    std::vector<Polygon*> obstacles(json_env[ENV_ID_OBSTACLES].size());
    for (size_t i = 0; i < obstacles.size(); i++) {
        Polygon* obstacle = json_polygon(json_env[ENV_ID_OBSTACLES][i]);
        obstacles[i] = obstacle;
    }

    double min_x = border->vertices[0].x;
    double min_y = border->vertices[0].y;

    for (size_t i = 1; i < border->vertices.size(); i++) {
        double x = border->vertices[i].x;
        double y = border->vertices[i].y;
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
    }

    double scale = json_env[ENV_ID_SCALE];

    auto norm_and_scale = [](Vector& vec, double min_x, double min_y, double scale) {
        vec.x -= min_x;
        vec.y -= min_y;
        vec.x *= scale;
        vec.y *= scale;
    };

    for (auto& vertex : border->vertices) {
        norm_and_scale(vertex, min_x, min_y, scale);
    }

    for (auto& obstacle : obstacles) {
        for (auto& vertex : obstacle->vertices) {
            norm_and_scale(vertex, min_x, min_y, scale);
        }
    }

    return new Environment(border, obstacles);
}

Model* json_model(const std::string& filename) {
    nlohmann::json json_mdl = open_json(MDL_PATH + filename + ".json");

    std::string envir_name = json_mdl[MDL_ID_ENVIRONMENT];
    std::string crowd_type = json_mdl[MDL_ID_CROWD_TYPE];
    double spacial_res = json_mdl[MDL_ID_SPACIAL_RES];
    double temporal_res = json_mdl[MDL_ID_TEMPORAL_RES];
    double updates_res = json_mdl[MDL_ID_UPDATE_RES];
    unsigned int updates_per_frame = json_mdl[MDL_ID_UPDATES_PER_FRAME];

    return new Model(envir_name, crowd_type, spacial_res, temporal_res, updates_res, updates_per_frame);
}