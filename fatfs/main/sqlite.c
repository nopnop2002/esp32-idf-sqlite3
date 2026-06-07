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
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "SQLITE";

void sqlite(void *pvParameters) {
	char *base_path = (char *)pvParameters;
	ESP_LOGI(TAG, "Start base_path=[%s]", base_path);

	// remove existing file
	char db1_name[64];
	char db2_name[64];
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
