/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "myCommonFunctions.h"
#include "myOSFunctions.h"
#include "myExternalLED.h"
#include "myInternalLED.h"
#include "myMotor.h"
#include "myUART.h"
#include "myMessageList.h"
#include "myAudio.h"

// DEBUG vars
uint8_t message = 0; // TODO define where first declare HERE FOR DEBUG
uint32_t flags = 0; // TODO define where first declared as well
int a = 0;
int b = 0;
int d = 0;
int e = 0;
int f = 0;
int g = 0;
int h = 0;

int start_bt = 0;
int start_song = 0;
int stop_song = 0;
int victory_tune = 0;
uint32_t flags_music = 0; 

static osEventFlagsId_t event_flags_id;

/*----------------------------------------------------------------------------
 * Thread - Running LED Control
 *---------------------------------------------------------------------------*/
void tMovingLED(void *argument) {
    for (;;) {
        a++;
        
        if (getIsConnected() == 0) {
            continue;
        }
        
        // Any set Motor bits in event flag signal indicate a moving robot
        if (osEventFlagsGet(event_flags_id) & EXT_LED_BOT_MOVING_EF_MASK) {
            osEventFlagsClear(event_flags_id, EXT_LED_BOT_MOVING_EF_MASK);
            setMovingLED();
        }
        
        if (getIsMoving()) {      
            runExternalMovingLED();
        }
    }
}

/*----------------------------------------------------------------------------
 * Thread - Stationery LED Control
 *---------------------------------------------------------------------------*/
