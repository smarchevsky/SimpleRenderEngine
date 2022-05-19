#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::setFOV(float fov, bool b_updateProjection)
{
    m_fov = fov;
    if (b_updateProjection)
        updateProjectionMatrix();
}

void Camera::setAR(float ar, bool b_updateProjection)
{
    m_ar = ar;
    if (b_updateProjection)
        updateProjectionMatrix();
}

void Camera::setPos(glm::vec3 pos, bool b_updateView)
{
    m_pos = pos;
    if (b_updateView)
        updateViewMatrix();
}

void Camera::setAim(glm::vec3 aim, bool b_updateView)
{
    m_aim = aim;
    if (b_updateView)
        updateViewMatrix();
}

void Camera::setUp(glm::vec3 up, bool b_updateView)
{
    m_up = up;
    if (b_updateView)
        updateViewMatrix();
}

float Camera::getDistance() const
{
    return glm::distance(m_pos, m_aim);
}

void Camera::setDistance(float newDistance, bool offsetAim, bool b_updateView)
{
    auto offset = m_pos - m_aim;
    auto dir = offset / glm::length(offset);

    m_pos = m_aim + dir * newDistance;
    if (b_updateView)
        updateViewMatrix();
}

void Camera::updateProjectionMatrix()
{
    m_projection = glm::perspective(m_fov, m_ar, m_near, m_far);
}

void Camera::updateViewMatrix()
{
    m_view = glm::lookAt(m_pos, m_aim, m_up);
}
