#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>  
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"  
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(5);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
            50.0f, -50.0f, 1.0f, 0.0f, // 1
            50.0f, 50.0f,  1.0f, 1.0f, // 2
            -50.0f, 50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indicies, 6);

        glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        glm::vec3 tansilationA(200, 200, 0);
        glm::vec3 tansilationB(400, 200, 0);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind(); 

        Texture texture("res/textures/flowers.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();

            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tansilationA);
                glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvpMatrix);
                
                renderer.Draw(va, ib, shader);
            }

            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tansilationB);
                glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvpMatrix);
                
                renderer.Draw(va, ib, shader);
            }

            {
                ImGui::SliderFloat2("TransilationA", &tansilationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat2("TransilationB", &tansilationB.x, 0.0f, 960.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    //glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}