#ifndef FLIGHTCONTROL_OSD_H_
#define FLIGHTCONTROL_OSD_H

#include <inttypes.h>
#include "max7456.h"
#include "max7456Helper.h"
#include "telemetry.h"

#define OSD_UART_BAUDRATE 115200
#define OSD_BOOT_TIME_MS 1000

uint8_t initOSD(void);
void updateOSD(void);

#endif
