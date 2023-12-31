#include "vulkan2.h"

typedef struct vulkan_instance_t{
       // Instance handels
       VkInstance pInstance;


       // logical device parameters
       VkDevice logical_device;
       VkPhysicalDevice phy_device_used;
       int queue_family_used;
       int n_queues_taken;

       // cmds
       VkCommandPool cmd_pool;
       VkCommandBuffer *cmd_buffers;
       int n_cmd_buffers;

       // first image, handle for host info? and device location
       VkImage              img1;
       VkDeviceMemory       img1_loc;
       VkImageView          img_view_1_1;
       uint32_t             img1_w;
       uint32_t             img1_h;
       

       // Shaders
       uint32_t                           n_shader_stages;
       VkShaderModule                     vertex_shader;
       VkShaderModule                     fragment_shader;
       VkPipelineShaderStageCreateInfo    shader_stages[2];


       // Pipeline and rendering objects
       VkPipelineLayout                   pipelineLayout;      // global variables
       VkRenderPass                       renderpass;          // specify allocations
       VkPipeline                         pipeline;            // pipline links stages and attachments


} vulkan_instance_t;



static VkResult ret;


void make_vulkan_instance(vulkan_instance_t *inst){
       const VkInstanceCreateInfo                 init_flags = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, NULL, 0, NULL, 0, NULL, 0, NULL};
       VkInstance                                 pInstance;

       // Flags may be overriden by text config file in user/apdata...
       ret = vkCreateInstance(&init_flags, NULL, &pInstance);
       check_err(ret, "Starting vulkan instance");
       inst->pInstance = pInstance;
}


void make_logical_device(vulkan_instance_t *inst, int phy_dev_indx, int queue_family_idx){
       VkInstance                  pInstance = inst->pInstance;


       // 1) Get the physical device you want to base this logical device off of
       uint32_t                           n_physical_devices = 0;
       VkPhysicalDevice                   *device_arr;
       VkPhysicalDeviceProperties         pProperties;

       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, NULL);
       check_err(ret, "Getting amount of physical devices");
       device_arr = (VkPhysicalDevice *)calloc(sizeof(VkPhysicalDevice), n_physical_devices);
       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, device_arr);
       check_err(ret, "Getting physical devices");


       // 2) Check that the requested phy dev idx exists and print it out
       passert(n_physical_devices > phy_dev_indx, "ERROR: need to re-evaluate device index");
       vkGetPhysicalDeviceProperties(device_arr[phy_dev_indx], &pProperties);
       printf("\t Using physical device: %s\n", pProperties.deviceName);


       // 3) get the queue family from the physical device.
       VkQueueFamilyProperties     *queue_family_arr;
       uint32_t                    n_queues_families;

       vkGetPhysicalDeviceQueueFamilyProperties(device_arr[phy_dev_indx], &n_queues_families, NULL);
       queue_family_arr = (VkQueueFamilyProperties *)calloc(sizeof(VkQueueFamilyProperties), n_queues_families);
       vkGetPhysicalDeviceQueueFamilyProperties(device_arr[phy_dev_indx], &n_queues_families, queue_family_arr);

       // 4) Check that your requested queue family indx exists
       passert(queue_family_idx < n_queues_families, "Queue family index does not exist for this device");
       printf("Queue #%d: ", queue_family_idx);
       print_queue_family(queue_family_arr + queue_family_idx);
       printf("\n");
       free(queue_family_arr);


       // 5) Make a new queue for the logical device
       VkDeviceQueueCreateInfo     new_queues[1];
       uint32_t                    n_new_queues;
       float                       queue_prio_1[1];
       
       queue_prio_1[0] = 1;
       VkDeviceQueueCreateInfo queue_info_graphics = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  NULL, 0, 0, 1, queue_prio_1};
       queue_info_graphics.queueFamilyIndex = queue_family_idx;


       // 6) Make the new logical device
       // sType, pNext, flags, queueFamilyIndex, queueCount, pQueuePriorities, queue_prio;
       VkDevice                    pDevice_o;
       VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, NULL, 0, 1, &queue_info_graphics, 0, NULL, 0, NULL, NULL};
       ret = vkCreateDevice(device_arr[phy_dev_indx], &device_create_info, NULL, &pDevice_o);
       check_err(ret, "Creating a logical device.");


      inst->logical_device = pDevice_o;
      inst->phy_device_used = device_arr[phy_dev_indx];
      inst->n_queues_taken = 1;
      inst->queue_family_used = queue_family_idx;

      free(device_arr);
}

