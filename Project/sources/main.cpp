#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <stdlib.h>

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
#include "LightSource.h"

#include <iostream>

#include "bullet/btBulletDynamicsCommon.h"

// #include <btphysics.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void renderScene(std::vector<PhysicModel> &bullets, Shader &shader, TankModel &tankModel, std::vector<TankModel *> &ennemies, int grid_size, glm::mat4 &floor, PhysicModel &floorModel, std::vector<PhysicModel*> &cactuses);

glm::mat4 btScalar2mat4(btScalar *matrix)
{
    return glm::mat4(
        matrix[0], matrix[1], matrix[2], matrix[3],
        matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11],
        matrix[12], matrix[13], matrix[14], matrix[15]);
}

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

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
    // glfwSetCursorPosCallback(window, mouse_callback);
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
    // Shaders
    Shader shader(PATH_TO_SHADERS "/backpack/shader.vert", PATH_TO_SHADERS "/backpack/shader.frag");
    Shader debugShader(PATH_TO_SHADERS "/debug/debug.vert", PATH_TO_SHADERS "/debug/debug.frag");
    Shader cubeMapShader(PATH_TO_SHADERS "/skybox/skybox.vert", PATH_TO_SHADERS "/skybox/skybox.frag");
    Shader animationShader(PATH_TO_SHADERS "/animation/animation.vert", PATH_TO_SHADERS "/animation/animation.frag");
    Shader depthMapShader(PATH_TO_SHADERS "/depthMap/depthMap.vert", PATH_TO_SHADERS "/depthMap/depthMap.frag");
    Shader reflectiveShader(PATH_TO_SHADERS "/reflectiveObjects/reflectiveObjects.vert", PATH_TO_SHADERS "/reflectiveObjects/reflectiveObjects.frag");
    Shader refractiveShader(PATH_TO_SHADERS "/refractiveObjects/refractiveObjects.vert", PATH_TO_SHADERS "/refractiveObjects/refractiveObjects.frag");

    //* Bullet Physics Rendering Debug Tool
    DebugDrawer *debugDrawer = new DebugDrawer(debugShader.ID);
    physics.getWorld()->setDebugDrawer(debugDrawer);
    
    // Cube map
    CubeMap cubeMap(PATH_TO_OBJECTS "/cube.obj", &cubeMapShader);
    std::string cubeMapTexturePath = PATH_TO_TEXTURES "/box/";
    cubeMap.addTexture(&cubeMapTexturePath);

    TankModel tankModel(PATH_TO_OBJECTS  "/tank/tank.obj");
    btCollisionShape *shape = new btBoxShape(btVector3(0.7, 0.7, 0.7));
    tankModel.createPhysicsObject(physics, shape, 1, btVector3(0.0, 2.0, -10.0));
    
    glm::vec3 reflsphere_pos = glm::vec3(7.0, 5.0, 15.0);
    glm::vec3 refrsphere_pos = glm::vec3(-7.0, 5.0, 15.0);
    PhysicModel reflectiveSphere = generatePhysicalSphere(PATH_TO_OBJECTS "/sphere_smooth.obj", reflsphere_pos, physics);
    PhysicModel refractiveSphere = generatePhysicalSphere(PATH_TO_OBJECTS "/sphere_smooth.obj", refrsphere_pos, physics);


     PhysicModel *platform = new PhysicModel(PATH_TO_OBJECTS "/tank/platform.dae");
     btCollisionShape* shape_deploy = new btBoxShape(btVector3(0.8, 0.3, 0.8));
     platform->createPhysicsObject(physics, shape_deploy, 100, btVector3(-0.0, 0.0, -10.0), 1, "platform");
     platform->physicsObject.get()->setUserIndex(25);
     Animation deployanimation(PATH_TO_OBJECTS "/tank/platform.dae", platform); 
     Animator anim(&deployanimation);

    // enemies
     PhysicModel *vampire_dancing = new PhysicModel(PATH_TO_OBJECTS "/animation/dancing_vampire.dae");
     btCollisionShape *shape_vampire = new btBoxShape(btVector3(1.0, 3.0, 1.0));
     vampire_dancing->createPhysicsObject(physics, shape_vampire, 0, btVector3(0.f, 0.f, -35.f), 10, "vampire");
     vampire_dancing->physicsObject.get()->setUserIndex(15);
     Animation danceAnimation(PATH_TO_OBJECTS "/animation/dancing_vampire.dae", vampire_dancing); 
     Animator animator(&danceAnimation);

     // removed as it uses fbx but working
     //PhysicModel *slenderman = new PhysicModel(PATH_TO_OBJECTS "/slenderman.fbx");
     //btCollisionShape* shape_slenderman = new btBoxShape(btVector3(0.8,4.7, 0.8));
     //slenderman->createPhysicsObject(physics, shape_slenderman, 0, btVector3(-15.0, 0.0, -30.0), 10, "slenderman");
     //slenderman->physicsObject.get()->setUserIndex(30);
     //Animation slenderanimation(PATH_TO_OBJECTS "/slenderman.fbx", slenderman);
     //Animator animslender(&slenderanimation);

    std::vector<TankModel*> ennemies;
    for (int i = -5; i < 5; i++) {
        TankModel *tankEnemy = new TankModel(PATH_TO_OBJECTS  "/tank/enemy.obj");
        btCollisionShape *shapeEnemy1 = new btBoxShape(btVector3(0.6, 0.7, 0.7));
        tankEnemy->createPhysicsObject(physics, shapeEnemy1, 10.0, btVector3(i*2, 0.7, 3.0));
        tankEnemy->physicsObject.get()->setUserIndex(i+5);
        ennemies.push_back(std::move(tankEnemy));
    }

    // floor 
    PhysicModel floorModel(PATH_TO_OBJECTS  "/floor/floor.obj");
    btCollisionShape *floor_shape = new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 0);
    floorModel.createPhysicsObject(physics, floor_shape, 0.0, btVector3(0, 0, 0));
    
    
    const int grid_size = 60;
    
    std::vector<PhysicModel*> cactuses;
    for (size_t i = 0; i < 5; i++) {
        float x = ((rand() % grid_size + 1) - grid_size/2) * 2;
        float z = ((rand() % grid_size + 1) - grid_size/2) * 2;
        PhysicModel *cactus = new PhysicModel(PATH_TO_OBJECTS "/floor/cactus.obj");
        btCollisionShape *cactus_shape = new btBoxShape(btVector3(0.6, 0.6, 0.2));
        cactus->createPhysicsObject(physics, cactus_shape, 0.0, btVector3(x, 0.5, z));
        cactuses.push_back(std::move(cactus));
    }

    // Configure the light source
    glm::vec3 light_pos = glm::vec3(0.0, 5.0, 0.0);
    LightSource lightSource(depthMapShader);
    lightSource.setPosition(light_pos);
    
    glm::mat4 floor = glm::mat4(1.0f);

    float ambient = 0.9;
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
    shader.setInt("shadowMap", 0);

    reflectiveShader.use();
    reflectiveShader.setFloat("shininess", 32.0f);
    reflectiveShader.setVec3("materialColour", materialColour);
    reflectiveShader.setFloat("light.ambient_strength", ambient);
    reflectiveShader.setFloat("light.diffuse_strength", diffuse);
    reflectiveShader.setFloat("light.specular_strength", specular);
    reflectiveShader.setFloat("light.constant", 1.0);
    reflectiveShader.setFloat("light.linear", 0.14);
    reflectiveShader.setFloat("light.quadratic", 0.07);

    refractiveShader.use();
    refractiveShader.setFloat("shininess", 32.0f);
    refractiveShader.setVec3("materialColour", materialColour);
    refractiveShader.setFloat("light.ambient_strength", ambient);
    refractiveShader.setFloat("light.diffuse_strength", diffuse);
    refractiveShader.setFloat("light.specular_strength", specular);
    refractiveShader.setFloat("light.constant", 1.0);
    refractiveShader.setFloat("light.linear", 0.14);
    refractiveShader.setFloat("light.quadratic", 0.07);


    std::vector<PhysicModel> bullets;
    std::vector<PhysicModel*> animated_enemies;
    std::vector<Animator> animations;
    animations.push_back(std::move(anim));
    animations.push_back(std::move(animator));
    //animations.push_back(std::move(animslender));
    animated_enemies.push_back(std::move(platform));
    animated_enemies.push_back(std::move(vampire_dancing));
    //animated_enemies.push_back(std::move(slenderman));


    glm::mat4 reflect = glm::mat4(1.0f);
    glm::mat4 itsmreflect = glm::transpose(glm::inverse(reflect));

    glm::mat4 refract = glm::mat4(1.0f);
    glm::mat4 itsmrefract = glm::transpose(glm::inverse(refract));

    while (!glfwWindowShouldClose(window))
    {

        std::vector<size_t> bulletsToRemove;
        std::vector<size_t> ennemiesToRemove;
        std::vector<size_t> anim_ennemiesToRemove;
        std::vector<size_t> animationsToRemove;

        for (size_t i = 0; i < bullets.size(); ++i) {
            for (size_t j = 0; j < ennemies.size(); ++j) {
                bool collided = physics.checkCollisions(bullets[i].physicsObject, ennemies[j]->physicsObject);
                if (collided) {

                    bulletsToRemove.push_back(i);
                    ennemiesToRemove.push_back(j);

                    // If you want to break out of the inner loop after a collision, you can use a break statement here.
                }
            }
             for (size_t k = 0; k < animated_enemies.size(); k++) {
                 bool collided = physics.checkCollisions(bullets[i].physicsObject, animated_enemies[k]->physicsObject);
                 if (collided) {
                   bulletsToRemove.push_back(i);
                     animated_enemies[k]->hp -= 1;
                     if (animated_enemies[k]->hp == 0) {
                         anim_ennemiesToRemove.push_back(k);
                         animationsToRemove.push_back(k);
                     }
                 }
             }
        }

        // Remove elements in reverse order to avoid invalidating indices
        for (auto it = bulletsToRemove.rbegin(); it != bulletsToRemove.rend(); ++it) {
            physics.dynamicsWorld->removeRigidBody(bullets[*it].physicsObject.get());
            bullets.erase(bullets.begin() + *it);
        }

        for (auto it = ennemiesToRemove.rbegin(); it != ennemiesToRemove.rend(); ++it) {
            physics.dynamicsWorld->removeRigidBody(ennemies[*it]->physicsObject.get());
            ennemies.erase(ennemies.begin() + *it);
        }


         for (auto it = anim_ennemiesToRemove.rbegin(); it != anim_ennemiesToRemove.rend(); ++it) {
             physics.dynamicsWorld->removeRigidBody(animated_enemies[*it]->physicsObject.get());
             animated_enemies.erase(animated_enemies.begin() + *it);
         }
         for (auto it = animationsToRemove.rbegin(); it != animationsToRemove.rend(); ++it) {
             animations.erase(animations.begin() + *it);
         }

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double now = glfwGetTime();
        processInput(window);

        // Shoot
        bool shot = tankModel.update(window, deltaTime);
        btVector3 forward_pos = tankModel.getForward();
        if (shot) {
            PhysicModel bullet = generatePhysicalSphere(PATH_TO_OBJECTS "/tank/ball.obj", 0.2, 10, tankModel.getPosition() + glm::vec3(forward_pos.x(), 0.2, forward_pos.z()), physics);
            // bullet.getTransform().setRotation(tankModel.getRotationQuat());
            bullet.applyImpulse((forward_pos + btVector3(0.0, camera.Position.y, 0.0)) * btVector3(500.f, 0.0, 500.f));
            bullets.push_back(std::move(bullet));
        }

        // Render scene from the light
        // ---------------------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup the depth map shader
        depthMapShader.use();
        depthMapShader.setMatrix4("lightSpaceMatrix", lightSource.getLightSpaceMatrix());
        // Render everything
        glViewport(0, 0, lightSource.getShadowWidth(), lightSource.getShadowHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, lightSource.getDepthMapFBO());
            glClear(GL_DEPTH_BUFFER_BIT);
            renderScene(bullets, depthMapShader, tankModel, ennemies, grid_size, floor, floorModel, cactuses);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // lightSource.renderSceneToLight(scene);
        // Reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene as normal
        // ----------------------

        for (auto& animation : animations) {
             animation.UpdateAnimation(deltaTime);
        }

        
        // view/projection transformations
        
        // glm::mat4 view = glm::lookAt(camera.Position, tankModel.getPosition() + glm::vec3(0.0, 1.5, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 view = camera.GetViewMatrix();
        // auto current_pos_light = light_pos;

        // Setup the shader
        glm::mat4 view = camera.GetViewMatrix(&tankModel);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightSource.setPosition(light_pos + glm::vec3(4 * std::sin(now / 2), 0.0,  4 * std::cos(now / 2)));
        shader.use();

        shader.setVec3("u_view_pos", camera.Position);
        shader.setMatrix4("model", tankModel.getModelMatrix(glm::vec3(1.0f)));
        shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", view);
        shader.setVec3("light.light_pos", lightSource.getPosition());
        shader.setMatrix4("lightSpaceMatrix", lightSource.getLightSpaceMatrix());
        shader.setInt("shadowMap", 0);  // Maybe not needed ?
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lightSource.getDepthMapID());
        // Can maybe be placed outside of the loop ?
        renderScene(bullets, shader, tankModel, ennemies, grid_size, floor, floorModel, cactuses);
        

        // reflective and refractive spheres
        reflectiveShader.use();
        reflectiveShader.setMatrix4("projection", projection);
        reflectiveShader.setMatrix4("view", view);
        reflectiveShader.setMatrix4("model", reflect);
        reflectiveShader.setMatrix4("itM", itsmreflect);
        reflectiveShader.setVec3("u_view_pos", camera.Position);
        reflectiveShader.setVec3("light_pos", lightSource.getPosition());
        reflectiveSphere.DrawWithShader(reflectiveShader, 1); // commentend en attendant de fix position

        refractiveShader.use();
        refractiveShader.setMatrix4("projection", projection);
        refractiveShader.setMatrix4("view", view);
        refractiveShader.setMatrix4("model", refract);
        refractiveShader.setMatrix4("itM", itsmrefract);
        refractiveShader.setVec3("u_view_pos", camera.Position);
        refractiveShader.setVec3("light_pos", lightSource.getPosition());
        refractiveSphere.DrawWithShader(refractiveShader, 1);


        animationShader.use();
        animationShader.setMatrix4("projection", projection);
        animationShader.setMatrix4("view", view);


         for (int i = 0; i < animated_enemies.size(); i++) {
             auto transforms = animations[i].GetFinalBoneMatrices();
             for (int j = 0; j < transforms.size(); ++j) {
                 animationShader.setMatrix4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);
             }
             glm::mat4 model = glm::mat4(1.0f);
             if (animated_enemies[i]->name == "platform") {
                 model = glm::translate(model, glm::vec3(-0.f, 0.f, -10.f));
                 model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
             }
             else if (animated_enemies[i]->name == "vampire") {
                 model = glm::translate(model, glm::vec3(0.f, 0.f, -35.f)); // keep same translation here as when object is init otherwise colision box diff than position
                 model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// it's a bit too big for our scene, so scale it down
            }
             //else if (animated_enemies[i]->name == "slenderman") {
             //    model = glm::translate(model, glm::vec3(-15.f, 0.f, -30.f));
             //    model = glm::scale(model, glm::vec3(.01f, .01f, .01f));	// it's a bit too big for our scene, so scale it down
             //}
             animationShader.setMatrix4("model", model);
             animated_enemies[i]->DrawWithShader(animationShader, 1);
        }

        //debugShader.use();
        //debugShader.setMatrix4("view", view);
        //debugShader.setMatrix4("projection", projection);

        //physics.getWorld()->debugDrawWorld();
        //debugDrawer->flushLines();


        physics.simulate(deltaTime);
        cubeMap.draw(view, projection);
        reflectiveSphere.updateFromPhysics();
        refractiveSphere.updateFromPhysics();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderScene(std::vector<PhysicModel> &bullets, Shader &shader, TankModel &tankModel, std::vector<TankModel *> &ennemies, int grid_size, glm::mat4 &floor, PhysicModel &floorModel, std::vector<PhysicModel *> &cactuses)
{
    for (int i = 0; i < bullets.size(); i++)
    {
        shader.setMatrix4("model", bullets[i].getModelMatrix(glm::vec3(1.0f)));
        bullets[i].DrawWithShader(shader, 1);
    }

    shader.setMatrix4("model", tankModel.getModelMatrix(glm::vec3(1.0f)));
    // shader.setMatrix4("projection", projection);
    // shader.setMatrix4("view", view);
    tankModel.DrawWithShader(shader, 1);

    for (auto enemy : ennemies)
    {
        shader.setMatrix4("model", glm::rotate(enemy->getModelMatrix(glm::vec3(1.0f)), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)));
        // shader.setMatrix4("projection", projection);
        // shader.setMatrix4("view", view);
        enemy->DrawWithShader(shader, 1);
    }
   
    for (auto cactus : cactuses)
    {
        shader.setMatrix4("model", cactus->getModelMatrix(glm::vec3(1.0f)));
        // shader.setMatrix4("projection", projection);
        // shader.setMatrix4("view", view);
        cactus->DrawWithShader(shader, 1);
    }

    floor = glm::mat4(1.0f);
    for (int i = 0; i < grid_size * grid_size; i++)
    {
        if (i % grid_size != 0)
            floor = glm::translate(floor, glm::vec3(0.0f, 0.0f, 2.0f));
        else
        {
            floor = glm::mat4(1.0f);
            floor = glm::translate(floor, glm::vec3(-grid_size + 2.0f * i / grid_size, 0.0f, -grid_size));
        }
        shader.setMatrix4("model", floor);
        floorModel.DrawWithShader(shader, 1);
    }
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
