#include <stdint.h>
#include <stdbool.h>

typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
} clock_time_t;

typedef struct clock_s * clock_t;

clock_t ClockCreate(uint16_t tick_for_second, uint8_t snooze);

/**
 * @brief Obtiene la hora actual del reloj.
 *
 * @param clock
 * @param result
 * @return true
 * @return false
 */
bool ClockGetTime(clock_t clock, clock_time_t * result);

/**
 * @brief Establecer la hora del reloj.
 *
 * @param clock
 * @param new_time
 * @return true
 * @return false
 */
bool ClockSetTime(clock_t clock, const clock_time_t * new_time);

/**
 * @brief Avanza el reloj un tick.
 *
 * @param clock
 */
void ClockNewTick(clock_t clock);

bool ClockSetAlarm(clock_t self, const clock_time_t * alarm_time);

bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time);

bool ClockIsAlarmTriggered(clock_t self);

void ClockDisableAlarm(clock_t self);

void ClockSnooze(clock_t self);

void ClockCancelAlarm(clock_t self);

bool ClockIsAlarmEnabled(clock_t self);