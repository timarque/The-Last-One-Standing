#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include "../../3rdParty/assimp/include/assimp/DefaultLogger.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "INFO-H502 Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // Désactive le curseur de la souris
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader ourShader(PATH_TO_SHADERS "/backpack/shader.vert", PATH_TO_SHADERS "/backpack/shader.frag");
    Shader shader(PATH_TO_SHADERS "/specularObjects/specularObjects.vert", PATH_TO_SHADERS "/specularObjects/specularObjects.frag");

    Model ourModel(PATH_TO_OBJECTS  "/sphere_smooth.obj");
    Model floorModel(PATH_TO_OBJECTS  "/floor/floor.obj");

    int grid_size = 3;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(grid_size - 1.0f, 2.0f, grid_size - 1.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// L'objet est super grand, on le déplace

    // Tentative
    glm::mat4 inverseModel = glm::transpose(glm::inverse(model));

    glm::vec3 light_pos = glm::vec3(grid_size - 1.0f, 3.0f, grid_size - 1.0f);

    float ambient = 0.1;
    float diffuse = 0.5;
    float specular = 0.8;
    
    glm::vec3 materialColour = glm::vec3(0.5f, 0.6, 0.8);
    shader.use();
    shader.setFloat("shininess", 32.0f);
    shader.setVec3("materialColour", materialColour);
    shader.setFloat("light.ambient_strength", ambient);
    shader.setFloat("light.diffuse_strength", diffuse);
    shader.setFloat("light.specular_strength", specular);
    shader.setFloat("light.constant", 1.0);
    shader.setFloat("light.linear", 0.14);
    shader.setFloat("light.quadratic", 0.07);

    camera.Position = glm::vec3(grid_size - 1.0f, 3.0f, grid_size - 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // don't forget to enable shader before setting uniforms
        shader.use();
        shader.setMatrix4("M", model);
        shader.setMatrix4("itM", inverseModel);
        shader.setMatrix4("V", view);
        shader.setMatrix4("P", projection);
        shader.setVec3("u_view_pos", camera.Position);
        // auto delta = light_pos + glm::vec3(0.0, 0.0, 2 * std::sin(currentFrame));
        shader.setVec3("light.light_pos", light_pos);

        ourModel.Draw(shader);

        // render the loaded model
        ourShader.use();
        ourShader.setMatrix4("model", model);
        ourShader.setMatrix4("projection", projection);
        ourShader.setMatrix4("view", view);

        
        glm::mat4 floor = glm::mat4(1.0f);
        floor = glm::translate(floor, glm::vec3(-grid_size, 0.0f, -grid_size));
        
        for (int i = 0; i < grid_size * grid_size; i++) {
            if (i % grid_size != 0) floor = glm::translate(floor, glm::vec3(0.0f, 0.0f, 2.0f)); 
            else {
                floor = glm::mat4(1.0f);
                // floor = glm::scale(floor, glm::vec3(1.0f, 1.0f, 1.0f));	
                floor = glm::translate(floor, glm::vec3(2.0f * i/grid_size, 0.0f, 0.0f));
            }
            ourShader.setMatrix4("model", floor);
            floorModel.Draw(ourShader);
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    // 3. Use the cameras class to change the parameters of the camera
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
