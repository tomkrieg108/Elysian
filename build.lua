workspace "elysian"

   architecture "x64"
   platforms {"Win64"}
   configurations { "Debug", "Release", "Dist" }
   startproject "sandbox1"

   -- Workspace-wide build options for MSVC
   -- will detect system as "windows" without needing to specify
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

output_dir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

--NOTE! SHOULD AVOID USING ABSOLUTE PATHS GENERALLY!!!
vendor_dir = "C:/dev/vendor"
external_libs_dir = "C:/dev/external_libs"

-- creates a 'virtual folder to contain 1 or more projects - don't want!
--group "Core" 
include "elysian/build_elysian.lua"
--group ""

include "sandbox1/build_sandbox1.lua"
--include "proj_terrain_gen/build_terrain_gen.lua"