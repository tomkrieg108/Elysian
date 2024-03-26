workspace "elysian"

   architecture "x64"
   platforms {"Win64"}
   configurations { "Debug", "Release", "Dist" }
   startproject "editor"

    --note: relative paths are relative to the location of this lua script
   output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
   vendor_dir = "%{wks.location}/../vendor"
   external_libs_dir = "%{wks.location}/../external_libs"

   include_dir = {}
   include_dir["glad"] = "%{vendor_dir}/glad/include"
   include_dir["glfw"] = "%{vendor_dir}/glfw/include"
   include_dir["spdlog"] = "%{vendor_dir}/spdlog/include"
   include_dir["imgui"] = "%{vendor_dir}/imgui_docking"
   include_dir["imgui_bankends"] = "%{vendor_dir}/imgui_docking/backends"
   include_dir["stb_image"] = "%{vendor_dir}/stb_img"
   include_dir["assimp"] = "%{vendor_dir}/assimp/include" 
   include_dir["entt"] = "%{vendor_dir}/entt/single_include"
  
   library = {}
   library["glfw"] = "%{vendor_dir}/glfw/src/%{cfg.buildcfg}/glfw3.lib"
   library["assimp"] = "%{vendor_dir}/assimp/UILD_SHARED_LIBS=ON/lib/Release/assimp-vc143-mt.lib"

   -- Workspace-wide build options for MSVC
   -- will detect system as "windows" without needing to specify
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

   group "Dependencies"
      include "elysian/vendor/glad/build_glad.lua"
      include "elysian/vendor/imgui/build_imgui.lua"
   group ""

   include "elysian/build_elysian.lua"
   include "sandbox/build_sandbox.lua"
   include "editor/build_editor.lua"

   ok, err = os.copyfile("%{vendor_dir}/assimp/UILD_SHARED_LIBS=ON/bin/assimp-vc143-mt.dll","%{wks.location}/bin/Debug-windows-x86_64/sandbox1/assimp-vc143-mt.dll")

   ok, err = os.copyfile("%{vendor_dir}/assimp/UILD_SHARED_LIBS=ON/bin/assimp-vc143-mt.dll","%{wks.location}/bin/Release-windows-x86_64/sandbox1/assimp-vc143-mt.dll")
