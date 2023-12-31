#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include "protocol.h"



static GLFWwindow* window;
ImGuiIO *io; 




// Receiver thread
static HANDLE receiver_tptr;
static uint32_t receiver_tid;

DWORD WINAPI receiver_loop(LPVOID param){


}



static int init(){
    /*
    Make reader thread
        - reads from fd and signals when there is new data updated to the protocol struct
    */

     receiver_tptr = CreateThread( 
          NULL,                    // default security attributes
          0,                       // use default stack size  
          receiver_loop,             // thread function name
          NULL,            // argument to thread function 
          0,                       // use default creation flags 
          &receiver_tid);
     passert(receiver_tptr != NULL, "GLFW_MASTER:: making reciever thread");



    if (!glfwInit())
        return 1;


    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "Vulkan controller", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return 0;

}

static void kill(){
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}


int main(int argc, char**argv){
    
    
    if(init() != 0){return 1;}


    // Our state
    bool show_demo_window = true;
    bool isSDL = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        readin_info(0);
        // 1) Init iformation
        ImGui::Begin("Vulkan information");
        ImGui::Text("Layers enabled by Vulkan front end (%d): ", vinfo_out.num_extensions);
        for(int i = 0; i < vinfo_out.num_extensions; i++ ){
            ImGui::Text("%s", vinfo_out.extension_names[i]);
        }

        ImGui::End();



            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Backend");                      // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &isSDL);      // Edit bools storing our window open/close state
            /*ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            */
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
            ImGui::End();

        // 3. Show another simple window.
        if (true)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    kill();

    return 0;

}