#include "vulkan3.h"

typedef struct protocol_t{
       const char *layers;
       const char *extensions;
       const char *phydevices;
       const char *bestPhyDevice;
} protocol_t;

static protocol_t global_protocol_names = {
       "LAYERS_ENABLED_FROM_WINDOW::",
       "EXTENSIONS_NEEDED_FROM_WINDOW::",
       "PHYSICAL_DEVICE_FOUND::",
       "BEST_PHYSICAL_DEVICE::"
};


typedef struct vulkan_output_t{
       char **extension_names;
       int num_extensions;
} vulkan_output_t;


/*
static vulkan_output_t vinfo_out = {0};
static char buffer[4096];


void readin_info(int fd){
       HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
       DWORD num_buf = 0;
       DWORD n_read = 0;

       while(WaitForSingleObject(hStdInput, 0) == WAIT_OBJECT_0){
              _read(hStdInput, buffer, 1, &n_read, NULL);
              
              result = WaitForSingleObject(hStdInput, 0);
              
       }
}*/



void print_extensions(const char **extennames, int nextens){
       printf("\n%s ", global_protocol_names.extensions);
       for(int i =0; i < nextens; i++){
              printf("%s", extennames[i]);
              if(i != nextens - 1){printf(", ");}
       }
       printf(";\n");

}

char **read_extensions(char *buffer, int n_valid, int fd){
       return NULL;
}

void print_phyDevice(char *devname){
       printf("\n%s ", global_protocol_names.phydevices);
       printf("%s;\n", devname);
}


/*
    uint32_t                         minImageCount;
    uint32_t                         maxImageCount;
    VkExtent2D                       currentExtent;
    VkExtent2D                       minImageExtent;
    VkExtent2D                       maxImageExtent;
    uint32_t                         maxImageArrayLayers;
    VkSurfaceTransformFlagsKHR       supportedTransforms;
    VkSurfaceTransformFlagBitsKHR    currentTransform;
    VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
    VkImageUsageFlags                supportedUsageFlags
*/
void print_surface_capabilities(uint32_t minImageCount, uint32_t max){

}

