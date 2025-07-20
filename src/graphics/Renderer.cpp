#include "Renderer.h"

#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Graphics/Camera.h"
#include "Graphics/GraphicsUtils.h"

#include <SDL3/SDL_log.h>

Renderer::Renderer(Camera* const camera)
    : RendererBase(camera)
{
    _cubeRenderer.Init();

    _generalShader = GraphicsUtils::LoadShader("basic.vert", "basic.frag");
    if (_generalShader == 0)
    {
        SDL_Log("Failed to load shader program");
    }
}

void Renderer::RenderPlane()
{
    glDepthMask(GL_FALSE);
    glUseProgram(_generalShader);

    glm::mat4 view = _camera->GetViewMatrix();
    glm::mat4 proj = _camera->GetProjectionMatrix();

    GLint locMVP = glGetUniformLocation(_generalShader, "uMVP");
    GLint locColor = glGetUniformLocation(_generalShader, "uColor");

    float scalevalue = 5.f;
    glm::vec3 scale = glm::vec3(scalevalue, scalevalue, 1.f);
    glm::vec3 center = glm::vec3(10.f, 0.f, 0.f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
    model *= glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    model = glm::scale(model, scale);
    glm::mat4 mvp = proj * view * model;
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &mvp[0][0]);
    glUniform4f(locColor, 0, 0, 0, 1.f);
    _cubeRenderer.Draw();

    glDepthMask(GL_TRUE);
}