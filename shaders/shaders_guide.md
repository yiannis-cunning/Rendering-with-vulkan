

# Shaders
       - begin at the 'main' function
       - process input variables
       - output results

# Data types
       - int, float, double, uint, bool

# vectors and matricies (n = 2, 3, 4 - x, y, z, w)
       - vecn - defualt vector of n floats
       - bvecn - n bools
       - ivecn - n ints
       - uvecn - n uints
       - dvecn - n doublts

       vec3 (float) name = othervec.xxz;
       uvec2 name2 = other.zzz + other.yyy;

# inputs and outputs and layouts
       - in = input
       - out = output
       - layouts:location will specify how inputs and outputs are attached
              - You are laying out the inputs/outputs
       - layout:bindings are similar - bind to other static resoursce
              - descriptor set can be specified with set=x
              - descriptor binding can be choosen with binding=y
              - descriptor sets will have to be bound to the graphics pipeline/renderer and during the draw commandss


<- example of vertex shader signature ->
layout(location = 0) in vec3 inPosition // A vertex can be defined in many ways. Here its defined as inputting vec3 position (x,y,z) and col (rgb)
layout(location = 1) in vec3 in Color   //       - color is by POINT not triangle -> make it much better/simple !
//                                          // BIG ARGUENTS WILL USE MULTIPLE LAYOUT SLOTS




### Vertex shader matrixs
       1. model[4x4]        -> allowes for all sorts of (resize, rotate, and translation)  
       2. view[4x4]         -> put the cords in camera space (subtract the offset and rotate for view)
       3. projection[4x4]   -> (the hard one)

### PRE DEFINED GRAPHICS PIPELINE ###
# Input Assemblers
       - Done by Vulkan
# Vertex Shaders
       - Ran for every input vertex
       - part of vkcmddraw - choose vertex count fist vertex
       - Input of 
              - space position
              - normal and texture coordanates
       - Output of 
              - clip coordanates
                     - 4D vector - turned into screen coordante by dividing by last component
                     - belongs to [-1, 1], [-1, 1] and [0,1]
                     - (by defualt defined output "gl_Position")
              - attributes - like color and texture cords

# Tessellation Shaders

# Geometery Shaders

# Rasterization Shaders
       - Done by vulkan
       - Takes the output of previous cycles... sets up the fragment shader to be run.
              - might extrapolate the input to the fragment shader 
# Fragment Shaders

# Color blending
       - Done by Vulkan
       - How are different fragements blended together
       - goes directly to the framebuffer
       - Consider multiple triangles blended with eachother after fragmentation.







### VULKAN NOTES
vertex buffer can be registered in terms of type (VkBuffer), allocated in terms of space (VkMemoryAllocate) and allocation can then be bound to the buffer
       - Usually you want to allocate memory only accessable to GPU - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag
       - Do this with a staging buffer
              - First allocate to a buffer on the CPU
              - then Final step is to copy to GPU

       - In order to copy a buffer you need a queue with VK_QUEUE_TRANSFER_BIT