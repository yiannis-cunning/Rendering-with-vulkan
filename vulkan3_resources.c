#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <io.h>
#include <fcntl.h>


#include "glfw_main.h"
#include "vulkan3.h"

typedef struct Vertex_t{
       float pos[2];
       float color[3];
} Vertex_t;

static Vertex_t Vertextriangle2D[3] = {
    (Vertex_t){{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    (Vertex_t){{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    (Vertex_t){{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

static VkVertexInputBindingDescription getBindingDescription() {
       VkVertexInputBindingDescription bindingDescription = {0};

       return bindingDescription;
}