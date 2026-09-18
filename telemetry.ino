uint8_t parseTelemetry(char* line) {
  char* token = strtok(line, ",");
  if (!token) return false;
  telemetry.rxBat = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.rxBatMax = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.alt = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.homeDistance = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.heading = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.headingRef = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.verticalSpeed = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.groundSpeed = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  {
    uint8_t satCountAndReliable = (uint8_t)atoi(token);
    telemetry.satCount     = satCountAndReliable & 0x3F;
    telemetry.gnssReliable = (satCountAndReliable >> 6) & 0x01;
  }

  token = strtok(NULL, ",");
  if (!token) return false;
  strncpy(telemetry.fm, token, sizeof(telemetry.fm) - 1);
  telemetry.fm[sizeof(telemetry.fm) - 1] = '\0';

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.pitch = atof(token);

  token = strtok(NULL, ",");
  if (!token) return false;
  telemetry.roll = atof(token);

  token = strtok(NULL, ",");           // NEW
  if (!token) return false;
  telemetry.throttle = (uint16_t)strtoul(token, NULL, 10);

  token = strtok(NULL, ",");           // NEW
  if (!token) return false;
  telemetry.batteryAlertState = (uint8_t)atoi(token);

  return 1;
}