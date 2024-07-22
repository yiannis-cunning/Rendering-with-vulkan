#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "glfw_main.h"
#include "vulkan3.h"
#include "protocol.h"
#include "linalg.h"
// #define VERBOSE


// 1) Structs
typedef struct QueueFamilyIndices_t {
       int graphicsAndComputeFamily;         // normal graphics queue index
       int n_graphics_qs;

       int compatableFamily;       // a queue family that is for sure compatable with the surface you are using
       int n_compatability_qs;
} QueueFamilyIndices_t;

typedef struct SwapChainSupportDetails_t {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR      *formats;
    uint32_t                n_formats;
    VkPresentModeKHR        *presentModes;
    uint32_t                n_presentModes;
} SwapChainSupportDetails_t;




// 2) private function defs

// Init - low amount of calling
void pickPhysicalDevice();

int isDeviceSuitable(VkPhysicalDevice device);

bool checkDeviceExtensionSupport(VkPhysicalDevice device);

SwapChainSupportDetails_t querySwapChainSupport(VkPhysicalDevice device);

void createInstance();

void createSurface();

void createLogicalDevice();

void createSwapChain();

void createImageViews();

QueueFamilyIndices_t findQueueFamilies(VkPhysicalDevice device);

void createRenderPass();

void createGraphicsPipeline();

void createComputePipeline();

void createComputeDescriptorSetLayout();

void createComputeDescriptorSets();

void createShaderStorageBuffers();

void createShaderStorageBuffers2();

void createComputeUniformBuffers();

void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);

void createVertexBuffer();

void createCommandPool();

void createColorResources();

void createDepthResources();

void createCommandBuffers();

void createFramebuffers();

void createSyncObjects();

void recreateSwapChain();

void cleanupSwapChain();

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, screenProperties_t screen);

void createIndexBuffer();

void createDescriptorSetLayout();

void createUniformBuffers();

void createDescriptorPool();

void createDescriptorSets();

void draw(screenProperties_t screen);

VkCommandBuffer beginSingleTimeCommands();

void endSingleTimeCommands(VkCommandBuffer commandBuffer);

/* Create texture vkImage from a image file*/
void createTextureImage(char *filename);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void createTextureSampler();

// helper
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory);

VkFormat findSupportedFormat(VkFormat *candidates, int n_candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

VkShaderModule make_shader_module(const char *filename);

void init_particles();

void create2dpiplineresources();

void updatetwodUniformBuffer(uint32_t currentImage, screenProperties_t screen);

void createGenericUniformBuffers(VkDeviceSize sizeofubo, int n_ubos, VkBuffer **uniformBuffer_out, VkDeviceMemory **unifromMemory_out, void ***uniformMap_out);

void createGPUBuffer();

void *safe_calloc(uint32_t size, uint32_t amt);

void createtwodDescriptorSetLayout();

void createtwodDescriptorSets();



// 3) local/private variables
typedef struct vulkan_instance_t{


       // Device + instance
       VkInstance           pInstance;
       VkSurfaceKHR         surface;
       VkPhysicalDevice     phyDevice;
       VkDevice             device;
       VkSampleCountFlagBits msaaSamples;

       VkQueue              graphicsQueue;
       VkQueue              presentQueue;


       /* Swap chain KHR and images */
       VkSwapchainKHR       swapChain;
       VkFormat             swapChainImageFormat;
       VkExtent2D           swapChainExtent;
       VkImage             *swapChainImages;
       VkImageView         *swapChainImageViews;
       uint32_t             imageCount;
       VkFramebuffer       *swapChainFramebuffers;

       /* Render pass and pipelines */
       VkRenderPass                renderPass;
       VkDescriptorSetLayout       descriptorSetLayout;
       VkPipelineLayout            pipelineLayout;
       VkPipeline                  graphicsPipeline;
       VkCommandPool               commandPool;
       VkCommandBuffer             *commandBuffers;     // 1 for every in flight frame

       /* Syncorhonization for frames*/
       VkSemaphore          *imageAvailableSemaphores;  // 1 for every in flight frame
       VkSemaphore          *renderFinishedSemaphores;  // 1 for every in flight frame
       VkFence              *inFlightFences;
       uint32_t             syncs_arr_size;
       uint32_t             currentFrame;


       // Render data
       /* Output image data */
       VkImage              colorImage;
       VkImageView          colorImageView;
       VkDeviceMemory       colorImageMemory;

       /*a) an image description, b) view of that image, c) allocation*/
       VkImage              depthImage;
       VkImageView          depthImageView;
       VkDeviceMemory       depthImageMemory;

       uint32_t             mipLevels;


       // Vertex data
       VkBuffer             vertexBuffer;
       VkDeviceMemory       vertexBufferMemory;
       VkBuffer             indexBuffer;
       VkDeviceMemory       indexBufferMemory;


       /* Uniform buffers/Global variables*/
       VkBuffer             *uniformBuffers;
       VkDeviceMemory       *uniformBuffersMemory;
       void                 **uniformBuffersMapped;
       VkDescriptorPool     descriptorPool;
       VkDescriptorSet      *descriptorSets;

       /* Compute resources */
       //     Layouts of the pipeline + inout data
       VkDescriptorSetLayout       computeDescriptorSetLayout; // Layout of descriptors
       VkPipelineLayout            computePipelineLayout;      // Layout of pipeline
       VkPipeline                  computePipeline;            // Pipeline itself

       //     allocations.
       VkDescriptorSet             *computeDescriptorSets;            // Desscriptor set(s) themselves - need one for each frame in flight
       VkBuffer                    *shaderStorageBuffers;             // Where the particles[] are stored.
       VkDeviceMemory              *shaderStorageBuffersMemory;       // ^

       VkBuffer                    *shaderStorageBuffers_3dgrid;             // Bigger allocation for 3D grid - will store 2x 4 vectors per point
       VkDeviceMemory              *shaderStorageBuffersMemory_3dgrid;       // ^

       VkBuffer                    shaderIndexBuffer;
       VkDeviceMemory              shaderIndexBufferMemory;

       VkBuffer                    *computeUniformBuffers;
       VkDeviceMemory              *computeUniformBuffersMemory;
       void                        **computeUniformBuffersMapped;

       VkCommandBuffer             *computeCommandBuffers;            // Place to put compute commands

       //     Synchronization
       VkSemaphore                 *computeFinishedSemaphores;  // 1 for every in flight frame
       VkFence                     *computeInFlightFences;


       /* line pipline */
       VkPipeline                  lineGraphicsPipeline;


       /* 2D stuff pipeline */
       VkPipeline                  twodGraphicsPipeline;

       VkBuffer                    twodvertexBuffer;
       VkDeviceMemory              twodvertexBufferMemory;
       VkBuffer                    twodindexBuffer;
       VkDeviceMemory              twodindexBufferMemory;

              /* Descriptor sets */
       VkDescriptorSetLayout       twddescriptorSetLayout;
       VkPipelineLayout            twodpipelineLayout;
       VkDescriptorSet             *twoddescriptorSets;

       VkImage                     textureImage;
       VkDeviceMemory              textureImageMemory;
       VkImageView                 textureImageView;

       VkSampler                   textureSampler;

       VkBuffer                    *twoduniformBuffers;
       VkDeviceMemory              *twoduniformBuffersMemory;
       void                        **twoduniformBuffersMapped;

       bool framebufferResized;


} vulkan_instance_t;

static vulkan_instance_t vulkan_info = {0};






// 4) Hardcoded vulkan preferences
static const char *global_required_extenions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
static uint32_t global_n_required_extenions = 1;


static VkFormat best_format = VK_FORMAT_B8G8R8A8_SRGB;
static VkColorSpaceKHR  best_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;    //
static VkPresentModeKHR best_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;         // How are images in swap chain taken/recived - affects preformace

const int MAX_FRAMES_IN_FLIGHT = 2;

VkFormat findDepthFormat(){     // Find a format with these properties, supported by device.
       /* Dont need a specific format - just need a depth component. (wont be accessing the texels directly */
       // VK_FORMAT_D32_SFLOAT is usually normal -> just adding the others for compatabliity cause they would also be fine
       VkFormat arr[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
       return findSupportedFormat(arr, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}






void vulkan_run(){

       init_particles();

       // 1. Init
       printf("\n1) Creating Vulkan instance\n");
       createInstance();           // No layers configured or debug -> extensions given by get_wind_extensions_sec()
       printf("\n2) Getting surface from the window\n");
       createSurface();            // Using window made function - get_wind_surface_spec()
       printf("\n3) Finding most suitable physical device\n");
       pickPhysicalDevice();       // Picking physical device and ensuring compatability
       printf("\n4) Creating a logical device based on best physical device\n");
       createLogicalDevice();      // Create the logical device

       // 2. SwapChain
       printf("\n5) Creating swap chain based on same image type from 2\n");
       createSwapChain();
       printf("\n6) Creating swap chain image views for each image\n");
       createImageViews();

       // 3. Rendering
       createDescriptorSetLayout();
       printf("\n7) Creating renderpass\n"); 
       createRenderPass();
       printf("\n8) Creating a pool for descriptors\n");
       createDescriptorPool();
       printf("\n9) Creating a command pool\n");
       createCommandPool();
       
       printf("\n10) Creating 2D pipeline resources\n");
       create2dpiplineresources();

       printf("\n11) Creating graphics pipeline\n");
       createGraphicsPipeline();


       printf("\n12) Createing a buffer for depth\n");
       createDepthResources();


       // 4. Compute
       printf("\n13)Creating a layout for the compute descriptor set\n");
       createComputeDescriptorSetLayout();
       printf("\n14)Creating the compute pipeline\n");
       createComputePipeline();

       printf("\n13)Creating/Allocation buffers for the points\n");
       createShaderStorageBuffers(); // Init to particals []
       //createShaderStorageBuffers2();
       printf("\n13)Creating/Allocation unifrom buffers\n");
       createComputeUniformBuffers();
       printf("\n14)Allocation The descriptor sets\n");
       createComputeDescriptorSets();

       printf("\n9) Createing a frambuffer for each image in swap chain.\n");
       createFramebuffers();


       printf("\n12) Allocating and moving vertex data to GPU\n");
       createVertexBuffer();
       createIndexBuffer();

       /* Uniform buffers*/
       createUniformBuffers();
       createDescriptorSets();
       

       createCommandBuffers();
       printf("\n12) Creating synchronizer objects to be used later\n");
       createSyncObjects();





       printf("\n Done with vulkan intiation. Ready to render frames to the swap chain.\n");



       // Render data / allocations
       //printf("\n10) Making and allocating depth and color images\n");
       //createColorResources();
       //printf("\n11) Loading in a texture.\n");


       //printf("\n11) Submitting draw\n");
       //submit_pass();
}

void vulkan_drawFrame(screenProperties_t screen){
       draw(screen);
}

void vulkan_resized(){
       vulkan_info.framebufferResized = true;
}

void cleanup(){
       cleanupSwapChain();


       vkDestroyDescriptorPool(vulkan_info.device, vulkan_info.descriptorPool, NULL);

       vkDestroyDescriptorSetLayout(vulkan_info.device, vulkan_info.descriptorSetLayout, NULL);


       // buffers for descriptors
       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              vkDestroyBuffer(vulkan_info.device, vulkan_info.uniformBuffers[i], NULL);
              vkFreeMemory(vulkan_info.device, vulkan_info.uniformBuffersMemory[i], NULL);
       }
       free(vulkan_info.uniformBuffers);
       free(vulkan_info.uniformBuffersMemory);


       vkDestroyDescriptorSetLayout(vulkan_info.device, vulkan_info.descriptorSetLayout, NULL);

       vkDestroyBuffer(vulkan_info.device, vulkan_info.indexBuffer, NULL);
       vkFreeMemory(vulkan_info.device, vulkan_info.indexBufferMemory, NULL);
       vkDestroyBuffer(vulkan_info.device, vulkan_info.vertexBuffer, NULL);
       vkFreeMemory(vulkan_info.device, vulkan_info.vertexBufferMemory, NULL);

       if(vulkan_info.swapChainImages != NULL){free(vulkan_info.swapChainImages);}
       if(vulkan_info.swapChainImageViews != NULL){free(vulkan_info.swapChainImageViews);}
       if(vulkan_info.commandBuffers != NULL){free(vulkan_info.commandBuffers);}
       if(vulkan_info.swapChainFramebuffers != NULL){free(vulkan_info.swapChainFramebuffers);}

       vkDestroyInstance(vulkan_info.pInstance, NULL);
       vkDestroySwapchainKHR(vulkan_info.device, vulkan_info.swapChain, NULL);
}



void *resize(void *allocation, uint32_t new_sz){
       if(allocation != NULL){free(allocation);}
       return safe_calloc(new_sz, 1);
}







typedef struct compute_ubo_t{ // alighend for 4 floats?
       float deltaTime;

} compute_ubo_t;


typedef struct partical_t{
       vector4_t position;
       vector4_t speed;
       vector4_t color;
} partical_t;


static uint32_t n_particles = 12;


static partical_t particles[] = {
       {(vector4_t){0, 0, 1}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},
       {(vector4_t){1, 1, 1}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},
       {(vector4_t){0, 1, 1}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},

       {(vector4_t){1, 0, 2}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},
       {(vector4_t){1, -1, 2}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},
       {(vector4_t){0, -1, 2}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},

       {(vector4_t){-1, 0, 0.5}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},
       {(vector4_t){-1, -1, 0.5}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},
       {(vector4_t){0, -1, 0.5}, (vector4_t){0, 0, 1}, (vector4_t){1, 1, 0}},

       {(vector4_t){-1, 0, -0.5}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},
       {(vector4_t){-1, 1, -0.5}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},
       {(vector4_t){0, 1, -0.5}, (vector4_t){0, 0, -1}, (vector4_t){1, 1, 0}},
};

static partical_t *particles_new = particles;

static uint32_t *particle_indxs = NULL;
static uint32_t n_particle_indxs = 0;

float f(float x, float y){
       return 3*exp(-1*(x*x + y*y)); // Guassian with height of 10 at origin
}

void init_particles(){
       // 10x10 with points every 0.1
       // start is -5, -5 and going to 5,5 
       n_particles = 101*101;
       particles_new = (partical_t *)calloc(n_particles, sizeof(partical_t));
       partical_t point;

       int i = 0;
       float x;
       float y;
       for(int yi = 0; yi <= 100; yi += 1){
              for(int xi = 0; xi <= 100; xi += 1){
                     x = -5 + 0.1*xi;
                     y = -5 + 0.1*yi;

                     particles_new[i] = (partical_t) { (vector4_t) {x, y, f(x, y), 0}, (vector4_t) {0, 0, 0, 0}, (vector4_t) {1, 1, 0, 0}};
                     i += 1;
              }
       }

       // 100x100x2 trigs * 3 indxes/trig
       n_particle_indxs = 100*100*6;
       particle_indxs = (uint32_t *)calloc(sizeof(uint32_t), n_particle_indxs);
       i = 0;
       for(int yi = 0; yi < 100; yi += 1){
              for(int xi = 0; xi < 100; xi += 1){
                     particle_indxs[i] = xi + yi*101;
                     particle_indxs[i + 1] = (xi + 1) + yi*101;
                     particle_indxs[i + 2] = (xi + 1) + (yi + 1)*101;

                     particle_indxs[i + 3] = xi + yi*101;
                     particle_indxs[i + 4] = xi  + (yi + 1)*101;
                     particle_indxs[i + 5] = (xi + 1) + (yi + 1)*101;
                     i += 6;
              }
       }
}

/*
Here to specify the format of the partical buffer array - only for graphics pipline part.
       - Need to specify the layout of position, speed, and color
       - alos the layout of consecutive elements in a array
*/
static VkVertexInputAttributeDescription particleAttributes[3] = {
       (VkVertexInputAttributeDescription){0, 0, VK_FORMAT_R32G32B32_SFLOAT /*not actually color related*/, offsetof(partical_t, position)},
       (VkVertexInputAttributeDescription){1, 0, VK_FORMAT_R32G32B32_SFLOAT /*not actually color related*/, offsetof(partical_t, color)}
};

static VkVertexInputBindingDescription particleBindingDescription[1] = {
       0, sizeof(partical_t), VK_VERTEX_INPUT_RATE_VERTEX
};


void createShaderStorageBuffers(){
       // 1. will need to allocate the saved buffers
       if(vulkan_info.shaderStorageBuffers != NULL){free(vulkan_info.shaderStorageBuffers);}
       vulkan_info.shaderStorageBuffers = (VkBuffer *)calloc(sizeof(VkBuffer), MAX_FRAMES_IN_FLIGHT);
       
       if(vulkan_info.shaderStorageBuffersMemory != NULL){free(vulkan_info.shaderStorageBuffersMemory);}
       vulkan_info.shaderStorageBuffersMemory = (VkDeviceMemory *)calloc(sizeof(VkDeviceMemory), MAX_FRAMES_IN_FLIGHT);

       // 2. Create temp staging buffer for initial data
       VkDeviceSize bufferSize = sizeof(partical_t) * n_particles;

       VkBuffer             stagingBuffer;
       VkDeviceMemory       stagingBufferMemory;
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

       // 3. Copy to the temp, cpu local, buffer
       void* data;
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &data);
       memcpy(data, particles_new, (size_t)bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);


       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              // 4. Make and copy data from the staging buffer (host) to the shader storage buffer (GPU)
              createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(vulkan_info.shaderStorageBuffers[i]), &(vulkan_info.shaderStorageBuffersMemory[i]));
              copyBuffer(stagingBuffer, vulkan_info.shaderStorageBuffers[i], bufferSize);
       }

}

