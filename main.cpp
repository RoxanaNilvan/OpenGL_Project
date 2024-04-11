#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

int glWindowWidth = 1024;
int glWindowHeight = 768;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// matrices
glm::mat4 model;
glm::mat4 modelCangur;
glm::mat4 modelCoada;
glm::mat4 modelCoada2;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;


// camera
gps::Camera myCamera(
    glm::vec3(-14.0f, 2.0f, 19.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

gps::Camera initilaCamera(
    glm::vec3(-14.0f, 2.0f, 19.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


bool cameraAnimationOn = true;

GLfloat cameraSpeed = 0.3f;
GLfloat cameraSpeed2 = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D scena;
GLfloat angle;
gps::Model3D cangur;
gps::Model3D coada;
gps::Model3D coada2;
gps::Model3D lightCube;
gps::Model3D screenQuad;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

// skyBox
gps::SkyBox mySkyBoxDay;
gps::SkyBox mySkyBoxNight;
gps::Shader skyboxShader;
bool isNight = false;

//mouse
bool firstMouse = true;
float lastX;
float lastY;
float pitch = 0.0f;
float yaw = 90.0f;

//fog
GLfloat fogDensity;
GLuint fogLoc;

// animatie cangur
bool cangurAnimationOn = false;
bool jumpUp = true;
GLfloat cangurRotAngle = 0.0f;
GLfloat jumpHeight = 1.5087f;

// animatie tigru
bool coadaAnimationOn = false;
bool moveRight = true;
GLfloat coadaMove = 0.0f;

// animatie tigru
bool coadaAnimationOn2 = false;
bool moveRight2 = true;
GLfloat coadaMove2 = 0.0f;

//point lights
struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;
};

GLint firstCabinPositionLoc;
GLint firstCabinColorLoc;
GLint firstCabinConstantLoc;
GLint firstCabinLinearLoc;
GLint firstCabinQuadraticLoc;
GLint secondCabinPositionLoc;
GLint secondCabinColorLoc;
GLint secondCabinConstantLoc;
GLint secondCabinLinearLoc;
GLint secondCabinQuadraticLoc;
GLint thirdCabinPositionLoc;
GLint thirdCabinColorLoc;
GLint thirdCabinConstantLoc;
GLint thirdCabinLinearLoc;
GLint thirdCabinQuadraticLoc;

PointLight pointLights[3] = {
    {   glm::vec3(1.95103f, 1.0765f, 21.9865f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        0.09f,
        0.032f
    },
    {   glm::vec3(-5.93351f, 1.0765f, 37.3529f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        0.09f,
        0.032f
    },
    {   glm::vec3(-20.655f, 1.0765f, 20.9279f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        0.09f,
        0.032f
    }
};


//moving light
float angleY = 0.0f;
GLfloat lightAngle;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (pressedKeys[GLFW_KEY_N]) {
        isNight = true;
        lightColor = glm::vec3(0.0f, 0.0f, 0.0f); //night
        // send light color to shader
        myBasicShader.useShaderProgram();
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    if (pressedKeys[GLFW_KEY_M]) {
        isNight = false;
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        // send light color to shader
        myBasicShader.useShaderProgram();
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    //fog
    if (pressedKeys[GLFW_KEY_F]) {
        fogDensity += 0.002f;
        if (fogDensity >= 0.3f)
            fogDensity = 0.3f;
        myBasicShader.useShaderProgram();
        glUniform1fv(fogLoc, 1, &fogDensity);
    }

    if (pressedKeys[GLFW_KEY_G]) {
        fogDensity -= 0.002f;
        if (fogDensity <= 0.0f)
            fogDensity = 0.0f;
        myBasicShader.useShaderProgram();
        glUniform1fv(fogLoc, 1, &fogDensity);
    }

    // animatie cangur
    if (pressedKeys[GLFW_KEY_O]) {
        cangurAnimationOn = true;
    }

    if (pressedKeys[GLFW_KEY_P]) {
        cangurAnimationOn = false;
    }

    if (pressedKeys[GLFW_KEY_K]) {
        coadaAnimationOn = true;
        coadaAnimationOn2 = true;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        coadaAnimationOn = false;
        coadaAnimationOn2 = false;
    }

    if (pressedKeys[GLFW_KEY_C]) {
        cameraAnimationOn = !cameraAnimationOn;
    }

    //moduri de vizionare
    if (key == GLFW_KEY_1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (key == GLFW_KEY_2) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_3) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (key == GLFW_KEY_U) {
        showDepthMap = !showDepthMap;
    }

    if (key == GLFW_KEY_R) {
        angleY -= 1.0f;
    }

    if (key == GLFW_KEY_T) {
        angleY += 1.0f;
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_H]) {
        lightAngle += 1.0f;
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }


}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    float sensitivity = 0.01f;
    xoffset = xoffset * sensitivity;
    yoffset = yoffset * sensitivity;

    angle = angle + xoffset;
    pitch = pitch + yoffset;

    if (pitch > 80.0f) {
        pitch = 80.0f;
    }

    if (pitch < -80.0f) {
        pitch = -80.0f;
    }

    myCamera.rotate(pitch, angle);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}



void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCangur = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCangur = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Z]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_X]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (cangurAnimationOn) {
        cangurRotAngle += 0.3f;

        if (jumpUp) {
            jumpHeight += 0.1000f;
        }
        else {
            jumpHeight -= 0.1000f;
        }

        if (jumpHeight >= 1.5087f) {
            jumpUp = false;
        }

        if (jumpHeight < -0.0087f) {
            jumpUp = true;
        }

        modelCangur = glm::translate(glm::mat4(1.0f), glm::vec3(1.1879f, jumpHeight, -1.0587f));
        modelCangur = glm::rotate(modelCangur, glm::radians(cangurRotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (coadaAnimationOn) {
        if (moveRight) {
            coadaMove += 0.001f;
        }
        else {
            coadaMove -= 0.001f;
        }

        if (coadaMove > 0.002f) {
            moveRight = false;
        }

        if (coadaMove < -0.003f) {
            moveRight = true;
        }
        modelCoada = glm::translate(modelCoada, glm::vec3(coadaMove, 0.0f, 0.0f));
    }

    if (coadaAnimationOn2) {
        if (moveRight2) {
            coadaMove2 += 0.001f;
        }
        else {
            coadaMove2 -= 0.001f;
        }

        if (coadaMove2 > 0.002f) {
            moveRight2 = false;
        }

        if (coadaMove2 < -0.003f) {
            moveRight2 = true;
        }
        modelCoada2 = glm::translate(modelCoada2, glm::vec3(coadaMove2, 0.0f, coadaMove2));
    }
}

bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Project Core", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);

    glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}


void initOpenGLState() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    scena.LoadModel("models/scena/scena.obj");
    cangur.LoadModel("models/cangur/cangur.obj");
    coada.LoadModel("models/tigru/coadaTigru.obj");
    coada2.LoadModel("models/tigru2/coadaTigru2.obj");
    lightCube.LoadModel("models/cube/cube.obj");
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();

    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();

    depthShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
    depthShader.useShaderProgram();
}

void initSkybox() {
    std::vector<const GLchar*> facesN;

    facesN.push_back("skybox/night/nightsky_rt.tga");
    facesN.push_back("skybox/night/nightsky_lf.tga");
    facesN.push_back("skybox/night/nightsky_up.tga");
    facesN.push_back("skybox/night/nightsky_dn.tga");
    facesN.push_back("skybox/night/nightsky_bk.tga");
    facesN.push_back("skybox/night/nightsky_ft.tga");
    mySkyBoxNight.Load(facesN);

    std::vector<const GLchar*> facesD;
    facesD.push_back("skybox/day/desertsky_rt.tga");
    facesD.push_back("skybox/day/desertsky_lf.tga");
    facesD.push_back("skybox/day/desertsky_up.tga");
    facesD.push_back("skybox/day/desertsky_dn.tga");
    facesD.push_back("skybox/day/desertsky_bk.tga");
    facesD.push_back("skybox/day/desertsky_ft.tga");
    mySkyBoxDay.Load(facesD);
}

void initUniforms() {
    myBasicShader.useShaderProgram();


    // create model matrix for the hole scene
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //create model for the cangur
    modelCangur = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    modelCoada = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    modelCoada2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    

    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1fv(fogLoc, 1, &fogDensity);
    // send light color to shader

    // Transform light positions to view space
    glm::vec3 firstCabinViewPos = glm::vec3(model * glm::vec4(pointLights[0].position, 1.0));
    glm::vec3 secondCabinViewPos = glm::vec3(model * glm::vec4(pointLights[1].position, 1.0));
    glm::vec3 thirdCabinViewPos = glm::vec3(model * glm::vec4(pointLights[2].position, 1.0));

    firstCabinPositionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].position");
    glUniform3fv(firstCabinPositionLoc, 1, glm::value_ptr(firstCabinViewPos));
    firstCabinColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].color");
    glUniform3fv(firstCabinColorLoc, 1, glm::value_ptr(pointLights[0].color));
    firstCabinConstantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].constant");
    glUniform1f(firstCabinConstantLoc, pointLights[0].constant);
    firstCabinLinearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].linear");
    glUniform1f(firstCabinLinearLoc, pointLights[0].linear);
    firstCabinQuadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].quadratic");
    glUniform1f(firstCabinQuadraticLoc, pointLights[0].quadratic);

    secondCabinPositionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].position");
    glUniform3fv(secondCabinPositionLoc, 1, glm::value_ptr(secondCabinViewPos));
    secondCabinColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].color");
    glUniform3fv(secondCabinColorLoc, 1, glm::value_ptr(pointLights[1].color));
    secondCabinConstantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].constant");
    glUniform1f(secondCabinConstantLoc, pointLights[1].constant);
    secondCabinLinearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].linear");
    glUniform1f(secondCabinLinearLoc, pointLights[1].linear);
    secondCabinQuadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].quadratic");
    glUniform1f(secondCabinQuadraticLoc, pointLights[1].quadratic);

    thirdCabinPositionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].position");
    glUniform3fv(thirdCabinPositionLoc, 1, glm::value_ptr(thirdCabinViewPos));
    thirdCabinColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].color");
    glUniform3fv(thirdCabinColorLoc, 1, glm::value_ptr(pointLights[2].color));
    thirdCabinConstantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].constant");
    glUniform1f(thirdCabinConstantLoc, pointLights[2].constant);
    thirdCabinLinearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].linear");
    glUniform1f(thirdCabinLinearLoc, pointLights[2].linear);
    thirdCabinQuadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].quadratic");
    glUniform1f(thirdCabinQuadraticLoc, pointLights[2].quadratic);

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.2f, far_plane = 200.0f;
    glm::mat4 lightProjection = glm::ortho(-60.0f, 40.0f, -20.0f, 40.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;


    return lightSpaceTrMatrix;
}

