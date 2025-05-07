#include "VulkanImage.hpp"
namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	hintinline VulkanImage::VulkanImage(const CommandBuffer& initCmd, const ImageSpecification& imageSpecs, const SamplerSpecification& samplerSpecs)
		: m_ImageSpecification(imageSpecs), m_SamplerSpecification(samplerSpecs)
	{
		LU_ASSERT((static_cast<bool>(m_ImageSpecification.Usage & ImageUsage::Colour) || static_cast<bool>(m_ImageSpecification.Usage & ImageUsage::DepthStencil)), "[VulkanImage] Tried to create image without specifying if it's a Colour or Depth image.");

		CreateImage(initCmd, m_ImageSpecification.Width, m_ImageSpecification.Height);
	}

	hintinline VulkanImage::VulkanImage(const CommandBuffer& initCmd, const ImageSpecification& imageSpecs, const SamplerSpecification& samplerSpecs, const std::filesystem::path& imagePath)
		: m_ImageSpecification(imageSpecs), m_SamplerSpecification(samplerSpecs)
	{
		LU_ASSERT((static_cast<bool>(m_ImageSpecification.Usage & ImageUsage::Colour) || static_cast<bool>(m_ImageSpecification.Usage & ImageUsage::DepthStencil)), "[VulkanImage] Tried to create image without specifying if it's a Colour or Depth image.");

		CreateImage(initCmd, imagePath);
	}

	hintinline VulkanImage::VulkanImage(const ImageSpecification& imageSpecs, VkImage image, VkImageView imageView)
		: m_ImageSpecification(imageSpecs), m_SamplerSpecification({}), m_Image(image), m_ImageView(imageView)
	{
	}

	hintinline VulkanImage::~VulkanImage()
	{
		DestroyImage();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void VulkanImage::SetData(const CommandBuffer& cmd, void* data, size_t size)
	{
		SetData(cmd, data, size, m_ImageSpecification.Layout);
	}

    ////////////////////////////////////////////////////////////////////////////////////
    // Convert functions
    ////////////////////////////////////////////////////////////////////////////////////
	constexpr ImageUsage VkImageUsageToImageUsage(VkImageUsageFlags usage)
	{
		ImageUsage result = ImageUsage::None;

		if (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)							result |= ImageUsage::TransferSrc;
		if (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)							result |= ImageUsage::TransferDst;
		if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)									result |= ImageUsage::Sampled;
		if (usage & VK_IMAGE_USAGE_STORAGE_BIT)									result |= ImageUsage::Storage;
		if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)						result |= ImageUsage::Colour;
		if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)				result |= ImageUsage::DepthStencil;
		if (usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)					result |= ImageUsage::Transient;
		if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)						result |= ImageUsage::Input;
		if (usage & VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR)					result |= ImageUsage::VideoDecodeDstKHR;
		if (usage & VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR)					result |= ImageUsage::VideoDecodeSrcKHR;
		if (usage & VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR)					result |= ImageUsage::VideoDecodeDpbKHR;
		if (usage & VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT)				result |= ImageUsage::FragmentDensityMapEXT;
		if (usage & VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR)	result |= ImageUsage::FragmentShadingRateKHR;
		if (usage & VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT)						result |= ImageUsage::HostTransferEXT;
		if (usage & VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR)					result |= ImageUsage::VideoEncodeDstKHR;
		if (usage & VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR)					result |= ImageUsage::VideoEncodeSrcKHR;
		if (usage & VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR)					result |= ImageUsage::VideoEncodeDpbKHR;
		if (usage & VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT)			result |= ImageUsage::FeedbackLoopEXT;
		if (usage & VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI)					result |= ImageUsage::InvocationMaskHuawei;
		if (usage & VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM)						result |= ImageUsage::SampleWeightQCOM;
		if (usage & VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM)					result |= ImageUsage::SampleBlockMatchQCOM;
		if (usage & VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV)					result |= ImageUsage::ShadingRateImageNV;

		return result;
	}

	constexpr VkImageUsageFlags ImageUsageToVkImageUsage(ImageUsage usage)
	{
		VkImageUsageFlags result = 0;

		if (static_cast<bool>(usage & ImageUsage::TransferSrc))					result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (static_cast<bool>(usage & ImageUsage::TransferDst))					result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (static_cast<bool>(usage & ImageUsage::Sampled))						result |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if (static_cast<bool>(usage & ImageUsage::Storage))						result |= VK_IMAGE_USAGE_STORAGE_BIT;
		if (static_cast<bool>(usage & ImageUsage::Colour))						result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (static_cast<bool>(usage & ImageUsage::DepthStencil))				result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if (static_cast<bool>(usage & ImageUsage::Transient))					result |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if (static_cast<bool>(usage & ImageUsage::Input))						result |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		if (static_cast<bool>(usage & ImageUsage::VideoDecodeDstKHR))			result |= VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::VideoDecodeSrcKHR))			result |= VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::VideoDecodeDpbKHR))			result |= VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::FragmentDensityMapEXT))		result |= VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;
		if (static_cast<bool>(usage & ImageUsage::FragmentShadingRateKHR))		result |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::HostTransferEXT))				result |= VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT;
		if (static_cast<bool>(usage & ImageUsage::VideoEncodeDstKHR))			result |= VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::VideoEncodeSrcKHR))			result |= VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::VideoEncodeDpbKHR))			result |= VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;
		if (static_cast<bool>(usage & ImageUsage::FeedbackLoopEXT))				result |= VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT;
		if (static_cast<bool>(usage & ImageUsage::InvocationMaskHuawei))		result |= VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI;
		if (static_cast<bool>(usage & ImageUsage::SampleWeightQCOM))			result |= VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM;
		if (static_cast<bool>(usage & ImageUsage::SampleBlockMatchQCOM))		result |= VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM;
		if (static_cast<bool>(usage & ImageUsage::ShadingRateImageNV))			result |= VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV;

		return result;
	}

	constexpr ImageLayout VkImageLayoutToImageLayout(VkImageLayout layout)
	{
		switch (layout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:											return ImageLayout::Undefined;
		case VK_IMAGE_LAYOUT_GENERAL:											return ImageLayout::General;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:							return ImageLayout::Colour;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:					return ImageLayout::DepthStencil;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:					return ImageLayout::DepthStencilRead;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:							return ImageLayout::ShaderRead;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:								return ImageLayout::TransferSrc;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:								return ImageLayout::TransferDst;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:									return ImageLayout::PreInitialized;
		case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:		return ImageLayout::DepthReadStencil;
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:		return ImageLayout::DepthReadStencilRead;
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:							return ImageLayout::Depth;
		case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:							return ImageLayout::DepthRead;
		case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:						return ImageLayout::Stencil;
		case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:							return ImageLayout::StencilRead;
		case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:									return ImageLayout::Read;
		case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:								return ImageLayout::Attachment;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:									return ImageLayout::PresentSrcKHR;
		case VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR:								return ImageLayout::VideoDecodeDstKHR;
		case VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR:								return ImageLayout::VideoDecodeSrcKHR;
		case VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR:								return ImageLayout::VideoDecodeDpbKHR;
		case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:								return ImageLayout::SharedPresentKHR;
		case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:					return ImageLayout::FragmentDensityMapEXT;
		case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:		return ImageLayout::FragmentShadingRateKHR;
		case VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR:							return ImageLayout::RenderingLocalReadKHR;
		case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR:								return ImageLayout::VideoEncodeDstKHR;
		case VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR:								return ImageLayout::VideoEncodeSrcKHR;
		case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR:								return ImageLayout::VideoEncodeDpbKHR;
		case VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT:				return ImageLayout::FeedbackLoopEXT;

		default:
			LU_ASSERT(false, "[VulkanImage] Image layout not implemented.");
			break;
		}
		return ImageLayout::Undefined;
	}

	constexpr VkImageLayout ImageLayoutToVkImageLayout(ImageLayout layout)
	{
		switch (layout)
		{
		case ImageLayout::Undefined:											return VK_IMAGE_LAYOUT_UNDEFINED;
		case ImageLayout::General:												return VK_IMAGE_LAYOUT_GENERAL;
		case ImageLayout::Colour:												return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthStencil:											return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthStencilRead:										return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::ShaderRead:											return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case ImageLayout::TransferSrc:											return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case ImageLayout::TransferDst:											return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case ImageLayout::PreInitialized:										return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case ImageLayout::DepthReadStencil:										return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthReadStencilRead:									return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::Depth:												return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthRead:											return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
		case ImageLayout::Stencil:												return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::StencilRead:											return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::Read:													return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		case ImageLayout::Attachment:											return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		case ImageLayout::PresentSrcKHR:										return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case ImageLayout::VideoDecodeDstKHR:									return VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR;
		case ImageLayout::VideoDecodeSrcKHR:									return VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR;
		case ImageLayout::VideoDecodeDpbKHR:									return VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR;
		case ImageLayout::SharedPresentKHR:										return VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
		case ImageLayout::FragmentDensityMapEXT:								return VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
		case ImageLayout::FragmentShadingRateKHR:								return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
		case ImageLayout::RenderingLocalReadKHR:								return VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR;
		case ImageLayout::VideoEncodeDstKHR:									return VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR;
		case ImageLayout::VideoEncodeSrcKHR:									return VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR;
		case ImageLayout::VideoEncodeDpbKHR:									return VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR;
		case ImageLayout::FeedbackLoopEXT:										return VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT;

		default:
			LU_ASSERT(false, "[VulkanImage] Image layout not implemented.");
			break;
		}

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	constexpr ImageFormat VkFormatToImageFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_UNDEFINED:												return ImageFormat::Undefined;
		case VK_FORMAT_R8G8B8A8_UNORM:											return ImageFormat::RGBA;
		case VK_FORMAT_B8G8R8A8_UNORM:											return ImageFormat::BGRA;
		case VK_FORMAT_R8G8B8A8_SRGB:											return ImageFormat::sRGB;
		case VK_FORMAT_D32_SFLOAT:												return ImageFormat::Depth32SFloat;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:										return ImageFormat::Depth32SFloatS8;
		case VK_FORMAT_D24_UNORM_S8_UINT:										return ImageFormat::Depth24UnormS8;

		default:
			LU_ASSERT(false, "[VulkanImage] Format not implemented.");
			break;
		}

		return ImageFormat::Undefined;
	}

	constexpr VkFormat ImageFormatToVkFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::Undefined:											return VK_FORMAT_UNDEFINED;
		case ImageFormat::RGBA:													return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::BGRA:													return VK_FORMAT_B8G8R8A8_UNORM;
		case ImageFormat::sRGB:													return VK_FORMAT_R8G8B8A8_SRGB;
		case ImageFormat::Depth32SFloat:										return VK_FORMAT_D32_SFLOAT;
		case ImageFormat::Depth32SFloatS8:										return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case ImageFormat::Depth24UnormS8:										return VK_FORMAT_D24_UNORM_S8_UINT;

		default:
			LU_ASSERT(false, "[VulkanImage] Format not implemented.");
			break;
		}

		return VK_FORMAT_UNDEFINED;
	}

	constexpr FilterMode VkFilterToFilterMode(VkFilter filter)
	{
		switch (filter)
		{
		case VK_FILTER_NEAREST:													return FilterMode::Nearest;
		case VK_FILTER_LINEAR:													return FilterMode::Linear;
		case VK_FILTER_CUBIC_EXT:												return FilterMode::CubicEXT;

		default:
			LU_ASSERT(false, "[VulkanImage] Filter not implemented.");
			break;
		}

		return FilterMode::CubicEXT;
	}

	constexpr VkFilter FilterModeToVkFilter(FilterMode filter)
	{
		switch (filter)
		{
		case FilterMode::Nearest:												return VK_FILTER_NEAREST;
		case FilterMode::Linear:												return VK_FILTER_LINEAR;
		case FilterMode::CubicEXT:												return VK_FILTER_CUBIC_EXT;

		default:
			LU_ASSERT(false, "[VulkanImage] Filter not implemented.");
			break;
		}

		return VK_FILTER_NEAREST;
	}

	constexpr AddressMode VkSamplerAddressModeToAddressMode(VkSamplerAddressMode mode)
	{
		switch (mode)
		{
		case VK_SAMPLER_ADDRESS_MODE_REPEAT:									return AddressMode::Repeat;
		case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:							return AddressMode::MirroredRepeat;
		case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:								return AddressMode::ClampToEdge;
		case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:							return AddressMode::ClampToBorder;
		case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:						return AddressMode::MirrorClampToEdge;

		default:
			LU_ASSERT(false, "[VulkanImage] Address not implemented.");
			break;
		}

		return AddressMode::Repeat;
	}

	constexpr VkSamplerAddressMode AddressModeToVkSamplerAddressMode(AddressMode mode)
	{
		switch (mode)
		{
		case AddressMode::Repeat:												return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case AddressMode::MirroredRepeat:										return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case AddressMode::ClampToEdge:											return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case AddressMode::ClampToBorder:										return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case AddressMode::MirrorClampToEdge:									return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

		default:
			LU_ASSERT(false, "[VulkanImage] Address not implemented.");
			break;
		}

		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}

	constexpr MipmapMode VkSamplerMipmapModeToMipmapMode(VkSamplerMipmapMode mode)
	{
		switch (mode)
		{
		case VK_SAMPLER_MIPMAP_MODE_NEAREST:									return MipmapMode::Nearest;
		case VK_SAMPLER_MIPMAP_MODE_LINEAR:										return MipmapMode::Linear;

		default:
			LU_ASSERT(false, "[VulkanImage] MipmapMode not implemented.");
			break;
		}

		return MipmapMode::Nearest;
	}

	constexpr VkSamplerMipmapMode MipmapModeToVkSamplerMipmapMode(MipmapMode mode)
	{
		switch (mode)
		{
		case MipmapMode::Nearest:												return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case MipmapMode::Linear:												return VK_SAMPLER_MIPMAP_MODE_LINEAR;

		default:
			LU_ASSERT(false, "[VulkanImage] MipmapMode not implemented.");
			break;
		}

		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	constexpr VkImageAspectFlags VkFormatToVkImageAspectFlags(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;

		case VK_FORMAT_S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT;

		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		return VK_IMAGE_ASPECT_COLOR_BIT;
	}

	constexpr bool VkFormatIsDepth(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		}

		return false;
	}

	constexpr bool VkFormatHasStencil(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		}

		return false;
	}

}