/*
void createShaderStorageBuffers2(){
       // 1. will need to allocate the saved buffers
       if(vulkan_info.shaderStorageBuffers_3dgrid != NULL){free(vulkan_info.shaderStorageBuffers_3dgrid);}
       vulkan_info.shaderStorageBuffers_3dgrid = (VkBuffer *)calloc(sizeof(VkBuffer), MAX_FRAMES_IN_FLIGHT);
       
       if(vulkan_info.shaderStorageBuffersMemory_3dgrid != NULL){free(vulkan_info.shaderStorageBuffersMemory_3dgrid);}
       vulkan_info.shaderStorageBuffersMemory_3dgrid = (VkDeviceMemory *)calloc(sizeof(VkDeviceMemory), MAX_FRAMES_IN_FLIGHT);

       // 2. Create temp staging buffer for initial data
       VkDeviceSize bufferSize = sizeof(partical_t) * n_particles;

       VkBuffer             stagingBuffer;
       VkDeviceMemory       stagingBufferMemory;
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

       // 3. Copy to the temp, cpu local, buffer
       void* data;
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &data);
       memcpy(data, particles_new, (size_t)bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);


       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              // 4. Make and copy data from the staging buffer (host) to the shader storage buffer (GPU)
              createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(vulkan_info.shaderStorageBuffers[i]), &(vulkan_info.shaderStorageBuffersMemory[i]));
              copyBuffer(stagingBuffer, vulkan_info.shaderStorageBuffers[i], bufferSize);
       }

}
*/
void createComputePipeline(){
       // Very simple.
       // 1. Make the layout
       VkShaderModule computeShaderModule = make_shader_module("shaders/compute.spv");

       VkPipelineShaderStageCreateInfo computeShaderStageInfo = {0};
       computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
       computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
       computeShaderStageInfo.module = computeShaderModule;
       computeShaderStageInfo.pName = "main";

       VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
       pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
       pipelineLayoutInfo.setLayoutCount = 1;
       pipelineLayoutInfo.pSetLayouts = &(vulkan_info.computeDescriptorSetLayout);

       check_err(vkCreatePipelineLayout(vulkan_info.device, &pipelineLayoutInfo, NULL, &(vulkan_info.computePipelineLayout)), "failed to create compute pipeline layout!");

       // 2. Create the pipeline
       VkComputePipelineCreateInfo pipelineInfo = {0};
       pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
       pipelineInfo.layout = vulkan_info.computePipelineLayout;
       pipelineInfo.stage = computeShaderStageInfo;
       check_err(vkCreateComputePipelines(vulkan_info.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &(vulkan_info.computePipeline)), "failed to create compute pipeline!");

       // 3. Free the shader
       vkDestroyShaderModule(vulkan_info.device, computeShaderModule, NULL);
}

void createComputeDescriptorSetLayout(){

       // 1. Create the descriptor set layouts
       VkDescriptorSetLayoutBinding layoutBindings[3] = {0}; // 3 bindings in the compute shader
       layoutBindings[0].binding = 0;
       layoutBindings[0].descriptorCount = 1;
       layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // Uniform
       layoutBindings[0].pImmutableSamplers = NULL;
       layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

       layoutBindings[1].binding = 1;
       layoutBindings[1].descriptorCount = 1;
       layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // input buffer
       layoutBindings[1].pImmutableSamplers = NULL;
       layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

       layoutBindings[2].binding = 2;
       layoutBindings[2].descriptorCount = 1;
       layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // output buffer
       layoutBindings[2].pImmutableSamplers = NULL;
       layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

       VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
       layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
       layoutInfo.bindingCount = 3;
       layoutInfo.pBindings = layoutBindings;

       check_err(vkCreateDescriptorSetLayout(vulkan_info.device, &layoutInfo, NULL, &(vulkan_info.computeDescriptorSetLayout)), "failed to create compute descriptor set layout!");

}

void createComputeDescriptorSets(){
       // Consider the layout we defined before ^^, the memory we allocated ^^, need to allocated MAX_FRAMES of them

       // 1. Save space for array of descriptorsets
       if(vulkan_info.computeDescriptorSets != NULL){free(vulkan_info.computeDescriptorSets);}
       vulkan_info.computeDescriptorSets = (VkDescriptorSet *)calloc(sizeof(VkDescriptorSet), MAX_FRAMES_IN_FLIGHT);

       // 2. Specify allocate info - (all the same layout)
       VkDescriptorSetLayout *layouts = (VkDescriptorSetLayout *) calloc(sizeof(VkDescriptorSetLayout), MAX_FRAMES_IN_FLIGHT);
       for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i+= 1){layouts[i] = vulkan_info.computeDescriptorSetLayout;}

       VkDescriptorSetAllocateInfo allocInfo = {0};
       allocInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
       allocInfo.descriptorPool        = vulkan_info.descriptorPool;
       allocInfo.descriptorSetCount    = MAX_FRAMES_IN_FLIGHT;
       allocInfo.pSetLayouts           = layouts;
       check_err(vkAllocateDescriptorSets(vulkan_info.device, &allocInfo, vulkan_info.computeDescriptorSets), "failed to allocate descriptor sets!");


       // 3. will need to update all the desccriptor sets with refrences to the correct bufferes
       for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i+= 1) {
              VkWriteDescriptorSet descriptorWrites[3] = {0}; // Have to bind 3 buffers to the descriptor set
              
              // a) unifrom buffer
              VkDescriptorBufferInfo uniformBufferInfo = {0};
              uniformBufferInfo.buffer = vulkan_info.computeUniformBuffers[i];      // The buffer
              uniformBufferInfo.offset = 0;
              uniformBufferInfo.range = sizeof(compute_ubo_t);                      // The size

              descriptorWrites[0].sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrites[0].dstSet                = vulkan_info.computeDescriptorSets[i];
              descriptorWrites[0].dstBinding            = 0;
              descriptorWrites[0].dstArrayElement       = 0;
              descriptorWrites[0].descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
              descriptorWrites[0].descriptorCount       = 1;
              descriptorWrites[0].pBufferInfo           = &uniformBufferInfo;

              // b) src buffer
              VkDescriptorBufferInfo storageBufferInfoLastFrame = {0}; // srcBuffer
              storageBufferInfoLastFrame.buffer = vulkan_info.shaderStorageBuffers[(i - 1 + MAX_FRAMES_IN_FLIGHT) % MAX_FRAMES_IN_FLIGHT]; // The buffer of prev frame
              storageBufferInfoLastFrame.offset = 0;
              storageBufferInfoLastFrame.range = sizeof(partical_t) * n_particles; // The size

              descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrites[1].dstSet                = vulkan_info.computeDescriptorSets[i];
              descriptorWrites[1].dstBinding            = 1;
              descriptorWrites[1].dstArrayElement       = 0;
              descriptorWrites[1].descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
              descriptorWrites[1].descriptorCount       = 1;
              descriptorWrites[1].pBufferInfo           = &storageBufferInfoLastFrame;

              // c) dest buffer
              VkDescriptorBufferInfo storageBufferInfoCurrentFrame = {0};
              storageBufferInfoCurrentFrame.buffer = vulkan_info.shaderStorageBuffers[i];
              storageBufferInfoCurrentFrame.offset = 0;
              storageBufferInfoCurrentFrame.range = sizeof(partical_t) * n_particles;

              descriptorWrites[2].sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrites[2].dstSet                = vulkan_info.computeDescriptorSets[i];
              descriptorWrites[2].dstBinding            = 2;
              descriptorWrites[2].dstArrayElement       = 0;
              descriptorWrites[2].descriptorType        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
              descriptorWrites[2].descriptorCount       = 1;
              descriptorWrites[2].pBufferInfo           = &storageBufferInfoCurrentFrame;

              vkUpdateDescriptorSets(vulkan_info.device, 3, descriptorWrites, 0, NULL); // 3 descriptors in each set, 1 set per frame
       }

       free(layouts);

}

void createComputeUniformBuffers(){
       VkDeviceSize bufferSize = sizeof(compute_ubo_t);

       if(vulkan_info.computeUniformBuffers != NULL){free(vulkan_info.computeUniformBuffers);}
       vulkan_info.computeUniformBuffers = (VkBuffer *)calloc(sizeof(VkBuffer), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.computeUniformBuffersMemory != NULL){free(vulkan_info.computeUniformBuffersMemory);}
       vulkan_info.computeUniformBuffersMemory = (VkDeviceMemory *)calloc(sizeof(VkDeviceMemory), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.computeUniformBuffersMapped != NULL){free(vulkan_info.computeUniformBuffersMapped);}
       vulkan_info.computeUniformBuffersMapped = (void **)calloc(sizeof(void *), 1);


       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &(vulkan_info.computeUniformBuffers[i]), &(vulkan_info.computeUniformBuffersMemory[i]));

              vkMapMemory(vulkan_info.device, vulkan_info.computeUniformBuffersMemory[i], 0, bufferSize, 0, &(vulkan_info.computeUniformBuffersMapped[i]));
       }
}


void updateComputeUniformBuffer(int currentFrame){
       compute_ubo_t ubo = {0};

       ubo.deltaTime = get_delta_frame_time();
       //printf("\n DeltaTime = %f\n", ubo.deltaTime);
       memcpy(vulkan_info.computeUniformBuffersMapped[currentFrame], &ubo, sizeof(compute_ubo_t));

}

















// Global / Dynamic data
typedef struct UniformBufferObject_t {
    //float model[4][4];
    //float view[4][4];
    //float proj[4][4];

    matrix4cm_t model;
    matrix4cm_t view;
    matrix4cm_t proj;


} UniformBufferObject_t; // clip = rotation*{(<model(4x4)> * <vertex(3x3), 1>).xyz - offset.xyz}

