#ifndef GLFW_MAIN_H
#define GLFW_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>


VkResult get_wind_surface_spec(VkSurfaceKHR *surface, VkInstance instance);


const char ** get_wind_extensions_sec(uint32_t *pCount);


void get_wind_dims(int *width, int *hight);

float get_delta_frame_time();

void mouselockon();

void mouselockoff();


#endif