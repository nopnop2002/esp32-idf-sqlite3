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
#include "esp_littlefs.h"

static const char *TAG = "MAIN";
const char *base_path = "/littlefs";

void sqlite(void *pvParameter);

void app_main() {
	ESP_LOGI(TAG, "Initializing LittleFS");
	esp_vfs_littlefs_conf_t conf = {
		.base_path = base_path,
		.partition_label = "storage",
		.format_if_mount_failed = true,
		.dont_mount = false,
	};

	// Use settings defined above to initialize and mount LittleFS filesystem.
	// Note: esp_vfs_littlefs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_littlefs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find LittleFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_littlefs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
		return;
	} else {
		ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
	}

	// Start task
	xTaskCreate(&sqlite, "SQLITE3", 1024*4, (void *)base_path, 5, NULL);

	// The main task is in an infinite loop to prevent stack variables from being released
	while(1) {
		vTaskDelay(1);
	}
}
