#pragma once

#include <cstdint>
#include <functional>

#include "Lumen/Internal/Core/WindowSpec.hpp"
#include "Lumen/Internal/Enum/Bitwise.hpp"

#include "Lumen/Internal/Renderer/RenderpassSpec.hpp"

#include "Lumen/Maths/Structs.hpp"

namespace Lumen::Internal
{

    class Window;
    class Image;

    ////////////////////////////////////////////////////////////////////////////////////
    // Configurations
    ////////////////////////////////////////////////////////////////////////////////////
    enum class ExecutionPolicy : uint8_t
    {
        InOrder = 1 << 0,           // Execute commands sequentially, submits to waited on by next (WaitForPrevious) commandBuffer
        Parallel = 1 << 1,          // Execute commands in parallel but synchronized by the frame

        NoWaiting = 1 << 2,         // Don't wait for the completion of the previous (InOrder) command buffer
    };
    LU_ENABLE_BITWISE(ExecutionPolicy)

    enum class Queue : uint8_t 
    { 
        Graphics, 
        Present, 
        Compute 
    };

    using FreeFn = std::function<void()>;

    ////////////////////////////////////////////////////////////////////////////////////
    // Dynamic Rendering
    ////////////////////////////////////////////////////////////////////////////////////
    struct DynamicRenderState
    {
    public:
        Image* ColourAttachment = nullptr; // If using swapchain images use Renderer::GetAcquiredImage() as the index.
        LoadOperation ColourLoadOp = LoadOperation::Clear;
        StoreOperation ColourStoreOp = StoreOperation::Store;
        Vec4<float> ColourClearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

        Image* DepthAttachment = nullptr;
        LoadOperation DepthLoadOp = LoadOperation::Clear;
        StoreOperation DepthStoreOp = StoreOperation::Store;
        float DepthClearValue = 1.0f;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    // RendererSpecification
    ////////////////////////////////////////////////////////////////////////////////////
    struct RendererSpecification
    {
    public:
        Window* WindowRef = nullptr;

        uint32_t Width = 0, Height = 0;

        WindowSpecification::BufferMode Buffers = WindowSpecification::BufferMode::Triple;
        bool VSync = true;
    };

    using RendererID = uint8_t;

}