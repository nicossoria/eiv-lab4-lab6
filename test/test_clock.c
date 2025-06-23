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

/** @file test_reloj.c
 ** @brief Codigo de prueba del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_FOR_SECOND 5 // Frecuencia del reloj
#define SNOOZE_TIME            5 // Tiempo de posposición de la alarma en minutos
#define TEST_ASSERT_TIME(seconds_units, seconds_tens, minutes_units, minutes_tens, hours_units, hours_tens)            \
    clock_time_t current_time = {0};                                                                                   \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");                            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.time.seconds[0], "Diference in unit seconds");         \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, current_time.time.seconds[1], "Diference in tens seconds");          \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.time.minutes[0], "Diference in unit minutes");         \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, current_time.time.minutes[1], "Diference in tens minutes");          \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, current_time.time.hours[0], "Diference in unit hours");               \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, current_time.time.hours[1], "Diference in tens hours")

/* === Private data type declarations ============================================================================== */
clock_t clock;
/* === Private function declarations =============================================================================== */
static void SimulateSeconds(clock_t clock, uint8_t seconds);
static void SimulateMinutes(clock_t clock, uint8_t minutes);
static void SimulateHours(clock_t clock, uint8_t hours);
/* === Private variable definitions ================================================================================
 */

/* === Public variable definitions =================================================================================
 */

/* === Public function definitions ============================================================================== */

void setUp(void) {
    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND, SNOOZE_TIME);
}

// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {.bcd = {1, 2, 3, 4, 5, 6}};

    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND, SNOOZE_TIME);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// Al ajustar la hora el reloj con valores correctos, queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_t new_time = {.time = {.seconds = {4, 5}, .minutes = {3, 0}, .hours = {4, 1}}};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));

    TEST_ASSERT_TIME(4, 5, 3, 0, 4, 1);
}
// Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {

    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 1);
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0);
}

// Después de n ciclos de reloj la hora avanza diez segundo
void test_clock_advance_ten_seconds(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 10);
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0);
}

// Después de n ciclos de reloj la hora avanza un minuto
void test_clock_advance_one_minute(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 60);
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0);
}
// Después de n ciclos de reloj la hora avanza diez minutos
void test_clock_advance_ten_minutes(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateMinutes(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0);
}

// Después de n ciclos de reloj la hora avanza una hora
void test_clock_advance_one_hour(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateMinutes(clock, 60);
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0);
}
// Después de n ciclos de reloj la hora avanza diez horas
void test_clock_advance_ten_hours(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateHours(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

// Después de n ciclos de reloj la hora avanza un día completo
void test_clock_advance_one_day(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateHours(clock, 24);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0);
}

//- Fijar la hora de la alarma y consultarla.
void test_set_alarm_and_get(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {0, 3}, .hours = {8, 0}}};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    clock_time_t alarm_read = {0};

    TEST_ASSERT_TRUE_MESSAGE(ClockGetAlarm(clock, &alarm_read), "Error getting alarm time");
}

// Fijar la alarma y avanzar el reloj para que suene.
void test_alarm_tiggers_when_is_time(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));
    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}
// Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
void test_alarm_is_disabled_so_not_trigger(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));
    ClockDisableAlarm(clock);
    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
}

// Hacer sonar la alarma y posponerla.
void test_alarm_snooze(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
    ClockSnooze(clock);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
    SimulateMinutes(clock, 5);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}
// Hacer sonar la alarma y cancelarla hasta el otro dia..
void test_alarm_cancelled_until_next_day(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});

    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));

    ClockCancelAlarm(clock);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));

    SimulateMinutes(clock, 5);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));

    SimulateHours(clock, 23);
    SimulateMinutes(clock, 54);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

