#pragma once

#include "Lumen/Internal/Renderer/Image.hpp"
#include "Lumen/Internal/Renderer/Buffers.hpp"
#include "Lumen/Internal/Renderer/Renderer.hpp"
#include "Lumen/Internal/Renderer/Pipeline.hpp"
#include "Lumen/Internal/Renderer/Descriptor.hpp"
#include "Lumen/Internal/Renderer/Renderpass.hpp"
#include "Lumen/Internal/Renderer/CommandBuffer.hpp"

#include "Lumen/Internal/Utils/Settings.hpp"	

#include "Lumen/Maths/Structs.hpp"

#include <cstdint>
#include <mutex>

namespace Lumen::Internal
{

	class BatchRenderer2D;

	////////////////////////////////////////////////////////////////////////////////////
	// BatchVertex
	////////////////////////////////////////////////////////////////////////////////////
	struct BatchVertex
	{
	public:
		Vec3<float> Position = { 0.0f, 0.0f, 0.0f };
		Vec2<float> UV = { 0.0f, 0.0f };
		Vec4<float> Colour = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Note: 0 is the white texure
		uint32_t TextureID = 0;

	public:
		// Constructors & Destructor
		BatchVertex() = default;
		BatchVertex(const Vec3<float>& position, const Vec2<float>& uv, const Vec4<float>& colour, uint32_t textureID)
			: Position(position), UV(uv), Colour(colour), TextureID(textureID) {}
		~BatchVertex() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// BatchResources2D
	////////////////////////////////////////////////////////////////////////////////////
	class BatchResources2D 
	{
	public:
		constexpr static const uint32_t MaxQuads = 10000;

		#if !defined(LU_PLATFORM_APPLE)
		constexpr static const uint32_t MaxTextures = 1024u;
		#else // Note: Apple devices have a very small limit of textures, to support most devices we stick with 16. The higher end devices support up to 128 (still low)
		constexpr static const uint32_t MaxTextures = 16u;
		#endif
	public:
		// Constructor & Destructor
		BatchResources2D() = default;
		~BatchResources2D() = default;

		// Init & Destroy
		void Init(const Internal::RendererID renderer, const std::vector<Image*>& colour, const std::vector<Image*>& depth);
		void Destroy();

		// Methods
		void Resize(uint32_t width, uint32_t height);

	private:
		RendererID m_RendererID = 0;
		std::vector<BatchVertex> m_CPUBuffer = { };
		UniformBuffer m_CameraBuffer = {};

		struct
		{
			std::vector<Image*> Colour = { };
			std::vector<Image*> Depth = { };
		} m_Images;

		struct
		{
			Pipeline Pipeline = {};
			DescriptorSets DescriptorSets = {};

			CommandBuffer CommandBuffer = {};
			Renderpass Renderpass = {};

			VertexBuffer VertexBuffer = {};
			IndexBuffer IndexBuffer = {};
		} m_Objects;

		struct
		{
			uint32_t Index = 0;
			std::unordered_map<Image*, uint32_t> Indices = { };

			Image White = {};
		} m_Textures;

		friend class BatchRenderer2D;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// BatchRenderer2D
	////////////////////////////////////////////////////////////////////////////////////
	class BatchRenderer2D
	{
	public:
		// Constructor & Destructor
		BatchRenderer2D() = default;
		~BatchRenderer2D() = default;

		// Init & Destroy
		void Init(const Internal::RendererID renderer, const std::vector<Image*>& colour, const std::vector<Image*>& depth);
		void Destroy();

		// Methods
		void Begin();
		void End();
		void Flush();

		void SetCamera(const Mat4& view, const Mat4& projection);

		// Note: We multiply the Z-axis by -1, so the depth is from 0 to 1
		void AddQuad(const Vec3<float>& position, const Vec2<float>& size, const Vec4<float>& colour);
		void AddQuad(const Vec3<float>& position, const Vec2<float>& size, Image* texture, const Vec4<float>& colour);

		// Internal
		void Resize(uint32_t width, uint32_t height);

	private:
		// Private methods
		uint32_t GetTextureID(Image* image);

	private:
		std::mutex m_ThreadSafety = { };

		uint32_t m_QuadCount = 0;
		BatchResources2D m_Resources = {};
	};

}