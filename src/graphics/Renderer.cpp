#include "Renderer.h"
#include <algorithm>
#include <vector>

#include "graphics/Shader.h"
#include "graphics/Camera.h"

#include "raytracing/Interval.h"
#include "objects/SphereObject.h"

#include "raytracing/Ray.h"
#include "raytracing/HitResult.h"
#include "scene/Scene.h"

#include "materials/Material.h"

#include <SDL3/SDL_log.h>

#include "utils/Utils.h"
#include <execution>

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

    _camera->OnCameraMoved = [this]()
    {
        ResetFrameIndex();
    };
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

        OnscreenResize();
    }

    //GenerateRGBImage();
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
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    _raytracingTextureBuffer);

    // Bind and draw
    glBindTextureUnit(0, _raytracedTexture);
    _raytracingShader.Use();
    _screenQuadRenderer.Draw();

    _frameIndex++;
}

void Renderer::OnscreenResize()
{
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _screenWidth, _screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    delete[] _raytracingTextureBuffer;
    _raytracingTextureBufferSize = _screenWidth * _screenHeight;
    _raytracingTextureBuffer = new uint32_t[_raytracingTextureBufferSize];

    delete[] _accumulation;
    _accumulation = new glm::vec4[_raytracingTextureBufferSize];

    _pixelScreenHorizontalIterator.resize(_screenWidth);
    _pixelScreenVerticalIterator.resize(_screenHeight);

    for (int i = 0; i < _screenWidth; ++i)
    {
        _pixelScreenHorizontalIterator[i] = i;
    }

    for (int i = 0; i < _screenHeight; ++i)
    {
        _pixelScreenVerticalIterator[i] = i;
    }
    ResetFrameIndex();
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

            color color(r, g, b);
            write_color(color, x, y);
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

    if (_frameIndex == 1)
    {
        memset(_accumulation, 0, sizeof(glm::vec4) * _raytracingTextureBufferSize);
    }

    unsigned int numberCores = std::thread::hardware_concurrency();

    std::for_each(std::execution::par,
                  _pixelScreenVerticalIterator.begin(),
                  _pixelScreenVerticalIterator.end(),
                  [&](int j)
                  {
                      std::for_each(std::execution::par,
                                    _pixelScreenHorizontalIterator.begin(),
                                    _pixelScreenHorizontalIterator.end(),
                                    [&, j](int i)
                                    {
                                        float v = static_cast<float>(j) / (_screenHeight - 1);
                                        auto vVertical = v * vertical;
                                        float u = static_cast<float>(i) / (_screenWidth - 1);

                                        glm::vec3 pixel_pos = lower_left_corner + u * horizontal + vVertical;
                                        //pixel_pos = glm::vec3(i, j, 0.0f);
                                        glm::vec3 rayPosition = _camera->GetPosition();
                                        glm::vec3 ray_dir = glm::normalize(pixel_pos - rayPosition);
                                        Ray ray(rayPosition, ray_dir);
                                        color pixel_color = RayTracePixelColor(ray);

                                        int index = j * _screenWidth + i;
                                        _accumulation[index] += glm::vec4(pixel_color, 1.f);
                                        glm::vec4 accumulatedColor = _accumulation[index] / static_cast<float>(
                                            _frameIndex);

                                        accumulatedColor =
                                            glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.f));
                                        _raytracingTextureBuffer[index] = Utils::ConvertToRGBA(accumulatedColor);
                                    });
                  });
}

void Renderer::RayTraceScreen2()
{
    for (int j = 0; j < _screenHeight; ++j)
    {
        for (int i = 0; i < _screenWidth; ++i)
        {
            glm::vec2 pixel_pos(i, j);
            color pixel_color = RayTracePixelColor2(pixel_pos);
            write_color(pixel_color, i, j);
        }
    }
}

color Renderer::RayTracePixelColor(Ray ray)
{
    auto rayPosition = ray.origin();
    auto ray_dir = ray.direction();

    color pixel_color = color(0.0f);
    float multiplier = 1.f;
    for (int b = 0; b < _bounces; ++b)
    {
        ray = Ray(rayPosition, ray_dir);
        HitResult hit = _scene->HitAny(ray, Interval(0, FLT_MAX));
        //TODO: cache hit?
        if (hit.HasHit())
        {
            float dot = glm::dot(hit.normal, -_lightDir);
            dot = std::max(dot, 0.0f);
            pixel_color += glm::vec3(hit.ObjectHit->GetMaterial()->Albedo) * dot * multiplier;
            multiplier *= 0.75f;
            rayPosition = hit.p + hit.normal * 0.01f;
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

color Renderer::RayTracePixelColor2(glm::vec2 pixel_pos)
{
    glm::vec3 rayPosition = vec3(0.f); //_camera->GetPosition();
    glm::vec3 ray_dir = glm::normalize(vec3(pixel_pos, -1.f));
    Ray ray(rayPosition, ray_dir);
    return RayTracePixelColor(ray);
}

void Renderer::write_color(const color& pixel_color, int i, int j)
{
    int index = j * _screenWidth + i;

    glm::vec4 color = glm::vec4(pixel_color, 1.f);
    color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.f));
    _raytracingTextureBuffer[index] = Utils::ConvertToRGBA(color);
}

void Renderer::ResetFrameIndex()
{
    _frameIndex = 1;
}