#include "controlModes.h"


#define PI_LOCAL 3.14159256

static control_mode_enum cur_state = CONTROL_FLY_MODE;

static screenProperties_t screen = {0};

static struct pressing_t pressing_local = {0};

/*
x down
y right
angel go from x to y...

*/

typedef struct state_data_t{
       // theta belongs to 0 - 2pi, r belogs to greater than equal 0, phi belogs 0-pi
       float theta;
       float r;
       float phi;

       double xpos_prev;
       double ypos_prev;
       bool good;


} state_data_t;

static state_data_t state_data = {0};




static void tickupdate_fly_mode(struct pressing_t pressing);

static void tickupdate_xyz_mode(struct pressing_t pressing);

static void view_to_rthetaphi();

static void rthetaphi_to_view();


// Text functions
void add_chr_screen(char *str, int n_chars);


void bkspc(){
       if(screen.n_chars != 0){
              screen.n_chars -= 1;
       }
}


static void pview(){
       printf("\nx: %f, y: %f, z: %f\n", screen.view[0], screen.view[1], screen.view[2]);
}

static void prthetaphi(){
       printf("\n r: %f, theta: %f, phi: %f\n", state_data.r, state_data.theta, state_data.phi);
       
}



screenProperties_t get_screen(){
       return screen;
}


void tick_update(struct pressing_t pressing){
       if(pressing.atoz['p' - 'a'].pressed && !pressing_local.atoz['p' - 'a'].pressed){
              state_data.good = 0;
              if(cur_state == CONTROL_FLY_MODE){
                     cur_state = PAUSE_MODE;
                     mouselockoff();
              } else{
                     cur_state = CONTROL_FLY_MODE;
                     mouselockon();
              }
       }
       pressing_local.atoz['p' - 'a'].pressed = pressing.atoz['p' - 'a'].pressed;

       switch(cur_state){
              case CONTROL_FLY_MODE:
                     tickupdate_fly_mode(pressing);
                     break;
              case CONTROL_XYZ_MODE:
                     tickupdate_xyz_mode(pressing);
                     break;
              case PAUSE_MODE:
                     //tickupdate_text_mode(pressing);
                     break;
              default:
                     break;
       }
}


static void tickupdate_fly_mode(struct pressing_t pressing){
       float temp[3] = {0, 0, 0};
       float zaxis[3] = {0, 0, 1};
       float xaxis[3] = {1, 0, 0};
       float yaxis[3] = {0, 1, 0};

       float inc = 0.1;

       int s = pressing.atoz['s' - 'a'].pressed;
       int w = pressing.atoz['w' - 'a'].pressed;
       int a = pressing.atoz['a' - 'a'].pressed;
       int d = pressing.atoz['d' - 'a'].pressed;
       int space = pressing.space.pressed;
       int shift = pressing.shift.pressed;


       if(s || w ){
              cpyVec(screen.view, temp);
              normalize(temp);
              constMult((inc)*(w-s), temp, temp);
              addVec(temp, screen.offset, screen.offset);
       }
       if(d || a){
              cross(zaxis, screen.view, temp);
              normalize(temp);
              constMult((inc)*(a-d), temp, temp);
              addVec(temp, screen.offset, screen.offset);
       }
       if(space || shift){
              cpyVec(zaxis, temp);
              normalize(temp);
              constMult((inc)*(space-shift), temp, temp);
              addVec(temp, screen.offset, screen.offset);
       }


       //printf("dxmov %f, dymov %f", pressing.xpos, pressing.ypos);
       if(state_data.good){
              view_to_rthetaphi();
              state_data.theta += -1*(pressing.xpos - state_data.xpos_prev)*0.001;
              state_data.phi += (pressing.ypos - state_data.ypos_prev)*0.001;

              if(state_data.theta < 0){state_data.theta += 2*PI_LOCAL;}
              if(state_data.theta > 2*PI_LOCAL){state_data.theta -= 2*PI_LOCAL;}

              if(state_data.phi < 0){state_data.phi = state_data.phi*-1;}
              if(state_data.phi > PI_LOCAL){state_data.phi = 2*PI_LOCAL - state_data.phi;}
              rthetaphi_to_view();
       }
       
       state_data.xpos_prev = pressing.xpos;
       state_data.ypos_prev = pressing.ypos;
       state_data.good = 1;


       
       //printf("\nON: %d, ticks: %d", pressing.atoz['j' - 'a'].pressed, pressing.atoz['j' - 'a'].ticks_pressed);   
       if(pressing.atoz['j' - 'a'].pressed && !pressing_local.atoz['j' - 'a'].pressed) {
              screen.fov += 0.1;
       }
       pressing_local.atoz['j' - 'a'].pressed = pressing.atoz['j' - 'a'].pressed;

       if(pressing.atoz['k' - 'a'].pressed && !pressing_local.atoz['k' - 'a'].pressed) {
              screen.fov -= 0.1;
       }
       pressing_local.atoz['k' - 'a'].pressed = pressing.atoz['k' - 'a'].pressed;

       

}

static void tickupdate_xyz_mode(struct pressing_t pressing){
       
}


