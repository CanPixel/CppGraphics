#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Utils.h"
    
// Forward declarations
void setupResources();
void renderTerrain(glm::vec3 position, glm::mat4 view, glm::mat4 projection);
void renderSkybox(glm::mat4 view, glm::mat4 projection);
void renderCube(unsigned int shader, glm::vec3 position, glm::vec3 rotation, float scale, glm::mat4 view, glm::mat4 projection);
void renderModel(Model * model, unsigned int shader, glm::vec3 position, glm::vec3 rotation, float scale, glm::mat4 view, glm::mat4 projection);
void renderLightDirection(unsigned int shader);
void renderToBuffer(unsigned int frameBufferTo, unsigned int colorBufferFrom, unsigned int shader);
void createFrameBuffer(int width, int height, unsigned int& frameBufferID, unsigned int& colorBufferID, unsigned int& depthBufferID);
void renderQuad();

// Variables
glm::vec3 cameraPosition(0, 5, 0), cameraForward(0, 0, 1), cameraUp(0, 1, 0);
float mouseX, mouseY;
int width, height;

unsigned int plane, planeSize, VAO, cubeSize;
unsigned int terrainProgram, skyProgram, modelProgram, blitProgram, charrProgram, fxProgram, customProgram;
unsigned int terrainWidth, terrainHeight;

/// need 24 vertices for normal/uv-mapped Cube
float vertices[] = {
    // positions            //colors            // tex coords   // normals
    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
    -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,

    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,

    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 2.f,       0.f, 0.f, 1.f,

    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
    -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   -1.f, 0.f,      -1.f, 0.f, 0.f,

    -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,

    -0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,

    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,

    -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,

    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,

    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       0.f, 1.f, 0.f
};
unsigned int indices[] = {  // note that we start from 0!
    // DOWN
    0, 1, 2,   // first triangle
    0, 2, 3,    // second triangle
    // BACK
    14, 6, 7,   // first triangle
    14, 7, 15,    // second triangle
    // RIGHT
    20, 4, 5,   // first triangle
    20, 5, 21,    // second triangle
    // LEFT
    16, 8, 9,   // first triangle
    16, 9, 17,    // second triangle
    // FRONT
    18, 10, 11,   // first triangle
    18, 11, 19,    // second triangle
    // UP
    22, 12, 13,   // first triangle
    22, 13, 23,    // second triangle
};

Model* backpackModel;
Model* asteroid;
Model* mars;

glm::vec3 skyBottom(0.2, 0.2, 0.4);

unsigned int backpackDiffID, backpackAOID, backpackNormalID, backpackRoughnessID, backpackSpecularID;
unsigned int woodTexID, woodNormalID;
unsigned int heightmapID;
unsigned int heightNormalID;
unsigned int dirtID, sandID, grassID, rockID, snowID;

unsigned int asteroidDiffuseID, asteroidNormalID;
unsigned int marsDiffuseID, marsHeightmapID, marsSpecularID;

const int SHADER_AMOUNT = 3;
int switchShader = 0;
float switchTimer = 0;

void handleInput(GLFWwindow* window, float deltaTime) {
    static bool w, s, a, d, space, ctrl;
    static double cursorX = -1, cursorY = -1, lastCursorX, lastCursorY;
    static float pitch, yaw;
    static float speed = 100.0f;

    float sensitivity = 50.0f * deltaTime;
    float step = speed * deltaTime;

    if (switchTimer > 0) switchTimer -= deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)				w = true;
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)		w = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)				s = true;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)		s = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)				a = true;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)		a = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)				d = true;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)		d = false;

    //Shader switch with E
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && switchTimer <= 0) {
        switchShader++;
        if (switchShader > SHADER_AMOUNT) switchShader = 0;
        switchTimer = 0.5f;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)				space = true;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)		space = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)		ctrl = true;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)	ctrl = false;

    if (cursorX == -1) glfwGetCursorPos(window, &cursorX, &cursorY);

    lastCursorX = cursorX;
    lastCursorY = cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    mouseX = (float)cursorX;
    mouseY = (float)cursorY;

    glm::vec2 mouseDelta(cursorX - lastCursorX, cursorY - lastCursorY);

    // TODO: calculate rotation & movement
    yaw -= mouseDelta.x * sensitivity;
    pitch += mouseDelta.y * sensitivity;
    pitch = glm::max(glm::min(pitch, 90.0f), -90.0f);

    if (pitch < -90.0f) pitch = -90.0f;
    else if (pitch > 90.0f)pitch = 90.0f;

    if (yaw < 180.0f) yaw += 360;
    else if (yaw > 180.0f) yaw -= 360;

    glm::vec3 euler(glm::radians(pitch), glm::radians(yaw), 0);
    glm::quat q(euler);

    // update camera position / forward & up
    glm::vec3 translation(0, 0, 0);

    //movement
    if (w) translation.z += speed * deltaTime;
    if (a) translation.x += speed * deltaTime;
    if (s) translation.z -= speed * deltaTime;
    if (d) translation.x -= speed * deltaTime;

    cameraPosition += q * translation;
    cameraUp = q * glm::vec3(0, 1, 0);
    cameraForward = q * glm::vec3(0, 0, 1);
}

