#include "Renderer.h"
#include <SDL3/SDL_log.h>

Renderer::Renderer(SDL_Window* window, Camera* const camera)
    : RendererBase(camera), _window(window)
{
    _screenQuadRenderer.Init();
    _cubeRenderer.Init();

    _raytracingShader.LoadVertexFragment("simple.vert", "texture.frag");
    if (!_raytracingShader.IsCompiled())
    {
        SDL_Log("Failed to load raytracing shader program");
    }

    _debugShader.LoadVertexFragment("default.vert", "default.frag");
    if (!_debugShader.IsCompiled())
    {
        SDL_Log("Failed to load debug shader program");
    }

    glGenTextures(1, &_raytracedTexture);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _raytracingShader.Use();
    const GLint raytracingTextureUniform = glGetUniformLocation(_raytracingShader.GetID(), "uTexture");
    glUniform1i(raytracingTextureUniform, 0);
}

Renderer::~Renderer()
{
    if (_raytracedTexture != 0)
    {
        glDeleteTextures(1, &_raytracedTexture);
    }
}

void Renderer::RenderBackground()
{
    RendererBase::RenderBackground();
    glClear(GL_DEPTH_BUFFER_BIT);
}

bool Renderer::SyncWindowSize()
{
    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetWindowSizeInPixels(_window, &screenWidth, &screenHeight);

    if (screenWidth <= 0 || screenHeight <= 0)
    {
        return false;
    }

    if (screenWidth != _screenWidth || screenHeight != _screenHeight)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
        OnscreenResize();
        return true;
    }

    return false;
}

void Renderer::Present(const uint32_t* pixels, int width, int height)
{
    if (!pixels || width <= 0 || height <= 0)
    {
        return;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    width,
                    height,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    pixels);

    glDisable(GL_DEPTH_TEST);
    glBindTextureUnit(0, _raytracedTexture);
    _raytracingShader.Use();
    _screenQuadRenderer.Draw();
    glEnable(GL_DEPTH_TEST);
}

void Renderer::UpdateAndRender(IRaytracer* raytracer)
{
    if (!raytracer)
    {
        return;
    }

    SyncWindowSize();
    raytracer->Resize(_screenWidth, _screenHeight);
    raytracer->Render();

    Present(raytracer->GetColorBuffer(), raytracer->GetWidth(), raytracer->GetHeight());
}

void Renderer::OnscreenResize()
{
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _screenWidth, _screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void Renderer::RenderDebug()
{
    _debugShader.Use();

    const glm::mat4 view = _camera->GetViewMatrix();
    const glm::mat4 proj = _camera->GetProjectionMatrix();
    const color debugColor(0.0f, 0.0f, 0.0f, 0.1f);

    const GLint locMVP = glGetUniformLocation(_debugShader.GetID(), "uMVP");
    const GLint locColor = glGetUniformLocation(_debugShader.GetID(), "uColor");
    glUniform4f(locColor, debugColor.r, debugColor.g, debugColor.b, debugColor.a);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f));
    model = glm::scale(model, glm::vec3(1.0f));
    const glm::mat4 mvp = proj * view * model;
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &mvp[0][0]);
    _cubeRenderer.Draw();
}
