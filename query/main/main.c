/*
	This example creates one databases on SPIFFS,
	Query whether a table and record exist.
*/
#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";
const char *base_path = "/spiffs";

static int callback(void *data, int argc, char **argv, char **azColName) {
	MessageBufferHandle_t *xMessageBuffer = (MessageBufferHandle_t *)data;
	ESP_LOGD(__FUNCTION__, "data=[%p] xMessageBuffer=[%p]", data, xMessageBuffer);
	int i;
	char tx_buffer[128];
	for (i = 0; i<argc; i++){
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		int tx_length = sprintf(tx_buffer, "%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
		if (xMessageBuffer) {
			size_t sended = xMessageBufferSendFromISR((MessageBufferHandle_t)xMessageBuffer, tx_buffer, tx_length, NULL);
			ESP_LOGD(__FUNCTION__, "sended=%d tx_length=%d", sended, tx_length);
			if (sended != tx_length) {
				ESP_LOGE(pcTaskGetName(NULL), "xMessageBufferSendFromISR fail tx_length=%d sended=%d", tx_length, sended);
			}
		} else {
			ESP_LOGE(pcTaskGetName(NULL), "xMessageBuffer is NULL");
		}
	}
	//printf("\n");
	return 0;
}

int db_query(MessageBufferHandle_t xMessageBuffer, sqlite3 *db, const char *sql) {
	ESP_LOGD(__FUNCTION__, "xMessageBuffer=[%p]", xMessageBuffer);
	char *zErrMsg = 0;
	printf("%s\n", sql);
	int rc = sqlite3_exec(db, sql, callback, xMessageBuffer, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	return rc;
}

void sqlite(void *pvParameter) {
	MessageBufferHandle_t xMessageBuffer = (MessageBufferHandle_t)pvParameter;
	ESP_LOGD(pcTaskGetName(0), "Start task_parameter=[%p]", xMessageBuffer);
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/test.db", base_path);

	sqlite3 *db;
	sqlite3_initialize();

	if (db_open(db_name, &db)) vTaskDelete(NULL);

	// Inquiry
	int rc = db_query(xMessageBuffer, db, "select count(*) from sqlite_master where name = 'test';");
	if (rc != SQLITE_OK) vTaskDelete(NULL);

	// Read reply
	char sqlmsg[256];
	size_t readBytes;
	readBytes = xMessageBufferReceive(xMessageBuffer, sqlmsg, sizeof(sqlmsg), 100);
	ESP_LOGI(pcTaskGetName(NULL), "readBytes=%d", readBytes);
	if (readBytes == 0) vTaskDelete(NULL);
	sqlmsg[readBytes] = 0;
	ESP_LOGI(pcTaskGetName(NULL), "sqlmsg=[%s]", sqlmsg);

	// Create table
	if (strcmp(sqlmsg, "count(*) = 0") == 0) {
		int rc = db_query(xMessageBuffer, db, "CREATE TABLE test (id INTEGER, content);");
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		ESP_LOGW(pcTaskGetName(NULL), "Table created");
	} else {
		ESP_LOGW(pcTaskGetName(NULL), "Table already exists");
	}

	// Inquiry
	rc = db_query(xMessageBuffer, db, "select count(*) from test;");
	if (rc != SQLITE_OK) vTaskDelete(NULL);

	// Read reply
	readBytes = xMessageBufferReceive(xMessageBuffer, sqlmsg, sizeof(sqlmsg), 100);
	ESP_LOGI(pcTaskGetName(NULL), "readBytes=%d", readBytes);
	if (readBytes == 0) vTaskDelete(NULL);
	sqlmsg[readBytes] = 0;
	ESP_LOGI(pcTaskGetName(NULL), "sqlmsg=[%s]", sqlmsg);

	// Insert record
	if (strcmp(sqlmsg, "count(*) = 0") == 0) {
		rc = db_query(xMessageBuffer, db, "INSERT INTO test VALUES (1, 'Hello, World');");
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		ESP_LOGW(pcTaskGetName(NULL), "Record inserted");
	} else {
		ESP_LOGW(pcTaskGetName(NULL), "Record already exists");
	}

	rc = db_query(xMessageBuffer, db, "SELECT * FROM test");
	if (rc != SQLITE_OK) vTaskDelete(NULL);
	while (1) {
		readBytes = xMessageBufferReceive(xMessageBuffer, sqlmsg, sizeof(sqlmsg), 100);
		ESP_LOGI(pcTaskGetName(NULL), "readBytes=%d", readBytes);
		if (readBytes == 0) break;
		sqlmsg[readBytes] = 0;
		ESP_LOGI(pcTaskGetName(NULL), "sqlmsg=[%s]", sqlmsg);
	}

	sqlite3_close(db);
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

	// Create Message Buffer
	MessageBufferHandle_t xMessageBuffer = xMessageBufferCreate(4096);
	configASSERT( xMessageBuffer );
	ESP_LOGD(TAG, "xMessageBuffer=[%p]", xMessageBuffer);

	// Start task
	xTaskCreate(&sqlite, "SQLITE3", 1024*6, (void *)xMessageBuffer, 5, NULL);
}
