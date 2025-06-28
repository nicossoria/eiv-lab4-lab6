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

#include "bsp.h"
#include "app.h"
#include <stdbool.h>
#include <stdint.h>
/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */

/* Errores:
 - Display 4 lo toma como posicion 0, el 3 como posicion 2, el 2 como posicion 3 y el 1 como posicion 4.
 - Parpadea muy rapido y no lo hace 2 display a la vez.
*/

#include "bsp.h"
#include "screen.h"
#include <stdbool.h>
#include <stdint.h>

void Delay1ms(void) {
    for (volatile int i = 0; i < 8000; i++) {
        __asm("NOP");
    }
}

int main(void) {
    board_t board = AppInit();

    while (true) {
        ScreenRefresh(board->screen);  // Muy seguido, para multiplexar bien
        Delay1ms();

        static uint8_t refresh_counter = 0;
        refresh_counter++;
        if (refresh_counter >= 20) {  // AppRun cada 20ms
            AppRun(board);
            refresh_counter = 0;
        }
    }

    return 0;

/*
    board_t board = board_create();

    uint8_t bcd_test[4] = {0, 1, 2, 3};  // Valores a mostrar en display

    // Apagar todos los puntos al inicio
    for (int i = 0; i < 4; i++) {
        ScreenDisablePoint(board->screen, i);
    }

    while (true) {
        // Refresca pantalla
        ScreenWriteBCD(board->screen, bcd_test, 4);
        ScreenRefresh(board->screen);

        // ======================
        // F1 - Incrementar unidad (posición 0)
        if (DigitalInputGetIsActive(board->set_time)) {
            bcd_test[0]++;
            if (bcd_test[0] > 9) bcd_test[0] = 0;
            while (DigitalInputGetIsActive(board->set_time));  // Espera que se suelte
        }

        // F2 - Incrementar decena (posición 1)
        if (DigitalInputGetIsActive(board->set_alarm)) {
            bcd_test[1]++;
            if (bcd_test[1] > 9) bcd_test[1] = 0;
            while (DigitalInputGetIsActive(board->set_alarm));
        }

        // F3 - Decrementar unidad (posición 0)
        if (DigitalInputGetIsActive(board->decrement)) {
            if (bcd_test[0] == 0)
                bcd_test[0] = 9;
            else
                bcd_test[0]--;
            while (DigitalInputGetIsActive(board->decrement));
        }

        // F4 - Decrementar decena (posición 1)
        if (DigitalInputGetIsActive(board->increment)) {
            if (bcd_test[1] == 0)
                bcd_test[1] = 9;
            else
                bcd_test[1]--;
            while (DigitalInputGetIsActive(board->increment));
        }

        // ACEPTAR - Enciende punto del dígito 0
        if (DigitalInputGetIsActive(board->accept)) {
            ScreenEnablePoint(board->screen, 0);
            while (DigitalInputGetIsActive(board->accept));
        }

        // CANCELAR - Apaga punto del dígito 0
        if (DigitalInputGetIsActive(board->cancel)) {
            ScreenDisablePoint(board->screen, 0);
            while (DigitalInputGetIsActive(board->cancel));
        }

        // Delay artificial
        for (volatile int i = 0; i < 100000; i++) {
            __asm("NOP");
        }
    }

    return 0;
    */
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
