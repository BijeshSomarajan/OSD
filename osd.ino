#include "osd.h"
TelemetryData telemetry;

#define MAX_LINE_LENGTH 100
#define HORIZON_HALF_WIDTH 4
#define HORIZON_PITCH_SCALE 0.5f
#define HORIZON_ROLL_ASPECT 2.5f

#define FIXED_REF_HALF_GAP 1  // columns beyond the moving horizon's outer edge
#define HORIZON_CENTER_COL_OFFSET -3
#define VERTICAL_CENTER_COL_OFFSET -2
#define COL_PADDING_RIGHT 3
#define COL_PADDING_LEFT 0
#define BATTERY_BLINK_INTERVAL_LOW_MS 700       // batteryAlertState == 1 (normal low)
#define BATTERY_BLINK_INTERVAL_CRITICAL_MS 200  // batteryAlertState == 2 (critically low) — faster
#define HORIZON_MAX_ANGLE 12.0f  // degrees — clamp pitch/roll input before drawing

char lineBuffer[MAX_LINE_LENGTH];
uint8_t lineIndex = 0;
uint8_t hasNewData = 0;
const uint8_t ROTATION_SYMBOLS[16] = {
  SYMBOL_ROT_0, SYMBOL_ROT_23, SYMBOL_ROT_45, SYMBOL_ROT_68,
  SYMBOL_ROT_90, SYMBOL_ROT_113, SYMBOL_ROT_135, SYMBOL_ROT_158,
  SYMBOL_ROT_180, SYMBOL_ROT_203, SYMBOL_ROT_225, SYMBOL_ROT_248,
  SYMBOL_ROT_270, SYMBOL_ROT_293, SYMBOL_ROT_315, SYMBOL_ROT_338
};

uint8_t initOSD() {
  Serial.begin(OSD_UART_BAUDRATE, SERIAL_8N2);
  Serial.setTimeout(0);
  delay(OSD_BOOT_TIME_MS);
  initMax7456();
  processLine("0.0,0.0,0,0,0,0,0,0,0,O-N-B-F-N,0,0,,0\n");
}

void serialEvent() {
  while (Serial.available()) {
    uint8_t data = (uint8_t)Serial.read();
    if (data == '\n') {
      lineBuffer[lineIndex] = '\0';  // null-terminate the string
      processLine(lineBuffer);       // handle the completed line
      lineIndex = 0;                 // reset for the next line
    } else if (data != '\r') {       // ignore CR (common with \r\n endings)
      if (lineIndex < MAX_LINE_LENGTH - 1) {
        lineBuffer[lineIndex++] = (char)data;
      } else {
        // line too long, discard and reset to avoid overflow/corruption
        lineIndex = 0;
      }
    }
  }
}

void processLine(char* line) {
  if (strcmp(line, "OSD?") == 0) {
    Serial.println("Y");
  } else {
    parseTelemetry(line);
    hasNewData = 1;
  }
}

uint8_t getNumericWidth(float value, uint8_t precision) {
  uint8_t len = precision + 1;  // Includes the decimal point
  float absValue = fabsf(value);
  if (absValue >= 1000.0f) {
    len += 4;
  } else if (absValue >= 100.0f) {
    len += 3;
  } else if (absValue >= 10.0f) {
    len += 2;
  } else {
    len += 1;
  }
  if (value < 0) {
    len += 1;  //for - simbol
  }
  return len;
}

void displayValueAtRight(uint8_t row, float value, uint8_t precision, uint8_t anchorSymbol) {
  uint8_t valueWidth = getNumericWidth(value, precision) + COL_PADDING_RIGHT + 1;  //1 for the anchor
  printNumericValue(SCREEN_COLS - valueWidth, row, value, precision, SYMBOL_NONE, FONT_SEGMENT_SMALL);
  printSymbol(SCREEN_COLS - COL_PADDING_RIGHT, row, anchorSymbol);
}

void displayValueAtLeft(uint8_t row, float value, uint8_t precision, uint8_t anchorSymbol) {
  printNumericValue(COL_PADDING_LEFT, row, value, precision, anchorSymbol, FONT_SEGMENT_SMALL);
}

// Row 0
void displayFlightMode() {
  printStringValue((SCREEN_COLS / 3) - 1, SCREEN_ROWS - 16, telemetry.fm, strlen(telemetry.fm), SYMBOL_NONE, FONT_SMALL);
}

