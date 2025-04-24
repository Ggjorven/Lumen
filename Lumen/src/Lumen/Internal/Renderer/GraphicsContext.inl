namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline bool GraphicsContext::Initialized()
    {
        return s_Initialized;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Static methods
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline void GraphicsContext::AttachWindow(void* nativeWindow)
    {
        s_ActiveWindow = nativeWindow;
    }

    hintinline void GraphicsContext::Init()
    {
        LU_ASSERT(s_ActiveWindow, "[GraphicsContext] No window has been attached.");

        s_GraphicsContext.Init(s_ActiveWindow);
        s_Initialized = true;
    }

    hintinline void GraphicsContext::Destroy()
    {
        s_GraphicsContext.Destroy();
        s_ActiveWindow = nullptr;
        s_Initialized = false;
    }

}