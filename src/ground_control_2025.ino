#include "data_struct.h"
#include "user_sd.h"
#include "lcd.h"
#include "user_esp_now.h"
#include "lora_e22.h"

#define SERIAL_BAUD 115200

#define LED_1 25
#define LED_2 26
#define LED_3 27

#define SW 13

extern uint8_t esp_now_on;

data_t casted_datas;
data_pack_t raw_datas;
e22_conf_struct_t lora_conf;

static unsigned long now = 0;
static unsigned long last_time = 0;

static void lora_init(void);
static void check_switch_status(void);
static void update_lora_status(void);

static_assert(sizeof(data_struct_t) == DATA_SIZE, "data_struct_t boyutu DATA_SIZE ile uyumsuz!");

void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(SW, INPUT);
  digitalWrite(LED_1, HIGH);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);

  Serial.begin(SERIAL_BAUD);
  lora_init();
  sd_init(SD, FILE_PATH);
  Lcd_init();
  user_esp_now_init();
  digitalWrite(LED_3, HIGH);
  delay(100);
  digitalWrite(LED_3, LOW);
}

void loop() {
  now = millis();
  check_switch_status();
  Lcd_check_command();
  Lcd_update_settings();
  if(LORA.available())
  {
    last_time = now;
    digitalWrite(LED_2, !digitalRead(LED_2));
    LORA.readBytes(raw_datas.arr, DATA_SIZE);
    if(raw_datas.arr[0] == 0xFF && raw_datas.arr[DATA_SIZE - 1] == 0x0A && raw_datas.arr[DATA_SIZE - 2] == 0x0D && raw_datas.arr[DATA_SIZE - 3] == check_sum_hesapla(&raw_datas))
    {
      if(esp_now_on == 1)
      {
        user_esp_now_send(&raw_datas.arr[0], DATA_SIZE);
      }
      else
      {
        printDatasPacked(&raw_datas);
      }
      cast_data_types(&raw_datas, &casted_datas);
      Lcd_update_datas(&casted_datas);
      sd_write_datas(SD, FILE_PATH, &casted_datas);
    }
  }
  update_lora_status();
}

static void lora_init(void)
{
  lora_conf.baud_rate 		  = E22_BAUD_RATE_115200;
	lora_conf.parity_bit		  =	E22_PARITY_8N1;
	lora_conf.packet_size		  =	E22_PACKET_SIZE_64;
	lora_conf.rssi_noise		  =	E22_RSSI_NOISE_DISABLE;
	lora_conf.power			      =	E22_TRANSMITTING_POWER_30dBm;
	lora_conf.rssi_enable		  =	E22_ENABLE_RSSI_DISABLE;
	lora_conf.mode				    = E22_TRANSMISSION_MODE_TRANSPARENT;
	lora_conf.repeater_func	  =	E22_REPEATER_FUNC_DISABLE;
	lora_conf.lbt			        =	E22_LBT_DISABLE;
	lora_conf.wor				      =	E22_WOR_RECEIVER;
	lora_conf.wor_cycle		    =	E22_WOR_CYCLE_1000;

  lora_e22_init(&lora_conf);
  Lcd_lora_info_update(lora_conf.channel, lora_conf.air_rate);
}

static void check_switch_status(void)
{
  if(digitalRead(SW) == LOW)
  {
    esp_now_on = 1;
  }
  else
  {
    esp_now_on = 0;
  }
}

static void update_lora_status(void)
{
  if(now - last_time >= 2000)
  {
    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_2, LOW);
  }
  else if(digitalRead(LED_1) == HIGH)
  {
    digitalWrite(LED_1, LOW);
  }
}