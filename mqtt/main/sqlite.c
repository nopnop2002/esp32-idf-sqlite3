/*
	sqlite for esp-idf

	This example code is in the Public Domain (or CC0 licensed, at your option.)
	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"

#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "SQL";

extern MessageBufferHandle_t xMessageBufferPublish;
extern MessageBufferHandle_t xMessageBufferSubscribe;
extern size_t xItemSize;

static int callback(void *data, int argc, char **argv, char **azColName) {
	ESP_LOGD(__FUNCTION__, "callback data=%p", data);

	// Initialize replay buffer
	memset(data, 0, 1);
	int data_len = 0;

	// Build reply buffer
	for (int i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		char work[128];
		int len = sprintf(work, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		strcat(data, work);
		data_len = data_len + len;
		ESP_LOGD(__FUNCTION__, "callback len=%d data_len=%d", len, data_len);
	}
	printf("\n");

	// Publish reply buffer
	size_t sended = xMessageBufferSend(xMessageBufferPublish, data, data_len, 100);
	if (sended != data_len) {
		ESP_LOGE(__FUNCTION__, "xMessageBufferSend fail data_len=%d sended=%d", data_len, sended);
	}
	return 0;
}

int mqtt_db_exec(sqlite3 *db, const char *sql, char *errmsg) {
	char *reply_buffer = (char *)malloc(1024);
	if (reply_buffer == NULL) {
		printf("malloc fail\n");
		sprintf(errmsg, "SQL error: reply_buffer malloc fail");
		return SQLITE_ERROR;
	}
	reply_buffer[0] = 0;
	ESP_LOGD(__FUNCTION__, "reply_buffer=%p", reply_buffer);

	char *zErrMsg = 0;
	printf("%s\n", sql);
	int rc = sqlite3_exec(db, sql, callback, reply_buffer, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sprintf(errmsg, "SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	free(reply_buffer);
	return rc;
}

void sqlite(void *pvParameters)
{
	char *base_path = (char *)pvParameters;
	ESP_LOGI(TAG, "Start base_path=[%s]", base_path);

	// Open database
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/example.db", base_path);
	sqlite3 *db;
	sqlite3_initialize();
	if (db_open(db_name, &db)) {
		ESP_LOGE(TAG, "db_open fail");
		vTaskDelete(NULL);
	}

	// Receive subscribe paylaad
	char rx_buffer[xItemSize];
	char errmsg[128];
	while (1) {
		size_t received = xMessageBufferReceive(xMessageBufferSubscribe, rx_buffer, sizeof(rx_buffer), portMAX_DELAY);
		ESP_LOGI(TAG, "xMessageBufferReceive received=%d", received);
		rx_buffer[received] = 0;
		ESP_LOGI(TAG, "xMessageBufferReceive rx_buffer=[%s]", rx_buffer);

		// Execte sql 
		int rc = mqtt_db_exec(db, rx_buffer, errmsg);
		ESP_LOGI(TAG, "rc=%d", rc);
		if (rc != SQLITE_OK) {
			int tx_len = strlen(errmsg);
			size_t sended = xMessageBufferSend(xMessageBufferPublish, errmsg, tx_len, 100);
			if (sended != tx_len) {
				ESP_LOGE(TAG, "xMessageBufferSend fail tx_len=%d sended=%d", tx_len, sended);
				break;
			}
		}
	} // end while

	// Never reach here
	printf("Closed database\n");
	sqlite3_close(db);
	vTaskDelete(NULL);
}
