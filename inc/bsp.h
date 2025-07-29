/*********************************************************************************************************************
Copyright (c) 2025, Soria Martin Nicolas <soria.m.nicolas@gmail.com>

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

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Definiciones de los pines de la placa de desarrollo
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
#include "poncho.h"
#include "chip.h"
#include "digital.h"
#include "screen.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define LED_R_PORT 2
#define LED_R_PIN  0
#define LED_R_FUNC SCU_MODE_FUNC4
#define LED_R_GPIO 5
#define LED_R_BIT  0

#define LED_G_PORT 2
#define LED_G_PIN  1
#define LED_G_FUNC SCU_MODE_FUNC4
#define LED_G_GPIO 5
#define LED_G_BIT  1

#define LED_B_PORT 2
#define LED_B_PIN  2
#define LED_B_FUNC SCU_MODE_FUNC4
#define LED_B_GPIO 5
#define LED_B_BIT  2

#define LED_1_PORT 2
#define LED_1_PIN  10
#define LED_1_FUNC SCU_MODE_FUNC0
#define LED_1_GPIO 0
#define LED_1_BIT  14

#define LED_2_PORT 2
#define LED_2_PIN  11
#define LED_2_FUNC SCU_MODE_FUNC0
#define LED_2_GPIO 1
#define LED_2_BIT  11

#define LED_3_PORT 2
#define LED_3_PIN  12
#define LED_3_FUNC SCU_MODE_FUNC0
#define LED_3_GPIO 1
#define LED_3_BIT  12

#define TEC_1_PORT 1
#define TEC_1_PIN  0
#define TEC_1_FUNC SCU_MODE_FUNC0
#define TEC_1_GPIO 0
#define TEC_1_BIT  4

#define TEC_2_PORT 1
#define TEC_2_PIN  1
#define TEC_2_FUNC SCU_MODE_FUNC0
#define TEC_2_GPIO 0
#define TEC_2_BIT  8

#define TEC_3_PORT 1
#define TEC_3_PIN  2
#define TEC_3_FUNC SCU_MODE_FUNC0
#define TEC_3_GPIO 0
#define TEC_3_BIT  9

#define TEC_4_PORT 1
#define TEC_4_PIN  6
#define TEC_4_FUNC SCU_MODE_FUNC0
#define TEC_4_GPIO 1
#define TEC_4_BIT  9

/* === Public data type declarations =============================================================================== */

/**
 * @brief Tipo que representa la placa base del sistema.
 *
 * No debe ser modificada directamente por el usuario.
 */
typedef struct board_s {
    digital_output_t alarm_led;    /**< Indicador de la alarma */
    digital_input_t set_time;   /**< Botón para configurar la hora */
    digital_input_t set_alarm;  /**< Botón para configurar la alarma */
    digital_input_t decrement;  /**< Botón para decrementar valores */
    digital_input_t increment;  /**< Botón para incrementar valores */
    digital_input_t accept;     /**< Botón para confirmar selección */
    digital_input_t cancel;     /**< Botón para cancelar selección */
    screen_t screen;            /**< Pantalla de 7 segmentos */
} const * const board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea una estructura de tipo board_t que representa la placa de desarrollo.
 * @return Un puntero a la estructura board_t creada.
 */
board_t board_create(void);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP.H */
