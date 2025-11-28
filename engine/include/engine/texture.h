#pragma once

#include <glad/glad.h>
#include <string>

enum TextureType
{
    DIFFUSE,
    SPECULAR,
    EMISSION,
};

class Texture
{
public:
    Texture() = delete;
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(const std::string imagePath, TextureType type, bool wrap = true);
    Texture(Texture &&) noexcept;
    Texture &operator=(Texture &&) noexcept;
    ~Texture();

    TextureType type;

    void use(int index) const;

private:
    GLuint _id;
};
