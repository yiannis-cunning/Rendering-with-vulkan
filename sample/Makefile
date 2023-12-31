



gcc_path = C:\Users\yiann\Desktop\installing_mingw_64-master\mingw64\bin\gcc.exe

LIB_GLFW = -L C:\clibs\glfw-3.3.8.bin.WIN64\lib-mingw-w64 -lglfw3 -lgdi32
INCL_GLFW = -I C:\clibs\glfw-3.3.8.bin.WIN64\include

LIB_VULKAN =  -L C:\VulkanSDK\1.3.268.0\Lib -lvulkan-1
INCL_VULKAN = -I C:\VulkanSDK\1.3.268.0\Include

INCL_GLM = -I C:\clibs\g-truc-glm-bf71a83

STB_INCL = -I C:\clibs\STB
TOL_INCL = -I C:\clibs\tinyobjloader-tinyobjloader-de948f1
TOL_CFILE = C:\clibs\tinyobjloader-tinyobjloader-de948f1\tiny_obj_loader.cc

LINK_OPTS = -L C:\Users\yiann\Desktop\installing_mingw_64-master\mingw64\bin -lstdc++


sample: main.cpp
	${gcc_path} $^ ${LINK_OPTS} ${LIB_GLFW} ${INCL_GLFW} ${LIB_VULKAN} ${INCL_VULKAN} ${INCL_GLM} ${STB_INCL} ${TOL_INCL}  -o sample.exe


glslc_path = C:\VulkanSDK\1.3.268.0\Bin\glslc.exe

all_shader: frag_shader vert_shader

frag_shader: shader_base.frag
	${glslc_path} $^ -o frag_base.spv
 
vert_shader: shader_base.vert
	${glslc_path} $^ -o vert_base.spv