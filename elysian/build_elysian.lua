project "elysian"

    location "%{prj.name}"   --Creates a new folder 'elysian' containing project files
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir ("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. output_dir .. "/%{prj.name}")
   
    pchheader "pch.h"
    pchsource "src/pch.cpp"

-- refer to 'tokens' in premake docs for %{prj.name} this 

   files 
   { 
        "src/**.h", 
        "src/**.cpp", 
    
	    "%{vendor_dir}/stb_image/stb_image.cpp",
	    "%{vendor_dir}/stb_image/stb_image.h",

        "%{vendor_dir}/glm/**.hpp",
	    "%{vendor_dir}/glm/**.inl",
    }

   includedirs
   {
        "src",
        "%{include_dir.glfw}",
        "%{include_dir.glad}",
        "%{include_dir.spdlog}",    
        "%{include_dir.imgui}",
        "%{include_dir.imgui_backends}",
        "%{vendor_dir}", --for std_image & glm
        "%{include_dir.assimp}",
   }

    links
	{
        "%{library.glfw}",
        "glad",
		"opengl32",
        "imgui",
        "%{library.assimp}",
	}

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
	}

   filter "system:windows"
       systemversion "latest"
       defines {  }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "on"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "on"
       symbols "on"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "on"
       symbols "Off"

   filter{	"files:../../vendor/**" }
	   flags { "NoPCH" }	

    -- Doesn't work!  
    --filter{	"files:" .. vendor_dir .. "/**" }
	   --flags { "NoPCH" }	   