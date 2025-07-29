/*********************************************************************************************************************
Copyright (c) 2025, Martin Nicolas Soria <soria.m.nicolas@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, disponiblestribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file app.c
 ** @brief Codigo fuente del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "app.h"
#include <string.h>


/* === Macros definitions ========================================================================================== */

#define TICKS_FOR_SECOND 10
#define SNOOZE_TIME      5

/* === Private data type declarations ============================================================================== */

typedef enum {
    STATE_CLOCK_RUNNING,
    STATE_SET_TIME_MINUTES,
    STATE_SET_TIME_HOURS,
    STATE_SET_ALARM_MINUTES,
    STATE_SET_ALARM_HOURS,
} app_state_t;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

static clock_t clock;

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

board_t AppInit() {
    board_t board = board_create();
    clock = ClockCreate(TICKS_FOR_SECOND, SNOOZE_TIME);

    ScreenDisablePoint(board->screen, 0); 
    ScreenDisablePoint(board->screen, 2);
    ScreenDisablePoint(board->screen, 3);

    DisplayFlashPoints(board->screen, 1, 1, 50); 

    ScreenWriteBCD(board->screen, (uint8_t[]){0, 0, 0, 0}, 4); 

    DisplayFlashDigit(board->screen, 0, 3, 50); 

    return board;
}

