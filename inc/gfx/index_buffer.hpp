#ifndef __INDEX_BUFFER_HPP__
#define __INDEX_BUFFER_HPP__

#include <cstdlib>

class IndexBuffer {
public:
    unsigned int id;
    size_t count;

    IndexBuffer(const void* data, size_t count);

    ~IndexBuffer();

    void bind() const;

    void unbind() const;
};

#endif  // __INDEX_BUFFER_HPP__