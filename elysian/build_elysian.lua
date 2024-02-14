project "elysian"

    --This will create a new folder called elysian that contains that project files
   location "%{prj.name}" 
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   --staticruntime "on"
   targetdir ("../bin/" .. output_dir .. "/%{prj.name}")
   objdir ("../bin/intermediates/" .. output_dir .. "/%{prj.name}")
   pchheader "pch.h"
   pchsource "src/pch.cpp"

-- refer to 'tokens' in premake docs for %{prj.name} this 

   --note: relative paths should be relative to the location of this lua script

   files 
   { 
        "src/**.h", 
        "src/**.cpp", 
		vendor_dir .. "/imgui_docking/*.cpp",
	    vendor_dir .. "/imgui_docking/*.h",
		vendor_dir .. "/imgui_docking/backends/imgui_impl_glfw.cpp",
		vendor_dir .. "/imgui_docking/backends/imgui_impl_opengl3.cpp",
		vendor_dir .. "/imgui_docking/backends/imgui_impl_glfw.h",
		vendor_dir .. "/imgui_docking/backends/imgui_impl_opengl3.h",
		vendor_dir .. "/stb_image/stb_image.cpp",
		vendor_dir .. "/stb_image/stb_image.h",
    }

   includedirs
   {
        "src",
        external_libs_dir .. "/GLEW/include",
	    external_libs_dir .. "/GLFW/%{cfg.platform}/include",
		external_libs_dir .. "/ASSIMP/%{cfg.platform}/include",
		vendor_dir .. "/imgui_docking",
		vendor_dir .. "/imgui_docking/backends",
        vendor_dir .. "/spdlog/include",
		vendor_dir,
   }

   libdirs 
	{ 
		external_libs_dir .. "/GLEW/lib/release/%{cfg.platform}",
		external_libs_dir .. "/GLFW/%{cfg.platform}/lib-vc2022",
		external_libs_dir .. "/ASSIMP/%{cfg.platform}/lib/release",
	}

    links
	{
		"glew32s",
		"glfw3",
		"opengl32",
		"assimp-vc143-mt", --64bit version

        --//https://stackoverflow.com/questions/36017915/visual-studio-2015-default-additional-libraries

        --these not needed ?
        --"kernel32",
		--"User32",
		--"Gdi32",
		--"Shell32",
	}
    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
        "GLEW_STATIC"
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

   filter{	"files:../../../vendor/**" }
	   flags { "NoPCH" }	