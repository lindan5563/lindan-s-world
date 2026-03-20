#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <random>
using namespace std;
struct Cube {
    glm::vec3 position;
    glm::vec3 color;
};
const char* vsdm = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"    gl_Position=projection*view*model*vec4(aPos, 1.0);\n"
"    ourColor=aColor;\n"
"}";
const char* fsdm = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0);\n"
"}";

GLuint shaderProgram;
GLuint VBO, VAO;
GLFWwindow* window;
glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 100.0f);
float cx = 2, cy = 2, cz = 2;
float clax = 0, clay = 0, claz = 0;
int windowWidth = 800;
int windowHeight = 600;

float yaw = -135.0f;
float pitch = -35.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
float sensitivity = 0.1f;
bool mouseControlEnabled = false;
const float cubeVertices[] = {
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f
};

std::vector<Cube> cubes = {
    {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}
};

const glm::vec3 faceNormals[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),   // óò
    glm::vec3(-1.0f, 0.0f, 0.0f),  // ×ó
    glm::vec3(0.0f, 1.0f, 0.0f),   // é?
    glm::vec3(0.0f, -1.0f, 0.0f),  // ??
    glm::vec3(0.0f, 0.0f, 1.0f),   // ?°
    glm::vec3(0.0f, 0.0f, -1.0f)   // oó
};
glm::vec3 getCameraFront()
{
    return glm::normalize(glm::vec3(clax - cx, clay - cy, claz - cz));
}
glm::vec3 getCameraRight()
{
    glm::vec3 front = getCameraFront();
    return glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
}
glm::vec3 getCameraUp()
{
    glm::vec3 front = getCameraFront();
    glm::vec3 right = getCameraRight();
    return glm::normalize(glm::cross(right, front));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!mouseControlEnabled) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    clax = cx + front.x;
    clay = cy + front.y;
    claz = cz + front.z;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    float x = (2.0f * xpos) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * ypos) / windowHeight;
    glm::vec3 rayNDS(x, y, 1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(cx, cy, cz), glm::vec3(clax, clay, claz), glm::vec3(0, 1, 0));
    glm::vec4 rayEye = glm::inverse(projection) * glm::vec4(rayNDS, 1.0);
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);
    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    Cube* selectedCube = nullptr;
    int selectedFace = -1;
    float minDistance = FLT_MAX;
    for (auto& cube : cubes)
    {
        glm::vec3 min = cube.position - glm::vec3(0.5f);
        glm::vec3 max = cube.position + glm::vec3(0.5f);
        float tmin = (min.x - cx) / rayWorld.x;
        float tmax = (max.x - cx) / rayWorld.x;
        if (tmin > tmax) std::swap(tmin, tmax);
        float tymin = (min.y - cy) / rayWorld.y;
        float tymax = (max.y - cy) / rayWorld.y;
        if (tymin > tymax) std::swap(tymin, tymax);
        if ((tmin > tymax) || (tymin > tmax)) continue;
        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;
        float tzmin = (min.z - cz) / rayWorld.z;
        float tzmax = (max.z - cz) / rayWorld.z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);
        if ((tmin > tzmax) || (tzmin > tmax)) continue;
        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;
        if (tmin < minDistance && tmin > 0)
        {
            minDistance = tmin;
            selectedCube = &cube;
            glm::vec3 intersectPoint = glm::vec3(cx, cy, cz) + rayWorld * tmin;
            glm::vec3 localPoint = intersectPoint - cube.position;
            glm::vec3 absPoint = glm::abs(localPoint);
            if (absPoint.x > absPoint.y && absPoint.x > absPoint.z)
            {
                selectedFace = localPoint.x > 0 ? 0 : 1;
            }
            else if (absPoint.y > absPoint.z)
            {
                selectedFace = localPoint.y > 0 ? 2 : 3;
            }
            else
            {
                selectedFace = localPoint.z > 0 ? 4 : 5;
            }
        }
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    if (selectedCube)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            cubes.erase(std::remove_if(cubes.begin(), cubes.end(),
                [&](const Cube& c) { return &c == selectedCube; }), cubes.end());
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            Cube newCube;
            newCube.position = selectedCube->position + faceNormals[selectedFace] * 1.0f;
            newCube.color = glm::vec3(dis(gen), dis(gen), dis(gen));
            cubes.push_back(newCube);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        glm::vec3 rayEnd = glm::vec3(cx, cy, cz) + rayWorld * 10.0f;
        Cube newCube;
        newCube.position = glm::round(rayEnd);
        newCube.color = glm::vec3(dis(gen), dis(gen), dis(gen));
        cubes.push_back(newCube);
    }
}
GLuint cpacrt()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsdm, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsdm, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
void draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = glm::lookAt(glm::vec3(cx, cy, cz), glm::vec3(clax, clay, claz), glm::vec3(0, 1, 0));
    for (const auto& cube : cubes)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.position);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        std::vector<float> vertices(sizeof(cubeVertices) / sizeof(float));
        memcpy(vertices.data(), cubeVertices, sizeof(cubeVertices));
        for (size_t i = 3; i < vertices.size(); i += 6)
        {
            vertices[i] = cube.color.r;
            vertices[i + 1] = cube.color.g;
            vertices[i + 2] = cube.color.b;
        }
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        int p = glGetUniformLocation(shaderProgram, "projection");
        int v = glGetUniformLocation(shaderProgram, "view");
        int m = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
}
void mainmenu()
{
    int a;
    string s;
    while (true)
    {
        cout << "\t\t\tWelcome back palyer!\n";
        cout << "\t\t\t\t1.open\n";
        cout << "\t\t\t\t2.save\n";
        cout << "\t\t\t\t3.exit\n";
        cin >> a;
        if (a == 1)
        {
            cubes.clear();
            system("cls");
            cout << "Enter file path: ";
            cin >> s;
            ifstream infile(s);
            if (!infile)
            {
                system("cls");
                cout << "Error opening file!\n";
                Sleep(1000);
                goto p;
            }
            infile >> a;
            float x, y, z, r, g, b;
            for (int i = 0; i < a; i++)
            {
                infile >> x >> y >> z >> r >> g >> b;
                Cube newcube = { glm::vec3(x, y, z), glm::vec3(r, g, b) };
                cubes.emplace_back(newcube);
            }
            infile.close();
        }
        else if (a == 2)
        {
            system("cls");
            cout << "Enter file path: ";
            cin >> s;
            ofstream outfile(s, ios::trunc);
            if (!outfile)
            {
                system("cls");
                cout << "Error creating file!\n";
                Sleep(1000);
                goto p;
            }
            outfile << cubes.size() << "\n";
            for (const auto& cube : cubes)
            {
                outfile << cube.position.x << " " << cube.position.y << " " << cube.position.z << " " << cube.color.r << " " << cube.color.g << " " << cube.color.b << "\n";
            }
            outfile.close();
        }
        else if (a == 3)
        {
            system("cls");
            exit(0);
        }
        else
        {
            system("cls");
            cout << "Invalid option! Please try again.\n";
            Sleep(1000);
        }
    p:;
        system("cls");
    }
}
int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(800, 600, "OpenGL Cube Editor", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    shaderProgram = cpacrt();
    std::thread menuThread(mainmenu);
    menuThread.detach();
    float moveSpeed = 0.05f;
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            mouseControlEnabled = !mouseControlEnabled;
            glfwSetInputMode(window, GLFW_CURSOR,
                mouseControlEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            firstMouse = true;
            while (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
                glfwPollEvents();
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            glm::vec3 front = getCameraFront();
            cx += front.x * moveSpeed;
            cy += front.y * moveSpeed;
            cz += front.z * moveSpeed;
            clax += front.x * moveSpeed;
            clay += front.y * moveSpeed;
            claz += front.z * moveSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            glm::vec3 front = getCameraFront();
            cx -= front.x * moveSpeed;
            cy -= front.y * moveSpeed;
            cz -= front.z * moveSpeed;
            clax -= front.x * moveSpeed;
            clay -= front.y * moveSpeed;
            claz -= front.z * moveSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            glm::vec3 right = getCameraRight();
            cx -= right.x * moveSpeed;
            cy -= right.y * moveSpeed;
            cz -= right.z * moveSpeed;
            clax -= right.x * moveSpeed;
            clay -= right.y * moveSpeed;
            claz -= right.z * moveSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glm::vec3 right = getCameraRight();
            cx += right.x * moveSpeed;
            cy += right.y * moveSpeed;
            cz += right.z * moveSpeed;
            clax += right.x * moveSpeed;
            clay += right.y * moveSpeed;
            claz += right.z * moveSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            glm::vec3 up = getCameraUp();
            cx += up.x * moveSpeed;
            cy += up.y * moveSpeed;
            cz += up.z * moveSpeed;
            clax += up.x * moveSpeed;
            clay += up.y * moveSpeed;
            claz += up.z * moveSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            glm::vec3 up = getCameraUp();
            cx -= up.x * moveSpeed;
            cy -= up.y * moveSpeed;
            cz -= up.z * moveSpeed;
            clax -= up.x * moveSpeed;
            clay -= up.y * moveSpeed;
            claz -= up.z * moveSpeed;
        }
        draw();
    }
    glfwTerminate();
    return 0;
}