// Make sure to tell the graphics pipeline what the descriptor set layout will be 
void createDescriptorSetLayout() {
       VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
       uboLayoutBinding.binding = 0;
       uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
       uboLayoutBinding.descriptorCount = 1;
       uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
       uboLayoutBinding.pImmutableSamplers = NULL; // Optional - multisampling

       VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
       layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
       layoutInfo.bindingCount = 1;
       layoutInfo.pBindings = &uboLayoutBinding;

       check_err(vkCreateDescriptorSetLayout(vulkan_info.device, &layoutInfo, NULL, &(vulkan_info.descriptorSetLayout)), "failed to create descriptor set layout!");

}

// Create the buffers, allocation and map but dont make the transfer. - making 1 uniform buffer per in flight frame
void createUniformBuffers(){
       VkDeviceSize bufferSize = sizeof(UniformBufferObject_t);

       if(vulkan_info.uniformBuffers != NULL){free(vulkan_info.uniformBuffers);}
       vulkan_info.uniformBuffers = (VkBuffer *)calloc(sizeof(VkBuffer), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.uniformBuffersMemory != NULL){free(vulkan_info.uniformBuffersMemory);}
       vulkan_info.uniformBuffersMemory = (VkDeviceMemory *)calloc(sizeof(VkDeviceMemory), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.uniformBuffersMapped != NULL){free(vulkan_info.uniformBuffersMapped);}
       vulkan_info.uniformBuffersMapped = (void **)calloc(sizeof(void *), 1);


       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &(vulkan_info.uniformBuffers[i]), &(vulkan_info.uniformBuffersMemory[i]));

              vkMapMemory(vulkan_info.device, vulkan_info.uniformBuffersMemory[i], 0, bufferSize, 0, &(vulkan_info.uniformBuffersMapped[i]));
              // Access the memory with uniform buffer mapped, 
       }
}







void updateUniformBuffer(uint32_t currentImage, screenProperties_t screen){
       // Will need to fill out the memory mapped region with the current dynamic data.
       //  1. offset/view input
       float offset[3], view[3], hx[3], hy[3];
       float zaxis[3] = {0, 0, 1};
       
       cpyVec(screen.offset, offset);
       cpyVec(screen.view, view);

       cross(view, zaxis, hx);
       cross(view, hx, hy);

       normalize(view);
       constMult(-1.0, view, view);
       normalize(hx);
       normalize(hy);
       
       // 2. image prefrences
       float fov = screen.fov*2*3.14159/360;
       float aspectRatio = (float)vulkan_info.swapChainExtent.height/ (float)vulkan_info.swapChainExtent.width; //= height/width
       float renderDistance = screen.renderDist;
       float clipDistance = screen.clipDist;

       // 3. scren dims.
       float n = clipDistance;
       float f = renderDistance;
       float r = n*tan(fov/2);
       float l = -1*r;
       float t = aspectRatio*(r);
       float b = -1*t;
       //printf("\nNear: %f, Far: %f, Left/Right: %f/%f, Top/Bot: %f, %f\n", n, f, l, r, t, b);

       UniformBufferObject_t ubo = {0};

       // A. Model
       matrix4_t model_mat = {0};
       model_mat.r0 = (vector4_t){1, 0, 0, 0};
       model_mat.r1 = (vector4_t){0, 1, 0, 0};
       model_mat.r2 = (vector4_t){0, 0, 1, 0};
       model_mat.r3 = (vector4_t){0, 0, 0, 1};

       // B. View
       matrix4_t view_mat = {0};
       view_mat.r0 = (vector4_t) {hx[0], hx[1], hx[2], -1*dotProduct(offset, hx)};
       view_mat.r1 = (vector4_t) {hy[0], hy[1], hy[2], -1*dotProduct(offset, hy)};
       view_mat.r2 = (vector4_t) {view[0], view[1], view[2], -1*dotProduct(offset, view)};
       view_mat.r3.w = 1;
       
       // C. Proj (non-linearities solved by HW with w comppnent of vetex shader output)
       matrix4_t proj_mat = {0};
       proj_mat.r0 = (vector4_t){2.0f*n/(r - l),     0,            (r+l)/(r-l),         0};
       proj_mat.r1 = (vector4_t){0,                  2.0f*n/(t-b), (t+b)/(t-b),         0};
       proj_mat.r2 = (vector4_t){0,                  0,             f/(n - f),    -1.0*f*n/(f - n)};
       proj_mat.r3 = (vector4_t){0,                  0,            -1,                  0};

       // p1 = -0.5, -0.5, 10, 1
       // p2 = 0.5, -0.5, 10, 1
       // p3 = 0.5, 0.5, 10, 1
       //float p1[3] = {-0.5, -0.5, 10, 1};

       ubo.model = convertcm(model_mat);
       ubo.view = convertcm(view_mat);
       ubo.proj = convertcm(proj_mat);

       /*
           {{-0.5f, -0.5f, 0.5}, {1.0f, 0.0f, 0.0f}},
       {{0.5f, -0.5f, 0.5}, {0.0f, 1.0f, 0.0f}},
       {{0.5f, 0.5f, 0.5}, {0.0f, 0.0f, 1.0f}},
       {{-0.5f, 0.5f, 0.5}, {1.0f, 1.0f, 1.0f}},

       {{-0.5f, -0.5f, -0.5}, {1.0f, 0.0f, 0.0f}},       printf("\n\n");
       vector4_t inp = {-0.5f, -0.5f, 0.5, 1};
       pvector4(inp, "TOP-POINT");
       pvector4(multiply4(proj_mat, multiply4(view_mat, inp)), "OUTPUT0");
       inp = (vector4_t) {0.5f, -0.5f, 0.5, 1};
       pvector4(inp, "INPUT1");
       pvector4(multiply4(proj_mat, multiply4(view_mat, inp)), "OUTPUT1");
       inp = (vector4_t) {-0.5f, -0.5f, -0.5, 1};
       pvector4(inp, "INPUT2");
       pvector4(multiply4(proj_mat, multiply4(view_mat, inp)), "OUTPUT2");*/


       // ...
       memcpy(vulkan_info.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


void createDescriptorPool(){
       // 1. Make size structure
       VkDescriptorPoolSize poolSizes[2] = {0};
       poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
       poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT*3;      // 3 for each frame, both compute and graphics, and graphics 2d
       
       poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT*1;         // only graphics 2d using this


       // 2. Make pool create structure
       VkDescriptorPoolCreateInfo poolInfo = {0};
       poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
       poolInfo.poolSizeCount = 2;
       poolInfo.pPoolSizes = poolSizes;
       poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT*3;

       /* use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT if you want to be able to free descriptor sets*/
       check_err(vkCreateDescriptorPool(vulkan_info.device, &poolInfo, NULL, &(vulkan_info.descriptorPool)), "failed to create descriptor pool!");
}

/* Allocate a 'set' in the pool with the layout specified. Also fix reference to vkbuffer. */
void createDescriptorSets(){

       // 1. Specify the layout of the descriptor sets you are making
       VkDescriptorSetLayout *layouts = (VkDescriptorSetLayout *)calloc(sizeof(VkDescriptorSetLayout), MAX_FRAMES_IN_FLIGHT);
       for(int i =0; i < MAX_FRAMES_IN_FLIGHT; i += 1){
              layouts[i] = vulkan_info.descriptorSetLayout;
       }

       // 2. Allocate the descriptor sets
       VkDescriptorSetAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
       allocInfo.descriptorPool = vulkan_info.descriptorPool;
       allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
       allocInfo.pSetLayouts = layouts;

       if(vulkan_info.descriptorSets != NULL){free(vulkan_info.descriptorSets); }
       vulkan_info.descriptorSets = (VkDescriptorSet *)calloc(sizeof(VkDescriptorSet), MAX_FRAMES_IN_FLIGHT);

       check_err(vkAllocateDescriptorSets(vulkan_info.device, &allocInfo, vulkan_info.descriptorSets), "failed to allocate descriptor sets!");

       /* Will need to fill up the descriptor sets -> these are 'sets' allocated in the 'pool' are refering to specific vkbuffers (made before)*/
       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              // 3. Fix up refrences to the buffers
              VkDescriptorBufferInfo bufferInfo = {0};
              bufferInfo.buffer = vulkan_info.uniformBuffers[i];
              bufferInfo.offset = 0;
              bufferInfo.range = sizeof(UniformBufferObject_t);

              // 4. update the sets
              VkWriteDescriptorSet descriptorWrite = {0};
              descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrite.dstSet = vulkan_info.descriptorSets[i];
              descriptorWrite.dstBinding = 0;
              descriptorWrite.dstArrayElement = 0;
              descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // We are making a uniform buffer descritpr
              descriptorWrite.descriptorCount = 1;
              descriptorWrite.pBufferInfo = &bufferInfo;
              descriptorWrite.pImageInfo = NULL; // Optional
              descriptorWrite.pTexelBufferView = NULL; // Optional

              vkUpdateDescriptorSets(vulkan_info.device, 1, &descriptorWrite, 0, NULL);
       }

}





/* This section is for handling vertext data
// Defined in header vulkan3.h
typedef struct Vertex_t{
       float pos[3];
       float color[3];
} Vertex_t;
*/

static VkVertexInputAttributeDescription VertextTrianlge2DAttributes[2] = {
       (VkVertexInputAttributeDescription){0, 0, VK_FORMAT_R32G32B32_SFLOAT /*not actually color related*/, offsetof(Vertex_t, pos)},
       (VkVertexInputAttributeDescription){1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_t, color) }
};

static int n_verticies = 9;


static Vertex_t VertexTriangle2D[] = {
       {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
       {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
       {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},


       {{10.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
       {{-10.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

       {{0.0f, 10.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
       {{0.0f, -10.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

       {{0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 1.0f}},
       {{0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 1.0f}}
};

static int n_indicies = 12;

const uint16_t indicies[] = {
       0, 3, 0, 4, 1, 5, 1, 6,
       2, 7, 2, 8
};

static VkVertexInputBindingDescription VertextTrianlge2Ddescription = {
       0, /* index of bindin in an array of this?*/  sizeof(Vertex_t) /*stride*/, VK_VERTEX_INPUT_RATE_VERTEX /*Changes with instanceing*/
};

void createIndexBuffer(){
       VkDeviceSize bufferSize = sizeof(indicies[0]) * n_indicies;

       // 1. Create a temprorty buffer to copy from

       VkBuffer stagingBuffer;
       VkDeviceMemory stagingBufferMemory;
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, \
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, \
              &stagingBuffer, &stagingBufferMemory);
       // 2. copy to tempororay buffer
       void* data;
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &data);
       memcpy(data, indicies, (size_t) bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);

       // 3. craete a device buffer
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT /* Its a index buffer and want to transfer too it*/, \
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, \
              &(vulkan_info.indexBuffer), &(vulkan_info.indexBufferMemory));

       // 4. Copy via gpu commands
       copyBuffer(stagingBuffer, vulkan_info.indexBuffer, bufferSize);

       vkDestroyBuffer(vulkan_info.device, stagingBuffer, NULL);
       vkFreeMemory(vulkan_info.device, stagingBufferMemory, NULL);



       bufferSize = sizeof(particle_indxs[0]) * n_particle_indxs;

       // 1. Create a temprorty buffer to copy from

       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, \
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, \
              &stagingBuffer, &stagingBufferMemory);
       // 2. copy to tempororay buffer
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &data);
       memcpy(data, particle_indxs, (size_t) bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);

       // 3. craete a device buffer
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT /* Its a index buffer and want to transfer too it*/, \
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, \
              &(vulkan_info.shaderIndexBuffer), &(vulkan_info.shaderIndexBufferMemory));

       // 4. Copy via gpu commands
       copyBuffer(stagingBuffer, vulkan_info.shaderIndexBuffer, bufferSize);

       vkDestroyBuffer(vulkan_info.device, stagingBuffer, NULL);
       vkFreeMemory(vulkan_info.device, stagingBufferMemory, NULL);


       
}
VkCommandBuffer beginSingleTimeCommands(){
       // 1. Allocate a new command buffer that the cpy cmd will be on.
       VkCommandBufferAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
       allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
       allocInfo.commandPool = vulkan_info.commandPool;
       allocInfo.commandBufferCount = 1;

       VkCommandBuffer commandBuffer;
       vkAllocateCommandBuffers(vulkan_info.device, &allocInfo, &commandBuffer);

       // 2. Start recording to this buffer

       VkCommandBufferBeginInfo beginInfo = {0};
       beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
       beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; /*Only will submit one time*/

       vkBeginCommandBuffer(commandBuffer, &beginInfo);
       return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer){
       vkEndCommandBuffer(commandBuffer);

       // 4. Submit this command buffer

       VkSubmitInfo submitInfo = {0};
       submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
       submitInfo.commandBufferCount = 1;
       submitInfo.pCommandBuffers = &commandBuffer;

       vkQueueSubmit(vulkan_info.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
       vkQueueWaitIdle(vulkan_info.graphicsQueue);

       vkFreeCommandBuffers(vulkan_info.device, vulkan_info.commandPool, 1, &commandBuffer);
}



// --> Copy data from a source buffer to a destination buffer. Buffers must be compatable
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
       // You will do this through vkCmds! -> for preformance reasons you can also allocate new pool with specific flags

       VkCommandBuffer commandBuffer = beginSingleTimeCommands();

       // 3. There is only 1 copy command that will be executred
       VkBufferCopy copyRegion = {0};
       copyRegion.srcOffset = 0; // Optional
       copyRegion.dstOffset = 0; // Optional
       copyRegion.size = size;
       vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

       endSingleTimeCommands(commandBuffer);
}

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height){
       VkCommandBuffer commandBuffer = beginSingleTimeCommands();

       VkBufferImageCopy region = {0};
       region.bufferOffset = 0;
       region.bufferRowLength = 0;
       region.bufferImageHeight = 0;

       region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
       region.imageSubresource.mipLevel = 0;
       region.imageSubresource.baseArrayLayer = 0;
       region.imageSubresource.layerCount = 1;

       region.imageOffset = (VkOffset3D) {0, 0, 0};
       region.imageExtent = (VkExtent3D) {
              width,
              height,
              1
       };
       vkCmdCopyBufferToImage(
              commandBuffer,
              buffer,
              image,
              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
              1,
              &region
       );
       endSingleTimeCommands(commandBuffer);
}

void createVertexBuffer(){

       VkDeviceSize bufferSize = sizeof(Vertex_t)*n_verticies;

       // 1. Make a new temorary staging buffer.
       VkBuffer             stagingBuffer;
       VkDeviceMemory       stagingBufferMemory;
       
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT /* Can be used as the source of memory transfer*/, \
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT /* On the CPU */, \
              &stagingBuffer, &stagingBufferMemory);
       
       // 2. Copy over the vertex data to the staging buffer
       void* data;
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &data);
              memcpy(data, VertexTriangle2D, bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);
       
       // 3. Create the corresponding vertex buffer. Has the transfer destination bit so this can be transfered too
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, \
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT /* On the GPU - Cannot mem map this.*/, \
              &(vulkan_info.vertexBuffer), &(vulkan_info.vertexBufferMemory));

       copyBuffer(stagingBuffer, vulkan_info.vertexBuffer, bufferSize);

       vkDestroyBuffer(vulkan_info.device, stagingBuffer, NULL);
       vkFreeMemory(vulkan_info.device, stagingBufferMemory, NULL);
}


