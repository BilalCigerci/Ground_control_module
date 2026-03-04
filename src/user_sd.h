#ifndef USER_SD_H
#define USER_SD_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "data_struct.h"
#include <stdint.h>
#include <cstring>

#define CS_PIN 5
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

#define FILE_PATH "/log.txt"

#define SD_BUFFER_SIZE 200

typedef enum {
  SD_OK,
  SD_UNAVAILABLE,
  SD_START_FAILED,
  SD_OPEN_FAILED,
  SD_APPEND_FAILED
}SD_STATE;

SD_STATE sd_init(fs::FS &fs, const char *path);
SD_STATE sd_write_datas(fs::FS &fs, const char *path, data_t *data_);
SD_STATE sd_read_all(fs::FS &fs, const char *path);

#endif