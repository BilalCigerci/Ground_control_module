#ifndef LCD_H
#define LCD_H

#include "HardwareSerial.h"
#include "data_struct.h"
#include <stdint.h>

#define LCD Serial1
#define LCD_BAUD 9600

#define LCD_RX 15
#define LCD_TX 14

typedef enum
{
  PAGE_ONE = 0x02,                // page one opened
  PAGE_ZERO = 0x03,               // page zero opended
  CHANNEL_SET = 0x04,             // pressed SET button (for channel)
  CHANNEL_DECREASE_TEN = 0x05,    // pressed -10 button
  CHANNEL_DECREASE = 0x06,        // pressed -1 button
  CHANNEL_INCREASE = 0x07,        // pressed +1 button
  CHANNEL_INCREASE_TEN = 0x08,    // pressed +10 button
  AIR_RATE_SET = 0x09,            // pressed SET button (for air rate)
  AIR_RATE_DECREASE = 0x0A,       // pressed - button
  AIR_RATE_INCREASE = 0x0B        // pressed + button
}LCD_COMMAND;

void Lcd_init(void);
void Lcd_update_datas(data_t *data_);
void Lcd_update_settings(void);
void Lcd_check_command(void);
void Lcd_lora_info_update(uint8_t channel_, uint8_t air_rate_);

#endif