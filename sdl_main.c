#include "sdl_main.h"
#include "vulkan3.h"

static SDL_Window *window;

bool get_wind_surface_spec(VkSurfaceKHR *surface, VkInstance instance){
    return (bool) SDL_Vulkan_CreateSurface(window, instance, surface);
}
bool get_wind_extensions_sec(uint32_t *pCount, const char **pNames){
    return (bool) SDL_Vulkan_GetInstanceExtensions(window, pCount, pNames);
}

int main(int argc, char *argv[]){
    SDL_WindowFlags window_flags   = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    passert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) >= 0, "Init sdl");
	

    window = SDL_CreateWindow("SDL window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    
    uint32_t this = 100;
    const char *that[100];
    SDL_bool ret = SDL_Vulkan_GetInstanceExtensions(window, &this, that);
    if(ret == SDL_TRUE){
        printf("Worked %d", this);
    } else{
        printf("Failed %d", this);
    }
    //vulkan_run(window);
    
    SDL_Event event;
    bool done;
    while(!done){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)){
                done = true;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

}