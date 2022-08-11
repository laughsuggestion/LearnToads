#pragma once

struct GLFWwindow;

/**
 * The wrapper around the glfw window.
 */
class LTGameWindow
{
    /**
     * Fields
     */
private:
    /**
     * The GLFW window
     */
    GLFWwindow* m_Window;

    /**
     * The title text
     */
    std::string m_Title;

    /**
     * The width of the window
     */
    uint32_t m_Width;

    /**
     * The height of the window
     */
    uint32_t m_Height;

    /**
     * Constructors
     */
public:
    LTGameWindow()
        : m_Window(nullptr)
        , m_Title("Game")
        , m_Width(800)
        , m_Height(600) { }

    /**
     * Copy/Move protection
     */
private:
    // non-copyable
    LTGameWindow(const LTGameWindow&) = delete;
    void operator=(const LTGameWindow&) = delete;
    // non-movable
    LTGameWindow(LTGameWindow&&) = delete;
    LTGameWindow& operator=(LTGameWindow&&) = delete;

    /**
     * Methods
     */
private:

public:
    /**
     * Initializes the game window.
     */
    void Initialize();

    /**
     * Updates the game window.
     */
    void Update();

    /**
     * Destroys the game window.
     */
    void Destroy();

    /**
     * Creates the surface for Vulkan to render to.
     */
    bool CreateWindowSurfaceVK(VkInstance vkInstance, VkSurfaceKHR* surface);

    /**
     * Returns true if GLFW has been given an indication to close the window.
     */
    bool ShouldClose() const
    {
        return !m_Window || glfwWindowShouldClose(m_Window);
    }

    /**
     * Gets the window width
     */
    inline const uint32_t GetWidth() const
    {
        return m_Width;
    }

    /**
     * Gets the window height
     */
    inline const uint32_t GetHeight() const
    {
        return m_Height;
    }
};

