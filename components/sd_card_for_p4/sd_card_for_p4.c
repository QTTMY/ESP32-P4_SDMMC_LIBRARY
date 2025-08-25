#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sd_card_for_p4.h"

static const char *TAG = "SD_CARD_P4";
static sdmmc_card_t *card = NULL;
static sd_pwr_ctrl_handle_t pwr_handle = NULL;

#define MOUNT_POINT "/sdcard"

esp_err_t sd_card_init(void)
{
    esp_err_t ret;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // Power control using on-chip LDO 
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,  // Use correct LDO channel for your board
    };
    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init LDO power control");
        return ret;
    }
    host.pwr_ctrl_handle = pwr_handle;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT(); //Check the pin configuration for your board, this is for ESP32-P4 Nano
    slot_config.width = 4;
    slot_config.clk = 43;
    slot_config.cmd = 44;
    slot_config.d0  = 39;
    slot_config.d1  = 40;
    slot_config.d2  = 41;
    slot_config.d3  = 42;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    slot_config.gpio_cd = -1;
    slot_config.gpio_wp = -1;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card); //This is what mounts the SD card
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card (%s)", esp_err_to_name(ret));
        return ret;
    }

    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "SD card mounted at %s", MOUNT_POINT);

    return ESP_OK;
}

esp_err_t sd_card_write_file(const char *path, const char *data) //Just read the function's name, it tells you what it does
{
    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", path);
        return ESP_FAIL;
    }
    fprintf(f, "%s", data);
    fclose(f);
    ESP_LOGI(TAG, "File written: %s", path);
    return ESP_OK;
}

esp_err_t sd_card_read_file(const char *path, char *buffer, size_t buffer_size) //I don't think I need to explain this one
{
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", path);
        return ESP_FAIL;
    }
    if (fgets(buffer, buffer_size, f) == NULL) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to read from file: %s", path);
        return ESP_FAIL;
    }
    fclose(f);

    // strip newline
    char *pos = strchr(buffer, '\n');
    if (pos) *pos = '\0';

    ESP_LOGI(TAG, "Read from file: %s", buffer);
    return ESP_OK;
}

esp_err_t sd_card_deinit(void) //Unmount the SD card and free resources (Yes, if you didn't know, mounted SD cards use resources)
{
    esp_err_t ret1 = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    ESP_LOGI(TAG, "SD card unmounted");

    esp_err_t ret2 = sd_pwr_ctrl_del_on_chip_ldo(pwr_handle);
    if (ret2 != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete LDO handle");
    }

    return (ret1 != ESP_OK) ? ret1 : ret2;
}
