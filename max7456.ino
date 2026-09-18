#include "max7456.h"
#include <SPI.h>

void max7456IOInit();
void max7456LoadSettings();
void max7456Select(void);
void max7456DeSelect(void);
void max7456Setup(void);

void max7456ClearDisplay(void);
void max7456StartDrawing(uint8_t x , uint8_t y);
void max7456EndDrawing(void);
void max7456Print(uint8_t index);
uint8_t getCenterRow(void);

uint8_t max7456VideoModeMask;
uint8_t max7456ScreenRows;
uint8_t max7456ScreenCols;
uint8_t max7456ColAdjust;
uint8_t max7456RowAdjust;

void max7456IOInit(){
  //Set the CS and Video Sync pins
  pinMode(MAX7456_SELECT, OUTPUT);
  digitalWrite(MAX7456_SELECT, HIGH);
  
  pinMode(MAX7456_VSYNC, INPUT);
  digitalWrite(MAX7456_VSYNC, HIGH);
  
  //Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
}

uint8_t initMax7456() {
  max7456IOInit();
  max7456LoadSettings();
  max7456DeSelect();   
  max7456Setup();
  max7456ClearDisplay();
}

uint8_t max7456GetCenterRow(){
 if(MAX7456_VIDEO_MODE == MAX7456_VIDEO_MODE_NTCS){
  return MAX7456_CENTER_ROW_NTCS;
 }else{
  return MAX7456_CENTER_ROW_PAL;
 }
}

uint8_t max7456GetCenterCol(){
 if(MAX7456_VIDEO_MODE == MAX7456_VIDEO_MODE_NTCS){
  return MAX7456_CENTER_COL_NTCS;
 }else{
  return MAX7456_CENTER_COL_PAL;
 }
}

void max7456Setup() {
  max7456Select();
  //read black level register
  SPI.transfer(MAX7456_OSDBL_REG_READ);//black level read register
  uint8_t osdbl_r = SPI.transfer(0xff);
  SPI.transfer(MAX7456_VM0_REG);
  SPI.transfer(MAX7456_RESET | max7456VideoModeMask);
  delay(50);
  //set black level
  uint8_t osdbl_w = (osdbl_r & 0xef); //Set bit 4 to zero 11101111
  SPI.transfer(MAX7456_OSDBL_REG); //black level write register
  SPI.transfer(osdbl_w);
  // set all rows to same charactor white level, 90%
  for (int x = 0; x < max7456ScreenRows; x++) {
    SPI.transfer(x + 0x10);
    SPI.transfer(MAX7456_WHITE_LEVEL_100);
  }
  max7456DeSelect();
  delay(50);
  // Enabling Max7456 display , AutoSync , Vertical
  max7456Select();
  SPI.transfer(MAX7456_VM0_REG);
  SPI.transfer((MAX7456_ENABLE_DISPLAY_VERT | max7456VideoModeMask) | MAX7456_SYNC_AUTO_SYNC); 
  max7456DeSelect();
}

void max7456LoadSettings() {
  switch (MAX7456_VIDEO_MODE) {
    case MAX7456_VIDEO_MODE_NTCS:
      max7456VideoModeMask = MAX7456_MODE_MASK_NTCS;
      max7456ScreenRows = MAX7456_NTCS_SCREEN_ROWS;
      max7456ScreenCols = MAX7456_NTCS_SCREEN_COLS;
      max7456ColAdjust = MAX7456_NTCS_SCREEN_COL_ADJUST;
      max7456RowAdjust = MAX7456_NTCS_SCREEN_ROW_ADJUST;
      break;
    case MAX7456_VIDEO_MODE_PAL:
       max7456VideoModeMask = MAX7456_MODE_MASK_PAL;
       max7456ScreenRows = MAX7456_PAL_SCREEN_ROWS;
       max7456ScreenCols = MAX7456_PAL_SCREEN_COLS;
       max7456ColAdjust = MAX7456_PAL_SCREEN_COL_ADJUST;
       max7456RowAdjust = MAX7456_PAL_SCREEN_ROW_ADJUST;
      break;
  }
}

void max7456Select(){
  digitalWrite(MAX7456_SELECT,LOW);
}

void max7456DeSelect(){
  digitalWrite(MAX7456_SELECT,HIGH);
}

void max7456ClearDisplay() {
  max7456Select();
  SPI.transfer(MAX7456_DMM_REG);
  SPI.transfer(MAX7456_CLEAR_DISPLAY);
  max7456DeSelect();
}

void max7456StartDrawing(uint8_t x , uint8_t y){
  y+=max7456RowAdjust;
  if(y >= max7456ScreenRows){
    y = max7456ScreenRows-max7456RowAdjust;
  }
  
  x+=max7456ColAdjust;
  if(x >= max7456ScreenCols){
    x = max7456ScreenCols-max7456ColAdjust;
  }
  
  //find [start address] position
  uint16_t linepos = (max7456ScreenRows - y) * max7456ScreenCols  + x;

  // divide 16 bits into hi & lo uint8_t
  uint8_t addressHigh = linepos >> 8;
  uint8_t addressLow = linepos;
 
  max7456Select();
  SPI.transfer(MAX7456_DMM_REG); //dmm
  //Auto increment turn writing fast (less SPI commands).
  //No need to set next char address. Just send them
  SPI.transfer(MAX7456_INCREMENT_AUTO);

  SPI.transfer(MAX7456_DMAH_REG); // set start address high
  SPI.transfer(addressHigh);

  SPI.transfer(MAX7456_DMAL_REG); // set start address low
  SPI.transfer(addressLow);
}

void max7456EndDrawing(){  
  SPI.transfer(MAX7456_DMDI_REG);
  SPI.transfer(MAX7456_END_STRING); //This is needed "trick" to finish auto increment
  max7456DeSelect();
}

void max7456Print(int16_t index){
  SPI.transfer(MAX7456_DMDI_REG);
  SPI.transfer(index);
}
