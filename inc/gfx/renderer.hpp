#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "graphics_object.hpp"
#include "shader.hpp"
#include "environment.hpp"
#include "color.hpp"
#include "agent.hpp"

void clear();
void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
void draw(GraphicsObject* go, const Shader& shader);
void set_color(Shader& shader, const Color& color);
GraphicsObject* environment_graphics_object(Environment* environment, int screen_width, int screen_height, float padding);
GraphicsObject* agent_graphics_object(Agent* agent, Environment* environment, int screen_width, int screen_height, float padding);
void refresh_environment_positions(GraphicsObject* ego, Environment* environment, int screen_width, int screen_height, float padding);
void refresh_polygon_positions(GraphicsObject* pgo, Environment* environment, int screen_width, int screen_height, float padding);

#endif  // __RENDERER_HPP__