void AppRun(board_t board) {
    static uint16_t ticks = 0;
    static uint16_t hold_ticks_time = 0;
    static uint16_t hold_ticks_alarm = 0;
    static uint16_t inactive_ticks = 0;
    static clock_time_t current_time;
    static clock_time_t backup_time;
    static clock_time_t alarm_time = {0};
    static clock_time_t backup_alarm_time = {0};
    static bool alarm_set = false;
    static bool alarm_active = false;
    static app_state_t state = STATE_CLOCK_RUNNING;
    static bool show_default = true;
    static bool time_set = false;

    ticks++;
    if (ticks >= TICKS_FOR_SECOND) {
        if (time_set) {
            ClockNewTick(clock);
        }
        ticks = 0;
    }

    switch (state) {

    case STATE_CLOCK_RUNNING:
        if (ClockGetTime(clock, &current_time)) {
            uint8_t display_bcd[4] = {
                current_time.bcd[5],
                current_time.bcd[4],  
                current_time.bcd[3],  
                current_time.bcd[2]   
            };
            ScreenWriteBCD(board->screen, display_bcd, 4);

            ScreenEnablePoint(board->screen, 1);
            ScreenDisablePoint(board->screen, 0);
            ScreenDisablePoint(board->screen, 2);
            ScreenDisablePoint(board->screen, 3);
            DisplayFlashDigit(board->screen, 0, 0, 0);
            show_default = false;
        } else if (!show_default) {
            uint8_t default_bcd[4] = {0, 0, 0, 0};
            ScreenWriteBCD(board->screen, default_bcd, 4);
            DisplayFlashDigit(board->screen, 0, 3, 50);
            DisplayFlashPoints(board->screen, 1, 1, 50);
            ScreenDisablePoint(board->screen, 0);
            ScreenDisablePoint(board->screen, 2);
            ScreenDisablePoint(board->screen, 3);
            show_default = true;
        }

        if(ClockIsAlarmTriggered(clock)){
            AlarmLedOn(board->alarm_led);
            if(DigitalInputGetIsActive(board->cancel)){
                ClockCancelAlarm(clock);
                AlarmLedOff(board->alarm_led);
                while (DigitalInputGetIsActive(board->cancel));
            }

            if(DigitalInputGetIsActive(board->accept)){
                ClockSnooze(clock);
                AlarmLedOff(board->alarm_led);
                while (DigitalInputGetIsActive(board->accept));
            }
        }else {
            AlarmLedOff(board->alarm_led);
        }

        if (DigitalInputGetIsActive(board->set_time)) {
            hold_ticks_time++;
            if (hold_ticks_time >= 3 * TICKS_FOR_SECOND) {
                state = STATE_SET_TIME_MINUTES;
                ClockGetTime(clock, &current_time);
                memcpy(&backup_time, &current_time, sizeof(clock_time_t));
                DisplayFlashDigit(board->screen, 0, 1, 50); 
                hold_ticks_time = 0;
            }
        } else {
            hold_ticks_time = 0;
        }

        if (DigitalInputGetIsActive(board->set_alarm)) {
            hold_ticks_alarm++;
            if (hold_ticks_alarm >= 3 * TICKS_FOR_SECOND && time_set) {
                state = STATE_SET_ALARM_MINUTES;
                memcpy(&current_time, &alarm_time, sizeof(clock_time_t));
                memcpy(&backup_alarm_time, &alarm_time, sizeof(clock_time_t));
                ScreenEnablePoint(board->screen, 0);
                ScreenEnablePoint(board->screen, 1);
                ScreenEnablePoint(board->screen, 2);
                ScreenEnablePoint(board->screen, 3);
                DisplayFlashDigit(board->screen, 0, 1, 50);
                inactive_ticks = 0; 
                hold_ticks_alarm = 0;
            }
        } else {
            hold_ticks_alarm = 0;
        }
        if (alarm_active) {
            ScreenEnablePoint(board->screen, 3); // último punto
        } else {
            ScreenDisablePoint(board->screen, 3);
        }
        if (DigitalInputGetIsActive(board->accept)&&alarm_set) {
            alarm_active = true;
            while (DigitalInputGetIsActive(board->accept));
        }
        if (DigitalInputGetIsActive(board->cancel)&&alarm_set){
            alarm_active = false;
            while (DigitalInputGetIsActive(board->cancel));
        }
        
        break;

    case STATE_SET_TIME_MINUTES:
        {
            inactive_ticks++;
            if (inactive_ticks >= 30 * TICKS_FOR_SECOND) {
                ClockSetTime(clock, &backup_time);
                state = STATE_CLOCK_RUNNING;
                inactive_ticks = 0;
                return;
            }
            
            
            uint8_t display_bcd[4] = {
                current_time.bcd[5],  
                current_time.bcd[4],  
                current_time.bcd[3],  
                current_time.bcd[2] 
            };
            ScreenWriteBCD(board->screen, display_bcd, 4);
            DisplayFlashDigit(board->screen, 0, 1, 50);

            if (DigitalInputGetIsActive(board->increment)) {
                inactive_ticks = 0;
                current_time.time.minutes[0]++;
                if (current_time.time.minutes[0] > 9) {
                    current_time.time.minutes[0] = 0;
                    current_time.time.minutes[1]++;
                    if (current_time.time.minutes[1] > 5) {
                        current_time.time.minutes[1] = 0;
                    }
                }
                while (DigitalInputGetIsActive(board->increment));
            }

            if (DigitalInputGetIsActive(board->decrement)) {
                inactive_ticks = 0;
                if (current_time.time.minutes[0] == 0) {
                    current_time.time.minutes[0] = 9;
                    if (current_time.time.minutes[1] == 0) {
                        current_time.time.minutes[1] = 5;
                    } else {
                        current_time.time.minutes[1]--;
                    }
                } else {
                    current_time.time.minutes[0]--;
                }
                while (DigitalInputGetIsActive(board->decrement));
            }

            if (DigitalInputGetIsActive(board->accept)) {
                inactive_ticks = 0;
                inactive_ticks = 0;
                DisplayFlashDigit(board->screen, 2, 3, 50);
                state = STATE_SET_TIME_HOURS;
                while (DigitalInputGetIsActive(board->accept));
            }

            if (DigitalInputGetIsActive(board->cancel)) {
                inactive_ticks = 0;
                ClockSetTime(clock, &backup_time);
                state = STATE_CLOCK_RUNNING;
                while (DigitalInputGetIsActive(board->cancel));
            }
            break;
        }
        

        case STATE_SET_TIME_HOURS:{
            inactive_ticks++;
            if (inactive_ticks >= 30 * TICKS_FOR_SECOND) {
                ClockSetTime(clock, &backup_time);
                state = STATE_CLOCK_RUNNING;
                inactive_ticks = 0;
                return;
            }

            uint8_t display_bcd[4]={
                current_time.bcd[5],  
                current_time.bcd[4],  
                current_time.bcd[3],  
                current_time.bcd[2] 
            };
            ScreenWriteBCD(board->screen, display_bcd, 4);
            DisplayFlashDigit(board->screen, 2, 3, 50);

            if (DigitalInputGetIsActive(board->increment))
            {
                inactive_ticks = 0;
                current_time.time.hours[0]++;
                if ((current_time.time.hours[1]==2 && current_time.time.hours[0]>3) || current_time.time.hours[0]>9) {
                    current_time.time.hours[0] = 0;
                    current_time.time.hours[1]++;
                    if (current_time.time.hours[1] > 2) {
                        current_time.time.hours[1] = 0;
                    }
                }
                while (DigitalInputGetIsActive(board->increment));
            }

            if (DigitalInputGetIsActive(board->decrement))
            {
                inactive_ticks = 0;
                if (current_time.time.hours[0] == 0) {
                    current_time.time.hours[0] = 9;
                    if (current_time.time.hours[1] == 0) {
                        current_time.time.hours[1] = 2;
                    } else {
                        current_time.time.hours[1]--;
                    }
                    if(current_time.time.hours[1] == 2 && current_time.time.hours[0] > 3) {
                        current_time.time.hours[0] = 3;
                    }
                }else {
                    current_time.time.hours[0]--;
                }
                while (DigitalInputGetIsActive(board->decrement));
            }
            if (DigitalInputGetIsActive(board->accept))
            {
                inactive_ticks = 0;
                time_set = true;
                ClockSetTime(clock, &current_time);
                state = STATE_CLOCK_RUNNING;
                while (DigitalInputGetIsActive(board->accept));
            }

            if(DigitalInputGetIsActive(board->cancel)) {
                inactive_ticks = 0;
                ClockSetTime(clock, &backup_time);
                state = STATE_CLOCK_RUNNING;
                while (DigitalInputGetIsActive(board->cancel));
            }
            break;
        }
    case STATE_SET_ALARM_MINUTES:{
        inactive_ticks++;
        if (inactive_ticks >= 30 * TICKS_FOR_SECOND) {
            memcpy(&alarm_time, &backup_alarm_time, sizeof(clock_time_t));
            state = STATE_CLOCK_RUNNING;
            inactive_ticks = 0;
            return;
        }
        uint8_t display_bcd[4] = {
            alarm_time.bcd[5],  
            alarm_time.bcd[4],  
            alarm_time.bcd[3],  
            alarm_time.bcd[2] 
        };
        ScreenWriteBCD(board->screen, display_bcd, 4);
        ScreenEnablePoint(board->screen, 0);
        ScreenEnablePoint(board->screen, 1);
        ScreenEnablePoint(board->screen, 2);
        ScreenEnablePoint(board->screen, 3);
        DisplayFlashPoints(board->screen, 1, 1, 0);
        DisplayFlashDigit(board->screen, 0, 1, 50);
        if (DigitalInputGetIsActive(board->increment)) {
            inactive_ticks = 0;
            alarm_time.time.minutes[0]++;
            if (alarm_time.time.minutes[0] > 9) {
                alarm_time.time.minutes[0] = 0;
                alarm_time.time.minutes[1]++;
                if (alarm_time.time.minutes[1] > 5) {
                    alarm_time.time.minutes[1] = 0;
                }
            }
            while (DigitalInputGetIsActive(board->increment));
        }
        if (DigitalInputGetIsActive(board->decrement)) {
            inactive_ticks = 0;
            if (alarm_time.time.minutes[0] == 0) {
                alarm_time.time.minutes[0] = 9;
                if (alarm_time.time.minutes[1] == 0) {
                    alarm_time.time.minutes[1] = 5;
                } else {
                    alarm_time.time.minutes[1]--;
                }
            } else {
                alarm_time.time.minutes[0]--;
            }
            while (DigitalInputGetIsActive(board->decrement));
        }

        if (DigitalInputGetIsActive(board->accept)) {
            inactive_ticks = 0;
            DisplayFlashDigit(board->screen, 2, 3, 50);
            state = STATE_SET_ALARM_HOURS;
            while (DigitalInputGetIsActive(board->accept));
        }
        
        if (DigitalInputGetIsActive(board->cancel)) {
            inactive_ticks = 0;
            memcpy(&alarm_time, &backup_alarm_time, sizeof(clock_time_t));
            state = STATE_CLOCK_RUNNING;
            while (DigitalInputGetIsActive(board->cancel));
        }
        break;
    }
    case STATE_SET_ALARM_HOURS:{
        inactive_ticks++;
        if (inactive_ticks >= 30 * TICKS_FOR_SECOND) {
            memcpy(&alarm_time, &backup_alarm_time, sizeof(clock_time_t));
            state = STATE_CLOCK_RUNNING;
            inactive_ticks = 0;
            return;
        }
        uint8_t display_bcd[4] = {
            alarm_time.bcd[5],  
            alarm_time.bcd[4],  
            alarm_time.bcd[3],  
            alarm_time.bcd[2] 
        };
        ScreenWriteBCD(board->screen, display_bcd, 4);
        DisplayFlashDigit(board->screen, 2, 3, 50);
        ScreenEnablePoint(board->screen, 0);
        ScreenEnablePoint(board->screen, 1);
        ScreenEnablePoint(board->screen, 2);
        ScreenEnablePoint(board->screen, 3);

        if (DigitalInputGetIsActive(board->increment)) {
            inactive_ticks = 0;
            alarm_time.time.hours[0]++;
            if ((alarm_time.time.hours[1]==2 && alarm_time.time.hours[0]>3) || alarm_time.time.hours[0]>9) {
                alarm_time.time.hours[0] = 0;
                alarm_time.time.hours[1]++;
                if (alarm_time.time.hours[1] > 2) {
                    alarm_time.time.hours[1] = 0;
                }
            }
            while (DigitalInputGetIsActive(board->increment));
        }

        if (DigitalInputGetIsActive(board->decrement)) {
            inactive_ticks = 0;
            if (alarm_time.time.hours[0] == 0) {
                alarm_time.time.hours[0] = 9;
                if (alarm_time.time.hours[1] == 0) {
                    alarm_time.time.hours[1] = 2;
                } else {
                    alarm_time.time.hours[1]--;
                }
                if(alarm_time.time.hours[1] == 2 && alarm_time.time.hours[0] > 3) {
                    alarm_time.time.hours[0] = 3;
                }
            }else {
                alarm_time.time.hours[0]--;
            }
            while (DigitalInputGetIsActive(board->decrement));
        }

        if (DigitalInputGetIsActive(board->accept)) {
            inactive_ticks = 0;
            alarm_set = true;
            alarm_active = true;
            ClockSetAlarm(clock, &alarm_time);
            state = STATE_CLOCK_RUNNING;
            while (DigitalInputGetIsActive(board->accept));
            
        }

        if (DigitalInputGetIsActive(board->cancel)) {
            inactive_ticks = 0;
            memcpy(&alarm_time, &backup_alarm_time, sizeof(clock_time_t));
            state = STATE_CLOCK_RUNNING;
            while (DigitalInputGetIsActive(board->cancel));
        }
        break;
    }
}
}


/* === Private function definitions ================================================================================
 */

/* === End of documentation ========================================================================================
 */
