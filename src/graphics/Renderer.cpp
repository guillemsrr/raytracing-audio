#include "Renderer.h"
#include <algorithm>
#include <vector>

#include "graphics/Camera.h"
#include "Graphics/GraphicsUtils.h"
#include "graphics/Shader.h"

#include "raytracing/Ray.h"

#include <SDL3/SDL_log.h>

Renderer::Renderer(SDL_Window* window, Camera* const camera)
    : RendererBase(camera), _window(window)
{
    _screenQuadRenderer.Init();
    _cubeRenderer.Init();

    _raytracingShader = Shader();
    _raytracingShader.LoadVertexFragment("simple.vert", "simple.frag");
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

    glGenTextures(1, &_raytracedTexture);
    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

        glViewport(0, 0, screenWidth, screenHeight);
        glBindTexture(GL_TEXTURE_2D, _raytracedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        _raytracingTextureVector = std::vector<unsigned char>(screenWidth * screenHeight * bytes_per_pixel);
    }

    //GenerateRGBImage();
    //GenerateBackground();
    ShootRays();

    _raytracingShader.Use();

    glBindTexture(GL_TEXTURE_2D, _raytracedTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // required if row pitch is not aligned to 4 bytes

    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    screenWidth,
                    screenHeight,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    _raytracingTextureVector.data());

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_raytracingShader.GetID(), "uTexture"), 0);

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

            int index = (y * _screenWidth + x);
            color color(r, g, b);
            write_color(color, index);
        }
    }
}

void Renderer::GenerateBackground()
{
    auto aspect_ratio = static_cast<float>(_screenWidth) / static_cast<float>(_screenHeight);
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;
    auto camera_center = _camera->GetPosition();

    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);
    auto pixel_delta_u = viewport_u / static_cast<float>(_screenWidth);
    auto pixel_delta_v = viewport_v / static_cast<float>(_screenHeight);

    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2.f - viewport_v / 2.f;
    auto pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    for (int j = 0; j < _screenHeight; j++)
    {
        for (int i = 0; i < _screenWidth; i++)
        {
            auto pixel_center = pixel00_loc + (static_cast<float>(i) * pixel_delta_u) + (static_cast<float>(j) *
                pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray ray(camera_center, ray_direction);

            color pixel_color = ray_color(ray);
            int index = (j * _screenWidth + i);
            write_color(pixel_color, index);
        }
    }
}

void Renderer::ShootRays()
{
    float aspect = _camera->GetAspectRatio();
    float fov_rad = _camera->GetFOVRad();
    float viewport_height = 2.0f * tan(fov_rad / 2.0f);
    float viewport_width = viewport_height * aspect;

    glm::vec3 cam_pos = _camera->GetPosition();
    glm::vec3 horizontal = viewport_width * _camera->GetRight();
    glm::vec3 vertical = viewport_height * _camera->GetUp();
    glm::vec3 lower_left_corner = cam_pos + _camera->GetForward() - horizontal * 0.5f - vertical * 0.5f;

    for (int j = 0; j < _screenHeight; ++j)
    {
        for (int i = 0; i < _screenWidth; ++i)
        {
            float u = static_cast<float>(i) / (_screenWidth - 1);
            float v = static_cast<float>(j) / (_screenHeight - 1);

            glm::vec3 pixel_pos = lower_left_corner + u * horizontal + v * vertical;
            glm::vec3 ray_dir = glm::normalize(pixel_pos - cam_pos);

            Ray ray(cam_pos, ray_dir);
            color pixel_color = ray_color(ray);

            int index = j * _screenWidth + i;
            write_color(pixel_color, index);
        }
    }
}

void Renderer::write_color(const color& pixel_color, int index)
{
    float r = std::clamp(pixel_color.x, 0.0f, 1.0f);
    float g = std::clamp(pixel_color.y, 0.0f, 1.0f);
    float b = std::clamp(pixel_color.z, 0.0f, 1.0f);

    index *= bytes_per_pixel;
    _raytracingTextureVector[index + 0] = static_cast<unsigned char>(255.999f * r);
    _raytracingTextureVector[index + 1] = static_cast<unsigned char>(255.999f * g);
    _raytracingTextureVector[index + 2] = static_cast<unsigned char>(255.999f * b);
}

color Renderer::ray_color(const Ray& r)
{
    point3 sphereCenter(0, 0, 0);
    auto t = hit_sphere(sphereCenter, 0.1, r);
    if (t > 0.f)
    {
        vec3 N = glm::normalize(r.at(t) - sphereCenter);
        return 0.5f * color(N.x + 1, N.y + 1, N.z + 1);
    }

    vec3 unit_direction = glm::normalize(r.direction());
    float a = 0.5f * (unit_direction.y + 1.f);
    color backgroundColor(0.1f, 0.2f, 0.3f);
    color white(1.0, 1.0, 1.0);

    return glm::mix(white, backgroundColor, a);
}

double Renderer::hit_sphere(const point3& center, double radius, const Ray& r)
{
    vec3 oc = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    auto b = -2.0 * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return -1.f;
    }

    return (-b - std::sqrt(discriminant)) / (2.0 * a);
}