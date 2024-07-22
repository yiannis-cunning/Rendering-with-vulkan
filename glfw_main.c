#include "glfw_main.h"
#include "vulkan3.h"
#include <GLFW/glfw3.h>
#include "linalg.h"
#include <string.h>

static GLFWwindow* window;
static screenProperties_t screen;


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

void bkspc(){
       if(screen.n_chars != 0 && screen.chars != NULL){
              screen.chars[screen.n_chars - 1].index = 255;
              screen.n_chars = screen.n_chars -1;
       }
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

       if(action == GLFW_PRESS){
              if(key == GLFW_KEY_BACKSPACE){
                     bkspc();
              } else {
                     char buf[1];
                     buf[0] = (char) key;
                     add_chr_screen(buf, 1);
              }
       }

       float offset[3] = {10, 10, zvalue};
       offset[0] = sqrt(200)*cos(theta);
       offset[1] = sqrt(200)*sin(theta);
       cpyVec(offset, screen.offset);
       constMult(-1, offset, screen.view);
}



void add_chr_screen(char *str, int n_chars){
       if(screen.chars == NULL || screen.chars_sz <= screen.n_chars + n_chars){
              printf("Resizeing %d, %d\n\n", screen.n_chars, n_chars);
              charcter_t *old = screen.chars;
              screen.chars = (charcter_t *)calloc((n_chars + screen.chars_sz )*2, sizeof(charcter_t));
              screen.chars_sz = (n_chars + screen.chars_sz )*2;

              if(old != NULL) {
                     memcpy(screen.chars, old, sizeof(charcter_t)*screen.n_chars);
                     free(old);
              }

       }
       for(int i = screen.n_chars; i <  screen.n_chars + n_chars ; i += 1){
              screen.chars[i].pos[0] = i;
              screen.chars[i].pos[1] = 0;

              if(str[i - screen.n_chars] >= 'a' && str[i - screen.n_chars] <= 'z'){
                     screen.chars[i].index = str[i - screen.n_chars] - 'a' + 65;
              } else if(str[i - screen.n_chars] >= 'A' && str[i - screen.n_chars] <= 'Z'){
                     screen.chars[i].index = str[i - screen.n_chars] - 'A' + 33;
              } else{
                     screen.chars[i].index = 10;
              }
       }
       screen.chars[screen.n_chars + n_chars].index = 255;
       screen.n_chars = screen.n_chars + n_chars;

}


int main(int argc, char *argv[]){
       setvector_t(screen.offset, 10, 10, 10);
       setvector_t(screen.view, -1, -1, -1);
       screen.fov = 45;
       screen.clipDist = 1;
       screen.renderDist = 100;
       add_chr_screen("Hello World", 11);
       //printf("a: %d vs %d", 'A', GLFW_KEY_A);
       //exit(1);




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
              glfwPollEvents();    // Update pressing
                                   // Update screen
              vulkan_drawFrame(screen);
              printf("\rDist: %f, theta: %f, fov %f", dist, theta, screen.fov);

       }
 


       glfwDestroyWindow(window);
       glfwTerminate();
}