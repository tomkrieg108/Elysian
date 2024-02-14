project "sandbox1"

   location "%{prj.name}"   --This will create a new folder called sandbox that contains th  project files
   kind "ConsoleApp" -- If don't want a console sindow to appear the need to set as a windows app (apparently)
   language "C++"
   cppdialect "C++17"
   --staticruntime "on"
   targetdir ("../bin/" .. output_dir .. "/%{prj.name}")
   objdir ("../bin/intermediates/" .. output_dir .. "/%{prj.name}")

   files 
   { 
        "src/**.h", 
        "src/**.cpp",
   }

    includedirs
    {
        "src",
	    "../elysian/src",
        external_libs_dir .. "/GLEW/include",
	    external_libs_dir .. "/GLFW/%{cfg.platform}/include",
		external_libs_dir .. "/ASSIMP/%{cfg.platform}/include",
        vendor_dir .. "/spdlog/include",
        vendor_dir .. "/imgui_docking",
		vendor_dir .. "/imgui_docking/backends",
		vendor_dir,
    }

   links
   {
    --dont need to specify libdirs for libs in the same workspace 
    -- see premake docs
      "elysian"
   }

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

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