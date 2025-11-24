#pragma once

#include <glad/glad.h>

class Texture
{
public:
    Texture() = delete;
    Texture(const char *imagePath);
    ~Texture();

    void use(int index) const;

private:
    GLuint _id;
};
