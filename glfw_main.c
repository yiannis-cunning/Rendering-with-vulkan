#include "glfw_main.h"
#include "vulkan3.h"
#include <GLFW/glfw3.h>
#include "linalg.h"


static GLFWwindow* window;
static screenProperties_t screen;

VkResult get_wind_surface_spec(VkSurfaceKHR *surface, VkInstance instance){
       return glfwCreateWindowSurface(instance, window, NULL, surface);
}
const char ** get_wind_extensions_sec(uint32_t *pCount){
    return glfwGetRequiredInstanceExtensions(pCount);

}
void get_wind_dims(int *width, int *height){
       glfwGetFramebufferSize(window, width, height);
       while (width == 0 || height == 0) {
              glfwGetFramebufferSize(window, width, height);
              glfwWaitEvents();
       }
}

static double prevtime = 0; // Return the delta between function calls of this
float get_delta_frame_time(){
       double currentTime = glfwGetTime();
       if(prevtime == 0){
              prevtime = currentTime;
              return 0;
       }
       double out = currentTime - prevtime;
       prevtime = currentTime;
       return out;
       
}


static float dist = 8.66;
static float dirc[3] = {1, 1, 1};
static float theta = 45*3.14159*2/360;
static float zvalue = 10;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
       // GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
       switch(key){
              case GLFW_KEY_I:
                     screen.fov += 2;
                     break;
              case GLFW_KEY_O:
                     screen.fov -= 2;
                     break;
              case GLFW_KEY_SPACE:
                     theta += 0.01;
                     break;
              case GLFW_KEY_LEFT_SHIFT:
                     theta -= 0.01;
                     break;
              case GLFW_KEY_W:
                     zvalue += 0.1;
                     break;
              case GLFW_KEY_S:
                     zvalue -= 0.1;
                     break;
       }
       float offset[3] = {10, 10, zvalue};
       offset[0] = sqrt(200)*cos(theta);
       offset[1] = sqrt(200)*sin(theta);
       cpyVec(offset, screen.offset);
       constMult(-1, offset, screen.view);
}



int main(int argc, char *argv[]){
       setvector_t(screen.offset, 10, 10, 10);
       setvector_t(screen.view, -1, -1, -1);
       screen.fov = 45;
       screen.clipDist = 1;
       screen.renderDist = 100;
       
       screen.char_grid_dims[0] = 10;
       screen.char_grid_dims[1] = 10;
       screen.n_characters = 10;
       screen.characters = (charcter_t *) calloc(sizeof(charcter_t), 10);
       screen.characters[0] = (charcter_t){'a', {0, 0}};
       screen.characters[1] = (charcter_t){'b', {0, 1}};
       screen.characters[2] = (charcter_t){'c', {0, 2}};



       if (!glfwInit()){
              exit(EXIT_FAILURE);
              printf("failed to init glfw");
       }

       glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       window = glfwCreateWindow(640, 480, "Vulkan", NULL, NULL);
       glfwSetKeyCallback(window, key_callback);

       vulkan_run();
       
       printf("Finished starting vulkan... Entering main loop\n");
       while (!glfwWindowShouldClose(window)){
              glfwPollEvents();
              vulkan_drawFrame(screen);
              printf("\rDist: %f, theta: %f, fov %f", dist, theta, screen.fov);

       }
 


       glfwDestroyWindow(window);
       glfwTerminate();
}