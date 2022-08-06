#include "PrecompiledHeader.h"
#include "LTGameWindow.h"

void LTGameWindow::Initialize()
{
    // initialize glfw - required before any other glfw calls
    glfwInit();

    // tells glfw not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(
        m_Width,
        m_Height,
        m_Title.c_str(),
        nullptr,
        nullptr);
}

void LTGameWindow::Update()
{
    glfwPollEvents();
}

void LTGameWindow::Destroy()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool LTGameWindow::CreateWindowSurfaceVK(VkInstance vkInstance, VkSurfaceKHR* surface)
{
    return glfwCreateWindowSurface(vkInstance, m_Window, nullptr, surface) == VK_SUCCESS;
}
