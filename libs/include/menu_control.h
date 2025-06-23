#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef unsigned int uint;

void init_buttons();
void button_isr(uint gpio, uint32_t events);
void reset_limits();

#endif