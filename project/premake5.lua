project "Scop"
  kind "ConsoleApp"
  language "C++"
  targetname "scop"
  cppdialect "C++20"
  targetdir "bin/%{cfg.buildcfg}"
  staticruntime "off"
  stl "libc++"
  buildoptions { "-Wall", "-Wextra", "-Werror", "-gdwarf-2" }

  files {
    "src/**.cpp",
    "shaders/**.vert",
    "shaders/**.frag"
  }

  defines {
    "GLFW_INCLUDE_VULKAN",
    "GLM_FORCE_RADIANS",
    "GLM_FORCE_DEPTH_ZERO_TO_ONE",
    -- "GLM_FORCE_LEFT_HANDED",
    ('SHADERS_PATH="bin/%s/%%{prj.name}/shaders/"'):format(outputdir)
  }

  includedirs {
    "includes",
    vendorFiles("glfw/include")(),
    vendorFiles("glm")(),
    vendorFiles("entt/src")(),
    vendorFiles("tinyobjloader")()
  }

  links {
    "GL",
    "GLFW",
    "vulkan"
  }

  targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
  objdir ("../objs/" .. outputdir .. "/%{prj.name}")

  filter "configurations:debug"
    defines { "DEBUG" }
    runtime "Debug"
    symbols "On"

  filter "configurations:release"
    defines { "RELEASE" }
    runtime "Release"
    optimize "On"

  filter "files:**.vert or **.frag"
    buildmessage "Compiling %{file.relpath}"
    buildcommands {
      "mkdir -p %{cfg.targetdir}/shaders",
      "glslc %{file.relpath} -o %{cfg.targetdir}/shaders/%{file.basename}%{file.extension}.spv"
    }
    buildoutputs {
      "%{cfg.targetdir}/shaders/%{file.basename}%{file.extension}.spv"
    }
-- include "shaders"