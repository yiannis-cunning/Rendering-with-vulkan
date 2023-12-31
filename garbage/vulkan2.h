
#include <vulkan\vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

static char *device_types[] = {
    "VK_PHYSICAL_DEVICE_TYPE_OTHER",
    "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_CPU"};


void print_queue_family(VkQueueFamilyProperties *queue){
       if((queue->queueFlags) & VK_QUEUE_GRAPHICS_BIT ){
              printf("Supports graphics queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_COMPUTE_BIT ){
              printf("Supports compute queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_TRANSFER_BIT ){
              printf("Supports trasfer queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_VIDEO_DECODE_BIT_KHR ){
              printf("Supports vidio decode queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_SPARSE_BINDING_BIT ){
              printf("Supports sparse binding queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_PROTECTED_BIT ){
              printf("Supports protected queues, ");
       }
       if((queue->queueFlags) & VK_QUEUE_OPTICAL_FLOW_BIT_NV ){
              printf("Supports optical flow queues, ");
       }
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
              printf("ERROR: %s", s);
              exit(1);
       }
}