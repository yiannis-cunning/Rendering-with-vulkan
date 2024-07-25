#ifndef CONTROL_MODES_H
#define CONTROL_MODES_H

#include "linalg.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include "../vulkan3.h"
#include "../glfw_main.h"


typedef enum {CONTROL_FLY_MODE, CONTROL_XYZ_MODE, TEXT_MODE, PAUSE_MODE} control_mode_enum;


struct key_press_t {
       bool pressed;
       uint32_t ticks_pressed;
};


struct pressing_t{
       struct key_press_t atoz[26];
       //struct key_press_t atoz_caps[26];
       struct key_press_t exlamation, question, colon, semicolon, backspace, shift, enter, slash, backslack, space;

       double xpos;
       double ypos;
};


void tick_update(struct pressing_t pressing);

void controller_init();

screenProperties_t get_screen();

void new_chr_in(unsigned int char_i);

void bkspc();


/*
class flyMode{
       float pos[3], view[3], phi, theta, r, inc, incang, *v, *offset;
public:
       flyMode(float *p_0, float *v_0, float *v_in, float *offset_in);
       void send_position();
       bool tick_update(struct pressing *press);
};


class jumpMode{
       float *v, *offset, pos[3], view[3], phi, theta, r, inc, incang, velup, accel;
       unsigned int tLast;
public:
       jumpMode(float *p_0, float *v_0, float *v_in, float *offset_in);
       void send_position();
       bool tick_update(struct pressing *press);
};

class controller{
       struct flyMode *fly_data;
       jumpMode *jmp_data;
       int mode;
public:
       struct pressing press;
       float v[3];
       float offset[3];
       
       controller(float *p_0, float *v_0);
       void send_position();
       bool tick_update();
};


class controller2{
       float r, theta, phi;
       float inc, incang;
       control_mode_enum mode;

public:
       float view[3];
       float offset[3];
       struct pressing press;

       controller2(float *view_init, float *offset_init);
       bool tick_update();
       void rotate_mode();
};*/


#endif