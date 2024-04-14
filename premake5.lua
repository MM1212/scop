workspace "Scop"
  architecture "x64"
  configurations { "debug", "release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
rootDir = path.getabsolute(".")

function vendorFiles(lib)
  local vendorPath = path.join(rootDir, "vendor", lib)
  return function(...)
    return path.join(vendorPath, ...);
  end
end

local function vendor(lib)
  include (path.join("vendor","scripts", lib .. ".lua"))
end

group "vendor"
  vendor "glfw"
group ""

include "project"