static void rthetaphi_to_view(){
       //printf("%f\n", state_data.r*cos(state_data.phi));
       setvector_t(screen.view, state_data.r*sin(state_data.phi)*cos(state_data.theta), \
                            state_data.r*sin(state_data.phi)*sin(state_data.theta), \
                            state_data.r*cos(state_data.phi));
}
static void view_to_rthetaphi(){
       state_data.r = vecMag(screen.view);



       if(screen.view[0] == 0.0f){state_data.theta = (screen.view[1] > 0)*PI_LOCAL/2 + 3*(screen.view[1] <= 0)*PI_LOCAL/2;}
       else if(screen.view[1] == 0.0f){
              state_data.theta = (screen.view[0] <= 0)*PI_LOCAL;
       }else if(screen.view[0] > 0){
              state_data.theta = atan(screen.view[1]/screen.view[0]);
       }else{
              state_data.theta = atan(screen.view[1]/screen.view[0]) + PI_LOCAL;
       }

       if(state_data.theta < 0){state_data.theta = state_data.theta + 2*PI_LOCAL;}
       float magbase = sqrt(screen.view[0]*screen.view[0] + screen.view[1]*screen.view[1]);

       if(screen.view[2] == 0.0f){state_data.phi = PI_LOCAL*2;}
       else if(magbase == 0.0f){state_data.phi = (screen.view[2] < 0)*PI_LOCAL; }
       else{state_data.phi = atan(magbase/screen.view[2]);}

       if(state_data.phi < 0){state_data.phi = state_data.phi + PI_LOCAL; }
}



