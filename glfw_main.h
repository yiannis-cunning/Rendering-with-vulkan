#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>


VkResult get_wind_surface_spec(VkSurfaceKHR *surface, VkInstance instance);


const char ** get_wind_extensions_sec(uint32_t *pCount);


void get_wind_dims(int *width, int *hight);