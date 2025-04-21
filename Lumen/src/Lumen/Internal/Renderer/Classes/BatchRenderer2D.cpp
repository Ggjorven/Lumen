#include "lupch.h"
#include "BatchRenderer2D.hpp"

#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Profiler.hpp"

#include "Lumen/Internal/Renderer/Shader.hpp"
#include "Lumen/Internal/Renderer/Renderer.hpp"
#include "Lumen/Internal/Renderer/GraphicsContext.hpp"

#include <array>
#include <string_view>

namespace
{

	////////////////////////////////////////////////////////////////////////////////////
	// Embedded shaders
	////////////////////////////////////////////////////////////////////////////////////
	constexpr const std::string_view s_VertexShader = R"glsl(
		#version 460 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec2 a_TexCoord;
		layout(location = 2) in vec4 a_Colour;
		layout(location = 3) in uint a_TextureID;

		layout(location = 0) out vec3 v_Position;
		layout(location = 1) out vec2 v_TexCoord;
		layout(location = 2) out vec4 v_Colour;
		layout(location = 3) flat out uint v_TextureID;

		layout(std140, set = 0, binding = 0) uniform CameraSettings
		{
			mat4 View;
			mat4 Projection;
		} u_Camera;

		void main()
		{
			v_Position = a_Position;
			v_TexCoord = a_TexCoord;
			v_Colour = a_Colour;
			v_TextureID = a_TextureID;

			gl_Position = u_Camera.Projection * u_Camera.View * vec4(a_Position, 1.0);
		}
	)glsl";

	constexpr const std::string_view s_FragmentShader = R"glsl(
		#version 460 core
		#extension GL_KHR_vulkan_glsl : enable
		#extension GL_EXT_nonuniform_qualifier : enable

		layout(location = 0) out vec4 o_Colour;

		layout(location = 0) in vec3 v_Position;
		layout(location = 1) in vec2 v_TexCoord;
		layout(location = 2) in vec4 v_Colour;

		// Note: underlying type is uint16_t, so will never exceed this value.
		layout(location = 3) flat in uint v_TextureID;

		layout (set = 0, binding = 1) uniform sampler2D u_Textures[];

		void main()
		{
			o_Colour = v_Colour * texture(u_Textures[v_TextureID], v_TexCoord); // TODO: Check if this is not happening: https://github.com/TheCherno/Hazel/pull/247/files
		}
	)glsl";

	////////////////////////////////////////////////////////////////////////////////////
	// Helper functions
	////////////////////////////////////////////////////////////////////////////////////
	Lumen::Internal::BufferLayout GetVertexBufferLayout()
	{
		return {
			{ Lumen::Internal::DataType::Float3,	0, "Position",	Lumen::Internal::VertexInputRate::Vertex },
			{ Lumen::Internal::DataType::Float2,	1, "UV",		Lumen::Internal::VertexInputRate::Vertex },
			{ Lumen::Internal::DataType::Float4,	2, "Colour",	Lumen::Internal::VertexInputRate::Vertex },
			{ Lumen::Internal::DataType::UInt,		3, "TextureID", Lumen::Internal::VertexInputRate::Vertex },
		};
	}

}

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Init & Destroy
	////////////////////////////////////////////////////////////////////////////////////
	void BatchResources2D::Init(const Internal::RendererID renderer, const std::vector<Image*>& colour, const std::vector<Image*>& depth)
	{
		m_RendererID = renderer;

		// Global
		{
			m_Textures.White.Init(m_RendererID, {
				.Usage = ImageUsage::Colour | ImageUsage::Sampled,
				.Layout = ImageLayout::ShaderRead,
				.Format = ImageFormat::RGBA,

				.Width = 1, .Height = 1,

				.MipMaps = false,
			}, {});

			uint32_t white = 0xFFFFFFFF;
			m_Textures.White.SetData(m_RendererID, &white, sizeof(uint32_t));

			m_CameraBuffer.Init(m_RendererID, {
				.Usage = BufferMemoryUsage::CPUToGPU
			}, (sizeof(Mat4) * 2)); // For View & Projection

			std::array<Mat4, 2> cameraData = { Mat4(1.0f), Mat4(1.0f) };
			m_CameraBuffer.SetData(m_RendererID, cameraData.data(), sizeof(cameraData));
		}

		// Objects
		{
			LU_ASSERT(!colour.empty(), "[BatchResources2D] No images passed in to render to.");

			m_Images.Colour = colour;
			m_Images.Depth = depth;

			std::vector<uint32_t> indices;
			indices.reserve(static_cast<size_t>(MaxQuads) * 6);

			for (uint32_t i = 0, offset = 0; i < MaxQuads * 6; i += 6, offset += 4)
			{
				indices.push_back(offset + 0);
				indices.push_back(offset + 1);
				indices.push_back(offset + 2);

				indices.push_back(offset + 2);
				indices.push_back(offset + 3);
				indices.push_back(offset + 0);
			}

			// Renderpass
			m_Objects.CommandBuffer.Init(m_RendererID);
			m_Objects.Renderpass.Init(m_RendererID, {
				.Usage = RenderpassUsage::Graphics,

				.ColourAttachment = m_Images.Colour,
				.ColourLoadOp = LoadOperation::Clear,
				.ColourStoreOp = StoreOperation::Store,
				.ColourClearColour { 0.0f, 0.0f, 0.0f, 1.0f },
				.PreviousColourImageLayout = ImageLayout::Undefined,
				.FinalColourImageLayout = ImageLayout::PresentSrcKHR,

				.DepthAttachment = m_Images.Depth,
				.DepthLoadOp = LoadOperation::Clear,
				.DepthStoreOp = StoreOperation::Store,
				.PreviousDepthImageLayout = ImageLayout::Undefined,
				.FinalDepthImageLayout = ImageLayout::DepthStencil,

				.Subpasses = { RenderpassTarget::Colour | RenderpassTarget::Depth }
			}, & m_Objects.CommandBuffer);

			// Shader
			Shader shader(m_RendererID, {
				.Shaders = {
					{ ShaderStage::Vertex,		ShaderCompiler::CompileGLSL(ShaderStage::Vertex, s_VertexShader.data()) },
					{ ShaderStage::Fragment,	ShaderCompiler::CompileGLSL(ShaderStage::Fragment, s_FragmentShader.data()) }
				}
			});

			// Descriptorsets
			m_Objects.DescriptorSets.Init(m_RendererID, {
				{ 1, { 0, {
					{ DescriptorType::UniformBuffer,		0, "u_Camera",	ShaderStage::Vertex, },
					{ DescriptorType::CombinedImageSampler, 1, "u_Textures", ShaderStage::Fragment, MaxTextures, DescriptorBindingFlags::UpdateAfterBind | DescriptorBindingFlags::PartiallyBound },
				}}}
			});

			// Pipeline
			m_Objects.Pipeline.Init(m_RendererID, {
				.Usage = PipelineUsage::Graphics,
				.Bufferlayout = GetVertexBufferLayout(),
				.Polygonmode = PolygonMode::Fill,
				.Cullingmode = CullingMode::None,
				.Blending = true
			}, m_Objects.DescriptorSets, shader, m_Objects.Renderpass);
			shader.Destroy(m_RendererID);

			// Buffers
			m_Objects.VertexBuffer.Init(m_RendererID, {
				.Usage = BufferMemoryUsage::CPUToGPU
			}, nullptr, sizeof(BatchVertex) * MaxQuads * 4);
			m_Objects.IndexBuffer.Init(m_RendererID, {}, indices.data(), static_cast<uint32_t>(indices.size()));

			// Reserve enough space for cpu buffer
			m_CPUBuffer.reserve(static_cast<size_t>(MaxQuads) * 4);
		}
	}

	void BatchResources2D::Destroy()
	{
		m_Textures.White.Destroy(m_RendererID);
		m_CameraBuffer.Destroy(m_RendererID);

		m_Objects.Pipeline.Destroy(m_RendererID);
		m_Objects.DescriptorSets.Destroy(m_RendererID);

		m_Objects.CommandBuffer.Destroy(m_RendererID);
		m_Objects.Renderpass.Destroy(m_RendererID);

		m_Objects.VertexBuffer.Destroy(m_RendererID);
		m_Objects.IndexBuffer.Destroy(m_RendererID);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void BatchResources2D::Resize(uint32_t width, uint32_t height)
	{
		// Colour images
		auto swapChainImages = Renderer::GetRenderer(m_RendererID).GetSwapChainImages();
		for (size_t i = 0; i < m_Images.Colour.size(); i++)
		{
			if (swapChainImages[i] == m_Images.Colour[i]) // TODO: Improve this system
				continue;
			
			m_Images.Colour[i]->Resize(m_RendererID, width, height);
		}

		// Depth images
		for (size_t i = 0; i < m_Images.Depth.size(); i++)
		{
			m_Images.Depth[i]->Resize(m_RendererID, width, height);
		}

		// Renderpass
		m_Objects.Renderpass.Resize(m_RendererID, width, height);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Init & Destroy
	////////////////////////////////////////////////////////////////////////////////////
	void BatchRenderer2D::Init(const RendererID renderer, const std::vector<Image*>& colour, const std::vector<Image*>& depth)
	{
		#if defined(LU_PLATFORM_APPLE)
		LU_LOG_WARN("[BatchRenderer2D] BatchRenderer2D only supports {0} simultaneous textures on apple devices. Should be used with care.", BatchResources2D::MaxTextures);
		#endif
		m_Resources.Init(renderer, colour, depth);
	}

	void BatchRenderer2D::Destroy()
	{
		m_Resources.Destroy();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void BatchRenderer2D::Begin()
	{
		LU_PROFILE("BatchRenderer2D::Begin()");
		m_QuadCount = 0;
		m_Resources.m_CPUBuffer.clear();

		m_Resources.m_Textures.Index = 0;
		m_Resources.m_Textures.Indices.clear(); // TODO: Maybe optimize

		// Set the white texture to index 0
		m_Resources.m_Textures.Indices[&m_Resources.m_Textures.White] = m_Resources.m_Textures.Index++;
	}

	void BatchRenderer2D::End()
	{
		LU_PROFILE("BatchRenderer2D::End()");
		std::vector<Uploadable> uploadQueue;
		uploadQueue.reserve(m_Resources.m_Textures.Indices.size() + 1); // + 1 for the Camera Buffer.
		{
			LU_PROFILE("BatchRenderer2D::End::FormUploadQueue");
			uploadQueue.push_back({ &m_Resources.m_CameraBuffer, m_Resources.m_Objects.DescriptorSets.GetLayout(0).GetDescriptorByName("u_Camera") });

			// Upload all images
			const auto& descriptor = m_Resources.m_Objects.DescriptorSets.GetLayout(0).GetDescriptorByName("u_Textures");
			for (const auto& [image, textureID] : m_Resources.m_Textures.Indices)
				uploadQueue.push_back({ image, descriptor, textureID });
		}
		// Actual upload command
		{
			LU_PROFILE("BatchRenderer2D::End::ExecuteUploadQueue");
			m_Resources.m_Objects.DescriptorSets.GetSets(0)[0]->Upload(m_Resources.m_RendererID, uploadQueue);
		}

		// Only draw if there's something TO draw
		if (m_Resources.m_CPUBuffer.size() > 0)
		{
			LU_PROFILE("BatchRenderer2D::End::UploadVertexData");
			m_Resources.m_Objects.VertexBuffer.SetData(m_Resources.m_RendererID, static_cast<void*>(m_Resources.m_CPUBuffer.data()), (m_Resources.m_CPUBuffer.size() * sizeof(BatchVertex)), 0);
		}
	}

	void BatchRenderer2D::Flush()
	{
		LU_PROFILE("BatchRenderer2D::Flush()");
		Renderer& renderer = Renderer::GetRenderer(m_Resources.m_RendererID);

		// Start rendering
		renderer.Begin(m_Resources.m_Objects.Renderpass);

		CommandBuffer& cmdBuf = m_Resources.m_Objects.Renderpass.GetCommandBuffer();
		m_Resources.m_Objects.Pipeline.Use(m_Resources.m_RendererID, cmdBuf, PipelineBindPoint::Graphics);

		m_Resources.m_Objects.DescriptorSets.GetSets(0)[0]->Bind(m_Resources.m_RendererID, m_Resources.m_Objects.Pipeline, cmdBuf);

		m_Resources.m_Objects.IndexBuffer.Bind(m_Resources.m_RendererID, cmdBuf);
		m_Resources.m_Objects.VertexBuffer.Bind(m_Resources.m_RendererID, cmdBuf);

		// Draw all at once
		renderer.DrawIndexed(cmdBuf, (m_QuadCount * 6ull), 1);

		// End rendering
		renderer.End(m_Resources.m_Objects.Renderpass);
		renderer.Submit(m_Resources.m_Objects.Renderpass, ExecutionPolicy::InOrder);
	}

	void BatchRenderer2D::SetCamera(const Mat4& view, const Mat4& projection)
	{
		std::array<Mat4, 2> cameraData = { view, projection };
		m_Resources.m_CameraBuffer.SetData(m_Resources.m_RendererID, cameraData.data(), sizeof(cameraData));
	}

	void BatchRenderer2D::AddQuad(const Vec3<float>& position, const Vec2<float>& size, const Vec4<float>& colour)
	{
		AddQuad(position, size, nullptr, colour);
	}

	void BatchRenderer2D::AddQuad(const Vec3<float>& position, const Vec2<float>& size, Image* texture, const Vec4<float>& colour)
	{
		LU_PROFILE("BatchRenderer2D::AddQuad()");
		constexpr const Vec2<float> uv0(1.0f, 0.0f);
		constexpr const Vec2<float> uv1(0.0f, 0.0f);
		constexpr const Vec2<float> uv2(0.0f, 1.0f);
		constexpr const Vec2<float> uv3(1.0f, 1.0f);

		#if defined(LU_CONFIG_DEBUG)
		if (m_QuadCount >= BatchResources2D::MaxQuads) [[unlikely]]
		{
			LU_LOG_WARN("[BatchRenderer2D] Reached max amount of quads ({0}), to support more either manually change BatchResources2D::MaxQuads or contact the developer.", BatchResources2D::MaxQuads);
			return;
		}
		#endif

		{
			std::lock_guard<std::mutex> lock(m_ThreadSafety);

			const uint32_t textureID = GetTextureID(texture);
			const float zAxis = position.z * -1.0f;

			m_Resources.m_CPUBuffer.emplace_back(Vec3<float>(position.x, position.y, zAxis), uv0, colour, textureID);
			m_Resources.m_CPUBuffer.emplace_back(Vec3<float>(position.x + size.x, position.y, zAxis), uv1, colour, textureID);
			m_Resources.m_CPUBuffer.emplace_back(Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv2, colour, textureID);
			m_Resources.m_CPUBuffer.emplace_back(Vec3<float>(position.x, position.y + size.y, zAxis), uv3, colour, textureID);

			++m_QuadCount;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Internal
	////////////////////////////////////////////////////////////////////////////////////
	void BatchRenderer2D::Resize(uint32_t width, uint32_t height)
	{
		m_Resources.Resize(width, height);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	uint32_t BatchRenderer2D::GetTextureID(Image* image)
	{
		// Early exit if nullptr
		if (image == nullptr)
			return m_Resources.m_Textures.Indices[&m_Resources.m_Textures.White];

		// Use the result of find directly
		auto it = m_Resources.m_Textures.Indices.find(image);
		if (it != m_Resources.m_Textures.Indices.end())
			return it->second;

		// Texture is not cached, so cache it
		#if defined(LU_CONFIG_DEBUG)
		if (m_Resources.m_Textures.Index >= (BatchResources2D::MaxTextures - 1)) [[unlikely]]
		{
			LU_LOG_WARN("[BatchRenderer2D] Reached max amount of textures ({0}), to support more either manually change BatchResources2D::MaxTextures or contact the developer. Be aware that apple devices have a very low hardware-set limit.", BatchResources2D::MaxTextures);
			return 0;
		}
		#endif

		// Assign new texture ID and return it
		m_Resources.m_Textures.Indices[image] = m_Resources.m_Textures.Index++;
		return m_Resources.m_Textures.Indices[image];
	}

}