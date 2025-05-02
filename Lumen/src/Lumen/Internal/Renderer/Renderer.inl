namespace Lumen::Internal
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    forceinline Renderer::Renderer(const RendererSpecification& specs)
        : m_Renderer(specs)
    {
        s_Renderer = this;
    }

    forceinline Renderer::~Renderer()
    {
        s_Renderer = nullptr;
    }


}