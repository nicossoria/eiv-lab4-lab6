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

/** @file screen.c
 ** @brief Implementación del modulo para la gestion de una pantalla de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include "screen.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

struct screen_s {
    uint8_t digits;
    uint8_t current_digit;

    uint8_t flashing_from;
    uint8_t flashing_to;
    uint16_t flashing_frecuency;
    uint8_t flashing_count;

    uint8_t point_flash_from;
    uint8_t point_flash_to;
    uint8_t point_flash_frecuency;
    uint8_t point_flash_count;

    screen_driver_t driver;

    uint8_t value[SCREEN_MAX_DIGITS];
    bool point_enabled[SCREEN_MAX_DIGITS];
};

/* === Private function declarations =============================================================================== */

static const uint8_t IMAGES[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};


/**
 * @brief Inicializa los pines asociados a los dígitos de la pantalla
 */
void DigitsInit(void);

/**
 * @brief Inicializa los pines asociados a los segmentos de la pantalla
 */
void SegmentsInit(void);
/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

screen_t ScreenCreate(uint8_t digits, screen_driver_t driver) {

    screen_t self = malloc(sizeof(struct screen_s));
    if (digits > SCREEN_MAX_DIGITS) {
        digits = SCREEN_MAX_DIGITS;
    }
    if (self != NULL) {
        self->digits = digits;
        self->driver = driver;
        self->current_digit = 0;
        self->flashing_count = 0;
        self->flashing_frecuency = 0;
        self->point_flash_count = 0;
        self->point_flash_frecuency = 0;
    }
    return self;
}

void ScreenWriteBCD(screen_t self, uint8_t value[], uint8_t size) {
    memset(self->value, 0, sizeof(self->value));

    if (size > self->digits) {
        size = self->digits;
    }
    for (uint8_t i = 0; i < size; i++) {
        self->value[i] = IMAGES[value[i]];
    }
}

void ScreenRefresh(screen_t self) {
    uint8_t segments;

    self->driver->DigitsTurnOff();
    self->current_digit = (self->current_digit + 1) % self->digits;

    segments = self->value[self->current_digit];

    if (self->current_digit == 0) {
        if (self->flashing_frecuency) {
            self->flashing_count = (self->flashing_count + 1) % self->flashing_frecuency;
        }
        if (self->point_flash_frecuency) {
            self->point_flash_count = (self->point_flash_count + 1) % self->point_flash_frecuency;
        }
    }

    if (self->flashing_frecuency) {
        if (self->flashing_count < (self->flashing_frecuency / 2)) {
            if ((self->current_digit >= self->flashing_from) && (self->current_digit <= self->flashing_to)) {
                segments = 0;
            }
        }
    }

    if (self->point_enabled[self->current_digit]) {
        segments |= SEGMENT_P;
    }

    if (self->point_flash_frecuency) {
        if (self->point_flash_count < (self->point_flash_frecuency / 2)) {
            if ((self->current_digit >= self->point_flash_from) && (self->current_digit <= self->point_flash_to)) {
                segments &= ~SEGMENT_P;
            }
        }
    }

    self->driver->SegmentsUpdate(segments);
    self->driver->DigitTurnOn(self->current_digit);
}

int DisplayFlashDigit(screen_t self, uint8_t from, uint8_t to, uint8_t divisor) {
    int result = 0;
    if (from > to || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)) {
        result = -1;
    } else if (!self) {
        result = -1;
    } else {
        if (self) {
            self->flashing_from = from;
            self->flashing_to = to;
            self->flashing_frecuency = 2 * divisor;
            self->flashing_count = 0;
        }
    }
    return result;
}

void ScreenEnablePoint(screen_t self, uint8_t digit) {
    if (self && digit < self->digits) {
        self->point_enabled[digit] = true;
    }
}

void ScreenDisablePoint(screen_t self, uint8_t digit) {
    if (self && digit < self->digits) {
        self->point_enabled[digit] = false;
    }
}

int DisplayFlashPoints(screen_t self, uint8_t from, uint8_t to, uint8_t divisor) {
    int result = 0;
    if (from > to || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)) {
        result = -1;
    } else if (!self) {
        result = -1;
    } else {
        if (self) {
            self->point_flash_from = from;
            self->point_flash_to = to;
            self->point_flash_frecuency = 2 * divisor;
            self->point_flash_count = 0;
        }
    }
    return result;
}
/* === Private function definitions ================================================================================ */

/* === End of documentation ======================================================================================== */
