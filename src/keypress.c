#include "keypress.h"

#include <keypadc.h>
#include <stdint.h>
#include <ti/getcsc.h>

sk_key_t getSingleCSCKey(void) {
	static uint8_t last_key;
	uint8_t only_key = 0;
	kb_Scan();
	for (uint8_t key = 1, group = 7; group; --group) {
		for (uint8_t mask = 1; mask; mask <<= 1, ++key) {
			if (kb_Data[group] & mask) {
				if (only_key) {
					last_key = 0;
					return 0;
				}
				else {
					only_key = key;
				}
			}
		}
	}
	if (only_key == last_key)
	{
		return 0;
	}
	last_key = only_key;
	return only_key;
}