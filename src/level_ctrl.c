#include "level_ctrl.h"



struct level_t{

       int ntextures;
       char **texture_files;

       int nobjects; // groups of mesh+tex combinations
       struct object_t{
              int nmeshes;
              struct mesh_t{
                     int texture_id;
                     int nverticies;
                     Vertex_tex_t vertex_arr[];
              } *mesh_arr;

       } *object_arr;


       int ninstances; // ojects, pos list
       struct instance_t{
              int object_id;
              vector4_t pos;
       } *instance_arr;
};

void free_mesh_arr(struct mesh_t *mesh_arr, int nmeshes){
       if(mesh_arr == NULL){
              return;
       }

       for(int i=0; i<nmeshes; i+=1){
              if(mesh_arr[i].vertex_arr != NULL){
                     free(mesh_arr[i].vertex_arr);
              }
       }
       free(mesh_arr);
}

void free_object_arr(struct object_t *obj_arr, int nobjects){
       if(obj_arr == NULL){
              return;
       }

       for(int i=0; i<nobjects; i +=1){
              free_mesh_arr(obj_arr[i].mesh_arr, obj_arr[i].nmeshes);
       }
}

void free_str_arr(char **str_arr, int nstrings){
       if(str_arr == NULL){
              return;
       }
       for(int i=0; i<nstrings; i += 1){
              if(str_arr[i] !=NULL){
                     free(str_arr[i]);
              }
       }
       free(str_arr);
}


void free_level(struct level_t *level){

       free_str_arr(level->texture_files, level->ntextures);

       free_object_arr(level->object_arr, level->nobjects);

       if(level->instance_arr != NULL){
              free(level->instance_arr);
       }

}



/*
// buffer[0] is at least not \0
int atoi_own(char *buffer, int *result){
       //-.-
       //-XXXXXX-
       //-dddc-
       //-
       int ibuf;
       bool hit = 0;
       int number = 0;
       bool negative = 0;

       while(buffer[ibuf] != '\0'){
              printf("atoi%c\n", buffer[ibuf]);
              if(( buffer[ibuf] >= '0' && buffer[ibuf] <= '9' ) || (buffer[ibuf] == '-' && hit == 0)){
                     printf("atoi%c\n", buffer[ibuf]);
                     if(buffer[ibuf] == '-'){ negative = 1;}
                     else{number = number*10 + buffer[ibuf] - '0';}
                     hit = 1;
                     ibuf += 1;
              }else{
                     break;
              }
       }
       printf("hit: %d, ibuf %d\n", hit, ibuf);
       if(hit == 0){return -1;}
       if(negative){number = number * -1;}
       *result = number;
       return ibuf;
}

// Matching buffer to pattern. newline ends the comarrison
/*
 (.) is any int
 (*) is any float
 (\n, space, tab) are all whitespace and will be skipped
*//*
int line_format_ext(char *buffer, char *pattern, float *floats, int *ints){
       int ibuf = 0;
       int ipat = 0;
       int iints = 0;

       int ret;

       while(buffer[ibuf] != '\0' && pattern[ipat] != '\0'){
              printf("%c == %c\n", buffer[ibuf], pattern[ipat] );
              //-.-
              //-XXXXXX-
              if(pattern[ipat] == '.'){
                     ret = atoi_own(buffer + ibuf, ints + iints);
                     printf("retur %d\n", ret);
                     if(ret == -1){printf("Bad match\n");return -1;}
                     ibuf += ret;
                     ipat += 1;
              }
              else if(pattern[ipat] != buffer[ibuf]){
                     printf("%c != %c\n", buffer[ibuf], pattern[ipat] );
                     return -1;
              }
              else{
                     ipat += 1;
                     ibuf += 1;
              }

       }
       if(pattern[ipat] != '\0'){
              printf("Pattern bad\n");
              return -1;
       }
       return ibuf;

}

// Get rid of '\t's, ' 's, comments
void pre_process(char *buffer){
       int ibuf = 0;
       int inew = 0;

       while(buffer[ibuf] != '\0'){
              if(buffer[ibuf] == ' ' || buffer[ibuf] == '\t'){
                     ibuf += 1;
              }

              if(buffer[ibuf] == '/' && buffer[ibuf + 1] != '\0' && buffer[ibuf + 1] == '/'){
                     while(buffer[ibuf] != '\n'){
                            ibuf += 1;
                     }
              }


              buffer[inew] = buffer[ibuf];
              inew += 1;
              ibuf += 1;
       }
       buffer[inew] = buffer[ibuf];
}


static soft_assert(bool condition, char *message)*/
// Helper functions
const char *token_type_string(mu_json_token_t *t) {
    switch (mu_json_token_type(t)) {
    case MU_JSON_TOKEN_TYPE_UNKNOWN: return "UNKNOWN";
    case MU_JSON_TOKEN_TYPE_ARRAY: return "ARRAY";
    case MU_JSON_TOKEN_TYPE_OBJECT: return "OBJECT";
    case MU_JSON_TOKEN_TYPE_STRING: return "STRING";
    case MU_JSON_TOKEN_TYPE_NUMBER: return "NUMBER";
    case MU_JSON_TOKEN_TYPE_INTEGER: return "INTEGER";
    case MU_JSON_TOKEN_TYPE_TRUE: return "TRUE";
    case MU_JSON_TOKEN_TYPE_FALSE: return "FALSE";
    case MU_JSON_TOKEN_TYPE_NULL: return "NULL";
    }
}

