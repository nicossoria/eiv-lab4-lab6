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

#ifndef SCREEN_H_
#define SCREEN_H_

/** @file screen.h
 ** @brief Declaraciones del modulo para la gestion de una pantalla de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)



/* === Public data type declarations =============================================================================== */
/** @brief Estructura privada para el controlador de pantalla */
typedef struct screen_s * screen_t;

/** @brief Funcion para apagar todos los digitos */
typedef void (*digits_turn_of_t)(void);

/** @brief Funcion para actualizar los segmentos de un digito */
typedef void (*digits_update_t)(uint8_t);

/** @brief Funcion para encender un digito especifico */
typedef void (*digit_turn_on_t)(uint8_t);

/** @brief Driver para controlar la pantalla de 7 segmentos*/
typedef struct screen_driver_s {
    digits_turn_of_t DigitsTurnOff;
    digits_update_t SegmentsUpdate;
    digit_turn_on_t DigitTurnOn;
} const * screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea una instancia de pantalla de 7 segmentos
 * 
 * @param digits Cantidad de dígitos a usar
 * @param driver Driver con funciones para controlar los pines
 * @return screen_t Puntero al objeto pantalla creado
 */
screen_t ScreenCreate(uint8_t digits, screen_driver_t driver);


/**
 * @brief Muestra un arreglo de dígitos BCD en la pantalla
 *
 * @param screen Pantalla a modificar
 * @param value Arreglo de valores BCD
 * @param size Cantidad de valores en el arreglo
 */
void ScreenWriteBCD(screen_t screen, uint8_t value[], uint8_t size);


/**
 * @brief Actualiza el estado visual de un dígito de la pantalla
 *
 * @param screen Pantalla a actualizar
 */
void ScreenRefresh(screen_t screen);


/**
 * @brief Hace parpadear un rango de dígitos de la pantalla
 *
 * @param self Instancia de pantalla
 * @param from Dígito inicial
 * @param to Dígito final
 * @param divisor Controla la velocidad del parpadeo
 * @return 0 si fue exitoso, -1 si hubo error
 */
int DisplayFlashDigit(screen_t self, uint8_t from, uint8_t to, uint8_t divisor);


/**
 * @brief Habilita el punto decimal de un dígito
 * 
 * @param self Instancia de pantalla
 * @param digit Índice del dígito
 */
void ScreenEnablePoint(screen_t self, uint8_t digit);


/**
 * @brief Deshabilita el punto decimal de un dígito
 * 
 * @param self Instancia de pantalla
 * @param digit Índice del dígito
 */
void ScreenDisablePoint(screen_t self, uint8_t digit);


/**
 * @brief Hace parpadear los puntos decimales de un rango de dígitos
 * 
 * @param self Instancia de pantalla
 * @param from Índice inicial
 * @param to Índice final
 * @param divisor Controla la velocidad del parpadeo
 * @return 0 si fue exitoso, -1 si hubo error
 */
int DisplayFlashPoints(screen_t self, uint8_t from, uint8_t to, uint8_t divisor);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
