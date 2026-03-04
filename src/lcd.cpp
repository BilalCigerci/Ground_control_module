#include "HardwareSerial.h"
#include <stdio.h>
#include <sys/_stdint.h>
#include <stdint.h>
#include "lcd.h"
#include "lora_e22.h"

uint8_t esp_now_on = 1;

static char data[100];
static float max_altitude = 0;
static uint8_t set_lora_channel = 0;
static uint8_t lora_current_channel;
static uint16_t set_lora_air_rate = 300;
static uint16_t lora_current_air_rate;
static uint8_t page = 0;
static unsigned long now;
static unsigned long start;

static void Lcd_send(char *data);
static void Lcd_command_handler(uint8_t command_);
static void lora_update_air_rate(uint8_t air_rate_);
static void lora_decrease_air_rate(uint16_t air_rate_);
static void lora_increase_air_rate(uint16_t air_rate_);
static uint8_t lora_get_air_rate(uint16_t air_rate_);

void Lcd_init(void)
{
  LCD.begin(LCD_BAUD, SERIAL_8N1, LCD_RX, LCD_TX);
  delay(100);
}

void Lcd_update_datas(data_t *data_)
{
  if(max_altitude < data_->bme_altitude)
  {
    max_altitude = data_->bme_altitude;
  }
  if(page == 0)
  {
    sprintf(data, "n0.val=%.0f", data_->theta);
    Lcd_send(data);
    sprintf(data, "n1.val=%.0f", data_->bme_altitude);
    Lcd_send(data);
    sprintf(data, "n2.val=%.0f", max_altitude);
    Lcd_send(data);
    sprintf(data, "t10.txt=\"%.6f\"", data_->latitude);
    Lcd_send(data);
    sprintf(data, "t11.txt=\"%.6f\"", data_->longitude);
    Lcd_send(data);
    sprintf(data, "t12.txt=\"%.1f\"", data_->temperature);
    Lcd_send(data);
    sprintf(data, "t15.txt=\"%.1f\"", data_->voltage);
    Lcd_send(data);
  }
}

void Lcd_update_settings(void)
{
  if(page == 0)
  {
    if(esp_now_on == 1)
    {
      sprintf(data, "t1.txt=\"on\"");
      Lcd_send(data);
    }
    else
    {
      sprintf(data, "t1.txt=\"off\"");
      Lcd_send(data);
    }
  }
  if(page == 1)
  {
    sprintf(data, "n3.val=%d", lora_current_channel);
    Lcd_send(data);
    sprintf(data, "n4.val=%d", set_lora_channel);
    Lcd_send(data);
    sprintf(data, "n5.val=%d", lora_current_air_rate);
    Lcd_send(data);
    sprintf(data, "n6.val=%d", set_lora_air_rate);
    Lcd_send(data);
  } 
}

void Lcd_check_command(void)
{
  if(LCD.available())
  {
    uint8_t command[4];
    LCD.readBytes(command, 4);
    if(command[0] == 0xAA && command[1] == 0xAB && command[2] == 0xBB)
    {
      Lcd_command_handler(command[3]);
    }
    else
    {
      start = millis();
      while(LCD.available())
      {
        now = millis();
        LCD.read();
        if(now - start >= 50)
        {
          break;
        }
      }
    }
  }
}

void Lcd_lora_info_update(uint8_t channel_, uint8_t air_rate_)
{
  lora_current_channel = channel_;
  lora_update_air_rate(air_rate_);
}

static void Lcd_send(char *data)
{
  LCD.write(data);
  LCD.write(0xFF);
  LCD.write(0xFF);
  LCD.write(0xFF);
}

