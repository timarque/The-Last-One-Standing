
#ifndef BULLETOPENGL_BULLETOPENGLAPPLICATION_H
#define BULLETOPENGL_BULLETOPENGLAPPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

struct Types {
    inline btVector3 glm2bt(const glm::vec3 &vec) { return {vec.x, vec.y, vec.z}; }
    inline glm::vec3 bt2glm(const btVector3 &vec) { return {vec.x, vec.y, vec.z}; }
};

class BulletOpenGLApplication
{
public:
    BulletOpenGLApplication();
    ~BulletOpenGLApplication();
    void Initialize();
    // FreeGLUT callbacks //
    virtual void Keyboard(unsigned char key, int x, int y);
    virtual void KeyboardUp(unsigned char key, int x, int y);
    virtual void Special(int key, int x, int y);
    virtual void SpecialUp(int key, int x, int y);
    virtual void Reshape(int w, int h);
    virtual void Idle();
    virtual void Mouse(int button, int state, int x, int y);
    virtual void PassiveMotion(int x, int y);
    virtual void Motion(int x, int y);
    virtual void Display();

    // physics functions. Can be overriden by derived classes (like BasicDemo)
    virtual void InitializePhysics(){};
    virtual void ShutdownPhysics(){};

    // camera functions
    void UpdateCamera();
    void RotateCamera(float &angle, float value);
    void ZoomCamera(float distance);

    // drawing functions
    void DrawBox(const glm::vec3 &halfSize, const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f));

protected:
    // camera control
    glm::vec3 m_cameraPosition; // the camera's current position
    glm::vec3 m_cameraTarget;   // the camera's lookAt target
    float m_nearPlane;          // minimum distance the camera will render
    float m_farPlane;           // farthest distance the camera will render
    glm::vec3 m_upVector;       // keeps the camera rotated correctly
    float m_cameraDistance;     // distance from the camera to its target
    float m_cameraPitch;        // pitch of the camera
    float m_cameraYaw;          // yaw of the camera

    int m_screenWidth;
    int m_screenHeight;

    // core Bullet components
    btBroadphaseInterface *m_pBroadphase;
    btCollisionConfiguration *m_pCollisionConfiguration;
    btCollisionDispatcher *m_pDispatcher;
    btConstraintSolver *m_pSolver;
    btDynamicsWorld *m_pWorld;
};

#endif // BULLETOPENGL_BULLETOPENGLAPPLICATION_H