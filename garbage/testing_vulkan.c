
#include <vulkan\vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include "testing_vulkan.h"


// https://docs.vulkan.org/spec/latest/chapters/cmdbuffers.html (documentation)
// https://vulkan.lunarg.com/doc/view/latest/windows/apispec.html (function list)



typedef struct vulkan_instance_t{
       // Instance handels
       VkInstance pInstance;


       // logical device parameters
       VkDevice logical_device;
       VkPhysicalDevice phy_device_used;
       int queue_family_used;
       int n_queues_taken;

       // Dynamic data
       VkCommandPool cmd_pool;

       

} vulkan_instance_t;



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
              printf("ERROR: %s\n", s);
              exit(1);
       }
}

void print_device_groups(VkInstance pInstance){
       VkResult                           ret;
       uint32_t                           n_device_groups;
       VkPhysicalDeviceGroupProperties    *device_grp_arr;
       VkPhysicalDeviceProperties          pProperties;

       ret = vkEnumeratePhysicalDeviceGroups(pInstance, &n_device_groups, NULL);
       check_err(ret, "Getting device groups arr size");
       device_grp_arr = (VkPhysicalDeviceGroupProperties *)calloc(sizeof(VkPhysicalDeviceGroupProperties), n_device_groups);
       ret = vkEnumeratePhysicalDeviceGroups(pInstance, &n_device_groups, device_grp_arr);
       check_err(ret, "Getting device groups arr");


       printf("There are %d device groups. All physical devices are belong to one of these groups.\n", n_device_groups);
       for(int i=0;i<n_device_groups;i+=1){
              printf("\t Group # %d\n", i);
              printf("\t\t There are %d devices in this group\n", device_grp_arr[i].physicalDeviceCount);
              for(int j=0;j<device_grp_arr[i].physicalDeviceCount;j+=1){
                     if(j == VK_MAX_DEVICE_GROUP_SIZE){break;}
                     vkGetPhysicalDeviceProperties(device_grp_arr[i].physicalDevices[j], &pProperties);
                     printf("\t\t\t Device # %d: %s\n", j, pProperties.deviceName);
              }
       }

}


void print_physical_devices(VkInstance pInstance){
       uint32_t                    n_physical_devices = 0;
       uint32_t                    n_queues = 0;
       VkResult                    ret;
       VkPhysicalDevice            *device_arr;
       VkQueueFamilyProperties     *queue_arr;
       VkPhysicalDeviceProperties  pProperties;


       // 1) Get a list from Vulkan for physical devices
       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, NULL);
       check_err(ret, "Getting amount of physical devices");

       device_arr = (VkPhysicalDevice *)calloc(sizeof(VkPhysicalDevice), n_physical_devices);

       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, device_arr);
       check_err(ret, "Getting physical devices");

       printf("PHYDEV%d\n", n_physical_devices);
       // 2) Taverse the list retured
       for(int i = 0; i < n_physical_devices; i+=1){
              vkGetPhysicalDeviceProperties(device_arr[i], &pProperties);
              printf("Device # %d\n", i);
              printf("\t api version: %d\n", pProperties.apiVersion);
              printf("\t driver version: %d\n", pProperties.vendorID);
              printf("\t vendor ID: %d\n", pProperties.deviceID);
              printf("\t device type: %s\n", device_types[pProperties.deviceType]);
              printf("\t device name: %s\n", pProperties.deviceName);
              // Limits, Sparse properties, pipline cache UUID

              vkGetPhysicalDeviceQueueFamilyProperties(device_arr[i], &n_queues, NULL);
              queue_arr = (VkQueueFamilyProperties *)calloc(sizeof(VkQueueFamilyProperties), n_queues);
              
              printf("\t This device supports %d queue families: \n");
              vkGetPhysicalDeviceQueueFamilyProperties(device_arr[i], &n_queues, queue_arr);
              for(int j = 0; j < n_queues; j += 1){
                     printf("\t\t Queue familiy # %d\n", j);
                     printf("\t\t Has %d queues of this type\n", queue_arr[j].queueCount);
                     if((queue_arr[j].queueFlags) & VK_QUEUE_GRAPHICS_BIT ){
                            printf("\t\t\t Supports graphics queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_COMPUTE_BIT ){
                            printf("\t\t\t Supports compute queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_TRANSFER_BIT ){
                            printf("\t\t\t Supports trasfer queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_VIDEO_DECODE_BIT_KHR ){
                            printf("\t\t\t Supports vidio decode queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_SPARSE_BINDING_BIT ){
                            printf("\t\t\t Supports sparse binding queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_PROTECTED_BIT ){
                            printf("\t\t\t Supports protected queues\n");
                     }
                     if((queue_arr[j].queueFlags) & VK_QUEUE_OPTICAL_FLOW_BIT_NV ){
                            printf("\t\t\t Supports optical flow queues\n");
                     }
                     //printf("LOOP #%d out of %d\n", j, n_queues);
              }

              free(queue_arr);


       }
       free(device_arr);
       return;
}





VkDevice make_logical_device(VkInstance pInstance){
       uint32_t                    n_physical_devices = 0;
       VkResult                    ret;
       VkPhysicalDevice            *device_arr;

       // 1) Get the physical device you want to base this logical device off of

       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, NULL);
       check_err(ret, "Getting amount of physical devices");

       device_arr = (VkPhysicalDevice *)calloc(sizeof(VkPhysicalDevice), n_physical_devices);

       ret = vkEnumeratePhysicalDevices(pInstance, &n_physical_devices, device_arr);
       check_err(ret, "Getting physical devices");
       

       // 2) set up the prams used to create the device. 
       //     a) set up the queues you want to tie to this device - 1 graphics, 1 compute?
       
       
       VkDeviceQueueCreateInfo     queues[2];
       uint32_t                    n_queues;
       float                       queue_prio_1[1];
       float                       queue_prio_2[1];  

       queue_prio_1[0] = 1;
       queue_prio_2[0] = 1;    

       VkDeviceQueueCreateInfo queue_info_compute = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0, 0, 1, queue_prio_2};
       VkDeviceQueueCreateInfo queue_info_graphics = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  NULL, 0, 0, 1, queue_prio_1};

       // sType, pNext, flags, queueFamilyIndex, queueCount, pQueuePriorities, queue_prio;

       //     b) make the create info struct to pass to creation of logical device
       VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, NULL, 0, 1, &queue_info_graphics, 0, NULL, 0, NULL, NULL};


       VkDevice                    pDevice_o;
       passert(n_physical_devices >= 2, "ERROR: need to re-evaluate device index");
       ret = vkCreateDevice(device_arr[0], &device_create_info, NULL, &pDevice_o);
       check_err(ret, "Creating a logical device. Based off of NVIDA graphics card and with 2 queues both from queue family inx 0");

       // What can you do with new device?
       /*
              - add more queues?
              - add sync constructs
              - allocate/free device memeory?
              - create + destroy command buffers
              - manage graphics state

              void vkDestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator);
       */

      free(device_arr);
      return pDevice_o;

}



