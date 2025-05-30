MacOSVersion = MacOSVersion or "14.5"

project "Lumen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "On"

	architecture "x86_64"

	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	-- Note: VS2022/Make only need the pchheader filename
	pchheader "lupch.h"
	pchsource "src/Lumen/lupch.cpp"

	files
	{
		"src/Lumen/**.h",
		"src/Lumen/**.hpp",
		"src/Lumen/**.inl",
		"src/Lumen/**.cpp"
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
		"src/Lumen",

		"%{Dependencies.GLFW.IncludeDir}",
		"%{Dependencies.glm.IncludeDir}",
		"%{Dependencies.stb.IncludeDir}",
		"%{Dependencies.Tracy.IncludeDir}",
		"%{Dependencies.Vulkan.IncludeDir}",
	}

	links
	{
		"%{Dependencies.GLFW.LibName}",
		"%{Dependencies.Tracy.LibName}",
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

		links
		{
			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.Vulkan.LibName}",
			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.ShaderC.LibName}",
		}

	filter "system:linux"
		defines "LU_PLATFORM_DESKTOP"
		defines "LU_PLATFORM_LINUX"
		systemversion "latest"
		staticruntime "on"

		links
		{
			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.Vulkan.LibName}",
			"%{Dependencies.Vulkan.LibDir}/%{Dependencies.ShaderC.LibName}",

			"Xrandr", "Xi", "GLU", "GL", "GLX", "X11", "dl", "pthread", "stdc++fs"
		}

    filter "system:macosx"
		defines "LU_PLATFORM_DESKTOP"
		defines "LU_PLATFORM_MACOS"
		systemversion(MacOSVersion)
		staticruntime "on"

	filter "action:xcode*"
		-- Note: XCode only needs the full pchheader path
		pchheader "src/Lumen/lupch.h"

		-- Note: If we don't add the header files to the externalincludedirs
		-- we can't use <angled> brackets to include files.
		externalincludedirs
		{
			"src",
			"src/Lumen",

			"%{Dependencies.GLFW.IncludeDir}",
			"%{Dependencies.glm.IncludeDir}",
			"%{Dependencies.stb.IncludeDir}",
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
		defines "LU_CONFIG_DIST"
		runtime "Release"
		optimize "Full"
		linktimeoptimization "on"
