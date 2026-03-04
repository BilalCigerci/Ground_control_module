#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "HardwareSerial.h"
#include <stdint.h>

#define DATA_SIZE 62

typedef union {
    float sayi;
    uint8_t array[4];
} FLOAT32_UINT8_DONUSTURUCU;

typedef union {
    uint16_t sayi;
    uint8_t array[2];
} UINT16_UINT8_DONUSTURUCU;

typedef struct data
{
  uint8_t basla;  // 0xFF (sabit)
  float bme_altitude;
  float gps_altitude;
  float latitude;
  float longitude;
  float gyro_x;
  float gyro_y;
  float gyro_z;
  float acc_x;
  float acc_y;
  float acc_z;
  float theta;
  float temperature;
  float velocity;
  float voltage;
  float current;
  uint8_t humidity;
  uint8_t durum;
  uint8_t check_sum;
  uint8_t	CR;
	uint8_t LF;
}data_t;

typedef struct data_struct
{
  uint8_t basla;  // 0xFF (sabit)
  uint8_t bme_altitude[4];
  uint8_t gps_altitude[4];
  uint8_t latitude[4];
  uint8_t longitude[4];
  uint8_t gyro_x[4];
  uint8_t gyro_y[4];
  uint8_t gyro_z[4];
  uint8_t acc_x[4];
  uint8_t acc_y[4];
  uint8_t acc_z[4];
  uint8_t theta[4];
  uint8_t temperature[4];
  uint8_t velocity[4];
  uint8_t voltage[2];
  uint8_t current[2];
  uint8_t humidity;
  uint8_t durum;
  uint8_t check_sum;
  uint8_t	CR;
	uint8_t LF;
}data_struct_t;

typedef union DataPack
{
	uint8_t arr[DATA_SIZE];
	data_struct_t dataYapi;
}data_pack_t;

typedef union Key {
    uint16_t key16;
    uint8_t key8[2];
} key;

uint8_t check_sum_hesapla(data_pack_t *data_pack_);
void printDatasPacked(data_pack_t *data_pack_);
void cast_data_types(data_pack_t *data_pack_, data_t *data_);

#endif
