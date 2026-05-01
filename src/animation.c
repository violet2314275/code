#include "animation.h"
#include "hc595.h"
#include <string.h>
#include <stdlib.h>

static uint8_t frame_buffer[8];
static AnimationId current_anim = ANIM_WAVE_UP;
static uint32_t anim_tick = 0;

static void _set_bit(uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t idx = (uint16_t)z * 16 + (uint16_t)y * 4 + (uint16_t)x;
    frame_buffer[idx >> 3] |= (1 << (idx & 0x07));
}

static void _clr_bit(uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t idx = (uint16_t)z * 16 + (uint16_t)y * 4 + (uint16_t)x;
    frame_buffer[idx >> 3] &= ~(1 << (idx & 0x07));
}

static uint8_t _get_bit(uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t idx = (uint16_t)z * 16 + (uint16_t)y * 4 + (uint16_t)x;
    return (frame_buffer[idx >> 3] >> (idx & 0x07)) & 0x01;
}

LedMap ANIM_MapXYZ(uint8_t x, uint8_t y, uint8_t z)
{
    LedMap map;
    uint8_t col_idx;

    if (y < 2) {
        col_idx = y * 4 + x;
        map.col_hi = (uint8_t)(1 << (7 - col_idx));
        map.col_lo = 0x00;
    } else {
        col_idx = (y - 2) * 4 + x;
        map.col_lo = (uint8_t)(1 << (7 - col_idx));
        map.col_hi = 0x00;
    }

    map.layer = (uint8_t)(~(1 << z));
    return map;
}

void ANIM_Render(void)
{
    for (uint8_t layer = 0; layer < CUBE_LAYERS; layer++)
    {
        Hc595Frame frame;
        memset(&frame, 0, sizeof(frame));

        uint8_t layer_bits = 0xFF;
        layer_bits &= ~(1 << layer);
        frame.buf[2] = layer_bits;

        for (uint8_t y = 0; y < CUBE_SIZE; y++)
        {
            for (uint8_t x = 0; x < CUBE_SIZE; x++)
            {
                if (_get_bit(x, y, layer))
                {
                    LedMap m = ANIM_MapXYZ(x, y, layer);
                    frame.buf[0] |= m.col_hi;
                    frame.buf[1] |= m.col_lo;
                }
            }
        }

        HC595_Flush(&frame);
        delay_ms(2);
    }
}

void ANIM_Init(void)
{
    memset(frame_buffer, 0, sizeof(frame_buffer));
    current_anim = ANIM_WAVE_UP;
    anim_tick = 0;
}

void ANIM_SetVoxel(uint8_t x, uint8_t y, uint8_t z, uint8_t on)
{
    if (x >= CUBE_SIZE || y >= CUBE_SIZE || z >= CUBE_SIZE)
        return;
    if (on) _set_bit(x, y, z);
    else    _clr_bit(x, y, z);
}

void ANIM_ClrVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    ANIM_SetVoxel(x, y, z, 0);
}

uint8_t ANIM_GetVoxel(uint8_t x, uint8_t y, uint8_t z)
{
    if (x >= CUBE_SIZE || y >= CUBE_SIZE || z >= CUBE_SIZE)
        return 0;
    return _get_bit(x, y, z);
}

void ANIM_ClearFrame(void)
{
    memset(frame_buffer, 0, sizeof(frame_buffer));
}

void ANIM_SetAnimation(AnimationId id)
{
    if (id < ANIM_COUNT)
        current_anim = id;
}

static void _anim_wave(void)
{
    ANIM_ClearFrame();
    for (uint8_t z = 0; z < CUBE_LAYERS; z++)
    {
        uint8_t phase = (anim_tick + z * 2) % 8;
        if (phase < 4)
        {
            for (uint8_t x = 0; x < CUBE_SIZE; x++)
                for (uint8_t y = 0; y < CUBE_SIZE; y++)
                    _set_bit(x, y, z);
        }
    }
}

static void _anim_snake(void)
{
    static int8_t sx = 0, sy = 0, sz = 0;
    static uint8_t dir = 0;
    ANIM_ClearFrame();
    _set_bit(sx, sy, sz);
    if ((anim_tick & 0x03) == 0)
    {
        if (dir == 0) { sx++; if (sx >= 4) { sx = 3; dir = 1; } }
        else if (dir == 1) { sy++; if (sy >= 4) { sy = 3; dir = 2; } }
        else if (dir == 2) { sx--; if (sx < 0) { sx = 0; dir = 3; } }
        else if (dir == 3) { sy--; if (sy < 0) { sy = 0; sz = (sz + 1) % 4; dir = 0; } }
    }
}

static void _anim_sparkle(void)
{
    ANIM_ClearFrame();
    for (uint8_t i = 0; i < 6; i++)
    {
        uint8_t rx = rand() % 4;
        uint8_t ry = rand() % 4;
        uint8_t rz = rand() % 4;
        _set_bit(rx, ry, rz);
    }
}

static void _anim_sine(void)
{
    ANIM_ClearFrame();
    for (uint8_t x = 0; x < CUBE_SIZE; x++)
    {
        for (uint8_t z = 0; z < CUBE_LAYERS; z++)
        {
            float angle = (float)(anim_tick * 3 + x * 20) * 3.14159f / 180.0f;
            int16_t y_offset = (int16_t)((sinf(angle) + 1.0f) * 1.5f);
            if (y_offset >= 0 && y_offset < 4)
                _set_bit(x, (uint8_t)y_offset, z);
        }
    }
}

static void _anim_plane_rotate(void)
{
    ANIM_ClearFrame();
    uint8_t plane = anim_tick % 4;
    for (uint8_t x = 0; x < CUBE_SIZE; x++)
        for (uint8_t y = 0; y < CUBE_SIZE; y++)
            _set_bit(x, y, plane);
}

void ANIM_Tick(void)
{
    anim_tick++;
    switch (current_anim)
    {
    case ANIM_WAVE_UP:
    case ANIM_WAVE_DOWN:
        _anim_wave();
        break;
    case ANIM_SNAKE_3D:
        _anim_snake();
        break;
    case ANIM_RANDOM_SPARKLE:
        _anim_sparkle();
        break;
    case ANIM_SINEWAVE:
        _anim_sine();
        break;
    case ANIM_PLANE_ROTATE:
        _anim_plane_rotate();
        break;
    default:
        break;
    }
}