typedef struct Vertex_tex_t {
       float pos[3];
       float color[3];
       float texCord[2];
} Vertex_tex_t;

static VkVertexInputBindingDescription VertextTrigTex2Ddescription = {
       0, /* index of bindin in an array of this?*/  sizeof(Vertex_tex_t) /*stride*/, VK_VERTEX_INPUT_RATE_VERTEX /*Changes with instanceing*/
};
static VkVertexInputAttributeDescription VertextTrigTex2DAttributes[3] = {
       (VkVertexInputAttributeDescription){0, 0, VK_FORMAT_R32G32B32_SFLOAT /*not actually color related*/, offsetof(Vertex_tex_t, pos)},
       (VkVertexInputAttributeDescription){1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_tex_t, color) },
       (VkVertexInputAttributeDescription){2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_tex_t, texCord) }
};



static Vertex_tex_t rectangle_2d[] = {
       {{-0.9f, 0.1f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.1f, 0.1f}},
       {{-0.9f, 0.95f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.1f, 0.2f}},
       {{-0.5f, 0.95f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.2f, 0.2f}},
       {{-0.5f, 0.1f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.2f, 0.1f}},
};

const uint16_t indicies_2d[] = {
       0, 1, 2, 0, 2, 3
};

static int n_2d_indicies = 6;


typedef struct twodUBO_t {
       matrix4cm_t mat4;
} twodUBO_t;


void create2dpiplineresources(){
       /* 1) Create and upload image to GPU*/
       createTextureImage("textures\\lucidagrande.bmp");

       /* 2) Create texture sampler*/
       createTextureSampler();

       printf("   a) Making Unifrom buffers\n");
       /* 3) Create all host local UBO objects - cann fill them out with the mem map */
       createGenericUniformBuffers(sizeof(twodUBO_t), MAX_FRAMES_IN_FLIGHT, \
                            &(vulkan_info.twoduniformBuffers), \
                            &(vulkan_info.twoduniformBuffersMemory), \
                            &(vulkan_info.twoduniformBuffersMapped));

       printf("   b) Creating 2D Unifrom layout \n");
       createtwodDescriptorSetLayout(); /* New descriptor type being used */

       printf("   c) Allocating descriptor sets \n");
       createtwodDescriptorSets();

       printf("   d) Creating input buffers \n");
       /* 3) Make descriptor sets
              - make sure engout space in pool (shared by all pipelines)
              - Ties in the sampler + image made before 
              - ties in the ubo buffer made before

       */

       /* 3) Input buffers */
       createGPUBuffer( &(vulkan_info.twodvertexBuffer), &(vulkan_info.twodvertexBufferMemory), sizeof(Vertex_tex_t)*4, rectangle_2d, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
       createGPUBuffer( &(vulkan_info.twodindexBuffer), &(vulkan_info.twodindexBufferMemory), sizeof(uint16_t)*6, indicies_2d, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

}



void updatetwodUniformBuffer(uint32_t currentImage, screenProperties_t screen){

       twodUBO_t ubo = {0};

       matrix4_t model_mat = {0};
       model_mat.r0 = (vector4_t){1, 0, 0, 0};
       model_mat.r1 = (vector4_t){0, 1, 0, 0};
       model_mat.r2 = (vector4_t){0, 0, 1, 0};
       model_mat.r3 = (vector4_t){0, 0, 0, 1};

       ubo.mat4 = convertcm(model_mat);

       memcpy(vulkan_info.twoduniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

/* Creates a array of uniform buffers  - will also resize*/ 
/* Array allowes access to the host buffer, host memory allocation and the memory map to access the content */
void createGenericUniformBuffers(VkDeviceSize sizeofubo, int n_ubos, VkBuffer **uniformBuffer_out, VkDeviceMemory **unifromMemory_out, void ***uniformMap_out) {
       
       VkDeviceSize bufferSize = sizeofubo;

       if(*uniformBuffer_out != NULL){free(*uniformBuffer_out);}
       *uniformBuffer_out = (VkBuffer *)calloc(sizeof(VkBuffer), n_ubos);

       if(*unifromMemory_out != NULL){free(*unifromMemory_out);}
       *unifromMemory_out = (VkDeviceMemory *)calloc(sizeof(VkDeviceMemory), n_ubos);

       if(*uniformMap_out != NULL){free(*uniformMap_out);}
       *uniformMap_out = (void **)calloc(sizeof(void *), 1);


       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, \
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, \
                            &((*uniformBuffer_out)[i]), &((*unifromMemory_out)[i]));

              vkMapMemory(vulkan_info.device, (*unifromMemory_out)[i], 0, bufferSize, 0, &((*uniformMap_out)[i]));
              // Access the memory with uniform buffer mapped, 
       }
}

void createtwodDescriptorSetLayout() {
       VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
       uboLayoutBinding.binding = 0;
       uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
       uboLayoutBinding.descriptorCount = 1;
       uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
       uboLayoutBinding.pImmutableSamplers = NULL; // Optional - multisampling

       VkDescriptorSetLayoutBinding samplerLayoutBinding = {0};
       samplerLayoutBinding.binding = 1;
       samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       samplerLayoutBinding.descriptorCount = 1;
       samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
       samplerLayoutBinding.pImmutableSamplers = NULL; // Optional - multisampling

       VkDescriptorSetLayoutBinding bindings[2] = {uboLayoutBinding, samplerLayoutBinding};

       VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
       layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
       layoutInfo.bindingCount = 2;
       layoutInfo.pBindings = bindings;

       check_err(vkCreateDescriptorSetLayout(vulkan_info.device, &layoutInfo, NULL, &(vulkan_info.twddescriptorSetLayout)), "failed to create 2D descriptor set layout!");

}

void createtwodDescriptorSets(){

       // 1. Specify the layout of the descriptor sets you are making
       VkDescriptorSetLayout *layouts = (VkDescriptorSetLayout *)calloc(sizeof(VkDescriptorSetLayout), MAX_FRAMES_IN_FLIGHT);
       for(int i =0; i < MAX_FRAMES_IN_FLIGHT; i += 1){
              layouts[i] = vulkan_info.twddescriptorSetLayout;
       }

       // 2. Allocate the descriptor sets
       VkDescriptorSetAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
       allocInfo.descriptorPool = vulkan_info.descriptorPool;
       allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
       allocInfo.pSetLayouts = layouts;
       if(vulkan_info.twoddescriptorSets != NULL){free(vulkan_info.twoddescriptorSets); }
       vulkan_info.twoddescriptorSets = (VkDescriptorSet *)calloc(sizeof(VkDescriptorSet), MAX_FRAMES_IN_FLIGHT);

       check_err(vkAllocateDescriptorSets(vulkan_info.device, &allocInfo, vulkan_info.twoddescriptorSets), "failed to allocate descriptor sets!");

       /* Will need to fill up the descriptor sets -> these are 'sets' allocated in the 'pool' are refering to specific vkbuffers (made before)*/
       for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              // 3. Fix up refrences to the buffers
              VkDescriptorBufferInfo bufferInfo = {0};
              bufferInfo.buffer = vulkan_info.twoduniformBuffers[i];
              bufferInfo.offset = 0;
              bufferInfo.range = sizeof(twodUBO_t);

              VkDescriptorImageInfo imageInfo = {0};
              imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
              imageInfo.imageView = vulkan_info.textureImageView;
              imageInfo.sampler = vulkan_info.textureSampler;

              // 4. update the sets
              VkWriteDescriptorSet descriptorWrite0 = {0};
              descriptorWrite0.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrite0.dstSet = vulkan_info.twoddescriptorSets[i];
              descriptorWrite0.dstBinding = 0;
              descriptorWrite0.dstArrayElement = 0;
              descriptorWrite0.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // We are making a uniform buffer descritpr
              descriptorWrite0.descriptorCount = 1;
              descriptorWrite0.pBufferInfo = &bufferInfo;
              descriptorWrite0.pImageInfo = NULL; // Optional
              descriptorWrite0.pTexelBufferView = NULL; // Optional

              VkWriteDescriptorSet descriptorWrite1 = {0};
              descriptorWrite1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
              descriptorWrite1.dstSet = vulkan_info.twoddescriptorSets[i];
              descriptorWrite1.dstBinding = 1;
              descriptorWrite1.dstArrayElement = 0;
              descriptorWrite1.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // We are making a uniform buffer descritpr
              descriptorWrite1.descriptorCount = 1;
              descriptorWrite1.pBufferInfo = NULL;
              descriptorWrite1.pImageInfo = &imageInfo;
              descriptorWrite1.pTexelBufferView = NULL; // Optional

              VkWriteDescriptorSet descriptorWrite[2] = {descriptorWrite0, descriptorWrite1};

              vkUpdateDescriptorSets(vulkan_info.device, 2, descriptorWrite, 0, NULL);
       }

}




void createGPUBuffer(VkBuffer *buffer_dest, VkDeviceMemory *bufferMemory_dest, VkDeviceSize bufferSize, void *src_data, VkBufferUsageFlags usage){

       // 1. Make a new temorary staging buffer.
       VkBuffer             stagingBuffer;
       VkDeviceMemory       stagingBufferMemory;
       
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT /* Can be used as the source of memory transfer*/, \
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT /* On the CPU */, \
              &stagingBuffer, &stagingBufferMemory);
       
       void *stagingdata;
       // 2. Copy over the vertex data to the staging buffer
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, bufferSize, 0, &stagingdata);
              memcpy(stagingdata, src_data, bufferSize);
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);
       
       // 3. Create the corresponding vertex buffer. Has the transfer destination bit so this can be transfered too
       createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, \
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT /* On the GPU - Cannot mem map this.*/, \
              buffer_dest, bufferMemory_dest);

       copyBuffer(stagingBuffer, *buffer_dest, bufferSize);

       vkDestroyBuffer(vulkan_info.device, stagingBuffer, NULL);
       vkFreeMemory(vulkan_info.device, stagingBufferMemory, NULL);

}

/* NOTE theres a hard limit on concurrent alocations!*/
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
       VkBufferCreateInfo bufferInfo = {0};
       bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
       bufferInfo.size = size;
       bufferInfo.usage = usage;
       bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

       check_err(vkCreateBuffer(vulkan_info.device, &bufferInfo, NULL, buffer), "failed to create buffer!");


       VkMemoryRequirements memRequirements;
       vkGetBufferMemoryRequirements(vulkan_info.device, *buffer, &memRequirements);

       VkMemoryAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
       allocInfo.allocationSize = memRequirements.size;
       allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

       check_err(vkAllocateMemory(vulkan_info.device, &allocInfo, NULL, bufferMemory), "failed to allocate buffer memory!");


       vkBindBufferMemory(vulkan_info.device, *buffer, *bufferMemory, 0);
}



void recordComputeCommandBuffer(VkCommandBuffer commandBuffer) {

       // 1. Start recording to the command buffer
       VkCommandBufferBeginInfo beginInfo = {0};
       beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

       check_err(vkBeginCommandBuffer(commandBuffer, &beginInfo), "failed to begin recording command buffer!");


       // 2. Bind the compute pipeline
       vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkan_info.computePipeline);

       // 4. Bind to the descriptor sets
       vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vulkan_info.computePipelineLayout, 0, 1, &(vulkan_info.computeDescriptorSets[vulkan_info.currentFrame]), 0, NULL);
       
       // 4. send the compute
       vkCmdDispatch(commandBuffer, ceil((float)n_particles / 256.0f), 1, 1);

       check_err(vkEndCommandBuffer(commandBuffer), "failed to record command buffer!");

}





