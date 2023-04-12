workspace "ECS_Project"
      startproject "ECS"
	  architecture "x64"

	  configurations
	  {
	     "Debug",
		 "Release",
		 "Distribution"
	  }

project_output = "%{cfg.buildcgf}-%{cfg.system}-%{cfg.architecture}"

project "ECS"
    location "ECS"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("Build/Bin".. project_output .. "{prj.name}")
	objdir ("Build/intermediate/".. project_output .. "{prj.name}")

	files
	{
	    "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"SnapCore/src"
	}

	filter "system:windows"
	    cppdialect "c++17" 
		staticruntime "On"
		systemversion "latest"

		defines
	{
		"ECS_WINDOWS",
		"ECS_ENABLE_ASSERTION"
	}
	
	filter "configurations:Debug"
		defines "ECS_DEBUG"
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "ECS_RELEASE"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ECS_DIST"
		buildoptions "/MT"
		runtime "Release"
		optimize "on"