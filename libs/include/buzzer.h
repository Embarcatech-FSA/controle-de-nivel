#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include "main.h"

void buzzer_init();
void buzzer_play_tone(uint freq, uint duration_ms);

#endif // BUZZER_H