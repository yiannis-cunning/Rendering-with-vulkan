#ifndef VULKAN3_H
#define VULKAN3_H 

#include <vulkan/vulkan.h>
#include "src/level_ctrl.h"

typedef struct charcter_t{
       uint8_t index;
       float pos[2];
} charcter_t;


typedef struct screenProperties_t{
       float offset[3];
       float view[3];

       float fov;
       float renderDist;
       float clipDist;

       charcter_t *chars;
       int chars_sz;
       //int n_chars;

       char *text_buffer;
       uint32_t n_chars;
       uint32_t alloc_size;
} screenProperties_t;


void vulkan_run();

void cleanup();

void createSurface();

void createInstance();

void passert(int tf, const char *s);

void check_err(VkResult retval, const char *s);

void vulkan_drawFrame(screenProperties_t screen);

void vulkan_resized();

void load_level(char *filename);

uint8_t *load_file(const char *filename, uint32_t *sz_out, int flags);

typedef struct Vertex_t{
       float pos[3];
       float color[3];
} Vertex_t;


typedef struct Vertex_tex_t {
       float pos[3];
       float color[3];
       float texCord[2];
} Vertex_tex_t;


#endif