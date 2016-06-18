#ifndef STATUS_LED_LOGIC_H
#define STATUS_LED_LOGIC_H

#include "temperature.h"

#if ENABLED(TEMP_STAT_LEDS)

void handle_status_leds();
void status_leds_flash_leds();
void status_leds_force_color(int r, int g, int b);
void status_leds_disable_force_color();

#endif

#endif
