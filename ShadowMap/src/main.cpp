
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
#include <random>

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "Physics.hpp"
#include "DepthBuffer.hpp"
#include "Render.hpp"

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)

unsigned int WIDTH = 1600;
unsigned int HEIGHT = 1200;

GLFWwindow *InitDefaults();
void InitPhysics(btDiscreteDynamicsWorld **dynamicsWorld);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void calculateDeltaTime();
void throwRandomBall();
void shotTheBall(bool fast);
void shotTheCube(bool fast);
void shotTheDonut(bool fast);
void ballCollisionCB(btRigidBody *rb);
void print_FPS();
void pickEntity();

PickingFramebuffer *pickingBuffer;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(INITIAL_POS);

// Entity Manager
GE::EntityManager EntManager;

// PHYSICS
GE::Physics WorldPhysics;

// RENDER
GE::Render render{EntManager, (int)WIDTH, (int)HEIGHT};

// Models
const Model *sphere_model;
const Model *ground_model;
const Model *cube_model;
const Model *donut_model;
const Model *donut_hitbox;
const Model *halfdonut_model;

int main(int argc, char **argv)
{
    GLFWwindow *window = InitDefaults();

    // Skybox
    int screen_w, screen_h;
    glfwGetWindowSize(window, &screen_w, &screen_h);
    Skybox skybox{screen_w, screen_h};

    // build and compile shaders
    // -------------------------
    Shader reflection_shader("shaders/cube_reflection.vs", "shaders/cube_reflection.fs");
    Shader textured_reflection_shader("shaders/cube_texturedreflection.vs", "shaders/cube_texturedreflection.fs");
    Shader texture_shader("shaders/vertexshader.vs", "shaders/fragmentshader.fs");
    Shader framebufferShader("shaders/framebuffershader.vs", "shaders/framebuffer.fs");
    Shader framebufferInvertShader("shaders/framebuffershader.vs", "shaders/framebufferinvert.fs");
    Shader kernelShader("shaders/framebuffershader.vs", "shaders/kernels.fs");

    Shader depthMapRender("shaders/depthMapRender.vs", "shaders/depthMapRender.fs");

    Shader shadowMapShader("shaders/shadowmap.vs", "shaders/shadowmap.fs");
    Shader shadowMapShader2("shaders/shadowmap2.vs", "shaders/shadowmap2.fs");

    Shader pickingShader("shaders/pickingVertShader.vs", "shaders/pickingFragShader.fs");

    // load models
    // -----------
    [[maybe_unused]] std::string backpack = "models/backpack/backpack.obj";
    [[maybe_unused]] std::string skull_model = "models/skull/skull.obj";
    [[maybe_unused]] std::string skull_model2 = "models/skull/skull2.obj";
    [[maybe_unused]] std::string skull_model3 = "models/skull/skull3.obj";
    [[maybe_unused]] std::string skull_hitbox_model = "models/skull/skull_hitbox.obj";

    // FRAMEBUFFERSddd
    Framebuffer framebuffer(WIDTH, HEIGHT);
    PickingFramebuffer pickingFramebuffer(WIDTH, HEIGHT);
    pickingBuffer = &pickingFramebuffer;
    Depthbuffer depthMap(4096, 4096, WIDTH, HEIGHT);

    // Models
    ground_model = EntManager.createModel(new FlatTerrain());
    sphere_model = EntManager.createModel(new SphereModel());
    cube_model = EntManager.createModel(new CubeModel());
    donut_model = EntManager.createModel(new DonutModel("donut_model"));
    halfdonut_model = EntManager.createModel(new HalfDonutModel("half_donut_model"));

    auto rotation = glm::rotate(glm::mat4(1), glm::radians(-0.0f), {1, 0, 0});
    rotation = glm::rotate(rotation, glm::radians(0.0f), {0, 0, 1});
    auto position = glm::vec3{0, 00, 00};
    auto dimensions = glm::vec2{20, 20};

    WorldPhysics.add2DBOX(EntManager.createEntity<Ground>(ground_model, position, dimensions, rotation),
                          position, dimensions, rotation, btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);

    // OUR LIGHT
    glm::vec3 LightInitPosition{0, 40, 40};
    glm::vec3 LightInitDirection{0, 0, 0};
    float near{0.5};
    float far{150.0f};
    Light light{LightInitPosition, near, far, LightInitDirection};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  RENDER LOOP                                                                                       //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        calculateDeltaTime();
        WorldPhysics.step(deltaTime);
        WorldPhysics.updateBodies();
        WorldPhysics.Collision();

        light.mPosition = glm::vec3(50 * glm::sin(3.14f / 8.0f * lastFrame), LightInitPosition.y, 50 * glm::cos(3.14f / 8.0f * lastFrame));
        // print_FPS();
        processInput(window);

        pickingFramebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render.RenderScene(&pickingShader, camera, light);
        pickingFramebuffer.Unbind();

        depthMap.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render.RenderScene(&shadowMapShader, camera, light);
        depthMap.Unbind();

        framebuffer.Bind();
        depthMap.BindTexture(shadowMapShader2);
        render.RenderScene(&shadowMapShader2, camera, light);
        skybox.Draw(camera);
        framebuffer.DrawFrame(framebufferShader);

        WorldPhysics.updateBodies();

        glfwSwapInterval(1);
        glfwSwapBuffers(window);
        glfwPollEvents();

        print_FPS();

        // EntManager.update();
    }
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
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, 0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewInit();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return window;
}

