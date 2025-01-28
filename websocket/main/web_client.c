/*
	Example using WEB Socket.
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
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "cJSON.h"

#include "sqlite3.h"
#include "sqllib.h"

#include "websocket_server.h"

extern MessageBufferHandle_t xMessageBufferToClient;

static int callback(void *data, int argc, char **argv, char **azColName) {
	char out[128];
	char DEL = 0x04;
	//printf("%s: ", (const char*)data);
	for (int i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		int outlen = sprintf(out,"REPLY%c%s = %s", DEL, azColName[i], argv[i] ? argv[i] : "NULL");
		ws_server_send_text_all(out, outlen);
	}
	printf("\n");
	return 0;
}

int sock_db_exec(sqlite3 *db, const char *sql, char *errmsg) {
	char *zErrMsg = 0;
	printf("%s\n", sql);
	char out[128];
	char DEL = 0x04;
	int outlen = sprintf(out,"REPLY%c%s", DEL, sql);
	ws_server_send_text_all(out, outlen);
	int rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sprintf(errmsg, "SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	return rc;
}

void client_task(void* pvParameters) {
	char *base_path = (char *)pvParameters;
	ESP_LOGI(pcTaskGetName(NULL), "Start base_path=[%s]", base_path);

	// Open database
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/example.db", base_path);
	sqlite3 *db;
	sqlite3_initialize();
	if (db_open(db_name, &db)) {
		ESP_LOGE(pcTaskGetName(NULL), "db_open fail");
		vTaskDelete(NULL);
	}

	char messageBuffer[512];
	while(1) {
		size_t readBytes = xMessageBufferReceive(xMessageBufferToClient, messageBuffer, sizeof(messageBuffer), portMAX_DELAY );
		ESP_LOGD(pcTaskGetName(NULL), "readBytes=%d", readBytes);
		cJSON *root = cJSON_Parse(messageBuffer);
		if (cJSON_GetObjectItem(root, "id")) {
			char *id = cJSON_GetObjectItem(root,"id")->valuestring;
			ESP_LOGI(pcTaskGetName(NULL), "id=%s",id);

			// Do something when the browser started
			if ( strcmp (id, "init") == 0) {
			} // end of init

			// Do something when the send button pressed.
			if ( strcmp (id, "send-request") == 0) {
			} // end of send-request

			if ( strcmp (id, "text") == 0) {
				char *sql = cJSON_GetObjectItem(root,"data")->valuestring;
				ESP_LOGI(pcTaskGetName(NULL),"sql=[%s]", sql);
				if (strlen(sql) != 0) {
					// Execte sql 
					char errmsg[128];
					int rc = sock_db_exec(db, sql, errmsg);
					ESP_LOGI(pcTaskGetName(NULL), "rc=%d", rc);
					if (rc != SQLITE_OK) {
						char out[256];
						char DEL = 0x04;
						int outlen = sprintf(out,"REPLY%c%s", DEL, errmsg);
						ESP_LOGI(pcTaskGetName(NULL),"outlen=%d out=[%s]", outlen, out);
						ws_server_send_text_all(out, outlen);
					}
				}
			} // end of text

		}
		cJSON_Delete(root);
	}
	vTaskDelete(NULL);
}
