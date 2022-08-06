#pragma once

#include <new>

#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/allocator.h>
#include <EASTL/queue.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <malloc.h>
#include <memory>