// Hacer sonar la alarma, dejarla sonar, posponerla y verificar que no suene de nuevo hasta que pase el tiempo de
// posposición
void test_alarm_snooze_and_wait_to_ring_again(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
    SimulateMinutes(clock, 1);
    ClockSnooze(clock);
    SimulateMinutes(clock, 4);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

// Hacer sonar la alarma, dejarla sonar, cancelarla y verificar que suena de nuevo a la misma hora al dia siguiente.
void test_alarm_rings_next_day_after_cancel(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));

    SimulateMinutes(clock, 2);
    ClockCancelAlarm(clock);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
    SimulateHours(clock, 23);
    SimulateMinutes(clock, 54);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
    SimulateMinutes(clock, 4);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

// Probar get_time con NULL;

void test_get_time_with_null(void) {
    static const clock_time_t new_time = {.bcd = {0, 0, 0, 0, 0, 0}};
    ClockSetTime(clock, &new_time);
    TEST_ASSERT_FALSE(ClockGetTime(clock, NULL));
}

// Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
void test_set_time_with_invalid_values(void) {
    clock_time_t current_time = {.bcd = {1, 2, 3, 4, 5, 6}};

    // Caso 1: NULL
    TEST_ASSERT_FALSE(ClockSetTime(clock, NULL));
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);

    // Caso 2: hora invalida
    clock_time_t invalid_time = {.bcd = {0, 8, 0, 7, 5, 2}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_time));
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// Tratar de ajustar la alarma con valores invalidos y verificar que los rechaza.
void test_set_alarm_with_invalid_values(void) {
    static const clock_time_t alarm = {.bcd = {0, 0, 5, 0, 8, 0}};
    clock_time_t alarm_read = {0};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm));

    // Caso 1: NULL
    TEST_ASSERT_FALSE(ClockSetAlarm(clock, NULL));
    TEST_ASSERT_TRUE(ClockGetAlarm(clock, &alarm_read));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarm.bcd, alarm_read.bcd, 6);

    // Caso 2: hora invalida
    clock_time_t invalid_alarm = {.bcd = {0, 8, 0, 7, 5, 2}};
    TEST_ASSERT_FALSE(ClockSetAlarm(clock, &invalid_alarm));
    TEST_ASSERT_TRUE(ClockGetAlarm(clock, &alarm_read));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarm.bcd, alarm_read.bcd, 6);
}

// Hacer una prueba con frecuencia de reloj diferente
void test_clock_with_different_frequency(void) {
    clock = ClockCreate(10, SNOOZE_TIME);
    ClockSetTime(clock, &(clock_time_t){0});
    ClockSetAlarm(clock, &(clock_time_t){0});
    for (uint16_t i = 0; i < 10; i++) {
        ClockNewTick(clock);
    }
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0);
}

// Tratar de posponer la alarma con valores invalidos y verificar que los rechaza.
void test_snooze_with_zero_value(void) {
    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND, 0);
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));
    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
    ClockSnooze(clock);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

// La alarma esta activada

void test_is_alarm_enabled_or_disabled(void) {
    TEST_ASSERT_FALSE(ClockIsAlarmEnabled(clock));
    ClockSetAlarm(clock, &(clock_time_t){0});
    TEST_ASSERT_TRUE(ClockIsAlarmEnabled(clock));
    ClockDisableAlarm(clock);
    TEST_ASSERT_FALSE(ClockIsAlarmEnabled(clock));
}

/* === Private function definitions ================================================================================ */
static void SimulateSeconds(clock_t clock, uint8_t seconds) {
    for (uint16_t i = 0; i < CLOCK_TICKS_FOR_SECOND * seconds; i++) {
        ClockNewTick(clock);
    }
}
static void SimulateMinutes(clock_t clock, uint8_t minutes) {
    for (uint16_t i = 0; i < CLOCK_TICKS_FOR_SECOND * 60 * minutes; i++) {
        ClockNewTick(clock);
    }
}
static void SimulateHours(clock_t clock, uint8_t hours) {
    for (uint32_t i = 0; i < CLOCK_TICKS_FOR_SECOND * 60 * 60 * hours; i++) {
        ClockNewTick(clock);
    }
}
/* === End of documentation ======================================================================================== */
