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
 ** @brief Pruebas unitarias para la biblioteca de reloj despertador.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_FOR_SECOND 5 // Frecuencia del reloj
#define SNOOZE_TIME            5 // Tiempo de posposición de la alarma en minutos

/**
 * @brief Verifica que la hora actual coincida con los valores esperados.
 *
 * Esta macro compara la hora actual obtenida con los valores esperados para segundos, minutos y horas
 * en formato BCD no compactado.
 *
 * @param seconds_units  Unidades de los segundos esperados.
 * @param seconds_tens   Decenas de los segundos esperados.
 * @param minutes_units  Unidades de los minutos esperados.
 * @param minutes_tens   Decenas de los minutos esperados.
 * @param hours_units    Unidades de las horas esperadas.
 * @param hours_tens     Decenas de las horas esperadas.
 */
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

/**
 * @brief Simula el paso de una cantidad de segundos.
 *
 * @param clock  Instancia del reloj.
 * @param seconds Cantidad de segundos a simular.
 */
static void SimulateSeconds(clock_t clock, uint8_t seconds);

/**
 * @brief Simula el paso de una cantidad de minutos.
 *
 * @param clock  Instancia del reloj.
 * @param minutes Cantidad de minutos a simular.
 */
static void SimulateMinutes(clock_t clock, uint8_t minutes);

/**
 * @brief Simula el paso de una cantidad de horas.
 *
 * @param clock  Instancia del reloj.
 * @param hours Cantidad de horas a simular.
 */
static void SimulateHours(clock_t clock, uint8_t hours);
/* === Private variable definitions ================================================================================
 */

/* === Public variable definitions =================================================================================
 */

/* === Public function definitions ============================================================================== */

void setUp(void) {
    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND, SNOOZE_TIME);
}

/**
 * @test Verifica que el reloj comienza con una hora inválida.
 */
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {.bcd = {1, 2, 3, 4, 5, 6}};

    clock = ClockCreate(CLOCK_TICKS_FOR_SECOND, SNOOZE_TIME);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

/**
 * @test Verifica que el reloj puede ser ajustado correctamente con una hora válida.
 */
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_t new_time = {.time = {.seconds = {4, 5}, .minutes = {3, 0}, .hours = {4, 1}}};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));

    TEST_ASSERT_TIME(4, 5, 3, 0, 4, 1);
}
/**
 * @test Verifica que el reloj despues de n ciclicos puede avanzar 1 segundo.
 */
void test_clock_advance_one_second(void) {

    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 1);
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0);
}

/**
 * @test Verifica que el reloj despues de n ciclos puede avanzar 10 segundo.
 */
void test_clock_advance_ten_seconds(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 10);
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0);
}

/**
 * @test Verifica que el reloj despues de n ciclos puede avanzar 1 minuto.
 */
void test_clock_advance_one_minute(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 60);
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0);
}

/**
 * @test Verifica que el reloj despues de n ciclos puede avanzar 10 minutos.
 */
void test_clock_advance_ten_minutes(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateMinutes(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0);
}

/**
 * @test Verifica que el reloj despues de n ciclos puede avanzar 1 hora.
 */
void test_clock_advance_one_hour(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateMinutes(clock, 60);
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0);
}

/**
 * @test Verifica que el reloj despues de n ciclos puede avanzar 10 horas.
 */
void test_clock_advance_ten_hours(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateHours(clock, 10);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

/**
 * @test Verifica que el reloj despues de n ciclos la hora avanza un dia.
 */
void test_clock_advance_one_day(void) {
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateHours(clock, 24);
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0);
}

/**
 * @test Verifica que la alarma se puede fijar y consultar correctamente.
 */
void test_set_alarm_and_get(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {0, 3}, .hours = {8, 0}}};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));

    clock_time_t alarm_read = {0};

    TEST_ASSERT_TRUE_MESSAGE(ClockGetAlarm(clock, &alarm_read), "Error getting alarm time");
}

/**
 * @test Verifica que la alarma se puede fijar y una vez coincida la hora con la alarma suena.
 */
void test_alarm_tiggers_when_is_time(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));
    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

/**
 * @test Verifica que la alarma se puede fijar y una vez deshabilitada no suena.
 */
void test_alarm_is_disabled_so_not_trigger(void) {
    static const clock_time_t alarm_time = {.time = {.seconds = {0, 0}, .minutes = {1, 0}, .hours = {8, 0}}};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_time));
    ClockDisableAlarm(clock);
    ClockSetTime(clock, &(clock_time_t){.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {8, 0}}});
    SimulateMinutes(clock, 1);
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));
}

/**
 * @test Verifica que la alarma se puede posponer.
 */
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

/**
 * @test Verifica que la alarma se puede cancelar y al dia siguiente suena.
 */
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

/**
 * @test Verifica que la alarma una vez pospuesta espera el tiempo de posposicion para sonar.
 */
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

/**
 * @test Verifica que la alarma suena a la misma hora todos los dias, sin importar que la hayan pospuesto.
 */
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

/**
 * @test Verifica que la hora no puede tener valores nulos
 */
void test_get_time_with_null(void) {
    static const clock_time_t new_time = {.bcd = {0, 0, 0, 0, 0, 0}};
    ClockSetTime(clock, &new_time);
    TEST_ASSERT_FALSE(ClockGetTime(clock, NULL));
}

/**
 * @test Verifica que la hora no se puede ajustar con valores invalidos.
 */
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

/**
 * @test Verifica que la alarma no se puede ajustar con valores invalidos.
 */
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

/**
 * @test Verifica que el reloj funciona a distintas frecuencias.
 */
void test_clock_with_different_frequency(void) {
    clock = ClockCreate(10, SNOOZE_TIME);
    ClockSetTime(clock, &(clock_time_t){0});
    ClockSetAlarm(clock, &(clock_time_t){0});
    for (uint16_t i = 0; i < 10; i++) {
        ClockNewTick(clock);
    }
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0);
}

/**
 * @test Verifica que la alarma no se puede posponer si los valores son invalidos.
 */
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

/**
 * @test Verifica que la alarma esta activa.
 */
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
