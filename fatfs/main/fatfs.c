/*
	This example creates two databases on FATFS,
	inserts and retrieves data from them.
*/
#include <stdio.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";
const char *base_path = "/fatfs";

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0))
#define esp_vfs_fat_spiflash_mount esp_vfs_fat_spiflash_mount_rw_wl
#define esp_vfs_fat_spiflash_unmount esp_vfs_fat_spiflash_unmount_rw_wl
#endif

void sqlite(void *pvParameter) {
	// remove existing file
	char db1_name[32];
	char db2_name[32];
	snprintf(db1_name, sizeof(db1_name)-1, "%s/census2000names.db", base_path);
	snprintf(db2_name, sizeof(db2_name)-1, "%s/mdr512.db", base_path);
	unlink(db1_name);
	unlink(db2_name);

	sqlite3 *db1;
	sqlite3 *db2;
	sqlite3_initialize();

	if (db_open(db1_name, &db1))
		vTaskDelete(NULL);
	if (db_open(db2_name, &db2))
		vTaskDelete(NULL);

	int rc = db_exec(db1, "CREATE TABLE test1 (id INTEGER, content);");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db2, "CREATE TABLE test2 (id INTEGER, content);");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db1, "INSERT INTO test1 VALUES (1, 'Hello, World from test1');");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db2, "INSERT INTO test2 VALUES (1, 'Hello, World from test2');");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db1, "SELECT * FROM test1");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	rc = db_exec(db2, "SELECT * FROM test2");
	if (rc != SQLITE_OK) {
		vTaskDelete(NULL);
	}

	sqlite3_close(db1);
	sqlite3_close(db2);
	printf("All Done\n");
	vTaskDelete(NULL);
}

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
	xTaskCreate(&sqlite, "SQLITE3", 1024*4, NULL, 5, NULL);
}
