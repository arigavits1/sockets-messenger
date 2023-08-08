#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <functional>
#include <thread>
#include <atomic>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

namespace glfw
{
    inline unsigned int WIN_WIDTH = 640;
    inline unsigned int WIN_HEIGHT = 500;

    inline GLFWwindow* Setup()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize glfw" << std::endl;
            return nullptr;
        }
        GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "ImGUI", nullptr, nullptr);
        if (window == NULL)
        {
            std::cerr << "Failed to initialize window" << std::endl;
            return nullptr;
        }
        glfwMakeContextCurrent(window);

        return window;
    }
}

namespace imgui
{
    inline ImGuiIO& Setup(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        return io;
    }

    inline void TextWindow(const char* name, std::string text, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(scale);

        ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Text("%s", text.c_str());
        ImGui::End();
    }

    inline void InputWindow(const char* name, char* inputBuffer, size_t inputBufferSize, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(scale);

        ImGui::Begin("Prompt");
        ImGui::InputText("Enter message", inputBuffer, inputBufferSize);
        ImGui::End();
    }
}

inline void processInput(GLFWwindow* window, std::atomic<bool> &should_run)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        should_run = false;
    }
}