void draw(screenProperties_t screen){
       
       if(1){
       // 1. Wait for previous submit commands to finish before starting new commands
       vkWaitForFences(vulkan_info.device, 1, &(vulkan_info.computeInFlightFences[vulkan_info.currentFrame]), VK_TRUE, UINT64_MAX);
       // 2. Reset the fences before submitting.
       vkResetFences(vulkan_info.device, 1, &(vulkan_info.computeInFlightFences[vulkan_info.currentFrame]));

       updateComputeUniformBuffer(vulkan_info.currentFrame);

       // 2. Record the command buffer
       vkResetCommandBuffer(vulkan_info.computeCommandBuffers[vulkan_info.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
       recordComputeCommandBuffer(vulkan_info.computeCommandBuffers[vulkan_info.currentFrame]); // , deltaTime??


       // 3. Submit
       VkSemaphore computeSignalSemaphores[1] = {vulkan_info.computeFinishedSemaphores[vulkan_info.currentFrame]};
       // Dont have to wait on previous frame finishing rendering? - assume the previous submit is done?

       VkSubmitInfo computeSubmitInfo = {0};
       computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
       computeSubmitInfo.commandBufferCount = 1;
       computeSubmitInfo.pCommandBuffers = &(vulkan_info.computeCommandBuffers[vulkan_info.currentFrame]); // Get the command buffer you just filled out
       computeSubmitInfo.signalSemaphoreCount = 1;
       computeSubmitInfo.pSignalSemaphores = computeSignalSemaphores;

       check_err(vkQueueSubmit(vulkan_info.graphicsQueue, 1, &computeSubmitInfo, vulkan_info.computeInFlightFences[vulkan_info.currentFrame]), "failed to submit compute render pass!");
       }
       



       vkWaitForFences(vulkan_info.device, 1, &(vulkan_info.inFlightFences[vulkan_info.currentFrame]), VK_TRUE, UINT64_MAX);

       uint32_t imageIndex = 0;
       
       // Get next availble index from the swap chain (for rendering?)
       VkResult result = vkAcquireNextImageKHR(vulkan_info.device, vulkan_info.swapChain, UINT64_MAX, vulkan_info.imageAvailableSemaphores[vulkan_info.currentFrame], VK_NULL_HANDLE, &imageIndex);

       if (result == VK_ERROR_OUT_OF_DATE_KHR) {
              //recreateSwapChain();
              printf("\nERROR: Need to add code to recrete swapchain\n");
              exit(1);
              return;
       } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
              printf("\nERROR: Failed to aquire swapchain info\n");
              exit(1);
       }

       // Reset the fences for the current frame
       vkResetFences(vulkan_info.device, 1, &(vulkan_info.inFlightFences[vulkan_info.currentFrame]));

       vkResetCommandBuffer(vulkan_info.commandBuffers[vulkan_info.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
       recordCommandBuffer(vulkan_info.commandBuffers[vulkan_info.currentFrame], imageIndex, screen);

       VkSubmitInfo submitInfo = {0};
       submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
       
       //VkSemaphore waitSemaphores[1] = { /*vulkan_info.computeFinishedSemaphores[vulkan_info.currentFrame],*/ vulkan_info.imageAvailableSemaphores[vulkan_info.currentFrame] };
       //VkPipelineStageFlags waitStages[1] = {/*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
       //submitInfo.waitSemaphoreCount = 1; replace below ((()))
       VkSemaphore waitSemaphores[2] = {vulkan_info.computeFinishedSemaphores[vulkan_info.currentFrame], vulkan_info.imageAvailableSemaphores[vulkan_info.currentFrame] };
       VkPipelineStageFlags waitStages[2] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
       submitInfo.waitSemaphoreCount = 2;
       
       submitInfo.pWaitSemaphores = waitSemaphores;
       submitInfo.pWaitDstStageMask = waitStages;

       submitInfo.commandBufferCount = 1;
       submitInfo.pCommandBuffers = &(vulkan_info.commandBuffers[vulkan_info.currentFrame]);

       VkSemaphore signalSemaphores[1] = {vulkan_info.renderFinishedSemaphores[vulkan_info.currentFrame]};
       submitInfo.signalSemaphoreCount = 1;
       submitInfo.pSignalSemaphores = signalSemaphores;

       check_err(vkQueueSubmit(vulkan_info.graphicsQueue, 1, &submitInfo, vulkan_info.inFlightFences[vulkan_info.currentFrame]), "failed to submit draw command buffer!");
       
       VkPresentInfoKHR presentInfo = {0};
       presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
       presentInfo.waitSemaphoreCount = 1;
       presentInfo.pWaitSemaphores = signalSemaphores;
       VkSwapchainKHR swapChains[1] = {vulkan_info.swapChain};
       presentInfo.swapchainCount = 1;
       presentInfo.pSwapchains = swapChains;
       presentInfo.pImageIndices = &imageIndex;

       result = vkQueuePresentKHR(vulkan_info.presentQueue, &presentInfo);

       if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkan_info.framebufferResized) {
              vulkan_info.framebufferResized = false;
              recreateSwapChain();
              //printf("\nERROR: Failed to aquire swapchain info\n");
              //exit(1);
       } else if (result != VK_SUCCESS) {
              printf("\nERROR: Failed to aquire swapchain info\n");
              exit(1);
       }

       vulkan_info.currentFrame = (vulkan_info.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void recreateSwapChain() {
       int width = 0, height = 0;
       get_wind_dims(&width, &height);
       vkDeviceWaitIdle(vulkan_info.device);
       printf("\n Recreating swap chain.\n");

       cleanupSwapChain();
       createSwapChain();
       createImageViews();
       createDepthResources();
       createFramebuffers();
}


void cleanupSwapChain() {

       vkDestroyImageView(vulkan_info.device, vulkan_info.depthImageView, NULL);
       vkDestroyImage(vulkan_info.device, vulkan_info.depthImage, NULL);
       vkFreeMemory(vulkan_info.device, vulkan_info.depthImageMemory, NULL);

       for(int i =0; i < vulkan_info.imageCount; i += 1){
              vkDestroyFramebuffer(vulkan_info.device, vulkan_info.swapChainFramebuffers[i], NULL);
              vkDestroyImageView(vulkan_info.device, vulkan_info.swapChainImageViews[i], NULL);

       }
       vkDestroySwapchainKHR(vulkan_info.device, vulkan_info.swapChain, NULL);
}



void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, screenProperties_t screen){
       VkCommandBufferBeginInfo beginInfo = {0};
       beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

       check_err(vkBeginCommandBuffer(commandBuffer, &beginInfo), "failed to begin recording command buffer!");

       VkRenderPassBeginInfo renderPassInfo = {0};
       renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
       renderPassInfo.renderPass = vulkan_info.renderPass;
       renderPassInfo.framebuffer = vulkan_info.swapChainFramebuffers[imageIndex];
       renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
       renderPassInfo.renderArea.extent = vulkan_info.swapChainExtent;     // renderpass on the whole image

       /* Specify how to clear attachments with OP_CLEAR flag (should be identical to order of attachments*/
       VkClearValue clearValues[2] = {0}; //        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
       clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
       clearValues[1].depthStencil = (VkClearDepthStencilValue) {1.0f, 0};

       renderPassInfo.clearValueCount = 2;
       renderPassInfo.pClearValues = clearValues;

       vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

       vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_info.graphicsPipeline);

       VkViewport viewport = {0};
       viewport.x = 0.0f;
       viewport.y = 0.0f;
       viewport.width = (float) vulkan_info.swapChainExtent.width;
       viewport.height = (float) vulkan_info.swapChainExtent.height;
       viewport.minDepth = 0.0f;
       viewport.maxDepth = 1.0f;
       vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

       VkRect2D scissor = {0};
       scissor.offset = (VkOffset2D){0, 0};
       scissor.extent = vulkan_info.swapChainExtent;
       vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


       VkBuffer vertexBuffers[] = {vulkan_info.vertexBuffer};
       VkDeviceSize offsets[] = {0};
       vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkan_info.shaderStorageBuffers[vulkan_info.currentFrame], offsets); // vertexBuffers

       vkCmdBindIndexBuffer(commandBuffer, vulkan_info.shaderIndexBuffer, 0, VK_INDEX_TYPE_UINT32); /* 16 is the type of index data (we dont have very big vertex array right now*/

       updateUniformBuffer(vulkan_info.currentFrame, screen);

       
       vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_info.pipelineLayout, 0, 1/*descriptorSetCount*/, &(vulkan_info.descriptorSets[vulkan_info.currentFrame]), 0, NULL);
       
       // vkCmdDraw(commandBuffer, n_particles, 1, 0, 0); // n_verticies
       vkCmdDrawIndexed(commandBuffer, n_particle_indxs, 1, 0, 0, 0); // Similar to draw but activates the shader for each index now!

       

       vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_info.lineGraphicsPipeline);

       vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

       vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

       vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkan_info.vertexBuffer, offsets); 
       
       vkCmdBindIndexBuffer(commandBuffer, vulkan_info.indexBuffer, 0, VK_INDEX_TYPE_UINT16); /* 16 is the type of index data (we dont have very big vertex array right now*/

       vkCmdDrawIndexed(commandBuffer, n_indicies, 1, 0, 0, 0); // Similar to draw but activates the shader for each index now!


       // Want to do 2D stuff now
       vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_info.twodGraphicsPipeline);
       
       vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
       
       vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

       vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkan_info.twodvertexBuffer, offsets); 
       
       vkCmdBindIndexBuffer(commandBuffer, vulkan_info.twodindexBuffer, 0, VK_INDEX_TYPE_UINT16); /* 16 is the type of index data (we dont have very big vertex array right now*/

       //printf("\n Before\n");
       updatetwodUniformBuffer(vulkan_info.currentFrame, screen);
       //printf("\n After\n");
       vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_info.twodpipelineLayout, 0, 1/*descriptorSetCount*/, &(vulkan_info.twoddescriptorSets[vulkan_info.currentFrame]), 0, NULL);


       vkCmdDrawIndexed(commandBuffer, n_2d_indicies, 1, 0, 0, 0); // Similar to draw but activates the shader for each index now!




       vkCmdEndRenderPass(commandBuffer);

       check_err(vkEndCommandBuffer(commandBuffer), "failed to record command buffer!");
}







// Making sync objects for each concurrent frame
void createSyncObjects() {
       if(vulkan_info.imageAvailableSemaphores != NULL){free(vulkan_info.imageAvailableSemaphores);}
       vulkan_info.imageAvailableSemaphores = (VkSemaphore *)calloc(sizeof(VkSemaphore), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.renderFinishedSemaphores != NULL){free(vulkan_info.renderFinishedSemaphores);}
       vulkan_info.renderFinishedSemaphores = (VkSemaphore *)calloc(sizeof(VkSemaphore), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.computeFinishedSemaphores != NULL){free(vulkan_info.computeFinishedSemaphores);}
       vulkan_info.computeFinishedSemaphores = (VkSemaphore *)calloc(sizeof(VkSemaphore), MAX_FRAMES_IN_FLIGHT); // added for compute

       if(vulkan_info.inFlightFences != NULL){free(vulkan_info.inFlightFences);}
       vulkan_info.inFlightFences = (VkFence *)calloc(sizeof(VkFence), MAX_FRAMES_IN_FLIGHT);

       if(vulkan_info.computeInFlightFences != NULL){free(vulkan_info.computeInFlightFences);}
       vulkan_info.computeInFlightFences = (VkFence *)calloc(sizeof(VkFence), MAX_FRAMES_IN_FLIGHT); // Added for compute

       vulkan_info.syncs_arr_size = MAX_FRAMES_IN_FLIGHT;


       VkSemaphoreCreateInfo semaphoreInfo = {0};
       semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

       VkFenceCreateInfo fenceInfo = {0};
       fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
       fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

       for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
              check_err(vkCreateSemaphore(vulkan_info.device, &semaphoreInfo, NULL, &(vulkan_info.imageAvailableSemaphores[i])) & \
                     vkCreateSemaphore(vulkan_info.device, &semaphoreInfo, NULL, &(vulkan_info.renderFinishedSemaphores[i])) & \
                     vkCreateSemaphore(vulkan_info.device, &semaphoreInfo, NULL, &(vulkan_info.computeFinishedSemaphores[i])) & \
                     vkCreateFence(vulkan_info.device, &fenceInfo, NULL, &(vulkan_info.computeInFlightFences[i])) & \
                     vkCreateFence(vulkan_info.device, &fenceInfo, NULL, &(vulkan_info.inFlightFences[i])),  \
                     "failed to create synchronization objects for a frame!");
       }
}





// Frame buffer for each Img in the swap chain
void createFramebuffers(){
       if(vulkan_info.swapChainFramebuffers != NULL){free(vulkan_info.swapChainFramebuffers);}
       vulkan_info.swapChainFramebuffers = (VkFramebuffer *)calloc(sizeof(VkFramebuffer), vulkan_info.imageCount);


       for (size_t i = 0; i < vulkan_info.imageCount; i++) {
              VkImageView attachments[2] = {vulkan_info.swapChainImageViews[i], vulkan_info.depthImageView};

              VkFramebufferCreateInfo framebufferInfo = {0};
              framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
              framebufferInfo.renderPass = vulkan_info.renderPass;
              framebufferInfo.attachmentCount = 2;
              framebufferInfo.pAttachments = attachments;
              framebufferInfo.width = vulkan_info.swapChainExtent.width;
              framebufferInfo.height = vulkan_info.swapChainExtent.height;
              framebufferInfo.layers = 1;

              check_err(vkCreateFramebuffer(vulkan_info.device, &framebufferInfo, NULL, &(vulkan_info.swapChainFramebuffers[i])), "failed to create framebuffer!");
       }
}









void createCommandBuffers() {
       if(vulkan_info.commandBuffers != NULL){free(vulkan_info.commandBuffers);}
       vulkan_info.commandBuffers = NULL;

       vulkan_info.commandBuffers = (VkCommandBuffer *)calloc(sizeof(VkCommandBuffer), MAX_FRAMES_IN_FLIGHT*2);

       VkCommandBufferAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
       allocInfo.commandPool = vulkan_info.commandPool;
       allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
       allocInfo.commandBufferCount = (uint32_t) MAX_FRAMES_IN_FLIGHT*2;

       vulkan_info.computeCommandBuffers = vulkan_info.commandBuffers + MAX_FRAMES_IN_FLIGHT;

       check_err(vkAllocateCommandBuffers(vulkan_info.device, &allocInfo, vulkan_info.commandBuffers), "failed to allocate command buffers!");

}


// Create a image/imageview (on the device) with same fomat as swapchain

