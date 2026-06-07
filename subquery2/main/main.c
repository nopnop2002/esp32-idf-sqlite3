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
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/census2000names.db", base_path);
	unlink(db_name);

	sqlite3 *db1;
	sqlite3_initialize();

	if (db_open(db_name, &db1)) vTaskDelete(NULL);

	int rc;
	rc = db_exec(db1,
		"CREATE TABLE pricelist(flower_ID integer primary key, flower_fruit text, family_ID integer, price integer);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	char sql_command[128];
	char *values[] = {
		"(10,'rose',10,150)",
		"(20,'azalea',20,200)",
		"(30,'orange',30,500)",
		"(1,'apple',10,120)",
		"(2,'grapefruit',30,150)",
		"(3,'rhododendron',20,250)",
		"(4,'blueberry',20,370)",
		"(5,'plum',10,400)",
		"(6,'peach',10,390)"
	};
	for (int i=0;i<9;i++) {
		snprintf(sql_command, sizeof(sql_command)-1, 
			"INSERT INTO pricelist (flower_ID,flower_fruit,family_ID,price) VALUES %s;", values[i]);
		//printf("sql_command=[%s]\n", sql_command);
		rc = db_exec(db1, sql_command);
		if (rc != SQLITE_OK) { vTaskDelete(NULL); }
	}
	
	rc = db_exec(db1, "SELECT * FROM pricelist;");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1, 
		"SELECT flower_fruit, price from pricelist where price = (select max(price) from pricelist);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1, 
		"SELECT flower_fruit, price from pricelist where price = (select min(price) from pricelist);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	sqlite3_close(db1);
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
