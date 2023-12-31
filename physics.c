#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <io.h>
#include <fcntl.h>

#include "linalg.h"
#include "vulkan3.h"

typedef struct vector2_t{
       float x;
       float y;
} vector2_t;

typedef struct sample_t{
       float magnatude;
       float speed;
} sample_t;

typedef struct extent_t{
       float width;  // x
       float length; // y
       float height; // z
} extent_t;



typedef struct mesh_t{

       extent_t      plane_dimensions;
       int           n_samples_x, n_samples_y;
       int           n_total_samples;
       sample_t      *samples;
       int           line_square_size;


       // Graphics info
       Vertex_t      *vertecies;
       uint32_t      n_vertecies;
       uint32_t      *indicies;
       uint32_t      n_indicies;

       uint32_t      *line_indicies;
       uint32_t      n_line_indicies;

} mesh_t;


// Private

float f(vec2_t p);

void createMesh();

void setConstants();

void createTriangles();

void createSamples();

void createLineIndicies();


// Helpers
vector2_t sampleToPos(uint32_t xindx, uint32_t yindx);





static mesh_t mesh = {0};



void createLineIndicies(){
       if(mesh.grid_resolution < mesh.resolution.x - 1 || mesh.grid_resolution < mesh.resolution.y - 1){return;}



       mesh.n_line_indicies = 

       for(int x = 0; x < mesh.resolution.x; x += 1){
              for(int y = 0; y < mesh.resolution.y; y += 1){

                     // 2. All have same color
                     vector2_t pos = sampleToPos(x, y);
                     mesh.vertecies[x + mesh.resolution.x*y].pos = {pos.x, pos.y, mesh.samples.magnatude};
                     mesh.vertecies[x + mesh.resolution.x*y].color = {1.0, 0.1, 0.1};
              }
       }


}



vector2_t sampleToPos(uint32_t xindx, uint32_t yindx){
       return (vector2_t) {-0.5*mesh.planeSize.x + (xindx/mesh.resolution.x)*mesh.planeSize.x, \
                     -0.5*mesh.planeSize.y + (yindx/mesh.resolution.y)*mesh.planeSize.y }
}

vector2_t sampleToPos(uint32_t arridx){
       return sampleToPos(arridx % mesh.n_samples_x, floor(arridx/ mesh.n_samples_x));
}




float f(vector2_t p){
       10*exp(-1*(p.x*p.x + p.y*p.y)); // Guassian with height of 10 at origin
}


// Creates the square field of triangles on the z=0 plane
void createTriangles(){

       // 1. Every sample point is a vertex point also
       mesh.vertecies =  (Vertex_t *)calloc(sizeof(Vertex_t), mesh.n_total_samples);
       mesh.n_vertecies = n_total_samples;
       
       for(int i = 0; i < mesh.n_total_samples; i += 1){
              vector2_t pos = sampleToPos(i);
              mesh.vertecies[i].pos = {pos.x, pos.y, mesh.samples.magnatude};
              mesh.vertecies[i].color = {1.0, 0.1, 0.1};
       }

       // 3. We have 2 triangles for every square and 3 indicies for every triangle
       mesh.n_indicies = (mesh.n_samples_x - 1) * (mesh.n_samples_y - 1);
       mesh.indicies = (uint32_t *)calloc(sizeof(uint32_t), mesh.n_indicies);
       uint32_t i = 0;

       /*
       *------*-------*-------*
       |.  0  | .  1  | .   2 |
       |  .   |   .   |   .   |
       | 6  . | 7   . | 8   . |
       *------*-------*-------*
       |.  3  | .  4  | .   5 |
       |  .   |   .   |   .   |
       | 9  . | x   . | x   . |
       *------*-------*-------*
       */

       // For each vertice in the array. Starting at first x = 0, y = 0, and going to x_end -1, y_end - 1
       for(int x = 0; x < mesh.n_samples_x - 1; x += 1){
              for(int y = 0; y < mesh.n_samples_y - 1; y += 1){
                     mesh.indicies[i] = x + mesh.n_samples_x*y;
                     mesh.indicies[i + 1] = (x + 1) + mesh.n_samples_x*y;
                     mesh.indicies[i + 2] = (x + 1) + mesh.n_samples_x*(y + 1);
                     i += 3;
              }
       }
       for(int x = 0; x < mesh.n_samples_x - 1; x += 1){
              for(int y = 0; y < mesh.n_samples_y - 1; y += 1){
                     mesh.indicies[i] = x + mesh.n_samples_x*y;
                     mesh.indicies[i + 1] = x + mesh.n_samples_x*(y + 1);
                     mesh.indicies[i + 2] = (x + 1) + mesh.n_samples_x*(y + 1);
                     i += 3;
              }
       }
       if(i != mesh.n_indicies){
              printf("ERROR with indici creation");
       }

}

void createSamples(){

       mesh.samples = (sample_t *)calloc(sizeof(sample_t), mesh.n_total_samples);
       passert(mesh.samples != NULL, "Allocating space for mesh samples");

       for(int i = 0; i < mesh.n_total_samples; i += 1){
              mesh.samples[i].magnatude = f(sampleToPos(i));
              mesh.samples[i].speed = 0;
       }
}

void setConstants(){


       mesh.plane_dimensions = (extent_t){10, 10};
       mesh.n_samples_x = 100;
       mesh.n_samples_y = 100;
       mesh.n_total_samples = mesh.n_samples_x*mesh.n_samples_y;
       

       mesh.line_square_size = 1*mesh.n_samples_x/mesh.plane_dimensions.x; // samples/unit * 1
}

void createMesh(){

       setConstants();
       createSamples();
       createTriangles();

}


void killMesh(){
       if(mesh.vertecies != NULL){free(mesh.vertecies);}
       if(mesh.indicies != NULL){free(mesh.indicies);}
}