void createColorResources() {
       VkFormat colorFormat = vulkan_info.swapChainImageFormat;

       createImage(vulkan_info.swapChainExtent.width, vulkan_info.swapChainExtent.height, 1, vulkan_info.msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(vulkan_info.colorImage), &(vulkan_info.colorImageMemory));
       vulkan_info.colorImageView = createImageView(vulkan_info.colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void createDepthResources() {
       VkFormat depthFormat = findDepthFormat();

       // You want the same size as the color attachment / swap chain images
       createImage(vulkan_info.swapChainExtent.width, vulkan_info.swapChainExtent.height, 1, vulkan_info.msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(vulkan_info.depthImage), &(vulkan_info.depthImageMemory));
       vulkan_info.depthImageView = createImageView(vulkan_info.depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

       // Dont need to map or copy anything to this resource! - it will be cleared at the start of a pass
}

// Find a go place to bind 
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
       VkPhysicalDeviceMemoryProperties memProperties;
       vkGetPhysicalDeviceMemoryProperties(vulkan_info.phyDevice, &memProperties);

       for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
              if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                     return i;
              }
       }

       printf("failed to find suitable memory type!");
       exit(1);
}

void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory) {
       VkImageCreateInfo imageInfo = {0};
       imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
       imageInfo.imageType = VK_IMAGE_TYPE_2D;
       imageInfo.extent.width = width;
       imageInfo.extent.height = height;
       imageInfo.extent.depth = 1;
       imageInfo.mipLevels = mipLevels;
       imageInfo.arrayLayers = 1;
       imageInfo.format = format;
       imageInfo.tiling = tiling;
       imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
       imageInfo.usage = usage;
       imageInfo.samples = numSamples;
       imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

       check_err(vkCreateImage(vulkan_info.device, &imageInfo, NULL, image), "failed to create image!");


       VkMemoryRequirements memRequirements;
       vkGetImageMemoryRequirements(vulkan_info.device, *image, &memRequirements);

       VkMemoryAllocateInfo allocInfo = {0};
       allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
       allocInfo.allocationSize = memRequirements.size;
       allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

       check_err(vkAllocateMemory(vulkan_info.device, &allocInfo, NULL, imageMemory), "failed to allocate image memory!");

       vkBindImageMemory(vulkan_info.device, *image, *imageMemory, 0);
}

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
       VkImageViewCreateInfo viewInfo = {0};
       viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
       viewInfo.image = image;
       viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
       viewInfo.format = format;
       viewInfo.subresourceRange.aspectMask = aspectFlags;
       viewInfo.subresourceRange.baseMipLevel = 0;
       viewInfo.subresourceRange.levelCount = mipLevels;
       viewInfo.subresourceRange.baseArrayLayer = 0;
       viewInfo.subresourceRange.layerCount = 1;

       VkImageView imageView;
       check_err(vkCreateImageView(vulkan_info.device, &viewInfo, NULL, &imageView), "failed to create image view!");
       
       return imageView;
}


void createCommandPool() {
       QueueFamilyIndices_t queueFamilyIndices = findQueueFamilies(vulkan_info.phyDevice);

       VkCommandPoolCreateInfo poolInfo = {0};
       poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
       poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
       poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily;

       check_err(vkCreateCommandPool(vulkan_info.device, &poolInfo, NULL, &(vulkan_info.commandPool)), "failed to create graphics command pool!");
}



uint8_t *load_shader(const char *filename, uint32_t *sz_out){
       int fd;
       int sz;
       int n;


       fd = _open(filename, _O_BINARY | _O_RDONLY);
       passert(fd != -1, "Opening file for read.");

       sz = _lseek(fd, 0, SEEK_END);
       *sz_out = sz;
       passert(sz != -1, "Finding end of file.");

       uint8_t *shader = (uint8_t *)calloc(sizeof(uint8_t), sz);
       passert(shader != NULL, "Allocating space for shader.");

       _lseek(fd, 0, SEEK_SET);

       n = 0;
       while(n < sz){
              n += _read(fd, shader + n, sz-n);
       }
       _close(fd);
       return shader;
}


VkShaderModule make_shader_module(const char *filename){
       uint32_t sz_shader = 0;
       uint32_t *bytecode = (uint32_t *)load_shader(filename, &sz_shader);
       VkShaderModule ans;


       VkShaderModuleCreateInfo create_info = {0};
       create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
       create_info.codeSize = sz_shader;
       create_info.pCode    = bytecode;

       check_err(vkCreateShaderModule(vulkan_info.device, &create_info, NULL, &(ans)), "Creating a shader module");
       free(bytecode);
       return ans;
}



void createGraphicsPipeline() {

       VkShaderModule vertShaderModule = make_shader_module("shaders/vert.spv");
       VkShaderModule fragShaderModule = make_shader_module("shaders/frag.spv");

       // 1. Shader code.
       VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
       vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
       vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Takes in vertex bits
       vertShaderStageInfo.module = vertShaderModule;          // Raw code
       vertShaderStageInfo.pName = "main";                     // main function name

       VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
       fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
       fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
       fragShaderStageInfo.module = fragShaderModule;
       fragShaderStageInfo.pName = "main";

       VkPipelineShaderStageCreateInfo shaderStages[2] = {vertShaderStageInfo, fragShaderStageInfo};

       // 2. buffer types?
       VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};    // Descirbes the input to the first vertex state
       vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
       vertexInputInfo.vertexBindingDescriptionCount = 1;
       vertexInputInfo.vertexAttributeDescriptionCount = 2;
       vertexInputInfo.pVertexBindingDescriptions = particleBindingDescription; //&VertextTrianlge2Ddescription;
       vertexInputInfo.pVertexAttributeDescriptions = particleAttributes; //VertextTrianlge2DAttributes;

       VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};    // Describes the assembly state -> how cords are draw
       inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
       inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // VK_PRIMITIVE_TOPOLOGY_LINE_LIST VK_PRIMITIVE_TOPOLOGY_POINT_LIST
       inputAssembly.primitiveRestartEnable = VK_FALSE;


       // 3. output viewport info
       VkPipelineViewportStateCreateInfo viewportState = {0};
       viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
       viewportState.viewportCount = 1;
       viewportState.scissorCount = 1;

       VkPipelineRasterizationStateCreateInfo rasterizer = {0};
       rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
       rasterizer.depthClampEnable = VK_FALSE;
       rasterizer.rasterizerDiscardEnable = VK_FALSE;
       rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
       rasterizer.lineWidth = 1.0f;
       rasterizer.cullMode = VK_CULL_MODE_NONE; // VK_CULL_MODE_BACK_BIT
       rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
       rasterizer.depthBiasEnable = VK_FALSE; // VK_FALSE

       VkPipelineMultisampleStateCreateInfo multisampling = {0};
       multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
       multisampling.sampleShadingEnable = VK_FALSE;
       multisampling.rasterizationSamples = vulkan_info.msaaSamples;

       VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
       colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
       colorBlendAttachment.blendEnable = VK_FALSE;

       VkPipelineColorBlendStateCreateInfo colorBlending = {0};
       colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
       colorBlending.logicOpEnable = VK_FALSE;
       colorBlending.logicOp = VK_LOGIC_OP_COPY;
       colorBlending.attachmentCount = 1;
       colorBlending.pAttachments = &colorBlendAttachment;
       colorBlending.blendConstants[0] = 0.0f;
       colorBlending.blendConstants[1] = 0.0f;
       colorBlending.blendConstants[2] = 0.0f;
       colorBlending.blendConstants[3] = 0.0f;

       VkDynamicState dynamicStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR}; // Changable after the pipeline is made

       VkPipelineDynamicStateCreateInfo dynamicState = {0};
       dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
       dynamicState.dynamicStateCount = 2;
       dynamicState.pDynamicStates = dynamicStates;

       VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
       pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
       pipelineLayoutInfo.setLayoutCount = 1;
       pipelineLayoutInfo.pushConstantRangeCount = 0;
       pipelineLayoutInfo.pSetLayouts = &(vulkan_info.descriptorSetLayout);

       check_err(vkCreatePipelineLayout(vulkan_info.device, &pipelineLayoutInfo, NULL, &(vulkan_info.pipelineLayout)), "failed to create pipeline layout!");
       
       VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
       depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
       depthStencil.depthTestEnable = VK_TRUE;
       depthStencil.depthWriteEnable = VK_TRUE;
       depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
       depthStencil.depthBoundsTestEnable = VK_FALSE;
       depthStencil.minDepthBounds = 0.0f; // Optional
       depthStencil.maxDepthBounds = 1.0f; // Optional
       depthStencil.stencilTestEnable = VK_FALSE;

       VkGraphicsPipelineCreateInfo pipelineInfo = {0};
       pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       pipelineInfo.stageCount = 2;
       pipelineInfo.pStages = shaderStages;
       pipelineInfo.pVertexInputState = &vertexInputInfo;
       pipelineInfo.pInputAssemblyState = &inputAssembly;
       pipelineInfo.pViewportState = &viewportState;
       pipelineInfo.pRasterizationState = &rasterizer;
       pipelineInfo.pMultisampleState = &multisampling;
       pipelineInfo.pColorBlendState = &colorBlending;
       pipelineInfo.pDynamicState = &dynamicState;
       pipelineInfo.pDepthStencilState = &depthStencil;
       pipelineInfo.layout = vulkan_info.pipelineLayout;
       pipelineInfo.renderPass = vulkan_info.renderPass;
       pipelineInfo.subpass = 0;
       pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

       check_err(vkCreateGraphicsPipelines(vulkan_info.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &(vulkan_info.graphicsPipeline)), "failed to create graphics pipeline!");


       // 1. A differnet assembler is needed (lines not trianlges)
       VkPipelineInputAssemblyStateCreateInfo lineInputAssembly = {0};    // Describes the assembly state -> how cords are draw
       lineInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
       lineInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // VK_PRIMITIVE_TOPOLOGY_LINE_LIST VK_PRIMITIVE_TOPOLOGY_POINT_LIST
       lineInputAssembly.primitiveRestartEnable = VK_FALSE;

       // 2. The buffer of vertecies is different format
       VkPipelineVertexInputStateCreateInfo lineVertexInputInfo = {0};    // Descirbes the input to the first vertex state
       lineVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
       lineVertexInputInfo.vertexBindingDescriptionCount = 1;
       lineVertexInputInfo.vertexAttributeDescriptionCount = 2;
       lineVertexInputInfo.pVertexBindingDescriptions = &VertextTrianlge2Ddescription;
       lineVertexInputInfo.pVertexAttributeDescriptions = VertextTrianlge2DAttributes;

       pipelineInfo.pInputAssemblyState = &lineInputAssembly;
       pipelineInfo.pVertexInputState = &lineVertexInputInfo;

       check_err(vkCreateGraphicsPipelines(vulkan_info.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &(vulkan_info.lineGraphicsPipeline)), "failed to create graphics pipeline!");

       vkDestroyShaderModule(vulkan_info.device, fragShaderModule, NULL);
       vkDestroyShaderModule(vulkan_info.device, vertShaderModule, NULL);





       // 2D PIPELINE
       pipelineLayoutInfo.pSetLayouts = &(vulkan_info.twddescriptorSetLayout);
       check_err(vkCreatePipelineLayout(vulkan_info.device, &pipelineLayoutInfo, NULL, &(vulkan_info.twodpipelineLayout)), "failed to create pipeline layout!");
       pipelineInfo.layout = vulkan_info.twodpipelineLayout;

       // 1. shader code - Use the new 2D shaders
       vertShaderModule = make_shader_module("shaders/vert_2d.spv");
       fragShaderModule = make_shader_module("shaders/frag_2d.spv");

       vertShaderStageInfo.module = vertShaderModule;          // Correct the pointers
       fragShaderStageInfo.module = fragShaderModule;
       shaderStages[0] = vertShaderStageInfo;
       shaderStages[1] = fragShaderStageInfo;

       // 2. Dont do depth testing
       depthStencil.depthTestEnable = VK_FALSE;
       depthStencil.depthWriteEnable = VK_FALSE;

       // 3. Revert changes made for the line one
       //pipelineInfo.pInputAssemblyState = &inputAssembly;
       lineInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
       lineVertexInputInfo.vertexBindingDescriptionCount = 1;
       lineVertexInputInfo.vertexAttributeDescriptionCount = 3;
       lineVertexInputInfo.pVertexBindingDescriptions = &VertextTrigTex2Ddescription;
       lineVertexInputInfo.pVertexAttributeDescriptions = VertextTrigTex2DAttributes;
       //pipelineInfo.pVertexInputState = &vertexInputInfo;

       
       check_err(vkCreateGraphicsPipelines(vulkan_info.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &(vulkan_info.twodGraphicsPipeline)), "failed to create graphics pipeline!");

       vkDestroyShaderModule(vulkan_info.device, fragShaderModule, NULL);
       vkDestroyShaderModule(vulkan_info.device, vertShaderModule, NULL);


       // We will also want to make a pipline for text rendering
       //     - Will have a 100x(axb) bitmap of character images -> will be mapping of letters to the array
       //     - shader will match top-right pixel to correct place in the screen, and then scale proporley
       //     ie - blit each to the screen - 1 draw call per liekely - give x0/y0 and scale and 


}

VkFormat findSupportedFormat(VkFormat *candidates, int n_candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {


       VkFormat format;
       for(int i =0; i < n_candidates; i += 1){
              format = candidates[i];
              VkFormatProperties props;
              vkGetPhysicalDeviceFormatProperties(vulkan_info.phyDevice, format, &props);

              if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                     return format;
              } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                     return format;
              }
       }
       printf("ERROR: finding supported depth fomat\n");
       exit(1);
}


