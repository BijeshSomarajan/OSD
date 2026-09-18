#define BIG_ALPHABETS_START_INDEX 11
#define SMALL_ALPHABETS_START_INDEX 47

#define BIG_NUMBERS_START_INDEX 1
#define SMALL_NUMBERS_START_INDEX 37

#define BIG_SEGMENT_NUMBERS_START_INDEX 240
#define SMALL_SEGMENT_NUMBERS_START_INDEX 224

#define ASCII_SMALL_APLHABETS_START_OFFSET 97
#define ASCII_SMALL_APLHABETS_END_OFFSET 122
#define ASCII_CAPITAL_APLHABETS_START_OFFSET 65
#define ASCII_CAPITAL_APLHABETS_END_OFFSET 90
#define ASCII_NUMBERS_START_OFFSET 48
#define ASCII_NUMBERS_END_OFFSET 57
#define ASCII_DOT_VALUE 46

#define ASCII_COMMA_VALUE 44
#define ASCII_SLASH_VALUE 47
#define ASCII_PERCENT_VALUE 37

#define COMMA_INDEX 73
#define DOT_INDEX 74
#define SLASH_INDEX 75
#define PERCENT_INDEX 76

int16_t getFontTypeStartIndex(uint8_t asciiPreset ,uint8_t fontType){
   int16_t mappedIndex = -1;
   if(FONT_LARGE == fontType){
    if(asciiPreset == ASCII_NUMBERS_START_OFFSET){
      mappedIndex =  BIG_NUMBERS_START_INDEX;
    } else if(asciiPreset == ASCII_CAPITAL_APLHABETS_START_OFFSET || asciiPreset == ASCII_SMALL_APLHABETS_START_OFFSET){
      mappedIndex = BIG_ALPHABETS_START_INDEX;
    }
   }else if(FONT_SMALL == fontType){
    if(asciiPreset == ASCII_NUMBERS_START_OFFSET){
      mappedIndex = SMALL_NUMBERS_START_INDEX;
    } else if(asciiPreset == ASCII_CAPITAL_APLHABETS_START_OFFSET || asciiPreset == ASCII_SMALL_APLHABETS_START_OFFSET){
      mappedIndex = SMALL_ALPHABETS_START_INDEX;
    }
   }else if(FONT_SEGMENT_LARGE == fontType){
    if(asciiPreset == ASCII_NUMBERS_START_OFFSET){
      mappedIndex = BIG_SEGMENT_NUMBERS_START_INDEX;
    }
   }else if(FONT_SEGMENT_SMALL == fontType){
     if(asciiPreset == ASCII_NUMBERS_START_OFFSET){
      mappedIndex = SMALL_SEGMENT_NUMBERS_START_INDEX;
    }
   }
   return mappedIndex;
}

int16_t getMappedIndex(char ch,uint8_t fontType){
   int16_t mappedIndex = -1;
   int16_t fontTypeStartIndex = -1;

   if(ch >= ASCII_NUMBERS_START_OFFSET && ch <= ASCII_NUMBERS_END_OFFSET){
      fontTypeStartIndex = getFontTypeStartIndex(ASCII_NUMBERS_START_OFFSET,fontType);
      if(fontTypeStartIndex >= 0){
         mappedIndex = (ch - ASCII_NUMBERS_START_OFFSET) + fontTypeStartIndex;
      }
   }else if(ch >= ASCII_CAPITAL_APLHABETS_START_OFFSET && ch <= ASCII_CAPITAL_APLHABETS_END_OFFSET){
      fontTypeStartIndex = getFontTypeStartIndex(ASCII_CAPITAL_APLHABETS_START_OFFSET,fontType);
      if(fontTypeStartIndex >= 0){
         mappedIndex = (ch - ASCII_CAPITAL_APLHABETS_START_OFFSET) + fontTypeStartIndex;
      }
   }else if(ch >= ASCII_SMALL_APLHABETS_START_OFFSET && ch <= ASCII_SMALL_APLHABETS_END_OFFSET){
      fontTypeStartIndex = getFontTypeStartIndex(ASCII_SMALL_APLHABETS_START_OFFSET,fontType);
      if(fontTypeStartIndex >= 0){
         mappedIndex = (ch - ASCII_SMALL_APLHABETS_START_OFFSET) + fontTypeStartIndex;
      }
   }else if(ch == ASCII_COMMA_VALUE){
      mappedIndex = COMMA_INDEX;
   }else if(ch == ASCII_DOT_VALUE){
      mappedIndex = DOT_INDEX;
   }else if(ch == ASCII_SLASH_VALUE){
      mappedIndex = SLASH_INDEX;
   }else if(ch == ASCII_PERCENT_VALUE){
      mappedIndex = PERCENT_INDEX;
   }

   return mappedIndex;
}

void printNumericValue(uint8_t x , uint8_t y ,float value,uint8_t precision, uint8_t symbol,uint8_t fontType){
  char tmp[255];
  dtostrf(value, 1, precision, tmp);
  printStringValue(x , y , tmp,strlen(tmp), symbol,fontType);
}

void printSymbol(uint8_t x , uint8_t y , uint8_t symbol){
  max7456StartDrawing(x ,y);
  if(SYMBOL_NONE == symbol){
    symbol = CHAR_CLEAR;
  }
  max7456Print((int16_t)symbol);
  max7456EndDrawing();
}

void printStringValue(uint8_t x , uint8_t y , char* data,uint8_t len, uint8_t symbol,uint8_t fontType){
  if(len > 0){
   max7456StartDrawing(x ,y);
   if(SYMBOL_NONE == symbol){
    symbol = CHAR_CLEAR;
   }
   max7456Print((int16_t)symbol);
   for(uint8_t indx = 0;indx < len ;indx++){
    int16_t mappedIndx = getMappedIndex(*data,fontType);
   
    if(*data == '-'){
      mappedIndx = SYMBOL_MINUS;
    }else if(mappedIndx < 0){
      mappedIndx = CHAR_CLEAR;
    }
     data++;
    max7456Print(mappedIndx);
   }
   max7456EndDrawing();
  }
}
