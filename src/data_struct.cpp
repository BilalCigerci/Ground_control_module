#include "data_struct.h"

static float uint8_arrayi_float32_ye_donustur(uint8_t byte_array_u8[4]);
static uint16_t uint8_arrayi_uint16_ye_donustur(uint8_t byte_array_u8[2]);

uint8_t check_sum_hesapla(data_pack_t *data_pack_)
{
	int check_sum = 0;
	for(int i = 0; i < sizeof(data_pack_->arr) - 3; i++){
		check_sum += data_pack_->arr[i];
	}
	return (uint8_t) (check_sum % 256);
}

void printDatasPacked(data_pack_t *data_pack_)
{
  Serial.write(data_pack_->arr, sizeof(data_pack_->arr));
}

void cast_data_types(data_pack_t *data_pack_, data_t *data_)
{
	data_->basla = data_pack_->dataYapi.basla;
	data_->bme_altitude = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.bme_altitude);
	data_->gps_altitude = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.gps_altitude);
	data_->latitude = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.latitude);
	data_->longitude = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.longitude);
	data_->gyro_x = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.gyro_x);
	data_->gyro_y = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.gyro_y);
	data_->gyro_z = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.gyro_z);
	data_->acc_x = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.acc_x);
	data_->acc_y = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.acc_y);
	data_->acc_z = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.acc_z);
	data_->theta = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.theta);
	data_->temperature = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.temperature);
	data_->velocity = uint8_arrayi_float32_ye_donustur(data_pack_->dataYapi.velocity);
	data_->voltage = (float)uint8_arrayi_uint16_ye_donustur(data_pack_->dataYapi.voltage);
	data_->current = (float)uint8_arrayi_uint16_ye_donustur(data_pack_->dataYapi.current);
	data_->humidity = data_pack_->dataYapi.humidity;
	data_->durum = data_pack_->dataYapi.durum;
	data_->check_sum = data_pack_->dataYapi.check_sum;
	data_->CR = data_pack_->dataYapi.CR;
	data_->LF = data_pack_->dataYapi.LF;
}

static float uint8_arrayi_float32_ye_donustur(uint8_t byte_array_u8[4])
{
	FLOAT32_UINT8_DONUSTURUCU float32_uint8_donusturucu;
	float32_uint8_donusturucu.array[0] = byte_array_u8[0];
	float32_uint8_donusturucu.array[1] = byte_array_u8[1];
	float32_uint8_donusturucu.array[2] = byte_array_u8[2];
	float32_uint8_donusturucu.array[3] = byte_array_u8[3];
	return float32_uint8_donusturucu.sayi;
}

static uint16_t uint8_arrayi_uint16_ye_donustur(uint8_t byte_array_u8[2])
{
	UINT16_UINT8_DONUSTURUCU uint16_uint8_donusturucu;
	uint16_uint8_donusturucu.array[0] = byte_array_u8[0];
	uint16_uint8_donusturucu.array[1] = byte_array_u8[1];
	return uint16_uint8_donusturucu.sayi;
}
