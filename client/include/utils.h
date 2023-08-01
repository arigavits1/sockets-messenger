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

    extern unsigned int WIN_WIDTH;
    extern unsigned int WIN_HEIGHT;
    extern GLFWwindow* window;
    GLFWwindow* Setup();
}

namespace imgui
{
    ImGuiIO& Setup(GLFWwindow* window);
    void TextWindow(const char* name, std::string text, ImVec2 scale, ImVec2 position);
    void InputWindow(const char* name, char* inputBuffer, size_t inputBufferSize, ImVec2 scale, ImVec2 position);
}

void processInput(GLFWwindow* window, std::atomic<bool>& should_run);
