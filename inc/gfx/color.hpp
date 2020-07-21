#ifndef __COLOR_HPP__
#define __COLOR_HPP__

/*
 * Equivalent to HTML color codes
 * Expressed as float list representing RGB
 * on a scale of [0.0, 1.0]
 */
#define RED_RGB 1.0f, 0.0f, 0.0f
#define CYAN_RGB 0.0f, 1.0f, 1.0f
#define BLUE_RGB 0.0f, 0.0f, 1.0f
#define DARK_BLUE_RGB 0.0f, 0.0f, 0.627451f,
#define LIGHT_BLUE_RGB 0.678431f, 0.847059f, 0.901961f
#define PURPLE_RGB 0.501961f, 0.0f, 0.501961f
#define YELLOW_RGB 1.0f, 1.0f, 0.0f
#define LIME_RGB 0.0f, 1.0f, 1.0f
#define MAGENTA_RGB 1.0f, 0.0f, 1.0f
#define WHITE_RGB 1.0f, 1.0f, 1.0f
#define SILVER_RGB 0.752941f, 0.752941f, 0.752941f
#define GREY_RGB 0.501961f, 0.501961f, 0.501961f
#define BLACK_RGB 0.0f, 0.0f, 0.0f
#define ORANGE_RGB 1.0f, 0.647059f, 0.0f
#define BROWN_RGB 0.647059f, 0.164706f, 0.164706f
#define MAROON_RGB 0.501961f, 0.0f, 0.0f
#define GREEN_RGB 0.0f, 0.501961f, 0.0f
#define OLIVE_RGB 0.501961f, 0.501961f, 0.0f

class Color {
public:
    float r; // red
    float g; // green
    float b; // blue
    float a; // alpha

    Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}

    void set(float r, float g, float b, float a);

    void set(float r, float g, float b);
};

#endif // #ifndef __COLOR_HPP__