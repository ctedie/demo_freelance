#ifndef MODULES_JOYSTICK_H_
#define MODULES_JOYSTICK_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t x;
    uint32_t y;
    bool btnSelect;
    bool btnS1;
    bool btnS2;
} Joystick_State_t;

void Joystick_Init(void);
void Joystick_ReadState(Joystick_State_t* state);

#endif /* MODULES_JOYSTICK_H_ */
