#ifndef PSN_SCREEN_128_64_H
#define PSN_SCREEN_128_64_H

#include "common.h"

void screen_init();
void screen_loop();

void screen_clear();
void screen_display();
void screen_set_time(const char time[]);
void screen_print(const char text[]);
#endif
