/*
	sqlite for esp-idf

	This example code is in the Public Domain (or CC0 licensed, at your option.)
	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"

#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "SQL";

extern MessageBufferHandle_t xMessageBufferRedirect;

static int callback_redirect(void *data, int argc, char **argv, char **azColName) {
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
				ESP_LOGE(__FUNCTION__, "xMessageBufferSendFromISR fail tx_length=%d sended=%d", tx_length, sended);
			}
		} else {
			ESP_LOGE(__FUNCTION__, "xMessageBuffer is NULL");
		}
	}
	//printf("\n");
	return 0;
}

int db_redirect(MessageBufferHandle_t xMessageBuffer, sqlite3 *db, const char *sql) {
	ESP_LOGD(__FUNCTION__, "xMessageBuffer=[%p]", xMessageBuffer);
	char *zErrMsg = 0;
	printf("%s\n", sql);
	int rc = sqlite3_exec(db, sql, callback_redirect, xMessageBuffer, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	return rc;
}

void sqlite(void *pvParameters) {
	char *base_path = (char *)pvParameters;
	ESP_LOGI(TAG, "Start base_path=[%s]", base_path);

	// Remove database
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/test.db", base_path);
	unlink(db_name);

	// Open database
	sqlite3 *db;
	sqlite3_initialize();
	if (db_open(db_name, &db)) {
		ESP_LOGE(TAG, "DB Open fail");
		vTaskDelete(NULL);
	}

	// Create table
	int rc = db_exec(db, "CREATE TABLE test (id INTEGER, datetime);");
	if (rc != SQLITE_OK) {
		ESP_LOGE(TAG, "CREATE TABLE fail");
		vTaskDelete(NULL);
	}

	// Get local current time
	time_t now;
	struct tm timeinfo;
	time(&now);
	now = now + (CONFIG_LOCAL_TIMEZONE*60*60);
	localtime_r(&now, &timeinfo);
	char time_buf[64];
	strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);

	// Insert new record
	char sql_buf[128];
	sprintf(sql_buf, "INSERT INTO test VALUES (1, '%s');", time_buf);
	ESP_LOGI(TAG, "sql_buf=[%s]", sql_buf);
	rc = db_exec(db, sql_buf);
	if (rc != SQLITE_OK) {
		ESP_LOGE(TAG, "INSERT INTO fail");
		vTaskDelete(NULL);
	}

	while(1) {
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		ESP_LOGI(TAG, "ulTaskNotifyTake");

		// Get local current time
		time_t now;
		struct tm timeinfo;
		time(&now);
		now = now + (CONFIG_LOCAL_TIMEZONE*60*60);
		localtime_r(&now, &timeinfo);
		char time_buf[64];
		strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "time_buf=[%s]", time_buf);

		// Update record
		sprintf(sql_buf, "UPDATE test SET datetime = '%s' where id = 1;", time_buf);
		ESP_LOGI(TAG, "sql_buf=[%s]", sql_buf);
		rc = db_exec(db, sql_buf);
		if (rc != SQLITE_OK) {
			ESP_LOGE(TAG, "UPDATE fail");
			vTaskDelete(NULL);
		}

		// Query record
		strcpy(sql_buf, "SELECT datetime FROM test where id = 1;");
		rc = db_exec( db, sql_buf);
		if (rc != SQLITE_OK) {
			ESP_LOGE(TAG, "SELECT fail");
			vTaskDelete(NULL);
		}

		// Redirect
#if CONFIG_REDIRECT_TO_UDP
		ESP_LOGW(TAG, "Redirect to UDP");
		rc = db_redirect(xMessageBufferRedirect, db, sql_buf);
		if (rc != SQLITE_OK) {
			ESP_LOGE(TAG, "db_redirect fail");
			vTaskDelete(NULL);
		}
#endif
#if CONFIG_REDIRECT_TO_MQTT
		ESP_LOGW(TAG, "Redirect to MQTT");
		rc = db_redirect(xMessageBufferRedirect, db, sql_buf);
		if (rc != SQLITE_OK) {
			ESP_LOGE(TAG, "db_redirect fail");
			vTaskDelete(NULL);
		}
#endif
#if CONFIG_REDIRECT_TO_HTTP
		ESP_LOGW(TAG, "Redirect to HTTP");
		rc = db_redirect(xMessageBufferRedirect, db, sql_buf);
		if (rc != SQLITE_OK) {
			ESP_LOGE(TAG, "db_redirect fail");
			vTaskDelete(NULL);
		}
#endif
	} // end while

	sqlite3_close(db);
	printf("All Done\n");
	vTaskDelete(NULL);
}
