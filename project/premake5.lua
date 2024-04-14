project "Scop"
  kind "ConsoleApp"
  language "C++"
  targetname "scop"
  cppdialect "C++20"
  targetdir "bin/%{cfg.buildcfg}"
  staticruntime "off"
  stl "libc++"
  buildoptions { "-Wall", "-Wextra", "-Werror" }

  files {
    "src/**.cpp"
  }

  includedirs {
    "includes",
    vendorFiles("glfw/include")(),
    vendorFiles("glm")(),
    vendorFiles("entt/src")()
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