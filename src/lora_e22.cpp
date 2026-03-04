#include <stdint.h>
#include "esp32-hal-gpio.h"
#include <cstring>
#include <sys/_stdint.h>
#include "esp32-hal.h"
#include "HardwareSerial.h"
#include "lora_e22.h"

static uint32_t lora_current_baud_rate = 9600;
static uint8_t lora_parity;
static uint8_t lora_baud_rate;

static void set_baud_rate(e22_conf_struct_t *lora_conf_struct_);

void lora_e22_init(e22_conf_struct_t *lora_conf_struct)
{
  uint8_t lora_configuration_packet[LORA_PACKET_SIZE];

  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  lora_e22_configuration_mode();

  LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  LORA.setTimeout(100);

  lora_conf_struct->channel         = lora_e22_get_channel();
  lora_conf_struct->air_rate			  =	lora_e22_get_air_rate();

	lora_configuration_packet[0] = 0xC0;	//Set register command.
	lora_configuration_packet[1] = 0x03;	//Starting from byte 0x03
	lora_configuration_packet[2] = 0x04;	//4 bytes will be configured.
  //Wireless air data rate（bps）, Serial parity bit, UART Serial port rate（bps).
	lora_configuration_packet[3] = lora_conf_struct->air_rate | lora_conf_struct->parity_bit << 3 | lora_conf_struct->baud_rate << 5;
  //Transmitting power, RSSI anbient noise enable, Sub packet settings.
	lora_configuration_packet[4] = lora_conf_struct->power | lora_conf_struct->rssi_noise << 5 | lora_conf_struct->packet_size << 6;
  //channel 0-83 (410.125 + CH *1M)
  if(lora_conf_struct->channel > 83)
  {
    lora_conf_struct->channel = 83;
  }
  else if(lora_conf_struct->channel < 0)
  {
    lora_conf_struct->channel = 0;
  }
	lora_configuration_packet[5] = lora_conf_struct->channel;
  //WOR cycle time, WOR transceiver control, LBT enable, Repeater function, Transmission mode, Enable RSSI.
  lora_configuration_packet[6] = lora_conf_struct->wor_cycle | lora_conf_struct->wor << 3 | lora_conf_struct->lbt << 4 | lora_conf_struct->repeater_func << 5 | lora_conf_struct->mode << 6 | lora_conf_struct->rssi_enable << 7;

  uint8_t response[LORA_PACKET_SIZE] = {0};
  for(uint8_t i = 0; i < 10; i++)
  {
    LORA.write(lora_configuration_packet, LORA_PACKET_SIZE);
    delay(10);
    if(LORA.available())
    {
      LORA.readBytes(response, LORA_PACKET_SIZE);
      if(memcmp(&response[1], &lora_configuration_packet[1], LORA_PACKET_SIZE - 1) == 0)
      {
        break;
      }
    }
    delay(10);
  }
  
  LORA.end();
  lora_e22_transmission_mode();
  delay(50);
  set_baud_rate(lora_conf_struct);
  delay(1000);

  lora_parity = lora_conf_struct->parity_bit;
  lora_baud_rate = lora_conf_struct->baud_rate;
}

void lora_e22_transmission_mode(void)
{
  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, LOW);
  delay(20);
}

void lora_e22_configuration_mode(void)
{
  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, HIGH);
  delay(20);
}

void lora_e22_configure(void)
{
  pinMode(LORA_M0, OUTPUT);
  pinMode(LORA_M1, OUTPUT);
  digitalWrite(LORA_M0, LOW);
  digitalWrite(LORA_M1, HIGH);
  Serial.begin(9600);
  LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  delay(20);
  while(1)
  {
    if(LORA.available())
    {
      Serial.write(LORA.read());
    }
    if(Serial.available())
    {
      LORA.write(Serial.read());
    }
  }
}

