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

#ifndef APP_H_
#define APP_H_

/** @file app.h
 ** @brief Declaraciones del modulo del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#include "bsp.h"
#include "clock.h"
#include "screen.h"
#include "digital.h"
#include "FreeRTOS.h"
#include "task.h"

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/** Inicializa BSP, display y reloj. Devuelve el handle de board para pasarlo a las tareas. */
board_t AppInit(void);

/**
 * @brief Tarea que gestiona el reloj, actualizando el tiempo y la alarma.
 * 
 * @param parameters 
 */
void TaskClock(void * parameters);

/**
 * @brief Tarea que gestiona los botones del reloj, permitiendo configurar la hora y la alarma.
 * 
 * @param parameters 
 */
void TaskButtons(void * parameters);

/**
 * @brief Tarea que gestiona la interfaz de usuario, actualizando la pantalla y mostrando la hora y la alarma.
 * 
 * @param parameters 
 */
void TaskUI(void * parameters);


/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */
