//
// Sandbox program for Computer Graphics For Games (G4G)
// created May 2021 by Eric Ameres for experimenting
// with OpenGL and various graphics algorithms
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

#include "shader_s.h"

#pragma warning( disable : 26451 )
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1240;
const unsigned int SCR_HEIGHT = 910;

unsigned int VBO, VAO[3], EBO, indexCount[3];
unsigned int texture[2];
unsigned int vaoCount = 0;

glm::mat4 pMat;
glm::mat4 vMat;

// image buffer used by raster drawing basics.cpp
extern unsigned char imageBuff[256][256][4];

char vtext[1024 * 8] = 
"#version 330 core\n\n"
"layout (location = 0) in vec3 aPos;\n\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"}\n" ; // fragment shader source code


char ftext[1024 * 8] =
"#version 330 core\n\n"
"out vec4 FragColor;\n\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f,0f,0f, 1.0f);\n"
"}\n"  ; // vertex shader source code

Shader ourShader;
int nTexture = 0;

using namespace std;

float myMatrix[4][4] = { { 1.0f, 0.0f, 0.0f, 0.5f },{ 0.0f, 1.0f, 0.0f, 0.5f },{ 0.0f, 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 0.0f, 1.0f } };


string readFile(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

void writeMatrix(const char* filePath) {
    std::ofstream myfile;

    myfile.open(filePath);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            myfile << myMatrix[i][j];
            myfile << " ";
        }
        myfile << "\n";
    }
    myfile.close();
}

void readMatrix(const char* filePath) {
    std::ofstream myfile;

    ifstream source;
    
    source.open(filePath, ios::in);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            source >> myMatrix[i][j];
        }
    }
    myfile.close();
}


void saveShaders() {
    std::ofstream myfile;

    myfile.open("data/texture.vs");
    myfile << vtext;
    myfile.close();

    myfile.open("data/texture.fs");
    myfile << ftext;
    myfile.close();

    writeMatrix("data/myMatrix.txt");
}

int myTexture();

void setupTriangle()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // top right
         0.5f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.5f, 1.0f, // bottom right
         0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    };
    unsigned int indices[] = {
        2, 1, 0, // first triangle
    };

    indexCount[vaoCount] = sizeof(indices) / sizeof(indices[0]);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO[vaoCount++]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void setupTexturedQuad()
{

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
         -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top left 
         -0.5f,  -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f  // bottom left
    };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        3, 2, 1  // second triangle
    };
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO[vaoCount++]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    indexCount[vaoCount-1] = sizeof(indices) / sizeof(indices[0]);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create textures 
    // -------------------------
    glGenTextures(2, texture);

    // first texture is loaded from a file
    glBindTexture(GL_TEXTURE_2D, texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width=0, height=0, nrChannels=0;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load("data/rpi.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    // second texture is a buffer we will be generating for pixel experiments
    glBindTexture(GL_TEXTURE_2D, texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)imageBuff);
    glGenerateMipmap(GL_TEXTURE_2D);
}
void setupCube()
{
    float vertexPositions[108] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO[vaoCount++]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    indexCount[vaoCount-1] = 0;

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void drawIMGUI() {
    // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float f = 0.0f;

        ImGui::Begin("Graphics For Games");  // Create a window and append into it.

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("Vertex Shader", vtext, IM_ARRAYSIZE(vtext), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        ImGui::InputTextMultiline("Fragment Shader", ftext, IM_ARRAYSIZE(ftext), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

        if (ImGui::Button("Use Shaders", ImVec2(100, 20)))
            ourShader.reload(vtext, ftext);



        ImGui::SameLine(115);

        if (ImGui::Button("Save Shaders", ImVec2(120, 20)))
        {
            saveShaders();
        }



        ImGui::SameLine(240);

        if (ImGui::Button("Swap Texture", ImVec2(100, 20)))
            nTexture ^= 1;


        ImGui::InputFloat4("one", myMatrix[0]);
        ImGui::InputFloat4("two", myMatrix[1]);
        ImGui::InputFloat4("three", myMatrix[2]);
        ImGui::InputFloat4("four", myMatrix[3]);

        ImGui::End();

        // IMGUI Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics4Games", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    strcpy_s(ftext, readFile("data/texture.fs").c_str());
    strcpy_s(vtext, readFile("data/texture.vs").c_str());

    readMatrix("data/myMatrix.txt");

    // build and compile our shader program
    // ------------------------------------
    ourShader.reload(vtext, ftext);

    myTexture();

    glGenVertexArrays(3, VAO);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    //setupTriangle();
    setupTexturedQuad();
    //setupCube();

    // render loop
    // -----------

    pMat = glm::perspective(1.0472f, ((float)SCR_WIDTH / (float)SCR_HEIGHT), .1f, 1000.0f);	//  1.0472 radians = 60 degrees
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f  , 0.0f, -4.0f));

    while (!glfwWindowShouldClose(window))
    {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();  
        
        drawIMGUI();

        // these following lines are really where our geometry gets rendered!
        glBindTexture(GL_TEXTURE_2D, texture[nTexture]);
        ourShader.use();

        // set the uniform for a specific program (the way learnOpenGL does it)
        //glProgramUniformMatrix4fv(ourShader.ID, glGetUniformLocation(ourShader.ID, "pMat"), 1, GL_FALSE, glm::value_ptr(pMat));
        //glProgramUniformMatrix4fv(ourShader.ID, glGetUniformLocation(ourShader.ID, "vMat"), 1, GL_FALSE, glm::value_ptr(vMat));
        
        // set the uniform for the current program (the way the "old" text and lecture do it)
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "pMat"), 1, GL_FALSE, glm::value_ptr(pMat));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "vMat"), 1, GL_FALSE, glm::value_ptr(vMat));
        
        //glBindVertexArray(VAO);
        
        static float x = 0.0f;
        static float inc = 0.01f;

        x += inc;
        if (x > 1.0f) inc = -0.01f;
        if (x < -1.0f) inc = 0.01f;

        glProgramUniform1f(ourShader.ID, glGetUniformLocation(ourShader.ID, "offset"), x);
        // create transformations
        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 transform2 = glm::mat4(
            myMatrix[0][0], myMatrix[1][0], myMatrix[2][0], myMatrix[3][0],
            myMatrix[0][1], myMatrix[1][1], myMatrix[2][1], myMatrix[3][1],
            myMatrix[0][2], myMatrix[1][2], myMatrix[2][2], myMatrix[3][2],
            myMatrix[0][3], myMatrix[1][3], myMatrix[2][3], myMatrix[3][3]
        );

        for (int i = 0; i < vaoCount; i++) {
            transform = glm::translate(transform, glm::vec3(0.5f, -0.5f+(float)i*2.0f, 0.0f));
            transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

            glProgramUniformMatrix4fv(ourShader.ID, glGetUniformLocation(ourShader.ID, "mMat"), 1, GL_FALSE, glm::value_ptr(transform2));
            glProgramUniformMatrix4fv(ourShader.ID, glGetUniformLocation(ourShader.ID, "mMat2"), 1, GL_FALSE, glm::value_ptr(transform));

            glBindVertexArray(VAO[i]);
            if (indexCount[i] > 0) {
                glDrawElementsInstanced(GL_TRIANGLES, indexCount[i], GL_UNSIGNED_INT, 0, 1);
                //glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);
            }
            else
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
    pMat = glm::perspective(1.0472f, (float)width/(float)height, 0.1f, 1000.0f);	//  1.0472 radians = 60 degrees
}