void make_command_pool(vulkan_instance_t *inst){
       
       // type_def, pnext, flags, queue family index
       VkCommandPoolCreateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, NULL, 0, 0};
       create_info.queueFamilyIndex = inst->queue_family_used;

       
       ret = vkCreateCommandPool(inst->logical_device, &create_info, NULL, &(inst->cmd_pool));
       check_err(ret, "Creating a command pool on logical device.");

}


void allocate_command_buffer(vulkan_instance_t *inst, uint32_t n_buffers, VkCommandBufferLevel level){
 
       inst->cmd_buffers = (VkCommandBuffer *)calloc(sizeof(VkCommandBuffer), n_buffers);
       passert(inst->cmd_buffers != NULL, "Allocating command buffers");

       // type_def, pnext, level, amt of buffers
       VkCommandBufferAllocateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO , NULL, 0 , 0, 0};
       create_info.commandPool = inst->cmd_pool;
       create_info.level = level;
       create_info.commandBufferCount = n_buffers;


       ret = vkAllocateCommandBuffers(inst->logical_device, &create_info, inst->cmd_buffers);
       check_err(ret, "Creating a command buffer on logical device.");
}



// extent in width, height, depth
void create_image(vulkan_instance_t *inst, int dim, VkExtent3D extent){


       passert(dim <= 3 && dim >= 1, "invalid dimension in create image");

       VkImageCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
       create_info.imageType = (dim == 1)*VK_IMAGE_TYPE_1D + (dim == 2)*VK_IMAGE_TYPE_2D + (dim == 3)*VK_IMAGE_TYPE_3D;
       create_info.format = VK_FORMAT_R8G8B8A8_UINT;
       create_info.extent = extent;
       create_info.mipLevels = 1;
       create_info.arrayLayers = 1; 
       create_info.samples = VK_SAMPLE_COUNT_1_BIT;
       create_info.tiling = VK_IMAGE_TILING_OPTIMAL; //VK_IMAGE_TILING_LINEAR 
       create_info.usage = VK_IMAGE_USAGE_STORAGE_BIT;
       create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // VK_SHARING_MODE_CONCURRENT
       create_info.queueFamilyIndexCount = 0;
       create_info.pQueueFamilyIndices = 0; // used for concurrent queue family sharing
       create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

       ret = vkCreateImage(inst->logical_device, &create_info, NULL, &(inst->img1));
       check_err(ret, "Creating a image");
       inst->img1_w = extent.width;
       inst->img1_h = extent.height;

}

void bind_to_device_memory(vulkan_instance_t *inst){
       // 1) based on the image requirements, find matching type in phy device
       VkMemoryRequirements               mem_requirements;
       VkPhysicalDeviceMemoryProperties   mem_propoperties;

       vkGetImageMemoryRequirements(inst->logical_device, inst->img1,  &mem_requirements);
       vkGetPhysicalDeviceMemoryProperties(inst->phy_device_used, &mem_propoperties);
       
       int i = 0;
       uint32_t supported__mem_type__indexes = mem_requirements.memoryTypeBits;
       while((supported__mem_type__indexes & 0x1) == 0){
              i += 1;
              passert(i < 32, "No memory types supported by created image");
       }
       passert(i < mem_propoperties.memoryTypeCount, "Invalid memory type required by created image");


       // 2) Allocate on device
       VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL, 0, 0};
       alloc_info.allocationSize = mem_requirements.size;
       alloc_info.memoryTypeIndex = i;

       ret = vkAllocateMemory(inst->logical_device, &alloc_info, NULL, &(inst->img1_loc));



       // 3) bind to the image that was created

       ret = vkBindImageMemory(inst->logical_device, inst->img1, inst->img1_loc, 0);
}