void controller_init(){
       mouselockon();
       setvector_t(screen.offset, 10, 10, 10);
       setvector_t(screen.view, -1, -1, -1);

       view_to_rthetaphi();
       rthetaphi_to_view();


       screen.fov = 45;
       screen.clipDist = 1;
       screen.renderDist = 100;


       // Text buffer
       screen.alloc_size = 256;
       screen.text_buffer = (char *)calloc(sizeof(char), 256);
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

void new_chr_in(unsigned int char_i){
       if(screen.alloc_size > screen.n_chars && cur_state == PAUSE_MODE ){
              screen.text_buffer[screen.n_chars] = (char)char_i;
              screen.n_chars += 1;
       }
}


#ifdef THISTHAT
controller2::controller2(float *view_init, float *offset_init){

       cpyVec(view_init, view);
       cpyVec(offset_init, offset);

       float v[3];
       subVec(view, offset, v);


       mode = CONTROL_FLY_MODE;
       phi = 1.593269;
       theta = 1.278754;
       r = vecMag(view_init);

       inc = 0.5;
       incang = 2*3.14159256*3/500;
}


bool controller2::tick_update(){
       float temp[3], temp2[3];
       float zaxis[3] = {0, 0, 1};
       float xaxis[3] = {1, 0, 0};
       float yaxis[3] = {0, 1, 0};
       bool changed;

       if(press.a == 1 || press.d == 1){
              if(mode == CONTROL_FLY_MODE){
                     cross(zaxis, view, temp);
                     normalize(temp);
                     constMult((inc/(press.j == 1 ? 0.1 : 1))*(press.d-press.a), temp, temp);
              }else{
                     constMult((inc/(press.j == 1 ? 0.1 : 1))*(press.a-press.d), xaxis, temp);
              }
              addVec(temp, offset, offset);
              changed = true;
       }
       if(press.s == 1 || press.w == 1){
              if(mode == CONTROL_FLY_MODE){
                     cpyVec(view, temp);
                     normalize(temp);
                     constMult((inc/(press.j == 1 ? 0.1 : 1))*(press.w-press.s), temp, temp);
              }else {
                     constMult(inc/(press.j == 1 ? 0.1 : 1)*(press.w-press.s), yaxis, temp);
              }
              addVec(temp, offset, offset);
              changed = true;
       }
       if(press.dxmov != 0 || press.dymov != 0){
              phi += incang*(press.dymov/100);
              phi = (phi < 3.14159256-incang) ? phi : phi - incang;
              phi = (phi > incang) ? phi : phi + incang;
              theta += incang*(press.dxmov/100);
              press.dxmov = 0;
              press.dymov = 0;
                            /*phi += incang*(press.i - press.k);
                            phi = (phi < pi-incang) ? phi : phi - incang;
                            phi = (phi > incang) ? phi : phi + incang;
                            theta += incang*(press.l - press.j);*/
              changed = true;
       }
       if(press.space == 1 || press.shift == 1){
              constMult((press.space - press.shift)*inc/(press.j == 1 ? 10 : 1), zaxis, temp);
              addVec(temp, offset, offset);
              changed = true;
       }
       
       setvector_t(view, r*cos(theta)*sin(phi), r*sin(phi)*sin(theta), r*cos(phi));

       return changed;
}



void controller2::rotate_mode(){
       if(mode == CONTROL_FLY_MODE){
              mode = CONTROL_XYZ_MODE;
       } else if(mode == CONTROL_XYZ_MODE){
              mode = CONTROL_FLY_MODE;
       }
}












// Set the data to initial position.
controller::controller(float *p_0, float *v_0)
{
       fly_data = nullptr;
       jmp_data = nullptr;
       mode = 1;

       if(mode == 1){jmp_data = new jumpMode(p_0, v_0, v, offset);}
       else{fly_data = new flyMode(p_0, v_0, v, offset);}

       send_position();
}

void controller::send_position()
{
       if(mode == 1){jmp_data->send_position();}
       else{fly_data->send_position();}
}

// Update the position based on the keys being pressed right now
bool controller::tick_update()
{
       if(mode == 1){return jmp_data->tick_update(&press);}
       else{return fly_data->tick_update(&press);}
}


jumpMode::jumpMode(float *p_0, float *v_0, float *v_in, float *offset_in){
       phi = 2.1862760354653;
       theta = -1*3*3.14159256/4;
       r = 10*sqrt(3);
       inc = 5; // 25
       incang = 2*3.14159256*3/500;
       velup = 0;
       tLast = 0;
       cpyVec(p_0, pos);
       cpyVec(v_0, view);
       v = v_in;
       offset = offset_in;
}

void jumpMode::send_position(){
       setvector_t(view, r*cos(theta)*sin(phi), r*sin(phi)*sin(theta), r*cos(phi));
       addVec(view, pos, offset);
       constMult(-1, view, v);
}

bool jumpMode::tick_update(struct pressing *press){
       float temp[3], temp2[3];
       float zaxis[3] = {0, 0, 1};
       float xaxis[3] = {1, 0, 0};
       float yaxis[3] = {0, 1, 0};
       bool changed;
       if(press->a == 1 || press->d == 1){
              cpyVec(view, temp);
              temp[2] = 0;
              cross(temp, zaxis, temp2);
              normalize(temp2);
              constMult((inc/(press->j == 1 ? 10 : 1))*(press->a-press->d), temp2, temp);
              constMult((inc/(press->j == 1 ? 10 : 1))*(press->a-press->d), xaxis, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       if(press->s == 1 || press->w == 1){
              cpyVec(view, temp);
              temp[2] = 0;
              normalize(temp);
              constMult(inc/(press->j == 1 ? 10 : 1)*(press->w-press->s), temp, temp);
              constMult(inc/(press->j == 1 ? 10 : 1)*(press->w-press->s), yaxis, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       if(press->dxmov != 0 || press->dymov != 0){
              phi += incang*(press->dymov/100);
              phi = (phi < 3.14159256-incang) ? phi : phi - incang;
              phi = (phi > incang) ? phi : phi + incang;
              theta += incang*(press->dxmov/100);
              press->dxmov = 0;
              press->dymov = 0;
                            /*phi += incang*(press->i - press->k);
                            phi = (phi < pi-incang) ? phi : phi - incang;
                            phi = (phi > incang) ? phi : phi + incang;
                            theta += incang*(press->l - press->j);*/
              changed = true;
       }
       if(press->space == 1 || press->shift == 1){
              constMult((press->space - press->shift)*inc/(press->j == 1 ? 10 : 1), zaxis, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       

       send_position();
       return changed;
}

flyMode::flyMode(float *p_0, float *v_0, float *v_in, float *offset_in){
       phi = 2.1862760354653;
       theta = -1*3*3.14159256/4;
       r = 10*sqrt(3);
       inc = 100;
       incang = 2*3.14159256*3/500;
       cpyVec(p_0, pos);
       cpyVec(v_0, view);
       v = v_in;
       offset = offset_in;
}

void flyMode::send_position(){
       setvector_t(view, r*cos(theta)*sin(phi), r*sin(phi)*sin(theta), r*cos(phi));
       addVec(view, pos, offset);
       constMult(-1, view, v);
}

bool flyMode::tick_update(struct pressing *press){
       float temp[3], temp2[3];
       float zaxis[3] = {0, 0, 1};
       bool changed;
       if(press->a == 1 || press->d == 1){
              cpyVec(view, temp);
              temp[2] = 0;
              cross(temp, zaxis, temp2);
              normalize(temp2);
              constMult(inc*(press->a-press->d), temp2, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       if(press->s == 1 || press->w == 1){
              cpyVec(view, temp);
              temp[2] = 0;
              normalize(temp);
              constMult(inc*(press->w-press->s), temp, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       if(press->dxmov != 0 || press->dymov != 0){
              phi += incang*(press->dymov/100);
              phi = (phi < 3.14159256-incang) ? phi : phi - incang;
              phi = (phi > incang) ? phi : phi + incang;
              theta += incang*(press->dxmov/100);
              press->dxmov = 0;
              press->dymov = 0;
                            /*phi += incang*(press->i - press->k);
                            phi = (phi < pi-incang) ? phi : phi - incang;
                            phi = (phi > incang) ? phi : phi + incang;
                            theta += incang*(press->l - press->j);*/
              changed = true;
       }
       if(press->space == 1 || press->shift == 1){
              constMult((press->space - press->shift)*inc, zaxis, temp);
              addVec(temp, pos, pos);
              changed = true;
       }
       send_position();
       return changed;
}
#endif