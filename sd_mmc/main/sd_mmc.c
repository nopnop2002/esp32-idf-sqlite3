/*
	This example opens Sqlite3 databases from SD Card and
	retrieves data from them.
	Before running please copy following files to SD Card:
	data/mdr512.db
	data/census2000names.db
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";
const char *mount_point = "/sdcard";

void sqlite(void *pvParameter) {
    char db1_name[32];
    char db2_name[32];
	snprintf(db1_name, sizeof(db1_name)-1, "%s/census2000names.db", mount_point);
	snprintf(db2_name, sizeof(db2_name)-1, "%s/mdr512.db", mount_point);

	sqlite3 *db1;
	sqlite3 *db2;
	sqlite3_initialize();

	// Open database 1
	if (db_open(db1_name, &db1))
		vTaskDelete(NULL);
	if (db_open(db2_name, &db2))
		vTaskDelete(NULL);

	int rc = db_exec(db1, "Select * from surnames where name = 'MICHELLE'");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db2, "Select * from domain_rank where domain between 'google.com' and 'google.com.z'");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db1, "Select * from surnames where name = 'SPRINGER'");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db2, "Select * from domain_rank where domain = 'zoho.com'");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	sqlite3_close(db1);
	sqlite3_close(db2);
	printf("All Done\n");
	vTaskDelete(NULL);
}

void app_main()
{
	esp_err_t ret;

	// Options for mounting the filesystem.
	// If format_if_mount_failed is set to true, SD card will be partitioned and
	// formatted in case when mounting fails.
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = false,
		.max_files = 5,
		.allocation_unit_size = 16 * 1024
	};
	sdmmc_card_t *card;
	ESP_LOGI(TAG, "Initializing SD card");

	// Use settings defined above to initialize SD card and mount FAT filesystem.
	// Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
	// Please check its source code and implement error recovery when developing
	// production applications.

	ESP_LOGI(TAG, "Using SDMMC peripheral");

	// By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
	// For setting a specific frequency, use host.max_freq_khz (range 400kHz - 40MHz for SDMMC)
	// Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
	sdmmc_host_t host = SDMMC_HOST_DEFAULT();

	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

	// Set bus width to use:
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
	slot_config.width = 4;
#else
	slot_config.width = 1;
#endif

	// On chips where the GPIOs used for SD card can be configured, set them in
	// the slot_config structure:
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
	slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
	slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
	slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
	slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
	slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
	slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
#endif // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

	// Enable internal pullups on enabled pins. The internal pullups
	// are insufficient however, please make sure 10k external pullups are
	// connected on the bus. This is for debug / example purpose only.
	slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

	ESP_LOGI(TAG, "Mounting filesystem");
	ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. "
				"If you want the card to be formatted, set format_if_mount_failed = true.");
		} else {
			ESP_LOGE(TAG, "Failed to initialize the card (%s). "
				"Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
		}
		return;
	}
	ESP_LOGI(TAG, "Filesystem mounted");

	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);

	// Start task
	xTaskCreate(&sqlite, "SQLITE3", 1024*6, NULL, 5, NULL);
}
