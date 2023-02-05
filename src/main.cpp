#include <iostream>
#include <format>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "glm/glm.hpp"
#include "Shader.h"
#include "Camera.h"

//functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void KwanGlfwStart();
void processInput(GLFWwindow* window);
void SetPointLights(const Shader& shader,const glm::vec3& position,  int place, bool isOn, bool isBig = false);
unsigned int loadTexture(char const* path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraMoveable = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 mainLightColor(1.0f, 1.0f, 1.0f);
static const float LanternMIN = 5.0f;
static const float LanternMAX = 25.0f;
float lanternValue=10.0f;
//your lights
bool lightsOn[6] = {1,1,1,1,1,0};
bool phongOn[3] = { 1,1,1 };

int main()
{
    //init glfw
    KwanGlfwStart();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //IMGUI
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImGui::StyleColorsDark();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader kwanShader = Shader("src/Vertex.Shader", "src/Fragment.Shader");
    Shader lightShader = Shader("src/lampVertex.Shader", "src/lampFragment.Shader");
    
    float vertices[] = {
    // positions             // normals           // texture coords
     //back
     -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
      0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    // Front face
     -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    // Left face
     -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    // Right face
      0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     // Bottom face
     -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     // Top face
     -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,  0.0f, 0.0f 
    };

    //10 cubes
    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,    0.0f),
    glm::vec3(4.0f,  0.0f,  -15.0f),
    glm::vec3(-3.0f, 0.0f,  -2.5f),
    glm::vec3(-5.0f, 0.0f, -12.3f),
    glm::vec3(5.0f,  0.0f,   6.5f),
    glm::vec3(-4.7f, 0.0f,  -7.5f),
    glm::vec3(2.5f,  0.0f,   -2.5f),
    glm::vec3(6.0f,  0.0f,   -1.5f),
    glm::vec3(5.0f,  0.0f,   -3.5f),
    glm::vec3(-1.5f, 0.0f,  -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    };


    GLuint VAO, VBO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    //glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint LightVAO;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
   
    unsigned int diffuseMap = loadTexture("src/container2.png");
    unsigned int specularMap = loadTexture("src/container2_specular.png");
    //unsigned int emissionMap = loadTexture("src/matrix.jpg");

    // shader configuration
    // --------------------
    kwanShader.use();

    kwanShader.setInt("material.diffuse", 0);
    kwanShader.setInt("material.specular", 1);

    kwanShader.setFloat("material.shininess", 32.0f);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0, 0.0, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        

        kwanShader.use();
        kwanShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

        if (lightsOn[5])
        {
            if(phongOn[0])
                kwanShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
            else
                kwanShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
            if(phongOn[1])
                kwanShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
            else
                kwanShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);
            if (phongOn[2])
                kwanShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
            else
                kwanShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);

        }
        else
        {
            kwanShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
            kwanShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);
            kwanShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
        }
        

        for (int i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); i++)
        {
            SetPointLights(kwanShader, pointLightPositions[i], i, lightsOn[i + 1], i == 0);
        }



        kwanShader.setVec3("viewPos", camera.Position);


        kwanShader.setVec3("spotLight.position", camera.Position);
        kwanShader.setVec3("spotLight.direction", camera.Front);
        kwanShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(lanternValue)));
        
        kwanShader.setVec3("spotLight.ambient", 0.05f, 0.05f, 0.05f);

        if (lightsOn[0])
        {
            kwanShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(lanternValue+1)));
            if (phongOn[1])
                kwanShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            else
                kwanShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            if (phongOn[2])
                kwanShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            else
                kwanShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }
        else
        {
            kwanShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            kwanShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }
        kwanShader.setFloat("spotLight.constant", 1.0f);
        kwanShader.setFloat("spotLight.linear", 0.09f);
        kwanShader.setFloat("spotLight.quadratic", 0.032f);
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        kwanShader.setMat4("projection", projection);
        kwanShader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        


        // render the cube
        glBindVertexArray(VAO);
        // world transformation
        for (auto cubePos : cubePositions)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
            kwanShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        glBindVertexArray(LightVAO);
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++)
        {
            if (!lightsOn[i + 1])
                continue;
            glm::vec3 pointLightPos = pointLightPositions[i];
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPos);
            if (i == 0)
            { 
                lightShader.setVec3("vColor", mainLightColor);
                model = glm::scale(model, glm::vec3(0.4f)); // bigger cube
            }
            else
            {
                lightShader.setVec3("vColor", 1.0f,1.0f,1.0f);
                model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            }
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        {
            ImGui::Begin("Light Controller");                          

            ImGui::Text("Controll the Lights");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Sun light", &lightsOn[5]);
            ImGui::Checkbox("Lantern", &lightsOn[0]);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Checkbox("Main Light", &lightsOn[1]);     
            ImGui::Checkbox("SubLight1", &lightsOn[2]);
            ImGui::Checkbox("SubLight2", &lightsOn[3]);
            ImGui::Checkbox("SubLight3", &lightsOn[4]);

            ImGui::ColorEdit3("Main color", (float*)&mainLightColor); // Edit 3 floats representing a color
            ImGui::SliderFloat ("Lantern Size", (float*)&lanternValue,LanternMIN,LanternMAX); // Edit 3 floats representing a color


            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            ImGui::Begin("Phong indices");
            
            ImGui::Checkbox("ambient", &phongOn[0]);
            ImGui::Checkbox("diffuse", &phongOn[1]);
            ImGui::Checkbox("specular", &phongOn[2]);

            ImGui::End();

        }
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &LightVAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void KwanGlfwStart()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //End Game
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //Move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    //Change Light
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraMoveable = false;
    else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
        cameraMoveable = true;


}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
    if(cameraMoveable)
        camera.ProcessMouseMovement(xoffset, yoffset);
    
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(cameraMoveable)
        camera.ProcessMouseScroll(static_cast<float>(yoffset));

}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void SetPointLights(const Shader& shader, const glm::vec3& position ,int place, bool isOn, bool isBig )
{
    if (isOn)
    {
        std::string Lightposition ="pointLights[" + std::to_string(place)+ "].position";
        shader.setVec3(Lightposition, position);

        std::string Lightambient = "pointLights[" + std::to_string(place) + "].ambient";
        std::string Lightdiffuse = "pointLights[" + std::to_string(place) + "].diffuse";
        std::string Lightspecular = "pointLights[" + std::to_string(place) + "].specular";

        if (isBig)
        {
            if (phongOn[0])
                shader.setVec3(Lightambient, 0.03f, 0.03f, 0.03f);
            else
                shader.setVec3(Lightambient, 0.00f, 0.00f, 0.00f);

            if (phongOn[1])
                shader.setVec3(Lightdiffuse, mainLightColor);
            else
                shader.setVec3(Lightdiffuse, 0.0f, 0.0f, 0.0f);
            if (phongOn[2])
                shader.setVec3(Lightspecular, 1.0f, 1.0f, 1.0f);
            else
                shader.setVec3(Lightspecular, 0.0f, 0.0f, 0.0f);

        }
        else {
            if (phongOn[0])
                shader.setVec3(Lightambient, 0.02f, 0.02f, 0.02f);
            else
                shader.setVec3(Lightambient, 0.00f, 0.00f, 0.00f);
            if (phongOn[1])
                shader.setVec3(Lightdiffuse, 0.4f, 0.4f, 0.4f);
            else
                shader.setVec3(Lightdiffuse, 0.0f, 0.0f, 0.0f);
            if (phongOn[2])
                shader.setVec3(Lightspecular, 0.5f, 0.5f, 0.5f);
            else
                shader.setVec3(Lightspecular, 0.0f, 0.0f, 0.0f);
        }
        std::string Lightlinear = "pointLights[" + std::to_string(place) + "].linear";
        std::string Lightquadratic = "pointLights[" + std::to_string(place) + "].quadratic";

        shader.setFloat(Lightlinear, 0.09f);
        shader.setFloat(Lightquadratic, 0.032f);
    }
    else
    {
        //std::string Lightposition = "pointLights[" + std::to_string(place) + "].position";
        //shader.setVec3(Lightposition, position);
        std::string Lightambient = "pointLights[" + std::to_string(place) + "].ambient";
        shader.setVec3(Lightambient, 0.0f, 0.0f, 0.0f);
        std::string Lightdiffuse = "pointLights[" + std::to_string(place) + "].diffuse";
        shader.setVec3(Lightdiffuse, 0.0f, 0.0f, 0.0f);
        std::string Lightspecular = "pointLights[" + std::to_string(place) + "].specular";
        shader.setVec3(Lightspecular, 0.0f, 0.0f, 0.0f);
        std::string Lightlinear = "pointLights[" + std::to_string(place) + "].linear";
    }
}