static void Lcd_command_handler(uint8_t command_)
{
  switch(command_)
  {
    case PAGE_ONE:
      page = 1;
      break;
    case PAGE_ZERO:
      page = 0;
      break;
    case CHANNEL_SET:
      if(lora_current_channel != set_lora_channel && set_lora_channel >= 0 && set_lora_channel <= 83)
      {
        if(lora_e22_set_channel(set_lora_channel) == 1)
        {
          lora_current_channel = set_lora_channel;
        }
      }
      break;
    case CHANNEL_DECREASE_TEN:
      if(set_lora_channel >= 10)
      {
        set_lora_channel -= 10;
      }
      else if(set_lora_channel < 10 && set_lora_channel > 0)
      {
        set_lora_channel = 0;
      }
      break;
    case CHANNEL_DECREASE:
      if(set_lora_channel > 0)
      {
        set_lora_channel--;
      }
      break;
    case CHANNEL_INCREASE:
      if(set_lora_channel < 83)
      {
        set_lora_channel++;
      }
      break;
    case CHANNEL_INCREASE_TEN:
      if(set_lora_channel <= 73)
      {
        set_lora_channel += 10;
      }
      else if(set_lora_channel > 73 && set_lora_channel < 83)
      {
        set_lora_channel = 83;
      }
      break;
    case AIR_RATE_SET:
      if(lora_e22_set_air_rate(lora_get_air_rate(set_lora_air_rate)) == 1)
      {
        lora_current_air_rate = set_lora_air_rate;
      }
      break;
    case AIR_RATE_DECREASE:
      lora_decrease_air_rate(set_lora_air_rate);
      break;
    case AIR_RATE_INCREASE:
      lora_increase_air_rate(set_lora_air_rate);
     break;
    default:
      break;
  }
}

static void lora_update_air_rate(uint8_t air_rate_)
{
  switch(air_rate_)
  {
    case 0x00: lora_current_air_rate = 300; break;
    case 0x01: lora_current_air_rate = 1200; break;
    case 0x02: lora_current_air_rate = 2400; break;
    case 0x03: lora_current_air_rate = 4800; break;
    case 0x04: lora_current_air_rate = 9600; break;
    case 0x05: lora_current_air_rate = 19200; break;
    case 0x06: lora_current_air_rate = 38400; break;
    case 0x07: lora_current_air_rate = 62500; break;
  }
}

static void lora_decrease_air_rate(uint16_t air_rate_)
{
  switch(air_rate_)
  {
    case 300: break;
    case 1200: set_lora_air_rate = 300; break;
    case 2400: set_lora_air_rate = 1200; break;
    case 4800: set_lora_air_rate = 2400; break;
    case 9600: set_lora_air_rate = 4800; break;
    case 19200: set_lora_air_rate = 9600; break;
    case 38400: set_lora_air_rate = 19200; break;
    case 62500: set_lora_air_rate = 38400; break;
  }
}

static void lora_increase_air_rate(uint16_t air_rate_)
{
  switch(air_rate_)
  {
    case 300: set_lora_air_rate = 1200; break;
    case 1200: set_lora_air_rate = 2400; break;
    case 2400: set_lora_air_rate = 4800; break;
    case 4800: set_lora_air_rate = 9600; break;
    case 9600: set_lora_air_rate = 19200; break;
    case 19200: set_lora_air_rate = 38400; break;
    case 38400: set_lora_air_rate = 62500; break;
    case 62500: break;
  }
}

static uint8_t lora_get_air_rate(uint16_t air_rate_)
{
  switch(air_rate_)
  {
    case 300: return E22_AIR_DATA_RATE_300;
    case 1200: return E22_AIR_DATA_RATE_1200;
    case 2400: return E22_AIR_DATA_RATE_2400;
    case 4800: return E22_AIR_DATA_RATE_4800;
    case 9600: return E22_AIR_DATA_RATE_9600;
    case 19200: return E22_AIR_DATA_RATE_19200;
    case 38400: return E22_AIR_DATA_RATE_38400;
    case 62500: return E22_AIR_DATA_RATE_62500;
    default: return E22_AIR_DATA_RATE_2400;
  }
}
