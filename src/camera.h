#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>

class Camera {
public:
    Camera()
    {
        updateViewMatrix();
        updateProjectionMatrix();

    }
    void setFOV(float fov, bool b_updateProjection = true);
    void setAR(float ar, bool b_updateProjection = true);

    glm::vec3 getPos() const { return m_pos; }
    void setPos(glm::vec3 pos, bool b_updateView = true);
    void offsetPos(glm::vec3 delta_pos, bool b_updateView = true) { setPos(m_pos + delta_pos, b_updateView); }

    glm::vec3 getAim() const { return m_aim; }
    void setAim(glm::vec3 aim, bool b_updateView = true);
    void offsetAim(glm::vec3 delta_aim, bool b_updateView = true) { setAim(m_pos + delta_aim, b_updateView); }

    glm::vec3 getUp() const { return m_up; }
    void setUp(glm::vec3 up, bool b_updateView = true);

    void updateProjectionMatrix();
    void updateViewMatrix();

    const glm::mat4& getView() { return m_view; };
    const glm::mat4& getProjection() { return m_projection; };

private:
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::vec3 m_pos = { 0.f, -3.f, 0.f }, m_aim = { 0.f, 0.f, 0.f }, m_up = { 0.f, 0.f, 1.f };
    float m_near = .1f, m_far = 2000.f;
    float m_fov = 1.f /*radians*/, m_ar = 1.f;
};

#endif // CAMERA_H