// Row 1
void displayBatteryInfo() {
  float batPer = 0;
  if (telemetry.rxBatMax > 0) {
    batPer = (telemetry.rxBat / telemetry.rxBatMax) * 100;
  }
  printNumericValue(0, SCREEN_ROWS - 4, batPer, 0, SYMBOL_PERCSMALL, FONT_SEGMENT_SMALL);
  bool lowBattery = (telemetry.batteryAlertState == 1 || telemetry.batteryAlertState == 2);
  uint16_t blinkInterval = (telemetry.batteryAlertState == 2) ? BATTERY_BLINK_INTERVAL_CRITICAL_MS : BATTERY_BLINK_INTERVAL_LOW_MS;
  bool blinkOn = ((millis() / blinkInterval) % 2) == 0;
  uint8_t batterySymbol;
  if (telemetry.batteryAlertState == 2) {
    batterySymbol = SYMBOL_BATTERY_0;
  } else if (telemetry.batteryAlertState == 1) {
    batterySymbol = SYMBOL_BATTERY_30;
  } else if (batPer > 90) {
    batterySymbol = SYMBOL_BATTERY_100;
  } else {
    batterySymbol = SYMBOL_BATTERY_50;
  }
  if (lowBattery && !blinkOn) {
    // Blink "off" phase: hide the icon, keep the voltage number visible
    printNumericValue(0, SCREEN_ROWS - 3, telemetry.rxBat, 1, SYMBOL_NONE, FONT_SEGMENT_SMALL);
  } else {
    printNumericValue(0, SCREEN_ROWS - 3, telemetry.rxBat, 1, batterySymbol, FONT_SEGMENT_SMALL);
  }
}

void displayGNSSInfo() {
  printNumericValue(SCREEN_COLS - 6, SCREEN_ROWS - 3, telemetry.satCount, 0, SYMBOL_GPS, FONT_SEGMENT_SMALL);
  if (telemetry.gnssReliable == 1) {
    printSymbol(SCREEN_COLS - 3, SCREEN_ROWS - 3, SYMBOL_FLAG);
  } else {
    printSymbol(SCREEN_COLS - 3, SCREEN_ROWS - 3, SYMBOL_NONE);
  }
}

void displayDistance() {
  displayValueAtLeft(SCREEN_ROWS - 6, telemetry.homeDistance, 0, SYMBOL_DIST);
  displayValueAtLeft(SCREEN_ROWS - 7, telemetry.alt, 1, SYMBOL_ALT);
}

void displayAltitude() {
  displayValueAtRight(SCREEN_ROWS - 6, telemetry.groundSpeed, 1, SYMBOL_SPD); // This is the current speed
  displayValueAtRight(SCREEN_ROWS - 7, telemetry.verticalSpeed, 1, CHAT_SMALL_T); // This is the target speed
}

uint8_t getRotationSymbol(float angleDegrees) {
  // Normalize to 0-360 in case angle is negative or >360
  while (angleDegrees < 0.0f) angleDegrees += 360.0f;
  while (angleDegrees >= 360.0f) angleDegrees -= 360.0f;
  uint8_t index = (uint8_t)roundf(angleDegrees / 22.5f) % 16;  // %16 handles the 359°→360° wrap rounding up to "16"
  return ROTATION_SYMBOLS[index];
}

void displayHeading() {
  displayValueAtLeft(SCREEN_ROWS - 9, telemetry.headingRef, 1, SYMBOL_RECYCLE);
  float relativeBearing = telemetry.heading - telemetry.headingRef;
  displayValueAtLeft(SCREEN_ROWS - 10, telemetry.heading, 1, getRotationSymbol(relativeBearing));
}

void displayAttitude() {
  displayValueAtRight(SCREEN_ROWS - 9, telemetry.pitch, 1, SYMBOL_VIEW_DIAMOND);
  displayValueAtRight(SCREEN_ROWS - 10, telemetry.roll, 1, SYMBOL_SLASH);
}

void displayThrottle() {
  printNumericValue(0, SCREEN_ROWS - 14, telemetry.throttle, 0, SYMBOL_POWER, FONT_SEGMENT_SMALL);
}



