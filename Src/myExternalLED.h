#include "myCommonFunctions.h"

#define PTA17 17

#define CONNECTION_FLASH_DELAY 250000
#define CONNECTION_REPS_DELAY 250000

#define GREEN_RUN_DELAY 750000
#define RED_RUN_DELAY 500000
#define RED_STOP_DELAY 250000
#define NUM_GREEN_LEDS 10

typedef enum {
    PTC3 = 3,
    PTC4 = 4,
    PTC5 = 5,
    PTC6 = 6,
    PTC10 = 10,
    PTC11 = 11,
    PTC12 = 12,
    PTC13 = 13,
    PTC16 = 16,
    PTC17 = 17
} port_c_t;




port_c_t greenLEDPins[NUM_GREEN_LEDS] = {PTC3, PTC4, PTC5, PTC6, PTC10, PTC11, PTC12, PTC13, PTC16, PTC17};
int activeGreenLED = 0;

void initExternalLED(){
    int i = 0;
    // Enable Clock to PORTA and PORTC
    SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTC_MASK));

    // Initalize GPIO Pins for the Green LED Strip
    for(i = 0; i < NUM_GREEN_LEDS; i++)
        initGPIOPin(&(PORTC->PCR[greenLEDPins[i]]), &(PTC->PDDR), greenLEDPins[i], 1);

    // Initalize GPIO Pins for the Red LED Strip
    initGPIOPin(&(PORTA->PCR[PTA17]), &(PTA->PDDR), PTA17, 1);
}

void toggleRedLED() {
    PTA->PTOR = MASK(PTA17);
}

void clearLitGreenLED(){
    int i;
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        PTC->PCOR = MASK(greenLEDPins[i]);
    }
}

void setNewActiveGreenLED(int newLED){
    if(activeGreenLED)
        PTC->PCOR = MASK(activeGreenLED);
    activeGreenLED = greenLEDPins[newLED];
    PTC->PSOR = MASK(greenLEDPins[newLED]);
    osDelay(GREEN_RUN_DELAY);
}

void setTrailGreenLED() {
    int i;    
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        if(!isLedTrailing)
            break;
        setNewActiveGreenLED(i);
    }
}

void setAllGreenLED() {
    int i;    
    for(i = 0; i < NUM_GREEN_LEDS; i++) 
        PTC->PSOR = MASK(greenLEDPins[i]);
}

void signalSuccessConnection() {
    int repetitions = 2;
    while(repetitions) {

        setAllGreenLED();
        delay(CONNECTION_FLASH_DELAY);
        clearLitGreenLED();

        repetitions--;
        delay(CONNECTION_REPS_DELAY);
    }	
}
