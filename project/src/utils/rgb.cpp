#include "utils/rgb.h"
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Scop::Utils {
  glm::vec3 Rgb2hsv(glm::vec3 in)
  {
    glm::vec3         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.z = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
      out.s = 0;
      out.x = 0; // undefined, maybe nan?
      return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
      out.s = (delta / max);                  // s
    }
    else {
      // if max is 0, then r = g = b = 0              
      // s = 0, h is undefined
      out.s = 0.0;
      out.x = NAN;                            // its now undefined
      return out;
    }
    if (in.r >= max)                           // > is bogus, just keeps compilor happy
      out.x = (in.g - in.b) / delta;        // between yellow & magenta
    else
      if (in.g >= max)
        out.x = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
      else
        out.x = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

    out.x *= 60.0;                              // degrees

    if (out.x < 0.0)
      out.x += 360.0;

    return out;
  }


  glm::vec3 Hsv2rgb(glm::vec3 in)
  {
    in.x = std::fmod(100.0 + in.x, 1.0);                                       // Ensure [0,1[

    float   HueSlice = 6.0 * in.x;                                            // In [0,6[
    float   HueSliceInteger = std::floor(HueSlice);
    float   HueSliceInterpolant = HueSlice - HueSliceInteger;                   // In [0,1[ for each hue slice

    glm::vec3  TempRGB = glm::vec3(in.z * (1.0 - in.y),
      in.z * (1.0 - in.y * HueSliceInterpolant),
      in.z * (1.0 - in.y * (1.0 - HueSliceInterpolant)));

    float   IsOddSlice = std::fmod(HueSliceInteger, 2.0);                          // 0 if even (slices 0, 2, 4), 1 if odd (slices 1, 3, 5)
    float   ThreeSliceSelector = 0.5 * (HueSliceInteger - IsOddSlice);          // (0, 1, 2) corresponding to slices (0, 2, 4) and (1, 3, 5)

    glm::vec3  ScrollingRGBForEvenSlices = glm::vec3(in.z, glm::vec2{ TempRGB.z, TempRGB.x });           // (V, Temp Blue, Temp Red) for even slices (0, 2, 4)
    glm::vec3  ScrollingRGBForOddSlices = glm::vec3(TempRGB.y, in.z, TempRGB.x);  // (Temp Green, V, Temp Red) for odd slices (1, 3, 5)
    glm::vec3  ScrollingRGB = glm::lerp(ScrollingRGBForEvenSlices, ScrollingRGBForOddSlices, IsOddSlice);

    float   IsNotFirstSlice = glm::saturate(ThreeSliceSelector);                   // 1 if NOT the first slice (true for slices 1 and 2)
    float   IsNotSecondSlice = glm::saturate(ThreeSliceSelector - 1.0);              // 1 if NOT the first or second slice (true only for slice 2)

    return  glm::lerp(ScrollingRGB, glm::lerp(ScrollingRGB, glm::vec3{ ScrollingRGB.y, ScrollingRGB.z, ScrollingRGB.x }, IsNotSecondSlice), IsNotFirstSlice);
  }
}