void displayHorizon() {
  int16_t centerCol = (SCREEN_COLS / 2) + SCREEN_COL_ADJUST + HORIZON_CENTER_COL_OFFSET;
  int16_t centerRow = (SCREEN_ROWS / 2) + SCREEN_ROW_ADJUST + VERTICAL_CENTER_COL_OFFSET;

  /*
   * Clamp pitch/roll used for drawing so the horizon can't shoot
   * off-screen at extreme attitudes. The real telemetry values are
   * left untouched for any numeric readout elsewhere.
   */
  float displayRoll = -telemetry.roll;
  if (displayRoll > HORIZON_MAX_ANGLE) {
    displayRoll = HORIZON_MAX_ANGLE;
  } else if (displayRoll < -HORIZON_MAX_ANGLE) {
    displayRoll = -HORIZON_MAX_ANGLE;
  }

  float displayPitch = telemetry.pitch;
  if (displayPitch > HORIZON_MAX_ANGLE) {
    displayPitch = HORIZON_MAX_ANGLE;
  } else if (displayPitch < -HORIZON_MAX_ANGLE) {
    displayPitch = -HORIZON_MAX_ANGLE;
  }

  float rollRad = displayRoll * (M_PI / 180.0f);
  /*
   * Limit the slope so the horizon does not become excessively steep
   * at large roll angles.
   */
  float tanRoll = tanApproxF(rollRad);
  if (tanRoll > 2.0f) {
    tanRoll = 2.0f;
  } else if (tanRoll < -2.0f) {
    tanRoll = -2.0f;
  }
  /*
   * Positive pitch (nose up) moves the horizon downward.
   */
  float pitchOffset = displayPitch * HORIZON_PITCH_SCALE;
  int16_t horizonCenterRow = (int16_t)roundf(centerRow - pitchOffset);

  /*
   * Fixed reference line — drawn at the TRUE screen center row
   * (centerRow, not horizonCenterRow), so it never moves with
   * pitch or roll. Compare this against the moving horizon below
   * to judge level flight.
   */
  int16_t refColLeft = centerCol - (HORIZON_HALF_WIDTH + FIXED_REF_HALF_GAP);
  int16_t refColRight = centerCol + 1 + (HORIZON_HALF_WIDTH  + FIXED_REF_HALF_GAP);
  if (refColLeft >= 0 && refColLeft < SCREEN_COLS && centerRow >= 0 && centerRow < SCREEN_ROWS) {
    printSymbol((uint8_t)refColLeft, (uint8_t)centerRow, SYMBOL_VERT_LINE_LEFT);
  }
  if (refColRight >= 0 && refColRight < SCREEN_COLS && centerRow >= 0 && centerRow < SCREEN_ROWS) {
    printSymbol((uint8_t)refColRight, (uint8_t)centerRow, SYMBOL_VERT_LINE_RIGHT);
  }

  /*
   * Aircraft reference symbol — split into left/right halves
   * straddling centerCol and centerCol + 1. The true visual
   * midpoint is centerCol + 0.5.
   */
  if (centerCol >= 0 && centerCol < SCREEN_COLS && horizonCenterRow >= 0 && horizonCenterRow < SCREEN_ROWS) {
    printSymbol((uint8_t)centerCol, (uint8_t)horizonCenterRow, SYMBOL_VIEW_LEFT);
  }
  if (centerCol + 1 >= 0 && centerCol + 1 < SCREEN_COLS && horizonCenterRow >= 0 && horizonCenterRow < SCREEN_ROWS) {
    printSymbol((uint8_t)(centerCol + 1), (uint8_t)horizonCenterRow, SYMBOL_VIEW_RIGHT);
  }

  /*
   * Draw horizon on both sides of the aircraft.
   * Loop runs one column wider on the right (-HALF_WIDTH..HALF_WIDTH+1)
   * to compensate for the icon occupying two columns instead of one,
   * keeping equal dash counts on both sides.
   */
  for (int8_t dx = -HORIZON_HALF_WIDTH; dx <= HORIZON_HALF_WIDTH + 1; dx++) {
    if (dx == 0 || dx == 1) {
      continue;  // reserved for SYMBOL_VIEW_LEFT / SYMBOL_VIEW_RIGHT
    }
    int16_t col = centerCol + dx;
    if (col < 0 || col >= SCREEN_COLS) {
      continue;
    }
    /*
     * Measure roll offset from the true midpoint (centerCol + 0.5),
     * not from centerCol itself, so both sides stay mirrored.
     */
    float rollOffset = (dx - 0.5f) * tanRoll * HORIZON_ROLL_ASPECT;
    int16_t row = (int16_t)roundf(horizonCenterRow + rollOffset);
    /*
     * Do not clamp to the screen edge. Just don't draw an
     * off-screen segment.
     */
    if (row < 0 || row >= SCREEN_ROWS) {
      continue;
    }
    printSymbol((uint8_t)col, (uint8_t)row, SYMBOL_HYPHEN);
  }
}


void displayTelemetry() {
  displayBatteryInfo();
  displayGNSSInfo();
  displayDistance();
  displayAltitude();
  displayHeading();
  displayAttitude();
  displayHorizon();
  displayThrottle();
  displayFlightMode();
}

void updateOSD() {
  if (hasNewData == 1) {
    hasNewData = 0;
    max7456ClearDisplay();
    displayTelemetry();
    max7456EndDrawing();
  }
}