void create_image_view(vulkan_instance_t *inst, int dim){

       VkImageViewCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
       create_info.image = inst->img1;
       create_info.viewType = (dim == 1)*VK_IMAGE_TYPE_1D + (dim == 2)*VK_IMAGE_TYPE_2D + (dim == 3)*VK_IMAGE_TYPE_3D;
       create_info.format = VK_FORMAT_R8G8B8A8_UINT;
       
       // for post-processing?
       create_info.components = (VkComponentMapping){VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
       
       // specifies what resources can this image-view access
       create_info.subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, \
                                          0, /* mip level start */\
                                          1, /* amt mip levels */ \
                                          0, /* arr level start */ \
                                          1}; /* amt arr levels */

       ret = vkCreateImageView(inst->logical_device, &create_info, NULL, &(inst->img_view_1_1));
       check_err(ret, "Creating a image view");

}
/*
void make_render_pass(vulkan_instance_t *inst){

       VkAttachmentDescription atchment1 = {0, VK_FORMAT_R8G8B8A8_UINT, VK_SAMPLE_COUNT_1_BIT, \
       VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, \
       VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE \
       VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL };


       VkRenderPassCreateInfo create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, NULL, 0, 0, NULL, 0, NULL, 0, NULL};
       create_info.attachmentCount = 1;
       create_info.pAttachments = atchment1;
       create_info.subpassCount = 0;
       create_info.pSubpasses = 0;
       create_info.dependencyCount = 0;
       create_info.pDependencies = 0;

       ret = vkCreateRenderPass(inst->logical_device, create_info, NULL, &(inst->renderpass));
}


void make_graphics_pipeline(){
       //VkPipeline    ans;
       uint32_t      n_creates;
       
       VkPipelineShaderStageCreateInfo shader_stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, NULL, \
       VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT, VK_SHADER_STAGE_ALL_GRAPHICS, NULL, "Shader stage 1", NULL};

       
       
       VkGraphicsPipelineCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       create_info.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
       create_info.stageCount = 1;
       create_info.pStages = shader_stage;
       ret = vkCreateGraphicsPipelines(inst->logical_device, VK_NULL_HANDLE, n_creates, create_info, NULL, &ans);


typedef struct VkGraphicsPipelineCreateInfo {
    VkStructureType                                  sType;
    const void*                                      pNext;
    VkPipelineCreateFlags                            flags;
    uint32_t                                         stageCount;
    const VkPipelineShaderStageCreateInfo*           pStages;
    const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
    const VkPipelineTessellationStateCreateInfo*     pTessellationState;
    const VkPipelineViewportStateCreateInfo*         pViewportState;
    const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
    const VkPipelineDynamicStateCreateInfo*          pDynamicState;
    VkPipelineLayout                                 layout;
    VkRenderPass                                     renderPass;
    uint32_t                                         subpass;
    VkPipeline                                       basePipelineHandle;
    int32_t                                          basePipelineIndex;
} VkGraphicsPipelineCreateInfo;
}
*/






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


VkShaderModule make_shader_module(vulkan_instance_t *inst, const char *filename){
       uint32_t sz_shader = 0;
       uint32_t *bytecode = (uint32_t *)load_shader(filename, &sz_shader);
       printf("%s :: %d\n", filename, sz_shader);
       VkShaderModule ans;


       VkShaderModuleCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
       create_info.codeSize = sz_shader;
       create_info.pCode = bytecode;

       ret = vkCreateShaderModule(inst->logical_device, &create_info, NULL, &(ans));
       check_err(ret, "Creating a shader module");
       free(bytecode);
       return ans;
}