int main() {
    static double previousT = 0;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    width = 1280;
    height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "Graphics Programming", NULL, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetWindowCenter(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);
    
    setupResources();

    unsigned int frameBuf1, frameBuf2;
    unsigned int colorBuf1, colorBuf2;
    unsigned int depthBuf1, depthBuf2;
    createFrameBuffer(width, height, frameBuf1, colorBuf1, depthBuf1);
    createFrameBuffer(width, height, frameBuf2, colorBuf2, depthBuf2);

    while (!glfwWindowShouldClose(window)) {
        double t = glfwGetTime();
        float deltaTime = t - previousT;
        previousT = t;

        handleInput(window, deltaTime);

        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), width / (float)height, 0.1f, 1000.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuf1);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderSkybox(view, projection);
            renderTerrain(glm::vec3(0, 100, 0), view, projection);
            renderCube(modelProgram, glm::vec3(0, -3, 20), glm::vec3(0, glm::radians(t * 90.0f), 0), 15, view, projection);

            renderModel(backpackModel, modelProgram, glm::vec3(10, 24, 5), glm::vec3(0, glm::radians(45.0f), 0), 8, view, projection);
            renderModel(asteroid, modelProgram, glm::vec3(-60, 80, 10), glm::vec3(0, glm::radians(t * 70.0f), 0), 6, view, projection);
            renderModel(mars, modelProgram, glm::vec3(0, 110, 0), glm::vec3(glm::radians(t * 20.0f), glm::radians(t * -55.0f), glm::radians(t * 10.0f)), 10, view, projection);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //POST FX
        renderToBuffer(frameBuf2, colorBuf1, blitProgram);

        unsigned int finProg = charrProgram;
        switch (switchShader) {
        case 0:
            finProg = charrProgram;
            break;
        case 1:
            finProg = fxProgram;
            break;
        case 2:
            finProg = customProgram;
            break;
        }
        renderToBuffer(0, colorBuf1, finProg);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void setupResources() {
    cubeSize = sizeof(indices);
    planeSize = sizeof(indices);

    stbi_set_flip_vertically_on_load(true);
    backpackModel = new Model("backpack/backpack.obj");
    stbi_set_flip_vertically_on_load(false);
    asteroid = new Model("asteroid/asteroid.obj");
    stbi_set_flip_vertically_on_load(true);
    mars = new Model("mars/Mars 2K.obj");

    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    //  SETUP OBJECT
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Building array buffer
    int stride = sizeof(float) * 11;
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    //normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //  END SETUP OBJECT

    //TERRAIN
    plane = GeneratePlane("terrain/heightmap.png", GL_RGBA, 4, 1.0f, 1.0f, planeSize, heightmapID, terrainWidth, terrainHeight);
    heightNormalID = loadTexture("terrain/heightnormal.png", GL_RGBA, 4);
    dirtID = loadTexture("terrain/dirt.jpg", GL_RGB, 3);
    sandID = loadTexture("terrain/sand.jpg", GL_RGB, 3);
    grassID = loadTexture("terrain/grass.png", GL_RGBA, 4);
    rockID = loadTexture("terrain/rock.jpg", GL_RGB, 3);
    snowID = loadTexture("terrain/snow.jpg", GL_RGB, 3);

    //backpack
    backpackDiffID = loadTexture("backpack/texture_diffuse1.jpg", GL_RGB, 3);
    backpackNormalID = loadTexture("backpack/texture_normal1.png", GL_RGB, 3);
    backpackRoughnessID = loadTexture("backpack/texture_roughness1.jpg", GL_RGB, 3);
    backpackAOID = loadTexture("backpack/texture_ao1.jpg", GL_RGB, 3);
    backpackSpecularID = loadTexture("backpack/texture_specular1.jpg", GL_RGB, 3);
    
    //SETUP SHADER PROGRAM
    char* vertexSource;
    loadFromFile("vertexShader.shader", &vertexSource);
    char* fragmentSource;
    loadFromFile("fragmentShader.shader", &fragmentSource);

    //Shader creation
    unsigned int vertSky, fragSky;
    CreateShader("vertexShaderSky.shader", GL_VERTEX_SHADER, vertSky);
    CreateShader("fragmentShaderSky.shader", GL_FRAGMENT_SHADER, fragSky);
    unsigned int vertModel, fragModel;
    CreateShader("vertModel.shader", GL_VERTEX_SHADER, vertModel);
    CreateShader("fragModel.shader", GL_FRAGMENT_SHADER, fragModel);
    unsigned int vertImg, fragImg;
    CreateShader("vertImg.shader", GL_VERTEX_SHADER, vertImg);
    CreateShader("fragImg.shader", GL_FRAGMENT_SHADER, fragImg);
    unsigned int fragFX;
    CreateShader("fragFX.shader", GL_FRAGMENT_SHADER, fragFX);
    unsigned int fragChrabb;
    CreateShader("fragChrabb.shader", GL_FRAGMENT_SHADER, fragChrabb);
    unsigned int fragCustom;
    CreateShader("fragCustom.shader", GL_FRAGMENT_SHADER, fragCustom);
    
    //LOAD TEXTURES
    woodTexID = loadTexture("backpack/Wood10.jpg", GL_RGB, 3);
    woodNormalID = loadTexture("backpack/Wood10_normal.jpg", GL_RGB, 3);
    
    asteroidDiffuseID = loadTexture("asteroid/texture_diffuse1.png", GL_RGBA, 4);
    asteroidNormalID = loadTexture("asteroid/texture_normal1.png", GL_RGB, 3);
    marsDiffuseID = loadTexture("mars/texture_diffuse1.png", GL_RGBA, 4);
    marsHeightmapID = loadTexture("mars/texture_normal1.png", GL_RGBA, 4);
    marsSpecularID = loadTexture("mars/texture_specular1.png", GL_RGBA, 4);
    
    unsigned int vertID = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertID, 1, &vertexSource, nullptr);
    glShaderSource(fragID, 1, &fragmentSource, nullptr);

    int success;
    char infoLog[512];
    glCompileShader(vertID);
    glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    glCompileShader(fragID);
    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    terrainProgram = glCreateProgram();
    glAttachShader(terrainProgram, vertID);
    glAttachShader(terrainProgram, fragID);
    glLinkProgram(terrainProgram);

    skyProgram = glCreateProgram();
    glAttachShader(skyProgram, vertSky);
    glAttachShader(skyProgram, fragSky);
    glLinkProgram(skyProgram);

    modelProgram = glCreateProgram();
    glAttachShader(modelProgram, vertModel);
    glAttachShader(modelProgram, fragModel);
    glLinkProgram(modelProgram);

    blitProgram = glCreateProgram();
    glAttachShader(blitProgram, vertImg);
    glAttachShader(blitProgram, fragImg);
    glLinkProgram(blitProgram);

    fxProgram = glCreateProgram();
    glAttachShader(fxProgram, vertImg);
    glAttachShader(fxProgram, fragFX);
    glLinkProgram(fxProgram);

    charrProgram = glCreateProgram();
    glAttachShader(charrProgram, vertImg);
    glAttachShader(charrProgram, fragChrabb);
    glLinkProgram(charrProgram);

    customProgram = glCreateProgram();
    glAttachShader(customProgram, vertImg);
    glAttachShader(customProgram, fragCustom);
    glLinkProgram(customProgram);

    glDeleteShader(vertID);
    glDeleteShader(fragID);
    glDeleteShader(vertSky);
    glDeleteShader(fragSky);
    glDeleteShader(vertModel);
    glDeleteShader(fragModel);
    glDeleteShader(vertImg);
    glDeleteShader(fragImg);
    glDeleteShader(fragFX);
    glDeleteShader(fragChrabb);
    glDeleteShader(fragCustom);
    //END SETUP SHADER PROGRAM

    glUseProgram(terrainProgram);
    glUniform1i(glGetUniformLocation(terrainProgram, "heightmap"), 0);
    glUniform1i(glGetUniformLocation(terrainProgram, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(terrainProgram, "dirt"), 2);
    glUniform1i(glGetUniformLocation(terrainProgram, "sand"), 3);
    glUniform1i(glGetUniformLocation(terrainProgram, "grass"), 4);

    // OPENGL SETTINGS //
    glEnable(GL_CULL_FACE);
}

