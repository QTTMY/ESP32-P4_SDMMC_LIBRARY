#pragma once
#include "esp_err.h"

// Max file size for buffer
#define SD_CARD_MAX_CHAR_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

// Initialize SD card and mount FAT filesystem, you must call this before other functions
// But you don't have to call it at the start of your program, just call it when you need to use the SD card
// The init script <<<I forgot why I wrote this comment, I went to grab some food and forgot and it's cut off>>>
esp_err_t sd_card_init(void);

// Write data to a file on SD card
esp_err_t sd_card_write_file(const char *path, const char *data);

// Read data from a file on SD card
esp_err_t sd_card_read_file(const char *path, char *buffer, size_t buffer_size);

// Unmount SD card and free resources
esp_err_t sd_card_deinit(void);

#ifdef __cplusplus
}
#endif
