#ifndef TELEMETRY_H_
#define TELEMETRY_H_

struct TelemetryData {
  float   rxBat;
  float   rxBatMax;
  float   alt;
  float   homeDistance;
  float   heading;
  float   headingRef;
  float   verticalSpeed;
  float   groundSpeed;
  uint8_t satCount;      // 0–63
  uint8_t gnssReliable;  // isNavDataReliable && isPositionHomeSet (0 or 1)
  char    fm[10];        // 9 chars + null — see FM_STATUS_BUF format above
  float   pitch;
  float   roll;
   uint16_t throttle;        
  uint8_t  batteryAlertState; 
};

uint8_t parseTelemetry(char* line);

#endif