void renderCube(unsigned int shader, glm::vec3 position, glm::vec3 rotation, float scale, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shader);
    glEnable(GL_DEPTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // World matrix init
    glm::mat4 world = glm::mat4(1);
    world = glm::translate(world, position);
    world = glm::scale(world, glm::vec3(scale));
    glm::quat q(rotation);
    world = world * glm::toMat4(q);

    //Shader settings
    glUniformMatrix4fv(glGetUniformLocation(shader, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shader, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

    renderLightDirection(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, woodNormalID);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}
void renderTerrain(glm::vec3 position, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(terrainProgram);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 world = glm::mat4(1.f);
    world = glm::translate(world, position * glm::vec3(-1, -1, -1) - glm::vec3(terrainWidth / 2, 0, terrainHeight / 2));

    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(terrainProgram, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniform3fv(glGetUniformLocation(terrainProgram, "skyBottom"), 1, glm::value_ptr(skyBottom));

    renderLightDirection(terrainProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmapID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightNormalID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dirtID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, sandID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, grassID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, rockID);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, snowID);

    glBindVertexArray(plane);
    glDrawElements(GL_TRIANGLES, planeSize, GL_UNSIGNED_INT, 0);
}
void renderSkybox(glm::mat4 view, glm::mat4 projection) {
    glUseProgram(skyProgram);
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 world = glm::mat4(1.f);
    world = glm::translate(world, cameraPosition);

    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(skyProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
    glUniform3fv(glGetUniformLocation(skyProgram, "skyBottom"), 1, glm::value_ptr(skyBottom));

    renderLightDirection(skyProgram);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cubeSize, GL_UNSIGNED_INT, 0);
}
void renderModel(Model* model, unsigned int shader, glm::vec3 position, glm::vec3 rotation, float scale, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shader);
    glEnable(GL_DEPTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // world matrix bouwen
    glm::mat4 world = glm::mat4(1);
    world = glm::translate(world, position);
    world = glm::scale(world, glm::vec3(scale));
    glm::quat q(rotation);
    world = world * glm::toMat4(q);

    // shader instellingen
    glUniformMatrix4fv(glGetUniformLocation(shader, "world"), 1, GL_FALSE, glm::value_ptr(world));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shader, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
    glUniform3fv(glGetUniformLocation(shader, "skyBottom"), 1, glm::value_ptr(skyBottom));

    renderLightDirection(shader);

    model->Draw(shader);
}

void renderLightDirection(unsigned int shader) {
    float t = glfwGetTime() / 2;
    glUniform3f(glGetUniformLocation(shader, "lightDirection"), glm::cos(t), -0.5f, glm::sin(t));
}

void createFrameBuffer(int width, int height, unsigned int& frameBufferID, unsigned int& colorBufferID, unsigned int& depthBufferID) {
    // Frame buffer
    glGenFramebuffers(1, &frameBufferID);

    // Color buffer
    glGenTextures(1, &colorBufferID);
    glBindTexture(GL_TEXTURE_2D, colorBufferID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    // Depth buffer
    glGenRenderbuffers(1, &depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderToBuffer(unsigned int frameBufferTo, unsigned int colorBufferFrom, unsigned int shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferTo);

    glUseProgram(shader);
    glUniform2i(glGetUniformLocation(shader, "iResolution"), width, height);
    glUniform2i(glGetUniformLocation(shader, "iMouse"), mouseX, mouseY);
    glUniform1f(glGetUniformLocation(shader, "iTime"), (float)glfwGetTime());
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBufferFrom);

    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
    if (quadVAO == 0) {
        float quadVerts[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}