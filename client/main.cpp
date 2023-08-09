#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <functional>
#include <thread>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "include/networking.h"
#include "include/utils.h"

int main(int argc, char* argv[])
{
    int sockfd = networkSetup();
    GLFWwindow* window = glfw::Setup();
    ImGuiIO& io = imgui::Setup(window); (void)io;
    std::atomic<bool> should_run(true);

    std::string dynamicTextBuffer;
    std::thread recvThread(recvFromServer, sockfd, &dynamicTextBuffer, std::ref(should_run));
    char inputBuffer[256] = "";

    while (!glfwWindowShouldClose(window) && should_run)
    {
        processInput(window, should_run);
        size_t inputBufferSize = sizeof(inputBuffer);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        imgui::TextWindow("Messages", dynamicTextBuffer, ImVec2(glfw::WIN_WIDTH, glfw::WIN_HEIGHT / 1.2f), ImVec2(0.0f, 0.0f));
        imgui::InputWindow("Prompt", inputBuffer, inputBufferSize, ImVec2(glfw::WIN_WIDTH, 100), ImVec2(0.0f, glfw::WIN_HEIGHT / 1.2f));
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && inputBuffer[0] != '\0')
        {
            //dynamicTextBuffer += "\n" + clientName + ": ";
            std::string tempBuffer(inputBuffer, inputBufferSize);
            sendToServer(sockfd, tempBuffer, inputBufferSize);
            // dynamicTextBuffer += inputBuffer;
            memset(inputBuffer, 0, sizeof(inputBuffer));
        }

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    std::string byeMessage = "bye";
    sendToServer(sockfd, std::ref(byeMessage), sizeof(byeMessage));

    recvThread.join();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
    return 0;
}
