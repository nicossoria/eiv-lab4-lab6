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

#ifndef DIGITAL_H
#define DIGITAL_H

/** @file digital.h
 ** @brief Declaraciones del modulo para la gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

typedef enum digital_states_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1,
    DIGITAL_INPUT_NO_CHANGE = 0,
    DIGITAL_INPUT_WAS_ACTIVATED = 1,
} digital_states_t;

//! Estructura que representa una salida digital
typedef struct digital_output_s * digital_output_t;

typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

//! Estructura que contiene los datos de un alumno
// typedef struct alumno_s * alumno_t;

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear una salida digital
 *
 * @param gpio
 * @param bit
 * @return digital_output_t
 */
digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit);

/**
 * @brief Funcion para activar una salida digital
 *
 * @param self
 */
void DigitalOutputActivate(digital_output_t self);

/**
 * @brief Funcion para desactivar una salida digital
 *
 * @param self
 */
void DigitalOutputDeactivate(digital_output_t self);

/**
 * @brief Funcion para alternar el estado de una salida digital
 *
 * @param self
 */
void DigitalOutputToggle(digital_output_t self);

/**
 * @brief Funcion para crear una entrada digital
 *
 * @param gpio
 * @param bit
 * @return digital_input_t
 */
digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted);

bool DigitalInputGetIsActive(digital_input_t input);

bool DigitalWasActive(digital_input_t input); //Opcion mas facil

bool DigitalWasInactive(digital_input_t input); //Opcion mas facil

digital_states_t DigitalWasChanged(digital_input_t input);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITALES_ */
