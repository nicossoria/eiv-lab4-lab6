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

/** @file digital.c
 ** @brief Codigo fuente del modulo para la gestion de entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include "config.h"
#include "chip.h"
#include "digital.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

//! Estructura que representa una salida digital
struct digital_output_s {
    uint8_t port; //!< Puerto de la salida digital
    uint8_t pin;  //!< Pin de la salida digital
};

//! Estructura que representa una entrada digital
struct digital_input_s {
    uint8_t port;   //!< Puerto de la entrada digital
    uint8_t pin;    //!< Pin de la entrada digital
    bool inverted;  //!< Indica si la entrada es invertida
    bool lastState; //!< Indica el estado anterior de la entrada
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin) {
    digital_output_t self = malloc(sizeof(struct digital_output_s));
    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, true);
    }

    return self;
}

void DigitalOutputActivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, true);
}

void DigitalOutputDeactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
}

void DigitalOutputToggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

digital_input_t DigitalInputCreate(uint8_t port, uint8_t pin, bool inverted) {
    digital_input_t self = malloc(sizeof(struct digital_input_s));
    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        self->inverted = inverted;
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, false);
        self->lastState = DigitalInputGetIsActive(self);
    }

    return self;
}

bool DigitalInputGetIsActive(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin) != 0;
    if (self->inverted) {
        state = !state;
    }
    return state;
}

bool DigitalWasActive(digital_input_t self) {
    return DIGITAL_INPUT_WAS_ACTIVATED == DigitalWasChanged(self);
}

bool DigitalWasInactive(digital_input_t self) {
    return DIGITAL_INPUT_WAS_DEACTIVATED == DigitalWasChanged(self);
}

digital_states_t DigitalWasChanged(digital_input_t self) {
    digital_states_t result = DIGITAL_INPUT_NO_CHANGE;

    bool state = DigitalInputGetIsActive(self);

    if (state && !self->lastState) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if (!state && self->lastState) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }
    self->lastState = state;
    return result;
}

/* === Private function definitions ================================================================================ */

/* === End of documentation ======================================================================================== */
