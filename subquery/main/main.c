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
		"CREATE TABLE departments ( id INTEGER PRIMARY KEY, name TEXT, location TEXT);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1,
		"CREATE TABLE employees ( id INTEGER PRIMARY KEY, name TEXT, department_id INTEGER);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	char sql_command[128];
	char *departments_values[] = {
		"(1, 'Engineering', 'New York')",
		"(2, 'Marketing', 'London')",
		"(3, 'Sales', 'New York')"
	};
	for (int i=0;i<3;i++) {
		snprintf(sql_command, sizeof(sql_command)-1, 
			"INSERT INTO departments (id, name, location) VALUES %s;", departments_values[i]);
		//printf("sql_command=[%s]\n", sql_command);
		rc = db_exec(db1, sql_command);
		if (rc != SQLITE_OK) { vTaskDelete(NULL); }
	}
	
	char *employees_values[] = {
		"('Alice', 1)",
		"('Bob', 2)",
		"('Charlie', 3)",
		"('David', 1)"
	};
	for (int i=0;i<4;i++) {
		snprintf(sql_command, sizeof(sql_command)-1, 
			"INSERT INTO employees (name, department_id) VALUES %s;", employees_values[i]);
		//printf("sql_command=[%s]\n", sql_command);
		rc = db_exec(db1, sql_command);
		if (rc != SQLITE_OK) { vTaskDelete(NULL); }
	}

	rc = db_exec(db1, "SELECT * FROM departments;");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1, "SELECT * FROM employees;");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1, "SELECT id FROM departments WHERE location = 'New York';");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1,
		"SELECT name FROM employees WHERE department_id IN ( SELECT id FROM departments WHERE location = 'New York');");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1,
		"SELECT name FROM employees WHERE department_id IN (1, 3);");
	if (rc != SQLITE_OK) { vTaskDelete(NULL); }

	rc = db_exec(db1,
		"SELECT employees.name FROM employees INNER JOIN departments ON employees.department_id = departments.id WHERE departments.location = 'New York';");
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
