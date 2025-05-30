namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Methods
    ////////////////////////////////////////////////////////////////////////////////////
    hintinline void DesktopWindow::PollEvents()
    {
        LU_MARK_FRAME();
        LU_PROFILE("DesktopWindow::PollEvents()");
        glfwPollEvents();
    }

    hintinline void DesktopWindow::SwapBuffers()
    {
        LU_PROFILE("DesktopWindow::SwapBuffers()");
    }

    forceinline void DesktopWindow::Resize(uint32_t width, uint32_t height)
    {
        LU_PROFILE("DesktopWindow::Resize()");
        m_Renderer->Recreate(width, height, m_Specification.VSync);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////////////////////////////
    hintinline Vec2<int32_t> DesktopWindow::GetPosition() const
    {
        LU_PROFILE("DesktopWindow::GetPosition()");

        Vec2<int32_t> position = { 0, 0 };
        glfwGetWindowPos(m_Window, &position.x, &position.y);
        return position;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Setters
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline void DesktopWindow::SetTitle(std::string_view title)
    {
        LU_PROFILE("DesktopWindow::SetTitle()");

        m_Specification.Title = title;
        glfwSetWindowTitle(m_Window, m_Specification.Title.data());
    }

    forceinline void DesktopWindow::SetVSync(bool vsync)
    {
        LU_PROFILE("DesktopWindow::SetVSync()");

        m_Specification.VSync = vsync;
        m_Renderer->Recreate(m_Specification.Width, m_Specification.Height, vsync);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Additional getters
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline double DesktopWindow::GetTime() const
    {
        LU_PROFILE("DesktopWindow::GetTime()");
        return glfwGetTime();
    }

}