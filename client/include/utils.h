#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "imgui.h"

unsigned int WIN_WIDTH = 640;
unsigned int WIN_HEIGHT = 500;

namespace glfw
{
    GLFWwindow* window;
    GLFWwindow* Setup()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize glfw\n";
            return NULL;
        }

        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "ImGUI", NULL, NULL);
        if (window == NULL)
        {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
            return NULL;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        return window;
    }
}

namespace imgui
{
    ImGuiIO& Setup(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        return io;
    }

    void TextWindow(const char* name, std::string text, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowSize(scale);
        ImGui::SetNextWindowPos(position);
        ImGui::Begin("Messages", NULL, ImGuiWindowFlags_NoResize);
        ImGui::TextUnformatted(text.c_str());
        ImGui::End();
    }

    void InputWindow(const char* name, char* inputBuffer, size_t inputBufferSize, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowSize(scale);
        ImGui::SetNextWindowPos(position);
        ImGui::Begin("Prompt", NULL, ImGuiWindowFlags_NoResize);
        ImGui::InputText("Enter message", inputBuffer, inputBufferSize, ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::End();
    }
}

void processInput(GLFWwindow* window, std::atomic<bool>& should_run)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "Escape\n";
        should_run = false;
        glfwSetWindowShouldClose(window, true);
    }
}
