#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>


bool get_wind_surface_spec(VkSurfaceKHR *surface, VkInstance instance);


bool get_wind_extensions_sec(uint32_t *pCount, const char **pNames);