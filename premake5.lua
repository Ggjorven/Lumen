------------------------------------------------------------------------------
-- Utilities
------------------------------------------------------------------------------
local function GetIOResult(cmd)
	local handle = io.popen(cmd) -- Open a console and execute the command.
	local output = handle:read("*a") -- Read the output.
	handle:close() -- Close the handle.

	return output:match("^%s*(.-)%s*$") -- Trim any trailing whitespace (such as newlines)
end

function GetOS()
	local osName = os.getenv("OS")

	if osName == "Windows_NT" then
		return "windows"
	else
		local uname = io.popen("uname"):read("*l")
		if uname == "Linux" then
			return "linux"
		elseif uname == "Darwin" then
			return "macosx"
		end
	end

	return "unknown-os"
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Bug fixes
------------------------------------------------------------------------------
-- Visual Studio: Bugfix for C++ Modules (same module file name per project)
-- https://github.com/premake/premake-core/issues/2177
require("vstudio")
premake.override(premake.vstudio.vc2010.elements, "clCompile", function(base, prj)
    local m = premake.vstudio.vc2010
    local calls = base(prj)

    if premake.project.iscpp(prj) then
		table.insertafter(calls, premake.xmlDeclaration,  function()
			premake.w('<ModuleDependenciesFile>$(IntDir)\\%%(RelativeDir)</ModuleDependenciesFile>')
			premake.w('<ModuleOutputFile>$(IntDir)\\%%(RelativeDir)</ModuleOutputFile>')
			premake.w('<ObjectFileName>$(IntDir)\\%%(RelativeDir)</ObjectFileName>')
		end)
    end

    return calls
end)
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Dependencies
------------------------------------------------------------------------------
VULKAN_SDK = os.getenv("VULKAN_SDK")
VULKAN_VERSION = VULKAN_SDK:match("(%d+%.%d+%.%d+)") -- Example: 1.3.290 (without the 0)

MacOSVersion = "14.5"

Dependencies =
{
	GLFW =
	{
		LibName = "GLFW",
		IncludeDir = "%{wks.location}/vendor/GLFW/GLFW/include"
	},
	glm =
	{
		IncludeDir = "%{wks.location}/vendor/glm/glm"
	},
	stb = 
	{
		IncludeDir = "%{wks.location}/vendor/stb/include"
	},
	Tracy = 
	{
		LibName = "Tracy",
		IncludeDir = "%{wks.location}/vendor/tracy/tracy/public"
	},
}

------------------------------------------------------------------------------
-- Platform specific
------------------------------------------------------------------------------
if GetOS() == "windows" then
	Dependencies.Vulkan =
    {
        LibName = "vulkan-1",
		IncludeDir = "%{VULKAN_SDK}/Include/",
		LibDir = "%{VULKAN_SDK}/Lib/"
    }
	Dependencies.ShaderC = { LibName = "shaderc_shared" }

elseif GetOS() == "linux" then
	Dependencies.Vulkan =
    {
        LibName = "vulkan",
		IncludeDir = "%{VULKAN_SDK}/include/",
		LibDir = "%{VULKAN_SDK}/lib/"
    }
	Dependencies.ShaderC = { LibName = "shaderc_shared" }

elseif GetOS() == "macosx" then
	Dependencies.Vulkan = -- Note: Vulkan on MacOS is currently dynamic. (Example: libvulkan1.3.290.dylib)
	{
        LibName = "vulkan.%{VULKAN_VERSION}",
		IncludeDir = "%{VULKAN_SDK}/../macOS/include/",
		LibDir = "%{VULKAN_SDK}/../macOS/lib/",
    }
	Dependencies.ShaderC = { LibName = "shaderc_combined" }
end
------------------------------------------------------------------------------
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Solution
------------------------------------------------------------------------------
outputdir = "%{cfg.buildcfg}-%{cfg.system}"

workspace "Lumen"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/tracy"
group ""

group "Lumen"
	include "Lumen"
group ""

include "Sandbox"
------------------------------------------------------------------------------