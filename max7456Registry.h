//MAX7456 reg read addresses
#define MAX7456_OSDBL_REG_READ 0xec //black level
#define MAX7456_STAT_REG_READ  0xa0 //0xa[X] Status

//MAX7456 reg write addresses
#define MAX7456_VM0_REG   0x00
#define MAX7456_VM1_REG   0x01
#define MAX7456_DMM_REG   0x04
#define MAX7456_DMAH_REG  0x05
#define MAX7456_DMAL_REG  0x06
#define MAX7456_DMDI_REG  0x07
#define MAX7456_OSDM_REG  0x0c //not used. Is to set mix
#define MAX7456_OSDBL_REG 0x6c //black level

//MAX7456 reg write addresses to recording NVM process
#define MAX7456_CMM_REG   0x08
#define MAX7456_CMAH_REG  0x09
#define MAX7456_CMAL_REG  0x0a
#define MAX7456_CMDI_REG  0x0b

//DMM commands
#define MAX7456_CLEAR_DISPLAY 0x04
#define MAX7456_CLEAR_DISPLAY_VERT 0x06

#define MAX7456_INCREMENT_AUTO 0x03
#define MAX7456_SETBG_LOCAL 0x20 //00100000 force local BG to defined brightness level VM1[6:4]

#define MAX7456_END_STRING 0xff

//VM0 commands mixed with mode NTSC or PAL mode
#define MAX7456_ENABLE_DISPLAY_VERT 0x0c //mask with NTSC/PAL
#define MAX7456_RESET 0x02 //mask with NTSC/PAL
#define MAX7456_DISABLE_DISPLAY 0x00 //mask with NTSC/PAL

//VM0 command modifiers
#define MAX7456_SYNC_AUTO_SYNC 0x10
#define MAX7456_SYNC_INTERNAL 0x30
#define MAX7456_SYNC_EXTERNAL 0x20
//VM1 command modifiers
#define MAX7456_WHITE_LEVEL_80 0x03
#define MAX7456_WHITE_LEVEL_90 0x02
#define MAX7456_WHITE_LEVEL_100 0x01
#define MAX7456_WHITE_LEVEL_120 0x00

#define MAX7456_NVM_RAM_SIZE 0x36
#define MAX7456_WRITE_NVR 0xa0
#define MAX7456_STATUS_REG_NVR_BUSY 0x20
