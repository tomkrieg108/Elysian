project "sandbox"

    location "%{prj.name}"   --This will create a new folder called sandbox that contains th  project files
    kind "ConsoleApp" -- If don't want a console window to appear the need to set as a windows app (apparently)
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetdir ("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. output_dir .. "/%{prj.name}")

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
    }

    includedirs
    {
        "src",
        "%{wks.location}/elysian/src",

        "%{include_dir.glfw}",
        "%{include_dir.glad}",
        "%{include_dir.spdlog}",    
        "%{include_dir.imgui}",
        "%{include_dir.imgui_backends}",
        "%{vendor_dir}", --for std_image & glm
        "%{include_dir.assimp}",
        "%{include_dir.entt}",
    }

    links
    {
        --dont need to specify libdirs for libs in the same workspace (see premake docs)
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
        symbols "off"

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "on"
        symbols "Off"