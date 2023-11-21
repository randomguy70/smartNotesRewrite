#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void copyBytes(uint8_t *dest, uint8_t *src, int num);

#ifdef __cplusplus
}
#endif

#endif