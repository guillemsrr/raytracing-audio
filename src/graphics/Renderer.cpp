#include "Renderer.h"
#include <algorithm>
#include <vector>

#include "graphics/Camera.h"
#include "Graphics/GraphicsUtils.h"
#include "graphics/Shader.h"

#include "raytracing/Interval.h"
#include "objects/SphereObject.h"

#include "raytracing/Ray.h"
#include "raytracing/HitResult.h"
#include "scene/Scene.h"

#include "materials/Material.h"

#include <SDL3/SDL_log.h>

#include "utils/Utils.h"

Renderer::Renderer(SDL_Window* window, Camera* const camera)
    : RendererBase(camera), _window(window)
{
    _screenQuadRenderer.Init();
    _cubeRenderer.Init();

    _raytracingShader = Shader();
    _raytracingShader.LoadVertexFragment("simple.vert", "texture.frag");
    if (!_raytracingShader.IsCompiled())
    {
        SDL_Log("Failed to load _raytracingShader program");
    }

    _debugShader = Shader();
    _debugShader.LoadVertexFragment("default.vert", "default.frag");
    if (!_debugShader.IsCompiled())
    {
        SDL_Log("Failed to load _debugShader program");
    }

    // Create raytraced texture
    glGenTextures(1, &_raytracedTexture);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);

    // Set filtering and wrapping (necessary!)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _raytracingShader.Use();
    GLint _raytracingTextureUniform = glGetUniformLocation(_raytracingShader.GetID(), "uTexture");
    glUniform1i(_raytracingTextureUniform, 0);
}

void Renderer::SetScene(Scene& scene)
{
    _scene = &scene;
}

void Renderer::RenderRaytracing()
{
    int screenWidth, screenHeight;
    SDL_GetWindowSizeInPixels(_window, &screenWidth, &screenHeight);

    if (screenWidth <= 0 || screenHeight <= 0)
    {
        return;
    }

    if (screenWidth != _screenWidth || screenHeight != _screenHeight)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;

        glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        _raytracingTextureVector = std::vector<uint8_t>(screenWidth * screenHeight * bytes_per_pixel);
    }

    RayTraceScreen();

    // Upload texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    _screenWidth,
                    _screenHeight,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    _raytracingTextureVector.data());

    // Bind and draw
    glBindTextureUnit(0, _raytracedTexture);
    _raytracingShader.Use();
    _screenQuadRenderer.Draw();
}

void Renderer::RenderDebug()
{
    _debugShader.Use();

    glm::mat4 view = _camera->GetViewMatrix();
    glm::mat4 proj = _camera->GetProjectionMatrix();

    glm::vec4 debugColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.1f);

    GLint locMVP = glGetUniformLocation(_debugShader.GetID(), "uMVP");
    GLint locColor = glGetUniformLocation(_debugShader.GetID(), "uColor");
    glUniform4f(locColor, debugColor.r, debugColor.g, debugColor.b, debugColor.a);

    glm::vec3 worldPos = glm::vec3();
    glm::mat4 model = glm::translate(glm::mat4(1.f), worldPos);
    model = glm::scale(model, glm::vec3(1.f));
    glm::mat4 mvp = proj * view * model;
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &mvp[0][0]);
    _cubeRenderer.Draw();
}

void Renderer::GenerateRGBImage()
{
    int colorGradientSize = 8;
    for (int y = 0; y < _screenHeight; y++)
    {
        for (int x = 0; x < _screenWidth; x++)
        {
            int blockX = x * colorGradientSize / _screenWidth;
            int blockY = y * colorGradientSize / _screenHeight;

            float r = float(blockX) / (colorGradientSize - 1);
            float g = float(blockY) / (colorGradientSize - 1);
            float b = 0.0f;

            int index = y * _screenWidth + x;
            color color(r, g, b);
            write_color(color, index);
        }
    }
}

void Renderer::RayTraceScreen()
{
    float aspect = _camera->GetAspectRatio();
    float fov_rad = _camera->GetFOVRad();
    float viewport_height = 2.0f * tan(fov_rad / 2.0f);
    float viewport_width = viewport_height * aspect;

    glm::vec3 horizontal = viewport_width * _camera->GetRight();
    glm::vec3 vertical = viewport_height * _camera->GetUp();
    glm::vec3 lower_left_corner = _camera->GetPosition() + _camera->GetForward() - horizontal * 0.5f - vertical * 0.5f;

    for (int j = 0; j < _screenHeight; j += _pixelSize)
    {
        float v = static_cast<float>(j) / (_screenHeight - 1);
        auto vVertical = v * vertical;
        for (int i = 0; i < _screenWidth; i += _pixelSize)
        {
            float u = static_cast<float>(i) / (_screenWidth - 1);

            glm::vec3 pixel_pos = lower_left_corner + u * horizontal + vVertical;
            color pixel_color = RayTracePixelColor(pixel_pos);
            write_color(pixel_color, i, j, _pixelSize);
        }
    }
}

color Renderer::RayTracePixelColor(glm::vec3 pixel_pos)
{
    glm::vec3 rayPosition = _camera->GetPosition();
    glm::vec3 ray_dir = glm::normalize(pixel_pos - rayPosition);

    color pixel_color = color(0.0f);
    float multiplier = 1.f;
    for (int b = 0; b < _bounces; ++b)
    {
        Ray ray(rayPosition, ray_dir);
        HitResult hit = _scene->HitAny(ray, Interval(0, FLT_MAX));
        //TODO: cache hit?
        if (hit.HasHit())
        {
            float dot = glm::dot(hit.normal, -_lightDir);
            dot = std::max(dot, 0.0f);
            pixel_color += glm::vec3(hit.ObjectHit->GetMaterial()->Albedo) * dot * multiplier;

            multiplier *= 0.75f;
            rayPosition = hit.p + hit.normal * 0.001f;
            float reflection = 0.25f;
            vec3 reflectionNormal = hit.normal + hit.ObjectHit->GetMaterial()->Roughness * Utils::RandomInRange(
                -reflection,
                reflection);
            ray_dir = glm::reflect(hit.direction, reflectionNormal);
            continue;
        }

        float a = 0.5f * (ray.direction().y + 1.f);
        color backgroundColor(0.1f, 0.2f, 0.3f);
        color white(1.0, 1.0, 1.0);

        pixel_color += glm::mix(white, backgroundColor, a);
        break;
    }

    return pixel_color;
}

void Renderer::write_color(const color& pixel_color, int i, int j, int pixelSize)
{
    for (int dy = 0; dy < pixelSize; ++dy)
    {
        for (int dx = 0; dx < pixelSize; ++dx)
        {
            int x = i + dx;
            int y = j + dy;

            int index = y * _screenWidth + x;
            write_color(pixel_color, index);
        }
    }
}

void Renderer::write_color(const color& pixel_color, int index)
{
    index *= bytes_per_pixel;

    if (index + 2 >= _raytracingTextureVector.size())
    {
        return;
    }

    _raytracingTextureVector[index + 0] = Utils::FloatToByte255(pixel_color.r);
    _raytracingTextureVector[index + 1] = Utils::FloatToByte255(pixel_color.g);
    _raytracingTextureVector[index + 2] = Utils::FloatToByte255(pixel_color.b);
}