void make_graphics_pipeline(vulkan_instance_t *inst){
       //VkPipeline    ans;
       uint32_t      n_creates;
       // 1) Make shader modules
       inst->fragment_shader = make_shader_module(inst, "./shaders/fragment.spv");
       inst->vertex_shader = make_shader_module(inst, "./shaders/vertice.spv");
       
       // 2) Make and store create infor for shaders
       inst->n_shader_stages = 2;
       VkPipelineShaderStageCreateInfo shader_stage_vertex = {0};
       shader_stage_vertex.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
       shader_stage_vertex.module = inst->vertex_shader;
       shader_stage_vertex.stage = VK_SHADER_STAGE_VERTEX_BIT; // A stage that takes in vertexes?
       shader_stage_vertex.pName = "main";       // function to invoke in shader module
       
       VkPipelineShaderStageCreateInfo shader_stage_fragment = {0};
       shader_stage_fragment.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
       shader_stage_fragment.module = inst->fragment_shader;
       shader_stage_fragment.stage = VK_SHADER_STAGE_FRAGMENT_BIT; // A stage that takes in fragments?
       shader_stage_fragment.pName = "main";       // function to invoke in shader module
       inst->shader_stages[0] = shader_stage_vertex;
       inst->shader_stages[1] = shader_stage_fragment;


       // 3) Import data for the vertex stage - format of triangle data in memory? -> some opperation will be done per vertex. \
       need to know format to be able to access x value for example in shader
       VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
       vertexInputInfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
       vertexInputInfo.vertexBindingDescriptionCount    = 0;                 
       vertexInputInfo.pVertexBindingDescriptions       = NULL; // Optional
       vertexInputInfo.vertexAttributeDescriptionCount  = 0;
       vertexInputInfo.pVertexAttributeDescriptions     = NULL; // Optional



       // 4) How will trianlges be used?
       VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
       inputAssembly.sType                   = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
       inputAssembly.topology                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;     // use as trianles
       inputAssembly.primitiveRestartEnable  = VK_FALSE;

       // 5) Specify the viewport to be used in this pipeline (can also declare through dynamic data) - 
       VkViewport viewp = {0, 0, 1000, 1000, 0.0f, 1.0f};
       viewp.width = inst->img1_w;
       viewp.height = inst->img1_h;
       VkRect2D scissor = {0, 0, 1000, 1000};
       scissor.extent.width = inst->img1_w;
       scissor.extent.height = inst->img1_h;


       VkPipelineViewportStateCreateInfo viewportState = {0};
       viewportState.sType                = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
       viewportState.viewportCount        = 1;
       viewportState.pViewports           = &viewp;
       viewportState.scissorCount         = 1;
       viewportState.pScissors            = &scissor;

       // 6) Rasterization info - takes geometry shaped by vertex shader and turns it into fragments that will be colored with fragment shader
       VkPipelineRasterizationStateCreateInfo rasterizer = {0}; // Specific stage info. Also includes depth testing
       rasterizer.sType                          = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
       rasterizer.depthClampEnable               = VK_FALSE;
       rasterizer.rasterizerDiscardEnable        = VK_FALSE;
       rasterizer.polygonMode                    = VK_POLYGON_MODE_FILL;
       rasterizer.lineWidth                      = 1.0f;
       rasterizer.cullMode                       = VK_CULL_MODE_BACK_BIT;
       rasterizer.frontFace                      = VK_FRONT_FACE_CLOCKWISE;

       rasterizer.depthBiasEnable                = VK_FALSE;
       rasterizer.depthBiasConstantFactor        = 0.0f; // Optional
       rasterizer.depthBiasClamp                 = 0.0f; // Optional
       rasterizer.depthBiasSlopeFactor           = 0.0f; // Optional


       // 7) Multi-sample properties to prevent aliasing - multiple pixels to same point -> so some mixing
       VkPipelineMultisampleStateCreateInfo multisampling = {0};
       multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
       multisampling.sampleShadingEnable  = VK_FALSE;
       multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
       multisampling.minSampleShading            = 1.0f; // Optional
       multisampling.pSampleMask                 = NULL; // Optional
       multisampling.alphaToCoverageEnable       = VK_FALSE; // Optional
       multisampling.alphaToOneEnable            = VK_FALSE; // Optional

       // 8) how will colors blend with frame buffer
       VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
       colorBlendAttachment.colorWriteMask       = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
       colorBlendAttachment.blendEnable          = VK_FALSE;
       colorBlendAttachment.srcColorBlendFactor  = VK_BLEND_FACTOR_ONE; // Optional
       colorBlendAttachment.dstColorBlendFactor  = VK_BLEND_FACTOR_ZERO; // Optional
       colorBlendAttachment.colorBlendOp         = VK_BLEND_OP_ADD; // Optional
       colorBlendAttachment.srcAlphaBlendFactor  = VK_BLEND_FACTOR_ONE; // Optional
       colorBlendAttachment.dstAlphaBlendFactor  = VK_BLEND_FACTOR_ZERO; // Optional
       colorBlendAttachment.alphaBlendOp         = VK_BLEND_OP_ADD; // Optional
       
       // one of the abover per each frambuffer 
       VkPipelineColorBlendStateCreateInfo colorBlending = {0};
       colorBlending.sType                       = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
       colorBlending.logicOpEnable               = VK_FALSE;
       colorBlending.logicOp                     = VK_LOGIC_OP_COPY; // Optional
       colorBlending.attachmentCount             = 1;
       colorBlending.pAttachments                = &colorBlendAttachment;
       colorBlending.blendConstants[0]           = 0.0f; // Optional
       colorBlending.blendConstants[1]           = 0.0f; // Optional
       colorBlending.blendConstants[2]           = 0.0f; // Optional
       colorBlending.blendConstants[3]           = 0.0f; // Optional

       // 7) Layer - used for global variables in shaders? - init to 0
       VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
       pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
       pipelineLayoutInfo.setLayoutCount         = 0; // Optional
       pipelineLayoutInfo.pSetLayouts            = NULL; // Optional
       pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
       pipelineLayoutInfo.pPushConstantRanges    = NULL; // Optional
       ret = vkCreatePipelineLayout(inst->logical_device, &pipelineLayoutInfo, NULL, &(inst->pipelineLayout));
       check_err(ret, "Creating a layout");


       // 8) Render pass - specify attachemnts/Allocations
       VkAttachmentDescription colorAttachment = {0};
       colorAttachment.format      = VK_FORMAT_R8G8B8A8_UINT;         // Specify image format
       colorAttachment.samples     = VK_SAMPLE_COUNT_1_BIT;           // 1 bit sampe image
       colorAttachment.loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR;     // When you load this attachment clear it
       colorAttachment.storeOp            = VK_ATTACHMENT_STORE_OP_STORE;    // Store results (dont just forget about them)
       colorAttachment.stencilLoadOp      = VK_ATTACHMENT_LOAD_OP_DONT_CARE;      // dont care about stencil array?
       colorAttachment.stencilStoreOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE;
       colorAttachment.initialLayout             = VK_IMAGE_LAYOUT_UNDEFINED;            //how are pixels layed out in memory - what layout the image will have before pass
       colorAttachment.finalLayout               = VK_IMAGE_LAYOUT_GENERAL;        // how are pixels layed out in memory - transitioned layout

       VkAttachmentReference colorAttachmentRef = {0};                       // refer to the above attachment
       colorAttachmentRef.attachment = 0;                                    // we only have 1 attchment -> first in attachemnt array
       colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // format of the color attachment

       VkSubpassDescription subpass = {0};
       subpass.pipelineBindPoint                 = VK_PIPELINE_BIND_POINT_GRAPHICS; // What type of pipline can you bind too
       subpass.colorAttachmentCount              = 1;                               // How many attachemts of type color do you have
       subpass.pColorAttachments                 = &colorAttachmentRef;               // link to it



       VkRenderPassCreateInfo renderPassInfo = {0};
       renderPassInfo.sType               = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
       renderPassInfo.attachmentCount     = 1;
       renderPassInfo.pAttachments        = &colorAttachment;
       renderPassInfo.subpassCount        = 1;
       renderPassInfo.pSubpasses          = &subpass;

       vkCreateRenderPass(inst->logical_device, &renderPassInfo, NULL, &(inst->renderpass));
       check_err(ret, "Creating a render pass object");

       // 9) Pipeline
       VkGraphicsPipelineCreateInfo pipelineInfo = {0};
       pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       pipelineInfo.stageCount            = inst->n_shader_stages;
       pipelineInfo.pStages               = inst->shader_stages;       // Describes code of each stage
       pipelineInfo.pVertexInputState                   = &vertexInputInfo;  // What is format of vertex data to be passed to vertex shader
       pipelineInfo.pInputAssemblyState                 = &inputAssembly;    // What will you do with vertex data?
       pipelineInfo.pViewportState                      = &viewportState;    // describe veiwport for frambuffer
       pipelineInfo.pRasterizationState                 = &rasterizer;       // describe how trigs are turned into drawable fragments
       pipelineInfo.pMultisampleState                   = &multisampling;    // describe mixing of next-to-eachother pixels
       pipelineInfo.pDepthStencilState                  = NULL; // Optional
       pipelineInfo.pColorBlendState                    = &colorBlending;    // How is incomming upates blended with current fram buffer contetns
       pipelineInfo.pDynamicState                       = NULL; //&dynamicState;
       pipelineInfo.layout                       = inst->pipelineLayout;    // global varibles
       pipelineInfo.renderPass            = inst->renderpass;  // attachments
       pipelineInfo.subpass               = 0;                 // which subpass of the renderpass to use.
       pipelineInfo.basePipelineHandle    = VK_NULL_HANDLE;
       pipelineInfo.basePipelineIndex     = -1;
       vkCreateGraphicsPipelines(inst->logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &(inst->pipeline));
       check_err(ret, "Creating a pipline");

       
       // 3) Make the pipeline
       /*
       VkGraphicsPipelineCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       create_info.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
       create_info.stageCount = 2;
       create_info.pStages = shader_stage;
       ret = vkCreateGraphicsPipelines(inst->logical_device, VK_NULL_HANDLE, n_creates, create_info, NULL, &ans);
       */
}

