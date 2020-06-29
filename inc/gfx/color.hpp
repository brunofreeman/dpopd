#ifndef __COLOR_HPP__
#define __COLOR_HPP__

class Color {
    public:
        float r;
        float g;
        float b;
        float a;
        Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
        Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
        void set(float r, float g, float b, float a);
        void set(float r, float g, float b);
};

#endif // #ifndef __COLOR_HPP__