void processInput(GLFWwindow *window)
{
    static float cadency = 20.0f;

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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.reset();

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        static float ts_E_key_press_last = 00.0f;
        float now = glfwGetTime();
        if (now - ts_E_key_press_last > 1.0f)
        {
            ts_E_key_press_last = glfwGetTime();
            cadency = (cadency == 20.0f) ? 3.0f : 20.0f;
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
    {
        static float ts_left_button_press_last = 0.0f;
        float now = glfwGetTime();
        if (now - ts_left_button_press_last > 1 / cadency)
        {
            ts_left_button_press_last = glfwGetTime();
            bool fast = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS;
            shotTheBall(fast);
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        static float ts_right_button_press_last = 00.0f;
        float now = glfwGetTime();
        if (now - ts_right_button_press_last > 1 / cadency)
        {
            ts_right_button_press_last = glfwGetTime();
            bool fast = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS;

            shotTheCube(fast);
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
    {
        static float ts_middle_button_press_last = 00.0f;
        float now = glfwGetTime();
        if (now - ts_middle_button_press_last > 1 / cadency)
        {
            ts_middle_button_press_last = glfwGetTime();
            bool fast = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS;
            shotTheDonut(fast);
        }
    }

    camera.MovementSpeed = MIN(camera.MovementSpeed, 400.0);
    camera.MovementSpeed = MAX(camera.MovementSpeed, 0.1);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        pickEntity();
}

void calculateDeltaTime()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void shotTheBall(bool fast)
{
    glm::vec3 camDir{camera.GetViewDirection()};
    glm::vec3 position{camera.Position + (1.0f + 3.0f) * camDir};
    glm::vec3 v = fast ? 200.0f * camDir : glm::vec3{0};
    int max = 2;
    float radius = 1.0f;
    Ball &b = EntManager.createEntity<Ball>(sphere_model, position, v, radius);
    WorldPhysics.addSphereBOX(b, position, radius, v, {});
    auto pointer = b.body->getUserPointer();
    /*
    printf("Sphere created in {%03.2f, %03.2f, %03.2f} dir {%03.2f, %03.2f, %03.2f}\n",
    position.x, position.y, position.z, glm::normalize(camDir).x, glm::normalize(camDir).y, glm::normalize(camDir).z);
    */
}

void shotTheCube(bool fast)
{
    glm::vec3 camDir{camera.GetViewDirection()};
    glm::vec3 position{camera.Position + (1.0f + 3.0f) * camDir};
    glm::vec3 v = fast ? 200.0f * camDir : glm::vec3{0};
    glm::mat4 rot = camera.GetViewMatrix();
    glm::vec3 size{1, 1, 1};
    WorldPhysics.addRigidBOX(EntManager.createEntity<ThrowingCube>(cube_model, position, size, v, rot), position, size, v, rot, {});
}

void shotTheDonut(bool fast)
{
    glm::vec3 camDir{camera.GetViewDirection()};
    glm::vec3 position{camera.Position + (1.0f + 3.0f) * camDir};
    glm::vec3 v = fast ? 200.0f * camDir : glm::vec3{0};
    glm::mat4 rot = camera.GetViewMatrix();
    glm::vec3 size{1, 1, 1};
    float radius = 1.0;
    auto pos_vector = donut_model->GetRawPositions();
    WorldPhysics.addRigidBoxFromModel(EntManager.createEntity<Donut>(donut_model, position, v, radius), donut_model->name, pos_vector.data(), pos_vector.size(), position, v, rot, {});
}

void print_FPS()
{
    printf("FPS: %04d\r", (int)(1.0 / deltaTime));
}

void ballCollisionCB(btRigidBody *rb)
{
    auto pos = rb->getWorldTransform().getOrigin();
    printf("This ball collided in { %.2f, %.2f, %.2f }\n", pos.getX(), pos.getY(), pos.getZ());
}

void pickEntity()
{
    PickingFramebuffer::PixelInfo pixelinfo = pickingBuffer->ReadPixel(WIDTH / 2, HEIGHT / 2);

    printf("Object ID is: %d \n", (int)pixelinfo.ObjectID);
    printf("Draw ID is: %d \n", (int)pixelinfo.DrawID);
    printf("Prim ID is: %d \n", (int)pixelinfo.PrimID);

    unsigned int id = (int)pixelinfo.ObjectID;
    if ((int)pixelinfo.DrawID == 3535)
        for (auto *e : EntManager.Entities)
        {
            if (e->m_id == id)
            {
                e->selected = true;
                EntManager.selected = e;

                if (dynamic_cast<const Ball *>(e))
                {
                    printf("Object is a BALL \n");
                }
                if (dynamic_cast<const Ground *>(e))
                {
                    printf("Object is a GROUND \n");
                }
                if (dynamic_cast<const ThrowingCube *>(e))
                {
                    printf("Object is a CUBE \n");
                }
                if (dynamic_cast<const Donut *>(e))
                {
                    printf("Object is a DONUT \n");
                }
            }
            else
            {
                e->selected = false;
            }
        }
    else if ((int)pixelinfo.DrawID == 5353)
    {
        printf("[Object already selected\n");    
    }
    else
    {
        printf("[UNKNOWN] object\n");
        if (EntManager.selected)
        {
            EntManager.selected->selected = false;
            EntManager.selected = nullptr;
        }
    }
}
/*
    // ADD GROUNDw

    auto rotation = glm::rotate(glm::mat4(1), glm::radians(-90.0f), {1, 0, 0});
    rotation = glm::rotate(rotation, glm::radians(45.0f), {0, 0, 1});
    auto position = glm::vec3{0, 10, 10};
    auto dimensions = glm::vec2{100, 10};

    WorldPhysics.add2DBOX(
        EntManager.createEntity<Ground>(ground_model, position, dimensions, rotation),
        position,
        dimensions,
        rotation,
        btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);

    rotation = glm::rotate(glm::mat4(1), glm::radians(90.0f), {1, 0, 0});
    rotation = glm::rotate(rotation, glm::radians(45.0f), {0, 0, 1});
    position = glm::vec3{0, 10, -10};
    dimensions = glm::vec2{100, 10};

    WorldPhysics.add2DBOX(
        EntManager.createEntity<Ground>(ground_model, position, dimensions, rotation),
        position,
        dimensions,
        rotation,
        btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);

    rotation = glm::rotate(glm::mat4(1), glm::radians(00.0f), {1, 0, 0});
    rotation = glm::rotate(rotation, glm::radians(45.0f), {0, 0, 1});
    position = glm::vec3{0, 0, 0};
    dimensions = glm::vec2{100, 10};

    WorldPhysics.add2DBOX(
        EntManager.createEntity<Ground>(ground_model, position, dimensions, rotation),
        position,
        dimensions,
        rotation,
        btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);

    rotation = glm::rotate(glm::mat4(1), glm::radians(90.0f), {0, 0, 1});
    rotation = glm::rotate(rotation, glm::radians(45.0f), {0, 0, 1});
    position = glm::vec3{-100, 10, 0};
    dimensions = glm::vec2{100, 10};

    WorldPhysics.add2DBOX(
        EntManager.createEntity<Ground>(ground_model, position, dimensions, rotation),
        position,
        dimensions,
        rotation,
        btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);

*/