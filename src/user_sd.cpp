#include "user_sd.h"

static uint8_t is_sd_available;

SD_STATE sd_init(fs::FS &fs, const char *path)
{
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  if(!SD.begin(CS_PIN))
  {
    is_sd_available = 0;
    return SD_START_FAILED;
  }
  else
  {
    is_sd_available = 1;
  }

  File file = fs.open(path, FILE_APPEND);
  if(!file)
  {
    return SD_OPEN_FAILED;
  }
  
  if(!file.print("\r\nDATA LOGGER STARTED\r\nbme_altitude,gps_altitude,latitude,longitude,gyro_x,gyro_y,gyro_z,acc_x,acc_y,acc_z,theta,temperature,velocity,voltage,current,humidity,durum,check_sum\r\n"))
  {
    return SD_APPEND_FAILED;
  }

  file.close();
  return SD_OK;
}

SD_STATE sd_write_datas(fs::FS &fs, const char *path, data_t *data_)
{
  if(is_sd_available == 1)
  {
    char sd_buffer[SD_BUFFER_SIZE];
    memset(sd_buffer, 0, SD_BUFFER_SIZE);
    File file = fs.open(path, FILE_APPEND);
    if(!file)
    {
      return SD_OPEN_FAILED;
    }
    snprintf(sd_buffer, SD_BUFFER_SIZE, "%.3f,%.3f,%.6f,%.6f,%.2f,%.2f,%.2f,%.3f,%.3f,%.3f,%.2f,%.1f,%.2f,%.2f,%.2f,%d,%d,%d\r\n",
          data_->bme_altitude, data_->gps_altitude, data_->latitude, data_->longitude,
          data_->gyro_x, data_->gyro_y, data_->gyro_z, data_->acc_x, data_->acc_y, data_->acc_z,
          data_->theta, data_->temperature,data_->velocity, data_->voltage, data_->current,
          data_->humidity, data_->durum, data_->check_sum);
    if(!file.print(sd_buffer))
    {
      return SD_APPEND_FAILED;
    }
    file.close();
    return SD_OK;
  }
  else
  {
    return SD_UNAVAILABLE;
  }
}

SD_STATE sd_read_all(fs::FS &fs, const char *path)
{
  if(is_sd_available == 1)
  {
    File file = fs.open(path, FILE_READ);
    if(!file)
    {
      return SD_OPEN_FAILED;
    }

    Serial.print("\r\nRead from file:\r\n");
    while(file.available())
    {
      String line = file.readStringUntil('\n');
      Serial.println(line);
    }
    file.close();
    return SD_OK;
  }
  else
  {
    return SD_UNAVAILABLE;
  }
}