void print_token_slice(mu_json_token_t *t) {
    mu_str_t *s = mu_json_token_slice(t);
    printf("'%.*s'\n", (int)mu_str_length(s), mu_str_buf(s));
}


int load_level_ext(char *filename){
       
       // 1) Load .json file
       printf("Loading new level...\n");
       uint32_t sz_file;
       char *fbuffer = (char *)load_file(filename, &sz_file, _O_TEXT);
       char *file_base = fbuffer;
       struct level_t level = {0};

       // 2) Parse as a json
       int n_tokens_alloc = 100;
       int n_tokens_ret = 0;
       const char *json = fbuffer;
       mu_json_token_t *tokens = NULL;

       
       tokens = (mu_json_token_t *)calloc(sizeof(mu_json_token_t), n_tokens_alloc);
       n_tokens_ret = mu_json_parse_c_str(tokens, n_tokens_alloc, json, NULL);
       while(n_tokens_ret < 0){
              if(n_tokens_ret != MU_JSON_ERR_NO_TOKENS){
                     printf("Bad Json file inputted!\n");
                     free(tokens);
                     return -1;
              } else if(n_tokens_alloc > 6400){
                     printf("Json file is too big!\n");
                     free(tokens);
                     return -1;
              }
              n_tokens_alloc = n_tokens_alloc*2;
              tokens = (mu_json_token_t *)calloc(sizeof(mu_json_token_t), n_tokens_alloc);
              n_tokens_ret = mu_json_parse_c_str(tokens, n_tokens_alloc, json, NULL);
       }
       /*
       printf("Json file parsed with %d tokens.\n", n_tokens_ret);
       for (int i = 0; i < n_tokens_ret; i++) {
              mu_json_token_t *t = &tokens[i];
              printf("token[%d]: %7s depth=%d, ", i, token_type_string(t),
                     mu_json_token_depth(t));
              print_token_slice(t);
       }*/

       // get n_textures
       strcmpn()
       
       print_token_slice(tokens + 2);
       // 3) save textures
       if(!mu_json_token_is_first(tokens) || !mu_json_token_is_last(tokens)){
              printf("There is more than one base token\n");
              free(tokens);
              return -1;
       }       


       /*
       pre_process(fbuffer);
       printf("Loading level... \n");

       int ints[4];
       float floats[4];       
       int delta;

       delta = line_format_ext(fbuffer, "ntextures=.\n", floats, ints);
       if(delta == -1){printf("Error parsing %s.", filename);return -1;}
       fbuffer += delta;
       level.ntextures = ints[0];
       printf("%d textures found.\n", ints[0]);

       for(int i=0; i<level.ntextures; i += 1){
              delta = line_format_ext(fbuffer, ".:", floats, ints);
              if(delta == -1){printf("Error parsing %s.", filename);return -1;}
       }*/

       printf("Done loading level...\n");
}