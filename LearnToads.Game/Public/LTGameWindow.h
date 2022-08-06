#pragma once

struct GLFWwindow;

class LTGameWindow
{
private:
    GLFWwindow* m_Window;
    std::string m_Title;
    uint32_t m_Width;
    uint32_t m_Height;

public:
    LTGameWindow()
        : m_Window(nullptr)
        , m_Title("Game")
        , m_Width(800)
        , m_Height(600) { }

private:
    // non-copyable
    LTGameWindow(const LTGameWindow&) = delete;
    void operator=(const LTGameWindow&) = delete;
    // non-movable
    LTGameWindow(LTGameWindow&&) = delete;
    LTGameWindow& operator=(LTGameWindow&&) = delete;

public:
    void Initialize();
    void Update();
    void Destroy();
    bool CreateWindowSurfaceVK(VkInstance vkInstance, VkSurfaceKHR* surface);

    bool ShouldClose() const
    {
        return !m_Window || glfwWindowShouldClose(m_Window);
    }
};

