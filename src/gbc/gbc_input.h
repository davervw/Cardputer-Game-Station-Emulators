#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void gbc_input_init(void);
int gbc_input_poll(void);

#ifdef __cplusplus
}
#endif
