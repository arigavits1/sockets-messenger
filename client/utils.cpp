#include "include/utils.h"

namespace glfw 
{
    unsigned int WIN_WIDTH = 640;
    unsigned int WIN_HEIGHT = 500;

    GLFWwindow* Setup()
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
    ImGuiIO& Setup(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        return io;
    }

    void TextWindow(const char* name, std::string text, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(scale);

        ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Text("%s", text.c_str());
        ImGui::End();
    }

    void InputWindow(const char* name, char* inputBuffer, size_t inputBufferSize, ImVec2 scale, ImVec2 position)
    {
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(scale);

        ImGui::Begin("Prompt");
        ImGui::InputText("Enter message", inputBuffer, inputBufferSize);
        ImGui::End();
    }
}

void processInput(GLFWwindow* window, std::atomic<bool> &should_run)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        should_run = false;
    }
}