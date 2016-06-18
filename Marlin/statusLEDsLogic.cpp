#include "statusLEDsLogic.h"

#if ENABLED(TEMP_STAT_LEDS)

#include "pins.h"

// Edit 01/24/2016 Marquis Johnson
// Edit 05/22/2016 Tomer Orian
static millis_t next_status_led_update_ms = 0;
//Virtual Variables
int t = 2;
int Rval = 0;
int Gval = 0;
int Bval = 0;
int forcedRedValue = 0;
int forcedGreenValue = 0;
int forcedBlueValue = 0;
boolean forcedColorOn = false;
boolean appliedForcedColor = true;
boolean shouldFlashLeds = true;
int LEDmax = 160;
int ExtTemp;
int ExtTarg;
//Static Colors
void off(){Rval = 0;Gval = 0;Bval = 0;}
void white(){Rval = LEDmax;Gval = LEDmax;Bval = LEDmax;}
void red(){Rval = LEDmax;Gval = 0;Bval = 0;}
void green(){Rval = 0;Gval = LEDmax;Bval = 0;}
void blue(){Rval = 0;Gval = 0;Bval = LEDmax;}
void yellow(){Rval = LEDmax;Gval = LEDmax;Bval = 0;}
void cyan(){Rval = 0;Gval = LEDmax;Bval = LEDmax;}
void magenta(){Rval = LEDmax;Gval = 0;Bval = LEDmax;}

//Handle Led Status
void handle_status_leds(void) {
  if (shouldFlashLeds) {
    appliedForcedColor = false;
    shouldFlashLeds = false;
    analogWrite(STAT_LED_RED, 0);
    analogWrite(STAT_LED_GREEN, 0);
    analogWrite(STAT_LED_BLUE, 0);
    
    for(int r = 0; r < LEDmax; r++) { analogWrite(STAT_LED_RED, r); delay(t); }
    for(int r = LEDmax; r > 0; r--) { analogWrite(STAT_LED_RED, r); delay(t); }
    for(int g = 0; g < LEDmax; g++) { analogWrite(STAT_LED_GREEN, g); delay(t); }
    for(int g = LEDmax; g > 0; g--) { analogWrite(STAT_LED_GREEN, g); delay(t); }
    for(int b = 0; b < LEDmax; b++) { analogWrite(STAT_LED_BLUE, b); delay(t); }
    for(int b = LEDmax; b>0; b--) { analogWrite(STAT_LED_BLUE, b); delay(t); }
    for(int w = 0; w < LEDmax; w++) { analogWrite(STAT_LED_RED, w);analogWrite(STAT_LED_GREEN, w);analogWrite(STAT_LED_BLUE, w); delay(t); }
  }

  if (forcedColorOn) {
    if (!appliedForcedColor) {
      analogWrite(STAT_LED_RED, min(forcedRedValue, LEDmax));
      analogWrite(STAT_LED_GREEN, min(forcedGreenValue, LEDmax));
      analogWrite(STAT_LED_BLUE, min(forcedBlueValue, LEDmax));
  
      appliedForcedColor = true;
    }

    return;
  }
  
  if (millis() > next_status_led_update_ms) {
    next_status_led_update_ms = millis() + 500; // Update every 0.5s
    for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder){
      ExtTemp = degHotend(cur_extruder);
      ExtTarg = degTargetHotend(cur_extruder);
      if ((degTargetBed() == 71) || (degTargetBed() == 1)) {off();}
      if (degTargetBed() == 100) {yellow();}
      if (degTargetBed() == 69) {cyan();}
      if ((ExtTarg == 0)&&(degTargetBed() == 0)) {white();}
      if ((ExtTarg != 0)
        &&(degTargetBed() != 1)
        &&(degTargetBed() != 69)
        &&(degTargetBed() != 71)
        &&(degTargetBed() != 100)){
        if((ExtTarg >= ExtTemp-TEMP_HYSTERESIS)
        &&(ExtTarg <= ExtTemp+TEMP_HYSTERESIS)) {white();}
        else if ((degTargetBed() == 5)&&(degBed() <= 50)) {green();}
        else {
          int MidTemp = (((EXTRUDE_MINTEMP)-40)/2);
          if (ExtTemp < 40){blue();}
          if (ExtTemp > EXTRUDE_MINTEMP){red();}
          if ((ExtTemp > 40) && (ExtTemp < MidTemp)) {
            Rval = map(ExtTemp,40,MidTemp,0,LEDmax);
            Gval = 0;  
            Bval = LEDmax;
          }
          if ((ExtTemp > MidTemp)&&(ExtTemp < EXTRUDE_MINTEMP)) {
            Rval = LEDmax;
            Gval = 0;
            Bval = map(ExtTemp,MidTemp,EXTRUDE_MINTEMP,LEDmax,0);
          }
        }
      }
    }
    
    //Write to LEDs
    analogWrite(STAT_LED_RED, Rval);
    analogWrite(STAT_LED_GREEN, Gval);
    analogWrite(STAT_LED_BLUE, Bval);
  }
}

void status_leds_flash_leds() {
  shouldFlashLeds = true;
}

void status_leds_force_color(int r, int g, int b) {
  forcedColorOn = true;
  appliedForcedColor = false;
  
  forcedRedValue = r;
  forcedGreenValue = g;
  forcedBlueValue = b;
}

void status_leds_disable_force_color() {
  forcedColorOn = false;
}

#endif

