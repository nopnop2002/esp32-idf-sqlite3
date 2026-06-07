/*
	sqlite for esp-idf

	This example code is in the Public Domain (or CC0 licensed, at your option.)
	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

static const char *TAG = "MAIN";
const char *base_path = "/fatfs";

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
#define esp_vfs_fat_spiflash_mount esp_vfs_fat_spiflash_mount_rw_wl
#define esp_vfs_fat_spiflash_unmount esp_vfs_fat_spiflash_unmount_rw_wl
#endif

void sqlite(void *pvParameter);

void app_main() {
	// Handle of the wear levelling library instance
	static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

	ESP_LOGI(TAG, "Mounting FAT filesystem");
	// To mount device we need name of device partition, define base_path
	// and allow format partition in case if it is new one and was not formatted before
	const esp_vfs_fat_mount_config_t mount_config = {
		.max_files = 4,
		.format_if_mount_failed = true,
		.allocation_unit_size = CONFIG_WL_SECTOR_SIZE
	};
	esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
		return;
	}

	// Start task
	xTaskCreate(&sqlite, "SQLITE3", 1024*4, (void *)base_path, 5, NULL);

	// The main task is in an infinite loop to prevent stack variables from being released
	while(1) {
		vTaskDelay(1);
	}
}
