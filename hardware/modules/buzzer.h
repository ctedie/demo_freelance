#ifndef MODULES_BUZZER_H_
#define MODULES_BUZZER_H_

#include <stdint.h>

void Buzzer_Init(void);
void Buzzer_PlayNote(uint32_t frequency);
void Buzzer_Stop(void);

#endif /* MODULES_BUZZER_H_ */
