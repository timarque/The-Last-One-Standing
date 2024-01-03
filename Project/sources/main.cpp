#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "PhysicsEngine.hpp"
#include "PhysicModel.h"
#include "Sphere.h"
#include "CubeMap.h"

#include <iostream>

#include "bullet/btBulletDynamicsCommon.h"

// #include <btphysics.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

glm::mat4 btScalar2mat4(btScalar *matrix)
{
    return glm::mat4(
        matrix[0], matrix[1], matrix[2], matrix[3],
        matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11],
        matrix[12], matrix[13], matrix[14], matrix[15]);
}

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

glm::vec3 carPosition = glm::vec3(0.0f);
float carSpeed = 0.01f;
bool posUpdated = false;
bool jump = false;

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

    PhysicsEngine physics(btVector3(0, -9.81, 0));

    Shader ourShader(PATH_TO_SHADERS "/backpack/shader.vert", PATH_TO_SHADERS "/backpack/shader.frag");
    Shader shader(PATH_TO_SHADERS "/specularObjects/specularObjects.vert", PATH_TO_SHADERS "/specularObjects/specularObjects.frag");
    Shader reflshader(PATH_TO_SHADERS "/reflectiveObjects/reflectiveObjects.vert", PATH_TO_SHADERS "/reflectiveObjects/reflectiveObjects.frag");
    Shader ambiant(PATH_TO_SHADERS "/ambiant_light/ambiant.vert", PATH_TO_SHADERS "/ambiant_light/ambiant.frag");
    Shader physical(PATH_TO_SHADERS "/physicalObjects/physicalObjects.vert", PATH_TO_SHADERS "/physicalObjects/physicalObjects.frag");
    Shader cubeMapShader(PATH_TO_SHADERS "/skybox/skybox.vert", PATH_TO_SHADERS "/skybox/skybox.frag");

    // Cube map
    CubeMap cubeMap(PATH_TO_OBJECTS "/cube.obj", &cubeMapShader);
    std::string cubeMapTexturePath = PATH_TO_TEXTURES "/skybox/";
    cubeMap.addTexture(&cubeMapTexturePath);

    // Model ourModel(PATH_TO_OBJECTS  "/cube.obj");
    // btCollisionShape *shape = new btBoxShape(btVector3(1, 1, 1));
    // ourModel.createPhysicsObject(physics, shape, 0.1, btVector3(1, 5, 0));
    PhysicModel ourModel(PATH_TO_OBJECTS  "/tank/tank.obj");
    btCollisionShape *shape = new btBoxShape(btVector3(0.3, 0, 0.5));
    ourModel.createPhysicsObject(physics, shape, 10, btVector3(0, 0, 0));
    
    PhysicModel earthM = generatePhysicalSphere(PATH_TO_OBJECTS "/earth/earth.obj", glm::vec3(0.0f, 5.0f, 0.0f), physics);
    PhysicModel moonM = generatePhysicalSphere(PATH_TO_OBJECTS "/moon.obj", glm::vec3(0.0f, 3.0f, 0.0f), physics);
    PhysicModel sunM = generatePhysicalSphere(PATH_TO_OBJECTS "/sun.obj", 0.33, 0.0, glm::vec3(0.0f, 15.0f, 0.0f), physics);
    PhysicModel lightM = generatePhysicalSphere(PATH_TO_OBJECTS "/sphere_smooth.obj", glm::vec3(0.0f, 0.0f, 0.0f), physics);

    PhysicModel heliport(PATH_TO_OBJECTS "/tank/heliport.obj");
    btCollisionShape *heliport_shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
    heliport.createPhysicsObject(physics, heliport_shape, 2, btVector3(-2.0, 0.0, -2.0));


    // Model sphere(PATH_TO_OBJECTS "/sun.obj");
    // btCollisionShape *sphere_shape = new btSphereShape(1);
    // sphere.createPhysicsObject(physics, sphere_shape, 10, btVector3(0, 15, 1.5));

    PhysicModel floorModel(PATH_TO_OBJECTS  "/floor/floor.obj");
    btCollisionShape *floor_shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    floorModel.createPhysicsObject(physics, floor_shape, 0.0, btVector3(0, 0, 0));
    

    int grid_size = 20;
    glm::vec3 light_pos = glm::vec3(0.0, 2.0, 0.0);

    glm::mat4 model = glm::mat4(1.0f);
    // btTransform transform;
    // ourModel.physicsObject->getMotionState()->getWorldTransform(transform);
    // btVector3 position = transform.getOrigin();
    // model = glm::translate(model, glm::vec3(grid_size - 1.0f, 2.0f, grid_size - 1.0f)); // translate it down so it's at the center of the scene
    // model = glm::translate(model, glm::vec3(position.x(), position.y(), position.z())); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// L'objet est super grand, on le déplace
    
    glm::mat4 sun = glm::mat4(1.0f);
    sun = glm::translate(sun, light_pos);
    sun = glm::scale(sun, glm::vec3(0.2f));
    glm::mat4 itMsun = glm::transpose(glm::inverse(sun));

    glm::mat4 floor = glm::mat4(1.0f);

    // Tentative
    glm::mat4 inverseModel = glm::transpose(glm::inverse(model));


    float ambient = 0.1;
    float diffuse = 2.0;
    float specular = 0.8;
    
    glm::vec3 materialColour = glm::vec3(0.5f, 0.6f, 0.8f);
    physical.use();
    physical.setFloat("shininess", 32.0f);
    physical.setVec3("materialColour", materialColour);
    physical.setFloat("light.ambient_strength", ambient);
    physical.setFloat("light.diffuse_strength", diffuse);
    physical.setFloat("light.specular_strength", specular);
    physical.setFloat("light.constant", 1.0);
    physical.setFloat("light.linear", 0.14);
    physical.setFloat("light.quadratic", 0.07);

    ourShader.use();
    ourShader.setFloat("shininess", 32.0f);
    ourShader.setVec3("materialColour", materialColour);
    ourShader.setFloat("light.ambient_strength", ambient);
    ourShader.setFloat("light.diffuse_strength", diffuse);
    ourShader.setFloat("light.specular_strength", specular);
    ourShader.setFloat("light.constant", 1.0);
    ourShader.setFloat("light.linear", 0.1);
    ourShader.setFloat("light.quadratic", 0.01);

    camera.Position = glm::vec3(-3.0f, 1.0f, -3.0f);
    camera.LookAtModel(glm::vec3(0.0f, 1.0f, 0.0f));
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double now = glfwGetTime();
        processInput(window);

        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        // don't forget to enable shader before setting uniforms

        btTransform transform;
        glm::mat4 m;
        ourShader.use();
        ourShader.setMatrix4("projection", projection);
        ourShader.setMatrix4("view", view);
        ourShader.setVec3("u_view_pos", camera.Position);
        auto delta = light_pos + glm::vec3(4 * std::sin(now / 2), 0.0,  4 * std::cos(now / 2));
        ourShader.setVec3("light.light_pos", delta);

        sunM.updatePosition(delta);
        m = sunM.getOpenGLMatrix();
        ourShader.setMatrix4("model", glm::scale(m, glm::vec3(0.5)));
        ourShader.setFloat("light.ambient_strength", 1.0);
        sunM.DrawWithShader(ourShader, 1);
        ourShader.setFloat("light.ambient_strength", ambient);

        m = moonM.getOpenGLMatrix();
        ourShader.setMatrix4("model", glm::scale(m, glm::vec3(0.5)));
        moonM.DrawWithShader(ourShader, 1);
        
        m = earthM.getOpenGLMatrix();
        ourShader.setMatrix4("model", glm::scale(m, glm::vec3(0.5)));
        earthM.DrawWithShader(ourShader, 1);

        m = heliport.getOpenGLMatrix();
        ourShader.setMatrix4("model", m);
        heliport.DrawWithShader(ourShader, 1);
        
        if (posUpdated) ourModel.updatePosition(carPosition);
        m = ourModel.getOpenGLMatrix();
        ourShader.setMatrix4("model", glm::scale(m, glm::vec3(0.2)));
        if (jump)
        {
            ourModel.applyImpulse(btVector3(0.0f, 5.0f, 0.0f));
            jump = false;
        }
        ourModel.DrawWithShader(ourShader, 1);

        physical.use();
        physical.setMatrix4("M", model);
        physical.setMatrix4("itM", inverseModel);
        physical.setMatrix4("V", view);
        physical.setMatrix4("P", projection);
        physical.setVec3("u_view_pos", camera.Position);
        physical.setVec3("light.light_pos", delta);
        
        m = lightM.getOpenGLMatrix();
        physical.setMatrix4("M", glm::scale(m, glm::vec3(0.5)));
        
        lightM.DrawWithShader(physical, 0); // pas besoin du param en plus mtn je pense mais je le garde car peut etre bien pr debug au cas ou 


        // sun = glm::translate(sun, glm::vec3(0.1 * cos(currentFrame), 0.0, 0.1 * sin(currentFrame)));

        // render the loaded model
        ourShader.use();
        ourShader.setMatrix4("model", model);
        ourShader.setMatrix4("projection", projection);
        ourShader.setMatrix4("view", view);

        for (int i = 0; i < grid_size * grid_size; i++) {
            if (i % grid_size != 0) floor = glm::translate(floor, glm::vec3(0.0f, 0.0f, 2.0f)); 
            else {
                floor = glm::mat4(1.0f);
                floor = glm::translate(floor, glm::vec3(-grid_size + 2.0f * i / grid_size, 0.0f, -grid_size));
            }
            ourShader.setMatrix4("model", floor);
            floorModel.DrawWithShader(ourShader, 1);
        }

        // ourShader.use();
        // ourShader.setMatrix4("model", model);
        // ourShader.setMatrix4("projection", projection);
        // ourShader.setMatrix4("view", view);
        // sphere.DrawWithShader(ourShader);

        physics.simulate(deltaTime);
        ourModel.updateFromPhysics();
        // sphere.updateFromPhysics();
        floorModel.updateFromPhysics();
        earthM.updateFromPhysics();
        moonM.updateFromPhysics();
        sunM.updateFromPhysics();
        lightM.updateFromPhysics();

        cubeMap.draw(&camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    posUpdated = false;
    // 3. Use the cameras class to change the parameters of the camera
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(LEFT, 0.3);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(RIGHT, 0.3);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(FORWARD, 0.3);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(BACKWARD, 0.3);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        carPosition.z += carSpeed;
        posUpdated = true;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        carPosition.z -= carSpeed;
        posUpdated = true;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        carPosition.x += carSpeed;
        posUpdated = true;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        carPosition.x -= carSpeed;
        posUpdated = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        jump = true;
    }

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
