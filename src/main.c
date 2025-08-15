/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "app.h"
#include "bsp.h"

#include "FreeRTOS.h"
#include "task.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ================================================================================= */

int main(void) {
    board_t board = AppInit();

    /* Crear tareas */
    xTaskCreate(TaskClock,   "clk",   256, board, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(TaskButtons, "keys",  256, board, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(TaskUI,      "ui",    512, board, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    while (1) { /* debería no llegar aquí */ }
}

/* === Private function definitions ================================================================================ */

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
