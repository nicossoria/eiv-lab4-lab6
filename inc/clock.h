/*********************************************************************************************************************
Copyright (c) 2025, Martin Nicolas Soria <soria.m.nicolas@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief Declaraciones de la biblioteca para la gestión de un reloj despertador con alarma.
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estructura para representar una hora en formato BCD no compactado.
 *
 * La hora se almacena como un arreglo de 6 bytes:
 * - bcd[0]: Unidades de los segundos
 * - bcd[1]: Decenas de los segundos
 * - bcd[2]: Unidades de los minutos
 * - bcd[3]: Decenas de los minutos
 * - bcd[4]: Unidades de las horas
 * - bcd[5]: Decenas de las horas
 */

typedef union {
    struct {
        uint8_t seconds[2]; /*!< Segundos en formato BCD (unidades y decenas) */
        uint8_t minutes[2]; /*!< Minutos en formato BCD (unidades y decenas) */
        uint8_t hours[2];   /*!< Horas en formato BCD (unidades y decenas) */
    } time;
    uint8_t bcd[6]; /*!< Hora en formato BCD no compactado */
} clock_time_t;

/**
 * @brief Puntero opaco a la estructura del reloj.
 */
typedef struct clock_s * clock_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea e inicializa una nueva instancia del reloj.
 *
 * @param tick_for_second Cantidad de ticks que representan un segundo.
 * @param snooze Tiempo de posposición de la alarma en minutos.
 * @return clock_t Instancia creada del reloj.
 */
clock_t ClockCreate(uint16_t tick_for_second, uint8_t snooze);

/**
 * @brief Obtiene la hora actual del reloj.
 *
 * @param clock Instancia del reloj.
 * @param result Puntero donde se almacenará la hora actual.
 * @return true Si la hora es válida.
 * @return false Si la hora es inválida (antes de la primera configuración).
 */
bool ClockGetTime(clock_t clock, clock_time_t * result);

/**
 * @brief Establece la hora actual del reloj.
 *
 * @param clock Instancia del reloj.
 * @param new_time Puntero a la nueva hora a configurar.
 * @return true Si la hora fue configurada correctamente.
 * @return false Si los valores ingresados no son válidos.
 */
bool ClockSetTime(clock_t clock, const clock_time_t * new_time);

/**
 * @brief Avanza el reloj un tick.
 *
 * @param clock Instancia del reloj.
 */
void ClockNewTick(clock_t clock);


/**
 * @brief Configura la hora de la alarma.
 *
 * @param clock Instancia del reloj.
 * @param alarm_time Puntero a la hora de la alarma.
 * @return true Si la alarma fue configurada correctamente.
 * @return false Si los valores ingresados no son válidos.
 */
bool ClockSetAlarm(clock_t self, const clock_time_t * alarm_time);

/**
 * @brief Obtiene la hora configurada para la alarma.
 *
 * @param clock Instancia del reloj.
 * @param alarm_time Puntero donde se almacenará la hora de la alarma.
 * @return true Si la hora de la alarma fue obtenida correctamente.
 * @return false Si la alarma no está configurada.
 */
bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time);

/**
 * @brief Indica si la alarma debe activarse en la hora actual.
 *
 * @param clock Instancia del reloj.
 * @return true Si la alarma debe sonar.
 * @return false Si la alarma no debe sonar.
 */
bool ClockIsAlarmTriggered(clock_t self);

/**
 * @brief Deshabilita temporalmente la alarma.
 *
 * @param clock Instancia del reloj.
 */
void ClockDisableAlarm(clock_t self);

/**
 * @brief Pospone la alarma una cantidad de minutos configurada.
 *
 * @param clock Instancia del reloj.
 */
void ClockSnooze(clock_t self);

/**
 * @brief Cancela la alarma hasta el próximo día.
 *
 * @param clock Instancia del reloj.
 */
void ClockCancelAlarm(clock_t self);

/**
 * @brief Indica si la alarma está habilitada.
 *
 * @param clock Instancia del reloj.
 * @return true Si la alarma está habilitada.
 * @return false Si la alarma está deshabilitada.
 */
bool ClockIsAlarmEnabled(clock_t self);

/**
 * @brief Habilita la alarma.
 *
 * @param clock Instancia del reloj.
 */
void AlarmLedOn(digital_output_t alarm_led);

/**
 * @brief Deshabilita la alarma.
 *
 * @param clock Instancia del reloj.
 */
void AlarmLedOff(digital_output_t alarm_led);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */