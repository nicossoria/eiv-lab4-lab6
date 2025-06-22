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
/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

/**
- , diez minutos, una hora, diez horas
y un día completo.
- Fijar la hora de la alarma y consultarla.
- Fijar la alarma y avanzar el reloj para que suene.
- Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
- Hacer sonar la alarma y posponerla.
- Hacer sonar la alarma y cancelarla hasta el otro dia..
- Probar get_time con NULL;
- Tratar de  ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
- Hacer una prueba con frecuencia de reloj diferente
*/

void setUp(void) {
    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND);
}

// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {.bcd = {1, 2, 3, 4, 5, 6}};

    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND);
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

/* === Private function definitions ================================================================================ */
static void SimulateSeconds(clock_t clock, uint8_t seconds) {
    for (uint16_t i = 0; i < CLOCK_TICKS_FOR_SECOND * seconds; i++) {
        ClockNewTick(clock);
    }
}
/* === End of documentation ======================================================================================== */