void create_framebuffer(vulkan_instance_t *inst){
       // Framebuffer wraps all atachment (VkImageView) objects
       //     - Framebuffer needs to be compatable with the renderpass

       

}



int main(int argc, char *argv[]){

       vulkan_instance_t *instance = (vulkan_instance_t *)calloc(sizeof(vulkan_instance_t), 1);
       make_vulkan_instance(instance);

       printf("Making logical device...\n");
       make_logical_device(instance, 1, 0);

       printf("Making command pool...\n");
       make_command_pool(instance);

       printf("Allocating a buffer inside the pool...\n");
       allocate_command_buffer(instance, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

       printf("Creating a image asset...\n");
       create_image(instance, 2, (VkExtent3D){1000,1000,1});


       printf("Binding the image to device memory...\n");
       bind_to_device_memory(instance);

       printf("Creating image view...\n");
       create_image_view(instance, 2);
       
       printf("Loading in external shaders...\n");
       //instance->fragment_shader = make_shader_module(instance, "./shaders/vertice.spv");
       //instance->vertex_shader = make_shader_module(instance, "./shaders/fragment.spv");
       make_graphics_pipeline(instance);






       vkDestroyShaderModule(instance->logical_device, instance->fragment_shader, NULL);
       vkDestroyShaderModule(instance->logical_device, instance->vertex_shader, NULL);
       vkDestroyPipelineLayout(instance->logical_device, instance->pipelineLayout, NULL);
       //vkDestroyShaderModule(instance->logical_device, instance->fragment_shader, NULL);
       //vkDestroyShaderModule(instance->logical_device, instance->vertex_shader, NULL);
       printf("Exiting program...\n");
}