void createRenderPass() {
       VkAttachmentDescription colorAttachment = {0};
       colorAttachment.format = vulkan_info.swapChainImageFormat;
       colorAttachment.samples = vulkan_info.msaaSamples;
       colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
       colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
       colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
       colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
       colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
       colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
       
       VkAttachmentDescription depthAttachment = {0};
       depthAttachment.format = findDepthFormat();             // Should be same as depth image
       depthAttachment.samples = vulkan_info.msaaSamples;
       depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;          // Clear the buffer on load
       depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // dont care after
       depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
       depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
       depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
       depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

       VkAttachmentDescription colorAttachmentResolve = {0};
       colorAttachmentResolve.format = vulkan_info.swapChainImageFormat;     // Refering to swapchain images
       colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
       colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
       colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
       colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
       colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
       colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
       colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

       VkAttachmentReference colorAttachmentRef = {0};
       colorAttachmentRef.attachment = 0;
       colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

       VkAttachmentReference depthAttachmentRef = {0};
       depthAttachmentRef.attachment = 1;
       depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

       VkAttachmentReference colorAttachmentResolveRef = {0};
       colorAttachmentResolveRef.attachment = 2;
       colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

       VkSubpassDescription subpass = {0};
       subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
       subpass.colorAttachmentCount = 1;
       subpass.pColorAttachments = &colorAttachmentRef;
       subpass.pDepthStencilAttachment = &depthAttachmentRef;
       //subpass.pResolveAttachments = &colorAttachmentResolveRef;

       VkSubpassDependency dependency = {0};
       dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
       dependency.dstSubpass = 0;
       dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
       dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // 0;      
       dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
       dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

       //VkAttachmentDescription attachments[3] = {colorAttachment, depthAttachment, colorAttachmentResolve };
       VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};
       VkRenderPassCreateInfo renderPassInfo = {0};
       renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
       renderPassInfo.attachmentCount = 2;       // arr size
       renderPassInfo.pAttachments = attachments;
       renderPassInfo.subpassCount = 1;
       renderPassInfo.pSubpasses = &subpass;
       renderPassInfo.dependencyCount = 1;
       renderPassInfo.pDependencies = &dependency;

       check_err(vkCreateRenderPass(vulkan_info.device, &renderPassInfo, NULL, &(vulkan_info.renderPass)), "failed to create render pass!");

}


void createImageViews(){
       // 1. resize image view array
       if(vulkan_info.swapChainImageViews != NULL){free(vulkan_info.swapChainImageViews);}
       vulkan_info.swapChainImageViews = (VkImageView *)calloc(sizeof(VkImageView), vulkan_info.imageCount);

       // 2. make a image view for each image
       for (int i = 0; i < vulkan_info.imageCount; i++) {
              vulkan_info.swapChainImageViews[i] = createImageView(vulkan_info.swapChainImages[i], vulkan_info.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
              continue;
              VkImageViewCreateInfo createInfo = {0};
              createInfo.sType       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
              createInfo.image       = vulkan_info.swapChainImages[i];
              createInfo.viewType    = VK_IMAGE_VIEW_TYPE_2D;
              createInfo.format      = vulkan_info.swapChainImageFormat; // Whatever format the image is i
              createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
              createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
              createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
              createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
              createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              createInfo.subresourceRange.baseMipLevel = 0;
              createInfo.subresourceRange.levelCount = 1;
              createInfo.subresourceRange.baseArrayLayer = 0;
              createInfo.subresourceRange.layerCount = 1;
              

              check_err(vkCreateImageView(vulkan_info.device, &createInfo, NULL, &(vulkan_info.swapChainImageViews[i])), "failed to create image views!");

       }


}


VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities) {
       if (capabilities.currentExtent.width != -1) {
              return capabilities.currentExtent;
       } else {
              printf("\n ERROR: Window maker is offering special current size?.\n");
              exit(1);

              /*
              int width, height;
              glfwGetFramebufferSize(window, &width, &height);

              VkExtent2D actualExtent = {width, height};

              actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
              actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

              return actualExtent;*/
        }
    }



VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR *availablePresentModes, uint32_t n_present_modes) {
       for (int i = 0; i < n_present_modes; i+=1) {
              if (availablePresentModes[i] == best_present_mode) {
                     return availablePresentModes[i];
              }
       }
       //return availablePresentModes[0];
       return VK_PRESENT_MODE_FIFO_KHR; // Assume to always be availbale
}


// Choose a format for the swap chain
VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR *availableFormats, uint32_t n_formats) {
       for (int i = 0; i < n_formats; i+=1) {
              if (availableFormats[i].format == best_format && availableFormats[i].colorSpace == best_color_space) {
                     return availableFormats[i];
              }
       }

       return availableFormats[0];
}



void createSwapChain(){

       SwapChainSupportDetails_t swapChainSupport = querySwapChainSupport(vulkan_info.phyDevice);

       VkSurfaceFormatKHR surfaceFormat   = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.n_formats);
       VkPresentModeKHR presentMode       = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.n_presentModes);
       VkExtent2D extent                  = chooseSwapExtent(swapChainSupport.capabilities);


       // 1. Choose minimum +1 swaps in the chain
       uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
       if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
              imageCount = swapChainSupport.capabilities.maxImageCount;
       }
       

       VkSwapchainCreateInfoKHR createInfo = {0};
       createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
       createInfo.surface = vulkan_info.surface;

       createInfo.minImageCount = imageCount;
       createInfo.imageFormat = surfaceFormat.format;
       createInfo.imageColorSpace = surfaceFormat.colorSpace;
       createInfo.imageExtent = extent;
       createInfo.imageArrayLayers = 1;
       createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;   // Used for images to screen

       QueueFamilyIndices_t indices = findQueueFamilies(vulkan_info.phyDevice);
       uint32_t queueFamilyIndices[2] = {indices.graphicsAndComputeFamily, indices.compatableFamily};

        if (indices.graphicsAndComputeFamily != indices.compatableFamily) {
              createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // When present modes are not equal to graphics mode
              createInfo.queueFamilyIndexCount = 2;
              createInfo.pQueueFamilyIndices = queueFamilyIndices;
       } else {
              createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
       }

       createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
       createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
       createInfo.presentMode = presentMode;
       createInfo.clipped = VK_TRUE;

       check_err(vkCreateSwapchainKHR(vulkan_info.device, &createInfo, NULL, &(vulkan_info.swapChain)), "failed to create swap chain!");
       

       // 2. Get the raw array of the swap chain.
       vkGetSwapchainImagesKHR(vulkan_info.device, vulkan_info.swapChain, &(vulkan_info.imageCount), NULL);
       passert(vulkan_info.imageCount == imageCount, "Swap chain created different sized array");
       if(vulkan_info.swapChainImages != NULL){free(vulkan_info.swapChainImages);}

       vulkan_info.swapChainImages = (VkImage *)calloc(sizeof(VkImage), imageCount);
       vkGetSwapchainImagesKHR(vulkan_info.device, vulkan_info.swapChain, &(vulkan_info.imageCount), vulkan_info.swapChainImages);

       // 3. Save the format you used and the extent you used
       vulkan_info.swapChainImageFormat   = surfaceFormat.format;
       vulkan_info.swapChainExtent        = extent;

       if(swapChainSupport.presentModes != NULL){free(swapChainSupport.presentModes);}
       if(swapChainSupport.formats != NULL){free(swapChainSupport.formats);}
}



void createLogicalDevice(){

       // 1) Make the queues for the device
       // sType, pNext, flags, queueFamilyIndex, queueCount, pQueuePriorities, queue_prio;
       QueueFamilyIndices_t good_qs = findQueueFamilies(vulkan_info.phyDevice);

       VkDeviceQueueCreateInfo *new_qs;
       passert(good_qs.compatableFamily != -1 && good_qs.graphicsAndComputeFamily != -1, "Bad device given to make logical device");
              
       float prio = 1.0f;
       uint32_t n_queues = 0;

       if(good_qs.compatableFamily != good_qs.graphicsAndComputeFamily){
              new_qs = (VkDeviceQueueCreateInfo *)calloc(sizeof(VkDeviceQueueCreateInfo), 2);
              new_qs[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
              new_qs[1].pNext = NULL;
              new_qs[1].flags = 0;
              new_qs[1].queueFamilyIndex = good_qs.compatableFamily;
              new_qs[1].queueCount = 1;
              new_qs[1].pQueuePriorities = &prio;
              n_queues = 2;
       } else{
              new_qs = (VkDeviceQueueCreateInfo *)calloc(sizeof(VkDeviceQueueCreateInfo), 1);
              n_queues = 1;
       }

       new_qs[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
       new_qs[0].pNext = NULL;
       new_qs[0].flags = 0;
       new_qs[0].queueFamilyIndex = good_qs.graphicsAndComputeFamily;
       new_qs[0].queueCount = 1;
       new_qs[0].pQueuePriorities = &prio;

       
       VkPhysicalDeviceFeatures deviceFeatures = {0};
       deviceFeatures.samplerAnisotropy = VK_TRUE;

       // 2) Make the new logical device - need to specify what queues you use with it
       VkDeviceCreateInfo device_create_info = {0};
       device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
       device_create_info.queueCreateInfoCount = n_queues;
       device_create_info.pQueueCreateInfos = new_qs;
       device_create_info.pEnabledFeatures = &deviceFeatures;
       device_create_info.enabledExtensionCount = global_n_required_extenions;
       device_create_info.ppEnabledExtensionNames = global_required_extenions;

       check_err(vkCreateDevice(vulkan_info.phyDevice, &device_create_info, NULL, &vulkan_info.device), "Creating a logical device.");
       free(new_qs);

       vkGetDeviceQueue(vulkan_info.device, good_qs.graphicsAndComputeFamily, 0, &(vulkan_info.graphicsQueue));
       vkGetDeviceQueue(vulkan_info.device, good_qs.compatableFamily, 0, &(vulkan_info.presentQueue));

}


VkSampleCountFlagBits getMaxUsableSampleCount() {
       VkPhysicalDeviceProperties physicalDeviceProperties;
       vkGetPhysicalDeviceProperties(vulkan_info.phyDevice, &physicalDeviceProperties);

       VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
       if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
       if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
       if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
       if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
       if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
       if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

       return VK_SAMPLE_COUNT_1_BIT;
}


// PHYSICAL DEVICE FUNCTIONS
void pickPhysicalDevice(){
       uint32_t deviceCount = 0;
       VkPhysicalDevice *phyDevices = NULL;

       // 1) Get phsical device list
       check_err(vkEnumeratePhysicalDevices(vulkan_info.pInstance, &deviceCount, NULL), "Getting amount of physical devices");
       passert(deviceCount != 0, "There are no supported graphics devices");
       phyDevices = (VkPhysicalDevice *)calloc(sizeof(VkPhysicalDevice), deviceCount);
       check_err(vkEnumeratePhysicalDevices(vulkan_info.pInstance, &deviceCount, phyDevices), "Getting physical devices");

       // 2) Get the best physical device
       int best_idx = -1;
       int best_score = 0;
       int temp = 0;
       for(int i = 0; i < deviceCount; i += 1){
              temp = isDeviceSuitable(phyDevices[i]);
              if(temp > best_score){
                     best_score = temp;
                     best_idx = i;
              }
       }
       passert(best_idx != -1, "Counld not find compatable device!");
       vulkan_info.phyDevice = phyDevices[best_idx];

       #ifdef VERBOSE
       VkPhysicalDeviceProperties pProperties;
       vkGetPhysicalDeviceProperties(vulkan_info.phyDevice, &pProperties);
       printf("\nUsing physical device: %s\n", pProperties.deviceName);
       #endif
       
       vulkan_info.msaaSamples = VK_SAMPLE_COUNT_1_BIT; //getMaxUsableSampleCount();
       
       free(phyDevices);
}


int isDeviceSuitable(VkPhysicalDevice device) {
       #ifdef VERBOSE
       VkPhysicalDeviceProperties pProperties;
       vkGetPhysicalDeviceProperties(device, &pProperties);
       print_phyDevice(pProperties.deviceName);
       #endif

       VkPhysicalDeviceFeatures supportedFeatures;
       vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

       if( ! supportedFeatures.samplerAnisotropy) {return -1;}
       // Need the following things:
       // 1) All extensions by window are supported by device
       // 2) There exists a queue family that supports a) graphics family + b) the window you are using
       // 3) Swap chain is supported on this device with the window
       printf("      Checking device extensions\n");
       if(checkDeviceExtensionSupport(device) == false){printf("      - Device does not support extensions window needs\n"); return -1;}


       printf("      Getting queue famiiy\n");
       QueueFamilyIndices_t indxes = findQueueFamilies(device);
       if(indxes.graphicsAndComputeFamily == -1 || indxes.compatableFamily == -1){printf("    - Device does not have a graphics Q and/or there is no Q that supports this KHR surface\n");return -1;}

       printf("      Checking support for swap chain\n");
       SwapChainSupportDetails_t details = querySwapChainSupport(device);
       if(details.formats == NULL || details.presentModes == NULL){
              if(details.presentModes != NULL){free(details.presentModes);}
              if(details.formats != NULL){free(details.formats);}
              printf("\n");
              return -1;
       }

       if(details.presentModes != NULL){free(details.presentModes);}
       if(details.formats != NULL){free(details.formats);}
       
       return indxes.n_graphics_qs;

}

bool strcmp_utf(char *a, const char *b){
       int n = 0;
       while(*a != 0 && *b != 0){
              if(*a != *b){
                     return false;
              }
              a += 1;
              b += 1;
              passert(n < 1000, "Strings are too long");
       }
       return *a == *b;
}


// Device should support all ext. from static list given -> 
bool checkDeviceExtensionSupport(VkPhysicalDevice device){
       uint32_t                    dev_extCount = 0;
       VkExtensionProperties       *dev_extPrp = NULL;
       
       // 2) Get list of extensions that are supported by this device
       check_err(vkEnumerateDeviceExtensionProperties(device, NULL, &dev_extCount, dev_extPrp), "Checking physical device extension properties");
       if(dev_extCount < global_n_required_extenions){return false;}
       dev_extPrp = (VkExtensionProperties *)calloc(sizeof(VkExtensionProperties), dev_extCount);
       check_err(vkEnumerateDeviceExtensionProperties(device, NULL, &dev_extCount, dev_extPrp), "Checking physical device extension properties");


       bool d_supported;
       bool ans;
       for(int j = 0; j < global_n_required_extenions; j+=1){

              d_supported = false;
              for(int i = 0; i < dev_extCount; i += 1 ){
                     d_supported = d_supported || strcmp_utf(dev_extPrp[i].extensionName, global_required_extenions[j]);
              }
              if(!d_supported){
                     break;
              }
       }

       free(dev_extPrp);
       return d_supported;
}

QueueFamilyIndices_t findQueueFamilies(VkPhysicalDevice device){
       QueueFamilyIndices_t indices;
       indices.graphicsAndComputeFamily = -1;
       indices.n_graphics_qs = 0;
       indices.compatableFamily = -1;
       indices.n_compatability_qs = 0;


       // 1) Get all the queue family properties 
       uint32_t queueFamilyCount = 0;
       VkQueueFamilyProperties *queueFamilies = NULL;
       vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

       queueFamilies = (VkQueueFamilyProperties *)calloc(sizeof(VkQueueFamilyProperties), queueFamilyCount);
       vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

       VkQueueFamilyProperties queueFamily;
       // 2) loop thorugh to find a the two compatable queue indexes
       for (int i = 0; i < queueFamilyCount; i += 1) {
              queueFamily = queueFamilies[i];
              if( (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) ){
                     indices.graphicsAndComputeFamily = i;
                     indices.n_graphics_qs = queueFamily.queueCount;
              }

              VkBool32 presentSupport = false;
              vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkan_info.surface, &presentSupport);

              if (presentSupport) {
                     indices.compatableFamily = i;
                     indices.n_compatability_qs = queueFamily.queueCount;
              }

              if (indices.graphicsAndComputeFamily != -1 && indices.compatableFamily != -1) {
                     break;
              }

       }
       free(queueFamilies);
       return indices;
}


/*
struct pair_t{
       int num;
       const char *name;
};
    VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
    VK_PRESENT_MODE_MAILBOX_KHR = 1,
    VK_PRESENT_MODE_FIFO_KHR = 2,
    VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
  // Provided by VK_KHR_shared_presentable_image
    VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR = 1000111000,
  // Provided by VK_KHR_shared_presentable_image
    VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR = 1000111001,
*/

void print_swap_chain_support(SwapChainSupportDetails_t details){
       // 1. Capabilities
       printf("\t This device and surface supports swap chains with:\n");
       printf("\t\t Image count range: between %d-%d images\n", details.capabilities.minImageCount, details.capabilities.maxImageCount);
       printf("\t\t Size of image inputted: (%d, %d)\n", details.capabilities.currentExtent.width, details.capabilities.currentExtent.height);
       printf("\t\t Accepted image extent range: (%d, %d) -> (%d, %d)\n", \
                     details.capabilities.minImageExtent.width, details.capabilities.minImageExtent.height, \
                     details.capabilities.maxImageExtent.width, details.capabilities.maxImageExtent.height);
       printf("\t\t Maximum array layers of presentable images (mip-maping): %d\n", details.capabilities.maxImageArrayLayers);
       printf("\t\t Other supported transfroms/flags of swapchain...\n");
       // Other supported transforms and usage flags of the device


       if(details.n_formats != 0){
              printf("\t This device and surface supports the following formats for swap chains:\n");
              VkSurfaceFormatKHR format;
              for(int i = 0; i < details.n_formats; i += 1){
                     format = details.formats[i];
                     printf("\t\t Supported fromat/clr pair #%d\n", i);
                     printf("\t\t\t Format: %d\n", format.format);
                     printf("\t\t\t Colors: %d\n", format.colorSpace);
              }
       }
       if(details.presentModes != 0){
              printf("\t This device and surface supports the following presentation modes for swap chains:\n");
              VkPresentModeKHR presentMode;
              for(int i = 0; i < details.n_presentModes; i += 1){
                     printf("\t\t Supported present mode #%d\n", i);
                     printf("\t\t\t VkPresentModeKHR = %d\n", details.presentModes[i]);
              }
       }


}


SwapChainSupportDetails_t querySwapChainSupport(VkPhysicalDevice device) {
       SwapChainSupportDetails_t details = {0};

       // 1. What type of swap chains can this device make given this surface (https://docs.vulkan.org/spec/latest/chapters/VK_KHR_surface/wsi.html#vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
       vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan_info.surface, &details.capabilities);

       // 2. Get the available formats 
       uint32_t formatCount;
       vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan_info.surface, &formatCount, NULL);

       if (formatCount != 0) {
              details.formats = (VkSurfaceFormatKHR *)calloc(sizeof(VkSurfaceFormatKHR), formatCount);
              details.n_formats = formatCount;
              vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan_info.surface, &formatCount, details.formats);
       }

       // 3. Get the modes that this swapchain can present in.
       uint32_t presentModeCount;
       vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan_info.surface, &presentModeCount, NULL);

       if (presentModeCount != 0) {
              details.presentModes = (VkPresentModeKHR *)calloc(sizeof(VkPresentModeKHR), presentModeCount);
              details.n_presentModes = presentModeCount;
              vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan_info.surface, &presentModeCount, details.presentModes);
       }

       #ifdef VERBOSE
       print_swap_chain_support(details);
       #endif

       return details;
}








