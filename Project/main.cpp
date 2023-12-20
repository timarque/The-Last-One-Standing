#include<iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <map>

#include "../../Project/camera.h"
#include "../../Project/shader.h"
#include "../../Project/object.h"
#include "../../Project/cubemap.h"

void processInput(GLFWwindow* window);
void createOpenGLContext();
GLFWwindow *createOpenGLWindow();
void setupDebug();
void loadOpenGLFunctions();

#ifndef NDEBUG
void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char* message,
                            const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
#endif

Camera camera(glm::vec3(0.0, 0.0, 0.1));
const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 500;

int main(int argc, char* argv[]) {

    createOpenGLContext();
    GLFWwindow *window = createOpenGLWindow();
    loadOpenGLFunctions();
    setupDebug();

    Shader shader("../../Project/shaders/commonObjects/commonObjects.vert",
                  "../../Project/shaders/commonObjects/commonObjects.frag");

    Shader cubeMapShader("../../Project/shaders/skybox/skybox.vert",
                         "../../Project/shaders/skybox/skybox.frag");

    Shader reflectiveShader("../../Project/shaders/reflectiveObjects/reflectiveObjects.vert",
                            "../../Project/shaders/reflectiveObjects/reflectiveObjects.frag");

    Shader refractiveShader("../../Project/shaders/refractiveObjects/refractiveObjects.vert",
                            "../../Project/shaders/refractiveObjects/refractiveObjects.frag");

    Object sphere1("../../Project/objects/sphere_smooth.obj", &refractiveShader);
    sphere1.makeObject(refractiveShader);

    CubeMap cubeMap("../../Project/objects/cube.obj", &cubeMapShader);
    cubeMap.makeObject(cubeMapShader);
    // TODO : solve errors shown by the debugger

    double prev = 0;
    int deltaFrame = 0;
    //fps function
    auto fps = [&](double now) {
        double deltaTime = now - prev;
        deltaFrame++;
        if (deltaTime > 0.5) {
            prev = now;
            const double fpsCount = (double)deltaFrame / deltaTime;
            deltaFrame = 0;
            std::cout << "\r FPS: " << fpsCount;
            std::cout.flush();
        }
    };


    glm::vec3 light_pos = glm::vec3(1.0, 2.0, 1.5);
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0, 0.0, -2.0));
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

    glm::mat4 inverseModel = glm::transpose( glm::inverse(model));

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 perspective = camera.GetProjectionMatrix();

    float ambient = 0.1;
    float diffuse = 0.5;
    float specular = 0.8;

    glm::vec3 materialColour = glm::vec3(0.5f,0.6,0.8);

    //Rendering

    shader.use();
    shader.setFloat("shininess", 32.0f);
    shader.setVector3f("materialColour", materialColour);
    shader.setFloat("light.ambient_strength", ambient);
    shader.setFloat("light.diffuse_strength", diffuse);
    shader.setFloat("light.specular_strength", specular);
    shader.setFloat("light.constant", 1.0);
    shader.setFloat("light.linear", 0.14);
    shader.setFloat("light.quadratic", 0.07);

    std::string pathToCubeMapTexture = "../../Project/textures/skybox/";
    cubeMap.addTexture(&pathToCubeMapTexture);

    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        view = camera.GetViewMatrix();
        glfwPollEvents();
        double now = glfwGetTime();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        shader.setMatrix4("M", model);
        shader.setMatrix4("itM", inverseModel);
        shader.setMatrix4("V", view);
        shader.setMatrix4("P", perspective);
        shader.setVector3f("u_view_pos", camera.Position);
        auto delta = light_pos + glm::vec3(0.0, 0.0, 2 * std::sin(now));
        shader.setVector3f("light.light_pos", delta);

        // reflective is exact same as here but + that last line that is commented 
        refractiveShader.use();

        refractiveShader.setMatrix4("M", model);
        refractiveShader.setMatrix4("itM", inverseModel);
        refractiveShader.setMatrix4("V", view);
        refractiveShader.setMatrix4("P", perspective);
        refractiveShader.setVector3f("u_view_pos", camera.Position);
        //reflective_shader.setVector3f("light.light_pos", delta);

        sphere1.draw();

        cubeMapShader.use();
        cubeMapShader.setMatrix4("V", view);
        cubeMapShader.setMatrix4("P", perspective);
        cubeMapShader.setInteger("cubemapTexture", 0);
        cubeMap.draw();

        
        //Show the object even if it's depth is equal to the depht of the object already present

        fps(now);
        glfwSwapBuffers(window);
    }

    //clean up ressource
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void loadOpenGLFunctions() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

void setupDebug() {
    glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif

}

GLFWwindow *createOpenGLWindow() {
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vr project", nullptr, nullptr);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }

    glfwMakeContextCurrent(window);
    return window;
}

void createOpenGLContext() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialise GLFW \n");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
//create a debug context to help with Debugging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


}

void processInput(GLFWwindow* window) {
    //3. Use the cameras class to change the parameters of the camera
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(LEFT, 0.1);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(RIGHT, 0.1);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(FORWARD, 0.1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(BACKWARD, 0.1);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(1, 0.0, 1);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(-1, 0.0, 1);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(0.0, 1.0, 1);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboardRotation(0.0, -1.0, 1);


}
