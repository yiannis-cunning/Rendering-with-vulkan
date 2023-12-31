#include <stdio.h>
#include <io.h>
#include <windows.h>


int main(){
       printf("HEllo world");



       int n = 0;
       char buf;
       HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
       DWORD num_buf = 0;
       DWORD n_read;


       while(1){
              GetNumberOfConsoleInputEvents(hStdInput, &num_buf);
              // 1) Check if there is a input buffer 
              if(num_buf != 0){
                     n = _read(STD_INPUT_HANDLE, &buf, 1);
                     ReadConsole(hStdInput, &buf, 1, &n_read, NULL);
                     printf("%d\n", num_buf);
                     //if(n != 0){
                     //_write(STD_OUTPUT_HANDLE, &buf, 1);
                     //printf("\n");
              }else{
                     printf("h");
              }
       }
       return 1;

}