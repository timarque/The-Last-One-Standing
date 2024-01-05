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
#include "TankModel.hpp"
#include "CubeMap.h"
#include "debugObject.hpp"
#include "Animator.h"
#include "Sphere.h"

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
    std::cout << 0 << std::endl;

    // // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    PhysicsEngine physics(btVector3(0, -9.81, 0));
    // Shaders
    Shader shader(PATH_TO_SHADERS "/backpack/shader.vert", PATH_TO_SHADERS "/backpack/shader.frag");
    Shader debugShader(PATH_TO_SHADERS "/debug/debug.vert", PATH_TO_SHADERS "/debug/debug.frag");
    Shader cubeMapShader(PATH_TO_SHADERS "/skybox/skybox.vert", PATH_TO_SHADERS "/skybox/skybox.frag");
    Shader animationShader(PATH_TO_SHADERS "/animation/animation.vert", PATH_TO_SHADERS "/animation/animation.frag");

    //* Bullet Physics Rendering Debug Tool
    DebugDrawer *debugDrawer = new DebugDrawer(debugShader.ID);
    physics.getWorld()->setDebugDrawer(debugDrawer);
    
    // Cube map
    CubeMap cubeMap(PATH_TO_OBJECTS "/cube.obj", &cubeMapShader);
    std::string cubeMapTexturePath = PATH_TO_TEXTURES "/skybox/";
    cubeMap.addTexture(&cubeMapTexturePath);

    TankModel tankModel(PATH_TO_OBJECTS  "/tank/tank.obj");
    btCollisionShape *shape = new btBoxShape(btVector3(0.7, 0.7, 0.7));
    tankModel.createPhysicsObject(physics, shape, 1, btVector3(0.0, 2.0, 0.0));

    PhysicModel* platform = new PhysicModel(PATH_TO_OBJECTS "/tank/platform.dae");
    btCollisionShape* shape_deploy = new btBoxShape(btVector3(0.8, 0.3, 0.8));
    platform->createPhysicsObject(physics, shape_deploy, 100, btVector3(0.0, 0.0, 0.0));
    Animation* deployanimation = new Animation(PATH_TO_OBJECTS "/tank/platform.dae", platform); // on peut en faire un autre pour montrer que ca marche y a un autre model => path = animation/model.dae
    Animator anim(deployanimation);

    // enemies
    PhysicModel *vampire_dancing = new PhysicModel(PATH_TO_OBJECTS "/animation/dancing_vampire.dae");
    btCollisionShape *shape_vampire = new btBoxShape(btVector3(0.8, 2.0, 0.8));
    vampire_dancing->createPhysicsObject(physics, shape_vampire, 100, btVector3(-5.f, 0.f, .5f));
    Animation *danceAnimation = new Animation(PATH_TO_OBJECTS "/animation/dancing_vampire.dae", vampire_dancing); // on peut en faire un autre pour montrer que ca marche y a un autre model => path = animation/model.dae
    Animator animator(danceAnimation);

    PhysicModel slenderman(PATH_TO_OBJECTS "/slenderman.fbx");
    btCollisionShape* shape_slenderman = new btBoxShape(btVector3(0.8,4.7, 0.8));
    slenderman.createPhysicsObject(physics, shape_slenderman, 0, btVector3(-12.0, 0.0, -12.0));
    Animation slenderanimation(PATH_TO_OBJECTS "/slenderman.fbx", &slenderman);
    Animator animslender(&slenderanimation);
    
    TankModel tankEnemyModel(PATH_TO_OBJECTS  "/tank/enemy.obj");
    btCollisionShape *shapeEnemy = new btBoxShape(btVector3(0.6, 0.7, 0.7));
    tankEnemyModel.createPhysicsObject(physics, shapeEnemy, 1, btVector3(0.0, 0.0, 3.0));


    // floor 
    PhysicModel floorModel(PATH_TO_OBJECTS  "/floor/floor.obj");
    btCollisionShape *floor_shape = new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 0);
    floorModel.createPhysicsObject(physics, floor_shape, 0.0, btVector3(0, 0, 0));
    
    int grid_size = 20;

    glm::vec3 light_pos = glm::vec3(0.0, 2.0, 0.0);
    
    glm::mat4 floor = glm::mat4(1.0f);

    float ambient = 0.1;
    float diffuse = 2.0;
    float specular = 0.8;
    
    glm::vec3 materialColour = glm::vec3(0.5f, 0.6f, 0.8f);

    shader.use();
    shader.setFloat("shininess", 32.0f);
    shader.setVec3("materialColour", materialColour);
    shader.setFloat("light.ambient_strength", ambient);
    shader.setFloat("light.diffuse_strength", diffuse);
    shader.setFloat("light.specular_strength", specular);
    shader.setFloat("light.constant", 1.0);
    shader.setFloat("light.linear", 0.1);
    shader.setFloat("light.quadratic", 0.01);

    std::vector<PhysicModel> bullets;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double now = glfwGetTime();
        processInput(window);
        camera.Position = tankModel.getPosition() + glm::vec3(0.0, 2.5, -4.0);
        // glm::mat4 view = glm::translate(glm::mat4(1.0f), tankModel.getPosition() + glm::vec3(0.0, 2.0, -5.0));
        // view = glm::rotate(view, atan2(-tankModel.getRotation()[2][0], -tankModel.getRotation()[0][0]), glm::vec3(0.0f, 1.0f, 0.0f));
        // view = glm::inverse(view);

        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        animator.UpdateAnimation(deltaTime);
        anim.UpdateAnimation(deltaTime);
        animslender.UpdateAnimation(deltaTime);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // glm::mat4 view = glm::lookAt(camera.Position, tankModel.getPosition() + glm::vec3(0.0, 1.5, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();
        auto current_pos_light = light_pos + glm::vec3(4 * std::sin(now / 2), 0.0,  4 * std::cos(now / 2));

        shader.use();
        shader.setVec3("u_view_pos", camera.Position);
        shader.setVec3("light.light_pos", current_pos_light);
        bool shot = tankModel.update(window, deltaTime);
        btVector3 forward_pos = tankModel.getForward();
        if (shot) {
            PhysicModel bullet = generatePhysicalSphere(PATH_TO_OBJECTS "/sphere_smooth.obj", tankModel.getPosition() + glm::vec3(forward_pos.x() * 1.0, 0.1, forward_pos.z() * 1.0), physics);
            bullet.applyImpulse(forward_pos * btVector3(500.f, 0.1f, 500.f));
            bullets.push_back(std::move(bullet));
        }
        for (int i = 0; i < bullets.size(); i++) {
            shader.setMatrix4("model", bullets[i].getModelMatrix(glm::vec3(1.0f)));
            bullets[i].DrawWithShader(shader, 1);
        }

        shader.setMatrix4("model", tankModel.getModelMatrix(glm::vec3(1.0f)));
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", view);
        tankModel.DrawWithShader(shader, 1);

        shader.setMatrix4("model", glm::rotate(tankEnemyModel.getModelMatrix(glm::vec3(1.0f)), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)));
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", view);
        tankEnemyModel.DrawWithShader(shader, 1);

        for (int i = 0; i < grid_size * grid_size; i++) {
            if (i % grid_size != 0) floor = glm::translate(floor, glm::vec3(0.0f, 0.0f, 2.0f)); 
            else {
                floor = glm::mat4(1.0f);
                floor = glm::translate(floor, glm::vec3(-grid_size + 2.0f * i / grid_size, 0.0f, -grid_size));
            }
            shader.setMatrix4("model", floor);
            floorModel.DrawWithShader(shader, 1);
        }


        animationShader.use();
        animationShader.setMatrix4("projection", projection);
        animationShader.setMatrix4("view", view);

        auto transforms_slenderman = animslender.GetFinalBoneMatrices();
        for (int i = 0; i < transforms_slenderman.size(); ++i) {
            animationShader.setMatrix4("finalBonesMatrices[" + std::to_string(i) + "]", transforms_slenderman[i]);
        }
        glm::mat4 model_slenderman = glm::mat4(1.0f);
        model_slenderman = glm::translate(model_slenderman, glm::vec3(-12.f, 0.f, -12.f));
        model_slenderman = glm::scale(model_slenderman, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down
        animationShader.setMatrix4("model", model_slenderman);
        slenderman.DrawWithShader(animationShader, 1);


        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            animationShader.setMatrix4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.f, 0.f, .5f)); // keep same translation here as when object is init otherwise colision box diff than position
        model = glm::scale(model, glm::vec3(.8f, .8f, .8f));	// it's a bit too big for our scene, so scale it down
        animationShader.setMatrix4("model", model);
        vampire_dancing->DrawWithShader(animationShader, 1);

        auto transforms_deploy = anim.GetFinalBoneMatrices();
        for (int i = 0; i < transforms_deploy.size(); ++i) {
            animationShader.setMatrix4("finalBonesMatrices[" + std::to_string(i) + "]", transforms_deploy[i]);
        }
        glm::mat4 model_deploy = glm::mat4(1.0f);
        model_deploy = glm::scale(model_deploy, glm::vec3(1.0f, 1.0f, 1.0f));
        animationShader.setMatrix4("model", model_deploy);;
        platform->DrawWithShader(animationShader, 1);


        debugShader.use();
        debugShader.setMatrix4("view", view);
        debugShader.setMatrix4("projection", projection);


        physics.getWorld()->debugDrawWorld();
        debugDrawer->flushLines();


        // ourShader.use();
        // ourShader.setMatrix4("model", model);
        // ourShader.setMatrix4("projection", projection);
        // ourShader.setMatrix4("view", view);
        // sphere.DrawWithShader(ourShader);
        physics.simulate(deltaTime);
        //tankModel.updateFromPhysics();
        //vampire_dancing.updateFromPhysics();
        // // sphere.updateFromPhysics();
        // floorModel.updateFromPhysics();

        cubeMap.draw(&camera);

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
        camera.ProcessKeyboardMovement(LEFT, 0.3);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(RIGHT, 0.3);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(FORWARD, 0.3);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardMovement(BACKWARD, 0.3);

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
