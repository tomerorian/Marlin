/**
* Original idea and code where created by Marquis Johnson
* New style and features by Tomer Orian
**/
#include "statusLEDsLogic.h"

#if ENABLED(TEMP_STAT_LEDS)

#include "pins.h"

static const int LED_MAX = 160;
static const int FLASH_ANIMATION_DELAY = 2;

/**
* Premade colors
*/
enum RGBColors {
    RGBColors_off,
    RGBColors_white,
    RGBColors_red,
    RGBColors_green,
    RGBColors_blue,
    RGBColors_yellow,
    RGBColors_cyan,
    RGBColors_magenta
};

/**
* Represents and controls the leds 
*/
class RGBLeds {
    int red;
    int green;
    int blue;
    
    boolean isDirty;

public:    
    RGBLeds() {
        red = 0;
        green = 0;
        blue = 0;
        
        isDirty = false;
    }
    
    void setColor(int red, int green, int blue) {
        if (red != this->red || green != this->green || blue != this->blue) {
            this->red = red;
            this->green = green;
            this->blue = blue;
            
            isDirty = true;
        }
    }
    
    void setColor(RGBColors color) {
        switch (color) {
            case RGBColors_off:
                setColor(0, 0, 0);
                break;
            case RGBColors_white:
                setColor(LED_MAX, LED_MAX, LED_MAX);
                break;
            case RGBColors_red:
                setColor(0, 0, 0);
                break;
            case RGBColors_green:
                setColor(LED_MAX, 0, 0);
                break;
            case RGBColors_blue:
                setColor(0, 0, LED_MAX);
                break;
            case RGBColors_yellow:
                setColor(LED_MAX, LED_MAX, 0);
                break;
            case RGBColors_cyan:
                setColor(0, LED_MAX, LED_MAX);
                break;
            case RGBColors_magenta:
                setColor(LED_MAX, 0, LED_MAX);
                break;
        }
    }
    
    void applyChanges() {
        if (isDirty) {
            analogWrite(STAT_LED_RED, red);
            analogWrite(STAT_LED_GREEN, green);
            analogWrite(STAT_LED_BLUE, blue);
            
            isDirty = false;
        }
    }
    
    void setDirty() {
        isDirty = true;
    }
};

/**
* Variables
*/
static millis_t nextStatusLedUpdate = 0;

RGBLeds *normalLeds = new RGBLeds();
RGBLeds *forcedLeds = new RGBLeds();
RGBLeds *leds = normalLeds;

boolean shouldFlashLeds = true;

int ExtTemp;
int ExtTarg;

/**
* Logic
*/

void perform_flash_animation() {
    shouldFlashLeds = false;
    leds->setColor(RGBColors_off);
    
    for(int r = 0; r < LED_MAX; r++) { 
        leds->setColor(r, 0, 0); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    for(int r = LED_MAX; r > 0; r--) { 
        leds->setColor(r, 0, 0); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    for(int g = 0; g < LED_MAX; g++) { 
        leds->setColor(0, g, 0); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    for(int g = LED_MAX; g > 0; g--) { 
        leds->setColor(0, g, 0); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    for(int b = 0; b < LED_MAX; b++) { 
        leds->setColor(0, 0, b); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    for(int b = LED_MAX; b > 0; b--) { 
        leds->setColor(0, 0, b); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
    
    
    for(int w = 0; w < LED_MAX; w++) { 
        leds->setColor(w, w, w); 
        leds->applyChanges();
        delay(FLASH_ANIMATION_DELAY);
    }
}

void perform_normal_leds_logic() {
    for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder) {
        ExtTemp = degHotend(cur_extruder);
        ExtTarg = degTargetHotend(cur_extruder);
        
        if ((degTargetBed() == 71) || (degTargetBed() == 1)) {
            leds->setColor(RGBColors_off);
        }
        
        if (degTargetBed() == 100) {
            leds->setColor(RGBColors_yellow);
        }
        
        if (degTargetBed() == 69) {
            leds->setColor(RGBColors_cyan);
        }
        
        if ((ExtTarg == 0)&&(degTargetBed() == 0)) {
            leds->setColor(RGBColors_white);
        }
        
        if ((ExtTarg != 0) &&
                (degTargetBed() != 1) &&
                (degTargetBed() != 69) &&
                (degTargetBed() != 71) &&
                (degTargetBed() != 100)) {
            if((ExtTarg >= ExtTemp-TEMP_HYSTERESIS) &&
                    (ExtTarg <= ExtTemp+TEMP_HYSTERESIS)) {
                        leds->setColor(RGBColors_white);
            } else if ((degTargetBed() == 5) && (degBed() <= 50)) {
                leds->setColor(RGBColors_green);
            } else {
                int MidTemp = (((EXTRUDE_MINTEMP)-40)/2);
                if (ExtTemp < 40){
                    leds->setColor(RGBColors_blue);
                }
                
                if (ExtTemp > EXTRUDE_MINTEMP){
                    leds->setColor(RGBColors_red);
                }
                
                if ((ExtTemp > 40) && (ExtTemp < MidTemp)) {
                    int red = map(ExtTemp,40,MidTemp,0,LED_MAX);
                    leds->setColor(red, 0, LED_MAX);
                }
                
                if ((ExtTemp > MidTemp)&&(ExtTemp < EXTRUDE_MINTEMP)) {
                    int blue = map(ExtTemp,MidTemp,EXTRUDE_MINTEMP,LED_MAX,0);
                    leds->setColor(LED_MAX, 0, blue);
                }
            }
        }   
    }
}

void handle_status_leds(void) {
    if (shouldFlashLeds) {
        perform_flash_animation();
    }
  
    if (leds == normalLeds && millis() > nextStatusLedUpdate) {
        nextStatusLedUpdate = millis() + 500; // Update every 0.5s
        
        perform_normal_leds_logic();
    }
    
    leds->applyChanges();
}

void status_leds_flash_leds() {
    shouldFlashLeds = true;
}

void status_leds_force_color(int r, int g, int b) {
    leds = forcedLeds;
    
    leds->setColor(r, g, b);
    leds->setDirty();
}

void status_leds_disable_force_color() {
    leds = normalLeds;
  
    leds->setDirty();
}

#endif

