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

int main(void) {
    board_t board = board_create();

    uint8_t value[4] = {1, 2, 3, 4};
    bool digit_flashing = true;
    bool point_flashing = true;

    ScreenWriteBCD(board->screen, value, 4);

    DisplayFlashDigit(board->screen, 0, 1, 150);

    ScreenEnablePoint(board->screen, 2);
    ScreenEnablePoint(board->screen, 3);

    DisplayFlashPoints(board->screen, 2, 3, 150);

    while (true) {

        ScreenRefresh(board->screen);

        // Leer teclas
        if (DigitalInputGetIsActive(board->set_time)) {
            value[0] = (value[0] + 1) % 10;
            ScreenWriteBCD(board->screen, value, 4);
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        if (DigitalInputGetIsActive(board->set_alarm)) {
            value[0] = (value[0] == 0) ? 9 : value[0] - 1;
            ScreenWriteBCD(board->screen, value, 4);
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        if (DigitalInputGetIsActive(board->decrement)) {
            static bool point0_on = false;
            point0_on = !point0_on;
            if (point0_on) {
                ScreenEnablePoint(board->screen, 0);
            } else {
                ScreenDisablePoint(board->screen, 0);
            }
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        if (DigitalInputGetIsActive(board->increment)) {
            static bool point1_on = false;
            point1_on = !point1_on;
            if (point1_on) {
                ScreenEnablePoint(board->screen, 1);
            } else {
                ScreenDisablePoint(board->screen, 1);
            }
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        if (DigitalInputGetIsActive(board->accept)) {
            digit_flashing = !digit_flashing;
            if (digit_flashing) {
                DisplayFlashDigit(board->screen, 0, 1, 150);
            } else {
                DisplayFlashDigit(board->screen, 4, 4, 150);
            }
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        if (DigitalInputGetIsActive(board->cancel)) {
            point_flashing = !point_flashing;
            if (point_flashing) {
                DisplayFlashPoints(board->screen, 2, 3, 150);
            } else {
                DisplayFlashPoints(board->screen, 4, 4, 150);
            }
            for (int i = 0; i < 100000; i++) {
                __asm("NOP");
            }
        }

        for (int i = 0; i < 10000; i++) {
            __asm("NOP");
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
