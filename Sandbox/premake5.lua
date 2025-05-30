MacOSVersion = MacOSVersion or "14.5"

project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "On"

	debugdir ("%{prj.location}")

	architecture "x86_64"

	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.inl",
		"src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",

		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",

		"%{wks.location}/Lumen/src",

		"%{Dependencies.GLFW.IncludeDir}",
		"%{Dependencies.glm.IncludeDir}",
		"%{Dependencies.Tracy.IncludeDir}",
		"%{Dependencies.Vulkan.IncludeDir}",
	}

	links
	{
		"Lumen",
	}

	filter "system:windows"
		defines "LU_PLATFORM_DESKTOP"
		defines "LU_PLATFORM_WINDOWS"
		systemversion "latest"
		staticruntime "on"
		editandcontinue "off"

        defines
        {
            "NOMINMAX"
        }

	filter "system:linux"
		defines "LU_PLATFORM_DESKTOP"
		defines "LU_PLATFORM_LINUX"
		systemversion "latest"
		staticruntime "on"

		links
		{
			"%{Dependencies.GLFW.LibName}",
			"%{Dependencies.Tracy.LibName}",

			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.Vulkan.LibName}",
			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.ShaderC.LibName}",
		}

    filter "system:macosx"
		defines "LU_PLATFORM_DESKTOP"
		defines "LU_PLATFORM_MACOS"
		systemversion(MacOSVersion)
		staticruntime "on"

		libdirs
		{
			"%{Dependencies.Vulkan.LibDir}"
		}

		links
		{
			"%{Dependencies.Vulkan.LibName}",
			"%{Dependencies.ShaderC.LibName}",

			"AppKit.framework",
			"IOKit.framework",
			"CoreGraphics.framework",
			"CoreFoundation.framework",
			"QuartzCore.framework",
		}

		postbuildcommands
		{
			'{COPYFILE} "%{Dependencies.Vulkan.LibDir}/libvulkan.1.dylib" "%{cfg.targetdir}"',
			'{COPYFILE} "%{Dependencies.Vulkan.LibDir}/lib%{Dependencies.Vulkan.LibName}.dylib" "%{cfg.targetdir}"',
		}

	filter "action:xcode*"
		-- Note: If we don't add the header files to the externalincludedirs
		-- we can't use <angled> brackets to include files.
		externalincludedirs
		{
			"src",

			"%{wks.location}/Lumen/src",

			"%{Dependencies.GLFW.IncludeDir}",
			"%{Dependencies.glm.IncludeDir}",
			"%{Dependencies.Tracy.IncludeDir}",
			"%{Dependencies.Vulkan.IncludeDir}",
		}

	filter "configurations:Debug"
		defines "LU_CONFIG_DEBUG"
		runtime "Debug"
		symbols "on"

		defines
		{
			"TRACY_ENABLE"
		}

	filter "configurations:Release"
		defines "LU_CONFIG_RELEASE"
		runtime "Release"
		optimize "on"

		defines
		{
			"TRACY_ENABLE"
		}

	filter "configurations:Dist"
		kind "WindowedApp"
		defines "LU_CONFIG_DIST"
		runtime "Release"
		optimize "Full"
		linktimeoptimization "on"
