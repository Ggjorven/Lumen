#pragma once

#include "Lumen/Internal/Utils/Settings.hpp"

#include "Lumen/Internal/Vulkan/Vulkan.hpp"
#include "Lumen/Internal/Vulkan/VulkanDevices.hpp"

#include "Lumen/Core/Core.hpp"
#include "Lumen/Utils/DeferredConstruct.hpp"

#include <array>

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// VulkanContext
    ////////////////////////////////////////////////////////////////////////////////////
	class VulkanContext
	{
    public:
        // Settings
        inline constinit static const std::tuple<uint8_t, uint8_t> Version = { 1, 3 };
        inline constexpr static const bool Validation = (Info::g_Configuration != Info::Configuration::Dist);
        inline constexpr static auto ValidationLayers = std::to_array<const char*>({ "VK_LAYER_KHRONOS_validation", "VK_LAYER_KHRONOS_synchronization2" });
        inline constexpr static auto DeviceExtensions = std::to_array<const char*>({
            VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,

            #if defined(LU_PLATFORM_MACOS)
            "VK_KHR_portability_subset"
            #endif
        });
    public:
        // Constructors & Destructor
        VulkanContext() = default;
        ~VulkanContext() = default;

		// Init & Destroy
		void Init(void* window);
		void Destroy();

        // Static getters
        static VulkanDevice& GetVulkanDevice();
        static VulkanPhysicalDevice& GetVulkanPhysicalDevice();

        static VkInstance GetVkInstance();
        static VkDebugUtilsMessengerEXT GetVkDebugger();

    private:
        // Private methods
        void InitInstance();
        void InitDevices(void* window);

	private:
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

        DeferredConstruct<VulkanDevice, true> m_Device = {};
        DeferredConstruct<VulkanPhysicalDevice> m_PhysicalDevice = {};
	};

}