uint8_t lora_e22_get_channel(void)
{
  uint8_t command[3] = {0xC1, 0x05, 0x01};
  uint8_t response[4];
  for(uint8_t i = 0; i < 10; i++)
  {
    LORA.write(command, 3);
    delay(10);
    if(LORA.available())
    {
      LORA.readBytes(response, 4);
      if(memcmp(command, response, 3) == 0)
      {
        return response[3];
      }
    }
  }
  return LORA_DEFAULT_CHANNEL;
}

uint8_t lora_e22_set_channel(uint8_t channel_)
{
  uint8_t lora_conf_ok = 0;
  LORA.end();
  lora_e22_configuration_mode();
  LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  
  uint8_t conf_packet[4];
  conf_packet[0] = 0xC0;
  conf_packet[1] = 0x05;
  conf_packet[2] = 0x01;
  conf_packet[3] = (uint8_t)channel_;

  uint8_t response[4];
  for(uint8_t i = 0; i < 10; i++)
  {
    LORA.write(conf_packet, 4);
    delay(10);
    if(LORA.available())
    {
      LORA.readBytes(response, 4);
      if(memcmp(&response[1], &conf_packet[1], 3) == 0)
      {
        lora_conf_ok = 1;
        break;
      }
    }

    delay(10);
  }
  LORA.end();
  lora_e22_transmission_mode();
  LORA.begin(lora_current_baud_rate, SERIAL_8N1, LORA_RX, LORA_TX);
  return lora_conf_ok;
}

uint8_t lora_e22_get_air_rate(void)
{
  uint8_t command[3] = {0xC1, 0x03, 0x01};
  uint8_t response[4];
  for(uint8_t i = 0; i < 10; i++)
  {
    LORA.write(command, 3);
    delay(10);
    if(LORA.available())
    {
      LORA.readBytes(response, 4);
      if(memcmp(command, response, 3) == 0)
      {
        return (response[3] & 0x07);
      }
    }
  }
  return LORA_DEFAULT_AIR_RATE;
}

uint8_t lora_e22_set_air_rate(uint8_t air_rate_)
{
  uint8_t lora_conf_ok = 0;
  LORA.end();
  lora_e22_configuration_mode();
  LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  
  uint8_t conf_packet[4];
  conf_packet[0] = 0xC0;
  conf_packet[1] = 0x03;
  conf_packet[2] = 0x01;
  conf_packet[3] = air_rate_ | lora_parity << 3 | lora_baud_rate << 5;;

  uint8_t response[4];
  for(uint8_t i = 0; i < 10; i++)
  {
    LORA.write(conf_packet, 4);
    delay(10);
    if(LORA.available())
    {
      LORA.readBytes(response, 4);
      if(memcmp(&response[1], &conf_packet[1], 3) == 0)
      {
        lora_conf_ok = 1;
        break;
      }
    }
    delay(10);
  }
  LORA.end();
  lora_e22_transmission_mode();
  LORA.begin(lora_current_baud_rate, SERIAL_8N1, LORA_RX, LORA_TX);
  return lora_conf_ok;

}

static void set_baud_rate(e22_conf_struct_t *lora_conf_struct_)
{
  switch(lora_conf_struct_->baud_rate)
  {
    case E22_BAUD_RATE_1200: lora_current_baud_rate = 1200; LORA.begin(1200, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_2400: lora_current_baud_rate = 2400; LORA.begin(2400, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_4800: lora_current_baud_rate = 4800; LORA.begin(4800, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_9600: lora_current_baud_rate = 9600; LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_19200: lora_current_baud_rate = 19200; LORA.begin(19200, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_38400: lora_current_baud_rate = 38400; LORA.begin(38400, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_57600: lora_current_baud_rate = 57600; LORA.begin(57600, SERIAL_8N1, LORA_RX, LORA_TX); break;
    case E22_BAUD_RATE_115200: lora_current_baud_rate = 115200; LORA.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX); break;
    default: lora_current_baud_rate = 9600; LORA.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX); break;
  }
  delay(20);
}
