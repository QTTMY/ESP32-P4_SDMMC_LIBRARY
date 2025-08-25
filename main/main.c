#include "sd_card_for_p4.h"

void app_main(void)
{
    if (sd_card_init() != ESP_OK) return;

    char buffer[SD_CARD_MAX_CHAR_SIZE];

    sd_card_write_file("/sdcard/hello.txt", "Hello ESP32-P4!"); //Create a .txt file that contains "Hello ESP32-P4!"
    sd_card_read_file("/sdcard/hello.txt", buffer, sizeof(buffer)); //Read the file
    sd_card_deinit(); //Unmount the SD card
}