#ifndef __ANIMATION_H
#define __ANIMATION_H

#include "stm32f10x.h"
#include "hc595.h"

#define CUBE_SIZE     4
#define CUBE_LAYERS   4
#define CUBE_COLUMNS  16
#define LED_COUNT     (CUBE_SIZE * CUBE_SIZE * CUBE_LAYERS)

typedef struct {
    uint8_t col_hi;
    uint8_t col_lo;
    uint8_t layer;
} LedMap;

typedef enum {
    ANIM_WAVE_UP,
    ANIM_WAVE_DOWN,
    ANIM_SNAKE_3D,
    ANIM_PLANE_ROTATE,
    ANIM_RANDOM_SPARKLE,
    ANIM_SINEWAVE,
    ANIM_COUNT
} AnimationId;

void ANIM_Init(void);
void ANIM_SetVoxel(uint8_t x, uint8_t y, uint8_t z, uint8_t on);
void ANIM_ClrVoxel(uint8_t x, uint8_t y, uint8_t z);
uint8_t ANIM_GetVoxel(uint8_t x, uint8_t y, uint8_t z);
void ANIM_ClearFrame(void);
void ANIM_Render(void);
void ANIM_SetAnimation(AnimationId id);
void ANIM_Tick(void);
LedMap ANIM_MapXYZ(uint8_t x, uint8_t y, uint8_t z);

#endif
