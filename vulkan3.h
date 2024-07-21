#ifndef VULKAN3_H
#define VULKAN3_H 

#include <vulkan/vulkan.h>

typedef struct charcter_t{
       char index;
       float pos[2];
} charcter_t;


typedef struct screenProperties_t{
       float offset[3];
       float view[3];

       float fov;
       float renderDist;
       float clipDist;

       int n_characters;
       charcter_t *characters;
       uint32_t char_grid_dims[2];

} screenProperties_t;


void vulkan_run();

void cleanup();

void createSurface();

void createInstance();

void passert(int tf, const char *s);

void check_err(VkResult retval, const char *s);

void vulkan_drawFrame(screenProperties_t screen);

void vulkan_resized();

typedef struct Vertex_t{
       float pos[3];
       float color[3];
} Vertex_t;



#endif