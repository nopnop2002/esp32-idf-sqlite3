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
#include "esp_timer.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";
const char *mount_point = "/sdcard";

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO CONFIG_EXAMPLE_PIN_MISO
#define PIN_NUM_MOSI CONFIG_EXAMPLE_PIN_MOSI
#define PIN_NUM_CLK CONFIG_EXAMPLE_PIN_CLK
#define PIN_NUM_CS CONFIG_EXAMPLE_PIN_CS


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
		.format_if_mount_failed = true,
		.max_files = 5,
		.allocation_unit_size = 16 * 1024
	};
	sdmmc_card_t *card;
	ESP_LOGI(TAG, "Initializing SD card");

	// Use settings defined above to initialize SD card and mount FAT filesystem.
	// Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
	// Please check its source code and implement error recovery when developing
	// production applications.
	ESP_LOGI(TAG, "Using SPI peripheral");

	// By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
	// For setting a specific frequency, use host.max_freq_khz (range 400kHz - 20MHz for SDSPI)
	// Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();

	spi_bus_config_t bus_cfg = {
		.mosi_io_num = PIN_NUM_MOSI,
		.miso_io_num = PIN_NUM_MISO,
		.sclk_io_num = PIN_NUM_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 4000,
	};
	ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize bus.");
		return;
	}

	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
	sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
	slot_config.gpio_cs = PIN_NUM_CS;
	slot_config.host_id = host.slot;

	ESP_LOGI(TAG, "Mounting filesystem");
	ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
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