// A pool belonging to a device that holds command buffers
VkCommandPool create_command_pool(VkDevice dev1){
       VkCommandPool        ans;
       VkResult             ret;
       

       // type_def, pnext, flags, queue family index
       VkCommandPoolCreateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, NULL, 0, 0};
       
       ret = vkCreateCommandPool(dev1, &create_info, NULL, &ans);
       check_err(ret, "Creating a command pool on logical device.");

       return ans;

}




VkCommandBuffer *allocate_command_buffer(VkDevice device, VkCommandPool pool, uint32_t n_buffers, VkCommandBufferLevel level){
       VkResult             ret;
 
       VkCommandBuffer *ans = (VkCommandBuffer *)calloc(sizeof(VkCommandBuffer), n_buffers);
       passert(ans != NULL, "Allocating command buffers");

       // type_def, pnext, level, amt of buffers
       VkCommandBufferAllocateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO , NULL, 0 , 0, 0};
       create_info.commandPool = pool;
       create_info.level = level;
       create_info.commandBufferCount = n_buffers;

       
       ret = vkAllocateCommandBuffers(device, &create_info, ans);
       check_err(ret, "Creating a command buffer on logical device.");

       return ans;
}


void start_buffer_recording(VkCommandBuffer buffer){
       VkResult             ret;

       const VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, NULL, 0, NULL};

       ret = vkBeginCommandBuffer(buffer, &begin_info);
       check_err(ret, "Starting the recording of a buffer.");
}

/*
VkDeviceMemory allocate_device_memory(uint32_t size, uint32_t types){
       VkResult             ret;
       VkDeviceMemory       ans;

       VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL, 0, 0};
       alloc_info->size = size;

       ret = vkAllocateMemory(device, alloc_info, NULL, &ans)

       return ans;
}*/

// extent in width, height, depth
VkImage create_image(VkDevice device, int dim, VkExtent3D extent){
       VkResult             ret;
       VkImage              new_image;

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

       ret = vkCreateImage(device, &create_info, NULL, &new_image);
       check_err(ret, "Creating a image");

       // Now bind image to memory (what was created is just info on the image, now need to allocate on GPU?)

       // 1) Find out the image allocation needs of this image
       //VkMemoryRequirements mem_req;
       //vkGetImageMemoryRequirements(device, new_image,  &mem_req);


       // 2) Allocate this amount of memory from the physical device


       //ret =  vkBindImageMemory(device, new_image,
    //VkDeviceMemory                              memory,
    //VkDeviceSize                                memoryOffset);

       return new_image;
}



