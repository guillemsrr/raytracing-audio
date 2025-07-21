#include "Renderer.h"
#include <vector>

#include "graphics/Camera.h"
#include "Graphics/GraphicsUtils.h"
#include "graphics/Shader.h"

#include <SDL3/SDL_log.h>

Renderer::Renderer(SDL_Window* window, Camera* const camera)
    : RendererBase(camera), _window(window)
{
    _screenQuadRenderer.Init();

    _generalShader = Shader();
    _generalShader.LoadVertexFragment("basic.vert", "basic.frag");
    if (!_generalShader.IsCompiled())
    {
        SDL_Log("Failed to load _generalShader program");
    }

    _raytracingShader = Shader();
    _raytracingShader.LoadVertexFragment("simple.vert", "simple.frag");
    if (!_raytracingShader.IsCompiled())
    {
        SDL_Log("Failed to load _raytracingShader program");
    }

    glGenTextures(1, &_raytracedTexture);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Renderer::RenderRaytracing()
{
    int screenWidth, screenHeight;
    SDL_GetWindowSizeInPixels(_window, &screenWidth, &screenHeight);

    if (screenWidth == 0 || screenHeight == 0)
    {
        return;
    }

    if (screenWidth != _lastScreenWidth || screenHeight != _lastScreenHeight)
    {
        _lastScreenWidth = screenWidth;
        _lastScreenHeight = screenHeight;

        glViewport(0, 0, screenWidth, screenHeight);
        glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }

    std::vector<unsigned char> image(screenWidth * screenHeight * 3);
    GenerateRGBImage(screenWidth, screenHeight, image);

    _raytracingShader.Use();

    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // required if row pitch is not aligned to 4 bytes

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, image.data());

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_raytracingShader.GetID(), "uTexture"), 0);

    _screenQuadRenderer.Draw();
}

void Renderer::GenerateRGBImage(int screenWidth, int screenHeight, std::vector<unsigned char>& image)
{
    int imagewidth = 8;
    int imageheight = 8;
    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            int blockX = x * imagewidth / screenWidth;
            int blockY = y * imageheight / screenHeight;

            float r = float(blockX) / (imagewidth - 1);
            float g = float(blockY) / (imageheight - 1);
            float b = 0.0f;

            int index = (y * screenWidth + x) * 3;
            image[index + 0] = static_cast<unsigned char>(255.f * r);
            image[index + 1] = static_cast<unsigned char>(255.f * g);
            image[index + 2] = static_cast<unsigned char>(255.f * b);
        }
    }
}