void tStationeryLED(void *argument) {
    for (;;) {
        h++;
        
        if (getIsConnected() == 0) {
            continue;
        }
         
        // Any set Motor bits in event flag signal indicate a moving robot
        if (osEventFlagsGet(event_flags_id) & EXT_LED_BOT_STATIONERY_EF_MASK) {
            osEventFlagsClear(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
            setStationeryLED();
        }
        
        if (!getIsMoving()) {
            runExternalStationeryLED();
        }
    }
}

/*----------------------------------------------------------------------------
 * Thread - Motor Control
 *---------------------------------------------------------------------------*/
void tMotorControl(void *argument) {
    for (;;) {
        b++;
        
        // Waits for event flag signalling that motor direction has changed
        osEventFlagsWait(event_flags_id, MOTOR_DIR_CHANGE_EF_MASK, osFlagsWaitAll, EVENT_TIME_OUT);
        setMotorMoveDir(message);
        // Handles the change in direction event
        switch(getMotorMoveDir()) {
        case MESSAGE_STOP:
            setStationeryLED();  // Clean this up
            stop();
            break;
        case MESSAGE_N:
            setMovingLED();
            moveN();
            break;
        case MESSAGE_NE:
            setMovingLED();
            moveNE();
            break;
        case MESSAGE_E:
            setMovingLED();
            moveE();
            break;
        case MESSAGE_SE:
            setMovingLED();
            moveSE();
            break;
        case MESSAGE_S:
            setMovingLED();
            moveS();
            break;
        case MESSAGE_SW:
            setMovingLED();
            moveSW();
            break;
        case MESSAGE_W:
            setMovingLED();
            moveW();
            break;
        case MESSAGE_NW:
            setMovingLED();
            moveNW();
            break;
        default:
            setStationeryLED();  // Clean this up
            stop();
            break;
        }
    }
}

/*----------------------------------------------------------------------------
 * Thread - Audio Control
 *---------------------------------------------------------------------------*/
void tAudio(void *argument) {
    for (;;) {
        g++;
        if (osEventFlagsGet(event_flags_id) & BT_CONNECT_EF_MASK) {
            osEventFlagsClear(event_flags_id, BT_CONNECT_EF_MASK);
            start_bt++; //debug
            startSuccessFx();
            osDelay(3000); 
            startSong();

//        } else if (osEventFlagsGet(event_flags_id) & START_CHALLENGE_MASK) {
//            osEventFlagsClear(event_flags_id, START_CHALLENGE_MASK);
//             startSong();
//            start_song++;   //debug
   
        } else if (osEventFlagsGet(event_flags_id) & STOP_CHALLENGE_MASK) {
            osEventFlagsClear(event_flags_id, STOP_CHALLENGE_MASK);
            stop_song++;
            stopSong();

        } else if (osEventFlagsGet(event_flags_id) & START_VICTORY_MASK) {
            osEventFlagsClear(event_flags_id, START_VICTORY_MASK);
            victory_tune++;    
            startVictoryTune();
        }

    }
}

/*----------------------------------------------------------------------------
 * Thread - Serial Data Decoder
 *---------------------------------------------------------------------------*/
void tBrain(void *arguement) {
    for (;;) {
       e++;
        
       Q_T* rxQ = getReceiveBuffer();
       if (!Q_Empty(rxQ)) {
            message = Q_Dequeue(rxQ);
            
            /* Sets the appropriate flag to indicate a task needing, 
                other threads will watch for the flag and handle it
                accordingly*/
            switch (message) {
            // Movement
            case MESSAGE_STOP:
                setMotorMoveDir(MESSAGE_STOP);
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
                osEventFlagsSet(event_flags_id, MOTOR_DIR_CHANGE_EF_MASK);
                break;
            case MESSAGE_N:  // Fallthrough
            case MESSAGE_NE: // Fallthrough
            case MESSAGE_E:  // Fallthrough
            case MESSAGE_SE: // Fallthrough
            case MESSAGE_S:  // Fallthrough
            case MESSAGE_SW: // Fallthrough
            case MESSAGE_W:  // Fallthrough
            case MESSAGE_NW:
                setMotorMoveDir(message);
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_MOVING_EF_MASK);
                osEventFlagsSet(event_flags_id, MOTOR_DIR_CHANGE_EF_MASK);
                break;

            // Bluetooth
            case MESSAGE_BT_CONNECT:
                setIsConnected(0);
                osEventFlagsSet(event_flags_id, BT_CONNECT_SUCCESS_MASK);
                signalSuccessConnection();    
                osEventFlagsSet(event_flags_id, BT_CONNECT_EF_MASK);
                break;
            
            case MESSAGE_START_CHALLENGE_MUSIC:
                osEventFlagsSet(event_flags_id, START_CHALLENGE_MASK);
                break;
            
            case MESSAGE_STOP_CHALLENGE_MUSIC:
                osEventFlagsSet(event_flags_id, STOP_CHALLENGE_MASK);
                break;
            
            case MESSAGE_START_VICTORY_TUNE:
                setMotorMoveDir(MESSAGE_STOP);
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
                osEventFlagsSet(event_flags_id, MOTOR_DIR_CHANGE_EF_MASK);
                osEventFlagsSet(event_flags_id, START_VICTORY_MASK);
                break;
            
            // Bad command
            default:
                setMotorMoveDir(MESSAGE_STOP);
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
                osEventFlagsSet(event_flags_id, MOTOR_DIR_CHANGE_EF_MASK);
                break;
            }
        }
    }
}

// Initialize event flags used to synchornize threads
void initEvent() {
    event_flags_id = osEventFlagsNew(NULL);
    osEventFlagsClear(event_flags_id, 0xFFFF);
    osEventFlagsSet(event_flags_id, ALL_EVENTS_HANDLED_EF_MASK);
}

void initRobotComponents(void) {
    initUART2(BAUD_RATE);
    initExternalLED();
    initInternalLED();
    
    initMotorPWM();
    initAudio();
}

// System Initialization
int main(void) {
  	SystemCoreClockUpdate();
    initRobotComponents();

    osKernelInitialize();				 // Initialize CMSIS-RTOS
    initEvent();                        // Note must be done AFTER initializing kernel
    
    osThreadNew(tBrain, NULL, NULL);
    osThreadNew(tMotorControl, NULL, NULL);
    osThreadNew(tAudio,  NULL, NULL); // TODO probably need to set to high priority, need to change notes exactly at deadline
    osThreadNew(tMovingLED, NULL, NULL);
    osThreadNew(tStationeryLED, NULL, NULL);

    osKernelStart();					  // Start thread execution
    for (;;) {
        a++;
    }
}
