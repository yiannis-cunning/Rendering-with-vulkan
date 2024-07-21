
gcc_path = C:\Users\yiann\Desktop\installing_mingw_64-master\mingw64\bin\gcc.exe
glslc_path = C:\VulkanSDK\1.3.268.0\Bin\glslc.exe

LIB_GLFW = -L C:\clibs\GLFW\lib-mingw-w64 -lglfw3 -lgdi32
INCL_GLFW = -I C:\clibs\GLFW\include

INCL_STB = -I src\

LIB_VULKAN =  -L C:\VulkanSDK\1.3.268.0\Lib -lvulkan-1
INCL_VULKAN = -I C:\VulkanSDK\1.3.268.0\Include

LINK_OPTS = -L C:\VulkanSDK\1.3.268.0\Lib -lvulkan-1 -L C:\clibs\SDL2\lib\x64 -lSDL2
INCL_OPTS = -I C:\VulkanSDK\1.3.268.0\Include -I C:\clibs\SDL2\Include


IMGUI_INCL = -I C:\clibs\imgui -I C:\clibs\imgui\backends
IMGUI_C_DIR = C:\clibs\imgui

IMGUI_CFILS = ${IMGUI_C_DIR}\imgui.cpp ${IMGUI_C_DIR}\imgui_draw.cpp ${IMGUI_C_DIR}\imgui_tables.cpp ${IMGUI_C_DIR}\imgui_widgets.cpp ${IMGUI_C_DIR}\imgui_demo.cpp

LINALG_INCL = -I C:\clibs\math
LINALG_C = C:\clibs\math\linalg.c


LINK_OPTS_STD = -L C:\Users\yiann\Desktop\installing_mingw_64-master\mingw64\bin -lstdc++ -lopengl32

glfw_wind: glfw_main.c vulkan3.c ${LINALG_C}
	${gcc_path} $^ ${LIB_GLFW} ${INCL_GLFW} ${LIB_VULKAN} ${INCL_VULKAN} ${LINALG_INCL} ${INCL_STB} -o glfw_wind.exe



sdl_wind: sdl_main.c vulkan3.c
	${gcc_path} $^  ${LINK_OPTS} ${INCL_OPTS} -o wind.exe



run2: vulkan2.c
	${gcc_path} $^ ${LINK_OPTS} ${INCL_OPTS} -o run

run: testing_vulkan.c
	${gcc_path} $^ ${LINK_OPTS} ${INCL_OPTS} -o run

# ${IMGUI_C_DIR}\backends\imgui_impl_glfw.cpp ${IMGUI_C_DIR}\backends\imgui_impl_opengl3.cpp
glfw_master: glfw_master.cpp ${IMGUI_C_DIR}\backends\imgui_impl_glfw.cpp ${IMGUI_C_DIR}\backends\imgui_impl_opengl3.cpp
	${gcc_path} $^ ${IMGUI_CFILS} ${LIB_GLFW} ${INCL_GLFW} ${IMGUI_INCL} ${LINK_OPTS_STD} -o glfw_master.exe

shaders: *
	cd shaders && $(MAKE)