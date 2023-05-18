
// GLEW needs to be included first
#include <GL/glew.h>

// GLFW is included next
#include <GLFW/glfw3.h>
#include <iostream>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <btBulletDynamicsCommon.h>

#include <filesystem>

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "EntityManager.hpp"
#include "Physics.hpp"

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)

constexpr unsigned int WIDTH = 1600;
constexpr unsigned int HEIGHT = 1200;
constexpr float MAP_LIMITS = 500.0f;

GLFWwindow *InitDefaults();
void InitPhysics(btDiscreteDynamicsWorld **dynamicsWorld);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void calculateDeltaTime();
void print_FPS();

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float ballSPeed = 200.0f;

// camera
Camera camera(INITIAL_POS);

// SPHERE PROTOTYPE
SphereModel *prototype;

// CONTROLS
bool invert = false;
bool e_press = false;

// Entity Manager
EntityManager<Model> EntManager;

// PHYSICS
Physics WorldPhysics;

int main(int argc, char **argv)
{

    GLFWwindow *window = InitDefaults();

    // Skybox
    Skybox skybox;

    // build and compile shaders
    // -------------------------
    Shader reflection_shader("shaders/cube_reflection.vs", "shaders/cube_reflection.fs");
    Shader textured_reflection_shader("shaders/cube_texturedreflection.vs", "shaders/cube_texturedreflection.fs");
    
    Shader texture_shader("shaders/vertexshader.vs", "shaders/fragmentshader.fs");
    Shader framebufferShader("shaders/framebuffershader.vs", "shaders/framebuffer.fs");
    Shader framebufferInvertShader("shaders/framebuffershader.vs", "shaders/framebufferinvert.fs");
    Shader kernelShader("shaders/framebuffershader.vs", "shaders/kernels.fs");

    // load models
    // -----------
    // [[maybe_unused]] std::string backpack = "models/backpack/backpack.obj";
    // [[maybe_unused]] std::string skull_model = "models/skull/skull.obj";
    [[maybe_unused]] std::string skull_model2 = "models/skull/skull2.obj";
    // [[maybe_unused]] std::string skull_model3 = "models/skull/skull3.obj";
    [[maybe_unused]] std::string skull_hitbox_model = "models/skull/skull_hitbox.obj";
    [[maybe_unused]] std::string punk_model = "models/Punk.obj";

    // SPHERE PROTOTYPE
    prototype = new SphereModel();
    prototype->setShader(&texture_shader);

    // FRAMEBUFFER
    Framebuffer framebuffer(WIDTH, HEIGHT);

    // Punk Model

    Model* punk = new Model(punk_model, false);
    punk->setShader(&textured_reflection_shader);
    punk->body = WorldPhysics.addRigidBox(glm::vec3(10,3,10), glm::vec3(0.5,2,0.5), btCollisionObject::CollisionFlags{});
    EntManager.Entities.push_back(punk);

    // SKULL MODEL
    Model *skull = new Model(skull_model2, false);
    Model skull_hitbox{skull_hitbox_model, false};
    skull->setShader(&textured_reflection_shader);
    skull->body = WorldPhysics.addRigidBoxFromModel( skull_hitbox, glm::vec3(0,100,0), btCollisionObject::CollisionFlags{});
    EntManager.Entities.push_back(skull);

    // // SKULLS
    // for (int i = 0; i < 3; ++i)
    //     for (int j = 0; j < 3; ++j)
    //     {
    //         Model *s = new Model{*skull};
    //         s->setShader(&texture_shader);
    //         s->body = WorldPhysics.addRigidBoxFromModel( skull_hitbox, glm::vec3(-40 + i*10,600,-40 + j*10), btCollisionObject::CollisionFlags{});
    //         EntManager.Entities.push_back(s);
    //     }

    // GROUND MODEL
    Model *groundModel = new Model();
    groundModel->meshes.push_back(CubeShape());
    groundModel->meshes[0].textures.emplace_back("textures/terrain.png", true);
    groundModel->setShader(&texture_shader);
    groundModel->scale = glm::vec3(1000.0f, 2.0f, 1000.0f);
    groundModel->body = WorldPhysics.addRigidBox(glm::vec3(0, -2.0f, 0), glm::vec3(1000.0f, 2.0f, 1000.0f), btCollisionObject::CF_STATIC_OBJECT);
    EntManager.Entities.push_back(groundModel);

    // SPHERES
    for (int i = 0; i < 4; ++i)
        for (int j = 400; j < 404; ++j)
            for (int k = 0; k < 4; ++k)
            {
                EntManager.Entities.push_back(new SphereModel(*prototype, glm::vec3(i * 2, j, k * 2)));
                auto s = EntManager.Entities.back();
                WorldPhysics.dynamicsWorld->addRigidBody(s->body);
            }
    printf("Entities: %03ld\n", EntManager.Entities.size());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  RENDER LOOP                                                                                       //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        calculateDeltaTime();
        print_FPS();
        processInput(window);

        // Process Physics
        WorldPhysics.dynamicsWorld->stepSimulation(deltaTime);

        // Bind Framebuffer
        framebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render ENtities
        EntManager.DrawEntities(WorldPhysics, camera);
        // Update Entitiy vector(remove)
        EntManager.update();

        // render SKYBOX
        skybox.Draw(camera);

        // DRAW THE PICTURE IN THE SCREEN
        if (!invert)
            framebuffer.DrawFrame(framebufferShader);
        else
            framebuffer.DrawFrame(framebufferInvertShader);

        glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    glfwTerminate();

    return 0;
}

GLFWwindow *InitDefaults()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL TESTS", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, 0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewInit();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return window;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if ((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS))
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        camera.MovementSpeed *= 1.0 + 2 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        camera.MovementSpeed /= 1.0 + 2 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (!e_press)
            invert = !invert;
        e_press = true;
    }
    else
        e_press = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.reset();
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
        ballSPeed *= 1.0 + 2 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL) == GLFW_PRESS)
        ballSPeed /= 1.0 + 2 * deltaTime;

    camera.MovementSpeed = MIN(camera.MovementSpeed, 400.0);
    camera.MovementSpeed = MAX(camera.MovementSpeed, 0.1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    static float lastX = WIDTH / 2.0f;
    static float lastY = HEIGHT / 2.0f;
    static bool firstMouse = true;

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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(static_cast<float>(yoffset));
    camera.MovementSpeed += 1000.0 * yoffset * deltaTime;
    printf("Speed: %f\n", camera.MovementSpeed);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        glm::vec3 v = glm::vec3(0);
        glm::vec3 camDir = camera.GetViewDirection();
        glm::vec3 position = glm::vec3(camera.Position + (prototype->radius + 3.0f) * camDir);

        if (button != GLFW_MOUSE_BUTTON_RIGHT)
        {
            v = ballSPeed * camDir;
        }
        Model* s = new SphereModel(*prototype, position, v);
        s->body = WorldPhysics.addSphere( position, prototype->radius, v,btCollisionObject::CollisionFlags{} );
        EntManager.Entities.push_back(s);
    }
}

void calculateDeltaTime()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void print_FPS()
{
    printf("FPS: %3d\r", (int) (1.0 / deltaTime) );
}
