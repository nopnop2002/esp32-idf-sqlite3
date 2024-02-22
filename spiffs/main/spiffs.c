/*
	This example creates two databases on SPIFFS,
	inserts and retrieves data from them.
*/
#include <stdio.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";
const char *base_path = "/spiffs";

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
	ESP_LOGI(TAG, "Initializing SPIFFS");
	esp_vfs_spiffs_conf_t conf = {
		.base_path = base_path,
		.partition_label = "storage",
		.max_files = 5,
		.format_if_mount_failed = true
	};
	
	// Use settings defined above to initialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}
		return;
	}

#if 0
	ESP_LOGI(TAG, "Performing SPIFFS_check().");
	ret = esp_spiffs_check(conf.partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		return;
	} else {
		ESP_LOGI(TAG, "SPIFFS_check() successful");
	}
#endif
	
	size_t total = 0, used = 0;
	//ret = esp_spiffs_info("storage", &total, &used);
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
		return;
	} else {
		ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
	}

	// Start task
	xTaskCreate(&sqlite, "SQLITE3", 1024*4, NULL, 5, NULL);
}