VkImageView create_image_view(VkDevice device, VkImage master_image, int dim){
       VkResult             ret;
       VkImageView          ans;

       VkImageViewCreateInfo create_info = {0};
       create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
       create_info.image = master_image;
       create_info.viewType = (dim == 1)*VK_IMAGE_TYPE_1D + (dim == 2)*VK_IMAGE_TYPE_2D + (dim == 3)*VK_IMAGE_TYPE_3D;
       create_info.format = VK_FORMAT_R8G8B8A8_UINT;
       
       // for post-processing?
       create_info.components = (VkComponentMapping){VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
       
       // specifies what resources can this image-view access
       create_info.subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, \
                                          1, /* mip level start */\
                                          1, /* amt mip levels */ \
                                          1, /* arr level start */ \
                                          1}; /* amt arr levels */

       ret = vkCreateImageView(device, &create_info, NULL, &ans);
       check_err(ret, "Creating a image view");

       return ans;
}




/*
void add_render_pass_cmd(VkCommandBuffer *buffer, VkRect2D rect_i){
       VkResult             ret;
       VkRenderingInfo render_info = {0};


       render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
       render_info.renderArea = rect_i;
       render_info.layerCount = layer_count;
       render_info.viewMask = rect_i;
       render_info.colorAttachmentCount = rect_i;

       render_info.pDepthAttachment = ;
       render_info.pStencilAttachment = ;
       
       
       ret = vkCmdBeginRendering(buffer, render_info);
       check_err(ret, "Adding a start-render-pass cmd to a command buffer");

}*/


/*
VkFramebuffer make_frame_buffer(VkDevice device, ){
       VkResult ret;
       VkFramebuffer ans;

       VkFramebufferCreateInfo create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, NULL, };

       ret = vkCreateFramebuffer(device, &create_info, NULL, ans);

*/
/*
typedef struct VkFramebufferCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkFramebufferCreateFlags    flags;
    VkRenderPass                renderPass;
    uint32_t                    attachmentCount;
    const VkImageView*          pAttachments;
    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    layers;
} VkFramebufferCreateInfo;

       check_err(ret, "Making a frame buffer.");

       return ans;
}*/





int main(int argc, char *argv[]){
       VkResult ret;
       uint32_t pApiVersion;
       
       // Get the current version infor from lib i guess?
       ret = vkEnumerateInstanceVersion(&pApiVersion);

       printf("Hello using vulkan\nversion variant: %d\napi version major: %d\napi version minor:%d\n", \
                     VK_API_VERSION_VARIANT(pApiVersion), \
                     VK_API_VERSION_MAJOR(pApiVersion),\
                     VK_API_VERSION_MINOR(pApiVersion) );



       // 1) INIT vulkan
       // This is the first command to initalize the library \
              init_falgs is a struct storing info on how to start up vulkan \
              pAllocator is host allocator? malloc? \
              pInstance is the pointer to output instance object?
       const VkInstanceCreateInfo                 init_flags = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, NULL, 0, NULL, 0, NULL, 0, NULL};
       VkInstance                                 pInstance;
       // Flags may be overriden by text config file in user/apdata...
       ret = vkCreateInstance(&init_flags, NULL, &pInstance);
       check_err(ret, "Starting vulkan instance");


       // 2) Find physical devices, their queue families and amount of queues in each family
       print_physical_devices(pInstance);

       // 3) Find device groups -> physical devices are grouped into these
       print_device_groups(pInstance);


       // Now that we know what physical devices... we can find out what kind of queue families they have/support
       //     - multiple physical devices can band together to form 1 logical device if they have shareable \
              memeory and command queues

       // 4) Make a connection to a physical device with a logical device

       printf("Making a loigical device...\n");
       VkDevice dev1 = make_logical_device(pInstance);
       passert(dev1 != NULL, "Making logical device");


       // 5) make a pool where cmd groups will live
       printf("Making a command pool...\n");
       VkCommandPool cmd_pool = create_command_pool(dev1);
       passert(cmd_pool != NULL, "Making command pool");

       // 6) make a cmd group
       printf("Allocating a command group in the pool...\n");
       VkCommandBuffer *buffers = allocate_command_buffer(dev1, cmd_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
       passert(buffers != NULL, "Allocating command buffers");



       // 7) make resources
       printf("Creating a image asset\n");
       VkImage image = create_image(dev1, 2, (VkExtent3D){1000,1000,1});

       printf("Creating a sub-image asset\n");
       //VkImageView image_view = create_image_view(dev1, image, 2 );










       vkDestroyImage(dev1, image, NULL);

       printf("Freeing all used objects...\n");
       vkFreeCommandBuffers(dev1, cmd_pool, 1, buffers);
       free(buffers);

       vkDestroyCommandPool(dev1, cmd_pool, NULL);
       //check_err(ret, "Destroying cmd pool");
       vkDestroyDevice(dev1, NULL);
       //check_err(ret, "Destroying logical device");
       printf("Exiting program...\n");
       return 1;
}


/*
Object types used
       1) VkInstance -> application instance of vulkan
       2) VkPhysicalDevice -> physical device
       3) VkDevice -> logical device
       4) VkCommandPool -> command pool belonging to a device
       5) VkCommandBuffer -> command buffer allocated from pool


*/
