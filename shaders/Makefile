

glslc_path = C:\VulkanSDK\1.3.268.0\Bin\glslc.exe

all_shader: frag_shader vert_shader compute_shader

frag_shader: shader_base.frag
	${glslc_path} $^ -o frag.spv
 
vert_shader: shader_base.vert
	${glslc_path} $^ -o vert.spv

compute_shader: compute.comp
	${glslc_path} $^ -o compute.spv