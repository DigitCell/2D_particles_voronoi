#include "Buffer.h"


Buffer::Buffer()
{

}

Buffer::Buffer(GLenum _target, GLenum _usage) : target(_target), usage(_usage)
{
    glGenBuffers(1, &id);
}

Buffer::Buffer(GLenum _target, GLenum _usage, size_t _size) : size(_size), target(_target), usage(_usage)
{
    glGenBuffers(1, &id);
    glBindBuffer(target, id);
    glBufferData(target, size, NULL, usage);
    glBindBuffer(target, 0);
}

Buffer::Buffer(GLenum _target, GLenum _usage, size_t _size, void* data) : size(_size), target(_target), usage(_usage)
{
    glGenBuffers(1, &id);
    glBindBuffer(target, id);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
}

void Buffer::setData(const void* data)
{
    setSubData(0, size, data);
}

void Buffer::setSubData(int offset, size_t _size, const void* data)
{
    glBindBuffer(target, id);
    glBufferSubData(target, offset, _size, data);
    glBindBuffer(target, 0);
}

void Buffer::clearSubData(int offset, size_t _size, const void *data)
{
    glBindBuffer(target, id);
    //glClearBufferSubData(target, GL_SHADER_STORAGE_BUFFER, offset, _size, data);
    glBindBuffer(target, 0);
}

void Buffer::getData(void* container)
{
    getSubData(0, size, container);
}

void Buffer::getSubData(int offset, size_t _size, void* container)
{
    glGetNamedBufferSubData(id, offset, _size, container);
}

void Buffer::setBindingPoint(int binding)
{
    glBindBufferBase(target, binding, id);
}

void Buffer::resize(size_t _size)
{
    resize(_size, NULL);
}

void Buffer::resize(size_t _size, void* data)
{
    glBindBuffer(target, id);
    glBufferData(target, _size, data, usage);
    glBindBuffer(target, 0);

    size = _size;
}

void* Buffer::map(GLenum access)
{
    return glMapNamedBuffer(id, access);
}

void* Buffer::map(int offset, size_t _size, GLenum access)
{
    return glMapNamedBufferRange(id, offset, _size, access);
}

void Buffer::unmap()
{
    glUnmapNamedBuffer(id);
}

void Buffer::copy(Buffer& source)
{
    GLint _size;

    glBindBuffer(GL_COPY_READ_BUFFER, source.id);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &_size);

    glBindBuffer(GL_COPY_WRITE_BUFFER, id);
    if((size_t)_size != size)
        glBufferData(GL_COPY_WRITE_BUFFER, _size, NULL, usage);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _size);

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    size = _size;
}

void Buffer::deleteBuffer()
{
    glDeleteBuffers(1, &id);
}

Buffer::~Buffer()
{

}