// SURFACE AND VULKAN INSTANCE CREATE FUNCTIONS

void createSurface(){
       check_err(get_wind_surface_spec(&(vulkan_info.surface), vulkan_info.pInstance), "Making surface");
       
}


// call vkCreateInstance
void createInstance(){
       uint32_t extCount = 0;
       

       VkApplicationInfo appInfo   = {0};
       appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
       appInfo.pApplicationName    = "Vulkan";
       appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
       appInfo.pEngineName         = "No Engine";
       appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
       appInfo.apiVersion          = VK_API_VERSION_1_0;
       
       const char **extension_names = get_wind_extensions_sec(&extCount);
       passert(extCount == 0 || extension_names != NULL, "Getting required extensions from window");

       #ifdef VERBOSE
       print_extensions(extension_names, extCount);
       #endif

       VkInstanceCreateInfo createInfo    = {0};
       createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
       createInfo.pApplicationInfo        = &appInfo;
       createInfo.enabledExtensionCount   = extCount;
       createInfo.ppEnabledExtensionNames = extension_names;

       check_err(vkCreateInstance(&createInfo, NULL, &(vulkan_info.pInstance)), "Starting vulkan instance");
       vulkan_info.currentFrame = 0;
       vulkan_info.syncs_arr_size = 0;
       vulkan_info.framebufferResized = false;
}



void passert(int tf, const char *s){
       if(tf){
              return;
       }
       printf("ERROR: %s", s);
       exit(1);
}



void check_err(VkResult retval, const char *s){
       if(retval == VK_INCOMPLETE){
              printf("WARNING: did not finish cmd: %s", s);
              return;
       }
       if(retval != VK_SUCCESS ){
              printf("ERROR %d: %s", (int)retval, s);
              exit(1);
       }
}

void *safe_calloc(uint32_t size, uint32_t amt){
       void *temp = calloc(size, amt);
       passert(temp != NULL, "Allocating space");
}

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
       VkCommandBuffer commandBuffer = beginSingleTimeCommands();
       VkImageMemoryBarrier barrier = {0};
       barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
       barrier.oldLayout = oldLayout;
       barrier.newLayout = newLayout;
       barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
       barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
       barrier.image = image;
       barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
       barrier.subresourceRange.baseMipLevel = 0;
       barrier.subresourceRange.levelCount = 1;
       barrier.subresourceRange.baseArrayLayer = 0;
       barrier.subresourceRange.layerCount = 1;
       barrier.srcAccessMask = 0; // TODO
       barrier.dstAccessMask = 0; // TODO

       VkPipelineStageFlags sourceStage;
       VkPipelineStageFlags destinationStage;

       if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
              barrier.srcAccessMask = 0;
              barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

              sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
              destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
       } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
              barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
              barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

              sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
              destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
       } else {
              passert(false, "unsupported layout transition!");
       }

       vkCmdPipelineBarrier(
              commandBuffer,
              sourceStage, destinationStage,
              0,
              0, NULL,
              0, NULL,
              1, &barrier
       );
       endSingleTimeCommands(commandBuffer);
}


void createTextureImage(char *filename){
       
       int texWidth, texHeight, texChannels;
       stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha); // raw pixel array 
       VkDeviceSize imageSize = texWidth * texHeight * 4;

       passert(pixels != NULL, "failed to load texture image!");

       // Make a staging buffer 
       VkBuffer stagingBuffer;
       VkDeviceMemory stagingBufferMemory;

       createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, \
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, \
                     &stagingBuffer, &stagingBufferMemory);

       void* data;
       vkMapMemory(vulkan_info.device, stagingBufferMemory, 0, imageSize, 0, &data);
              memcpy(data, pixels, (size_t)(imageSize));
       vkUnmapMemory(vulkan_info.device, stagingBufferMemory);

       stbi_image_free(pixels);

       // Make a new 'image' type 


       createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, \
              VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, \
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(vulkan_info.textureImage), &(vulkan_info.textureImageMemory));

       transitionImageLayout(vulkan_info.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
       copyBufferToImage(stagingBuffer, vulkan_info.textureImage, (uint32_t)(texWidth), (uint32_t)(texHeight));
       transitionImageLayout(vulkan_info.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

       vkDestroyBuffer(vulkan_info.device, stagingBuffer, NULL);
       vkFreeMemory(vulkan_info.device, stagingBufferMemory, NULL);

       vulkan_info.textureImageView = createImageView(vulkan_info.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1);
       
}



void createTextureSampler(){
       // https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler
       VkSamplerCreateInfo samplerInfo = {0};
       samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
       samplerInfo.magFilter = VK_FILTER_LINEAR;
       samplerInfo.minFilter = VK_FILTER_LINEAR;
       samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
       samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
       samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

       VkPhysicalDeviceProperties properties = {0};
       vkGetPhysicalDeviceProperties(vulkan_info.phyDevice, &properties);
       samplerInfo.anisotropyEnable = VK_TRUE;
       samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

       samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
       samplerInfo.unnormalizedCoordinates = VK_FALSE;
       samplerInfo.compareEnable = VK_FALSE;
       samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

       samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
       samplerInfo.mipLodBias = 0.0f;
       samplerInfo.minLod = 0.0f;
       samplerInfo.maxLod = 0.0f;

       passert(vkCreateSampler(vulkan_info.device, &samplerInfo, NULL, &(vulkan_info.textureSampler)) == VK_SUCCESS, "failed to create texture sampler!");
}




/*
       vulkan instance:
              - configure debug layers for Vulkan API
              - tell vulkan what extensions to enable.

       image surface
              - should be compatable with the window you are using
       
       physical devices
              - can look though all that are available

       logical devices
              - choose a piysical device to use
              - physical device should support the image surface type you are using
       
       swap chain
              - you can render multiple images at the same time. When presenting to screen -> the program will take the latest one.
              - swap chain type should be supported by the physical device.




Adding vertex buffer
       - Will need to allocate, buffer, and copy the data
       - Will need to specify the formate inputted in the graphics pipeline
       - Will need to bind the buffers/allocaions as a cmd before drawing.

Adding global vairables/ dynamic data - unifrom buffers
       - Make a descriptor set layout during pipeline creation
       - Allocate descriptor set from descriptor pool
       - Bind the set during rendeing


Adding a compute stage
       - Will need descriptor set layout
              - Uniform
                     - Make struct for arguments (certain alignemnet)
              - storage buffers
                     - specify struct for storage elements
       - Will need allocations of the buffers
       - Will need the descriptor sets themselves (MAX_IN_FLIGHT_FRAMESx)
              - Uniform
                     - Make buffer + allocation for this
                     - move data to this buffer by mapping memory
              - storage buffers
                     - Make buffer + allocation for this
                     - init data with cmd/submit from staging buffer (quicker)
       - Will need a compute pipeline. (VkPipelineLayout // VkPipeline)
              - Upload the code
              - specify descriptor set layout format. (VkDescriptorSetLayout)
              - bind to the desciptor set for currframe
       - Will need synchronization.
              - Will need MAX_IN_FLIGHT_FRAMESx fences (dont start compute record before same compute record still not submitted.)
              - Will need MAX_IN_FLIGHT_FRAMESx semaphores (dont start graphics before compute done)
       - To link the output buffer of the compute with the input vertex buffer of the graphics
              - Will need to specify the bindings/attributes of array buffer in graphics pipeline
       

       Compute shader
              - will require a defined input layout (remmeber its aligned to 16 bytes?)
              - in a layout definition you can specify other properties of the memory you read/write to. (std140?)
              - it will also have a binding that should match the pipline/renderpass object


Custimizing the Compute shader
       - Will need to make comute shader process the inputs -> outputs
       - Will need to link the output of the compute shader as the input of the vertex shader
              - Make graphics commands take in compute output for that frame, draw non-indexed with amt = n_particles


Adding a pipeline for lines.
       - Will need a new pipeline to execute every renderpass
       - This can be done in parralel with the trianlge submission
              - Bind the line_graphics_pipeline
              - Bind the line buffer
              (re-using the vertex buffer we created last time as now the triangle buffers come from compute)

2D waves
       - Make many more particles/points to cover a set feild.


Making better 2D waves
       - caluculate accelaeration based on surrounding paricles a = c*(d_pxpy + d_nxpy + d_pxny + d_nxny)
       - will need to know bounds of the particle array in both x and y dimensions

NOTES
       - No Callback to recreate swap chain
       

*/






