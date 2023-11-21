#include "system.h"

#include <stdint.h>

void copyBytes(uint8_t *dest, uint8_t *src, int num)
{
    for(int i = 0; i < num; i++)
    {
        dest[i] = src[i];
    }
}