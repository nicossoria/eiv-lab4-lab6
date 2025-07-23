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

/* === Macros definitions ========================================================================================== */

#define TICKS_FOR_SECOND 5
#define SNOOZE_TIME      5

/* === Private data type declarations ============================================================================== */

typedef enum {
    STATE_CLOCK_RUNNING,
    STATE_SET_TIME_MINUTES,
    STATE_SET_TIME_HOURS,
    STATE_SET_ALARM_MINUTES,
    STATE_SET_ALARM_HOURS,
    STATE_SNOOZE,
    STATE_CLOCK
} app_state_t;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

static clock_t clock;
static app_state_t current_state = STATE_CLOCK_RUNNING;

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
    static uint16_t hold_ticks = 0;
    static clock_time_t current_time;
    static clock_time_t backup_time;
    static app_state_t state = STATE_CLOCK_RUNNING;
    static bool show_default = true;

    ticks++;
    if (ticks >= TICKS_FOR_SECOND) {
        ClockNewTick(clock);
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

        if (DigitalInputGetIsActive(board->set_time)) {
            hold_ticks++;
            if (hold_ticks >= 3 * TICKS_FOR_SECOND) {
                state = STATE_SET_TIME_MINUTES;
                ClockGetTime(clock, &current_time);
                memcpy(&backup_time, &current_time, sizeof(clock_time_t));
                DisplayFlashDigit(board->screen, 0, 1, 50); 
                hold_ticks = 0;
            }
        } else {
            hold_ticks = 0;
        }
        break;

    case STATE_SET_TIME_MINUTES:
        {
            uint8_t display_bcd[4] = {
                current_time.bcd[5],  
                current_time.bcd[4],  
                current_time.bcd[3],  
                current_time.bcd[2] 
            };
            ScreenWriteBCD(board->screen, display_bcd, 4);
            DisplayFlashDigit(board->screen, 0, 1, 50);

            if (DigitalInputGetIsActive(board->increment)) {
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
                DisplayFlashDigit(board->screen, 2, 3, 50);
                state = STATE_SET_TIME_HOURS;
                while (DigitalInputGetIsActive(board->accept));
            }

            if (DigitalInputGetIsActive(board->cancel)) {
                ClockSetTime(clock, &backup_time);
                state = STATE_CLOCK_RUNNING;
                while (DigitalInputGetIsActive(board->cancel));
            }
            break;
        }
        

        case STATE_SET_TIME_HOURS:{
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
                ClockSetTime(clock, &current_time);
                state = STATE_CLOCK_RUNNING;
                while (DigitalInputGetIsActive(board->accept));
            }

            if(DigitalInputGetIsActive(board->cancel)) {
                ClockSetTime(clock, &backup_time);
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