void renderDesert(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));;
    //send desert model matrix data to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send desert normal matrix data to shader
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    scena.Draw(shader);
}

void renderCangur(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    // send cangur model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCangur));

    // send cangur normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    cangur.Draw(shader);
}

void renderTigru(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    // send elefant model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCoada));

    // send elefant normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    coada.Draw(shader);
}

void renderTigru2(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    // send elefant model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCoada2));

    // send elefant normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    coada2.Draw(shader);
}

int numarIteratii = 0;
int numarTotal = 0;

void cameraAnimation() {
    if (numarIteratii < 50) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed2);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myCamera.move(gps::MOVE_LEFT, cameraSpeed2);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        numarIteratii++;
        numarTotal++;
    }

    if (numarIteratii >= 50) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCoada2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        modelCangur = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        numarTotal++;
        numarIteratii--;
    }

    if (numarTotal == 504) {
        myCamera = initilaCamera;
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    depthShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderDesert(depthShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);

    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        renderDesert(myBasicShader, false);

        //draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
        model = glm::translate(model, 1.0f * lightDir);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //lightCube.Draw(lightShader);

        // render the desert
        // renderDesert(myBasicShader);
        renderCangur(myBasicShader);
        renderTigru(myBasicShader);
        renderTigru2(myBasicShader);

        if (cameraAnimationOn) {
            cameraAnimation();
        }

        if (isNight == false) {
            mySkyBoxDay.Draw(skyboxShader, view, projection);
        }
        else {
            mySkyBoxNight.Draw(skyboxShader, view, projection);
        }
    }
    
}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initSkybox();
    initFBO();

    glCheckError();

    // application loop
    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
