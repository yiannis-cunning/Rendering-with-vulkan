


// device_types[VkPhysicalDeviceType]
static char *device_types[] = {
    "VK_PHYSICAL_DEVICE_TYPE_OTHER",
    "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU",
    "VK_PHYSICAL_DEVICE_TYPE_CPU"};


void print_device_groups(VkInstance pInstance);

void print_physical_devices(VkInstance pInstance);

VkDevice make_logical_device(VkInstance pInstance);

VkCommandPool create_command_pool(VkDevice dev1);

VkCommandBuffer *allocate_command_buffer(VkDevice device, VkCommandPool pool, uint32_t n_buffers, VkCommandBufferLevel level);

void start_buffer_recording(VkCommandBuffer buffer);



// VkRect2D = {x, y}, {w, h}