#include "glfw_main.h"
#include "vulkan3.h"
#include <GLFW/glfw3.h>
#include "linalg.h"
#include <string.h>

#include "controlModes.h"

static GLFWwindow* window;
static screenProperties_t screen;

static struct pressing_t pressing = {0};


void add_chr_screen(char *str, int n_chars);

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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){


       if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z){
              if(action ==  GLFW_PRESS){
                     pressing.atoz[key - GLFW_KEY_A].pressed = 1;
                     pressing.atoz[key - GLFW_KEY_A].ticks_pressed = 0;
              } else if(action == GLFW_REPEAT){
                     pressing.atoz[key - GLFW_KEY_A].pressed = 1;
                     pressing.atoz[key - GLFW_KEY_A].ticks_pressed += 1;
              } else if(action == GLFW_RELEASE){
                     pressing.atoz[key - GLFW_KEY_A].pressed = 0;         
              }
       }

       if(key == GLFW_KEY_LEFT_SHIFT){
              if(action ==  GLFW_PRESS){
                     pressing.shift.pressed = 1;
                     pressing.shift.ticks_pressed = 0;
              } else if(action == GLFW_REPEAT){
                     pressing.shift.pressed = 1;
                     pressing.shift.ticks_pressed += 1;
              } else if(action == GLFW_RELEASE){
                     pressing.shift.pressed = 0;         
              }
       }
       if(key == GLFW_KEY_SPACE){
              if(action ==  GLFW_PRESS){
                     pressing.space.pressed = 1;
                     pressing.space.ticks_pressed = 0;
              } else if(action == GLFW_REPEAT){
                     pressing.space.pressed = 1;
                     pressing.space.ticks_pressed += 1;
              } else if(action == GLFW_RELEASE){
                     pressing.space.pressed = 0;         
              }
       }

       if(key == GLFW_KEY_BACKSPACE){
              if(action ==  GLFW_PRESS){
                     bkspc();
                     pressing.backspace.pressed = 1;
                     pressing.backspace.ticks_pressed = 0;
              } else if(action == GLFW_REPEAT){
                     pressing.backspace.pressed = 1;
                     pressing.backspace.ticks_pressed += 1;
              } else if(action == GLFW_RELEASE){
                     pressing.backspace.pressed = 0;         
              }
       }



       // GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
}


void character_callback(GLFWwindow* window, unsigned int codepoint){
       new_chr_in(codepoint);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
       pressing.xpos = xpos;
       pressing.ypos = ypos;
}


void mouselockon(){
       glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void mouselockoff(){
       glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}




int main(int argc, char *argv[]){

       //add_chr_screen("Hello World", 11);



       if (!glfwInit()){
              exit(EXIT_FAILURE);
              printf("failed to init glfw");
       }

       glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       window = glfwCreateWindow(640, 480, "Vulkan", NULL, NULL);

       glfwSetKeyCallback(window, key_callback);
       glfwSetCharCallback(window, character_callback);
       glfwSetCursorPosCallback(window, cursor_position_callback);


       vulkan_run();
       controller_init();
       
       printf("Finished starting vulkan... Entering main loop\n");
       while (!glfwWindowShouldClose(window)){
              glfwPollEvents();           // Update pressing
              tick_update(pressing);        // Update screen

              vulkan_drawFrame(get_screen());
              //printf("\rDist: %f, theta: %f, fov %f", dist, theta, screen.fov);

       }
 


       glfwDestroyWindow(window);
       glfwTerminate();
}