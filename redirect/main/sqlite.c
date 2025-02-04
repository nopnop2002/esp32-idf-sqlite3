/*
	This example creates one databases on SPIFFS,
	Query whether a table and record exist.
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

extern MessageBufferHandle_t xMessageBufferQuery;
extern MessageBufferHandle_t xMessageBufferRedirect;

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

void sqlite(void *pvParameters) {
	char *base_path = (char *)pvParameters;
	ESP_LOGI(TAG, "Start base_path=[%s]", base_path);

	// Open database
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/test.db", base_path);
	sqlite3 *db;
	sqlite3_initialize();
	if (db_open(db_name, &db)) vTaskDelete(NULL);

	// Inquiry
	int rc = db_query(xMessageBufferQuery, db, "select count(*) from sqlite_master where name = 'test';");
	if (rc != SQLITE_OK) vTaskDelete(NULL);

	// Read reply
	char sqlmsg[256];
	size_t readBytes;
	readBytes = xMessageBufferReceive(xMessageBufferQuery, sqlmsg, sizeof(sqlmsg), 100);
	ESP_LOGI(pcTaskGetName(NULL), "readBytes=%d", readBytes);
	if (readBytes == 0) vTaskDelete(NULL);
	sqlmsg[readBytes] = 0;
	ESP_LOGI(pcTaskGetName(NULL), "sqlmsg=[%s]", sqlmsg);

	// Create table
	if (strcmp(sqlmsg, "count(*) = 0") == 0) {
		int rc = db_query(xMessageBufferQuery, db, "CREATE TABLE test (id INTEGER, datetime);");
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		ESP_LOGW(pcTaskGetName(NULL), "Table created");

		// get local current time
		time_t now;
		struct tm timeinfo;
		time(&now);
		now = now + (CONFIG_LOCAL_TIMEZONE*60*60);
		localtime_r(&now, &timeinfo);
		char time_buf[64];
		strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);

		// insert new record
		char sql_buf[128];
		sprintf(sql_buf, "INSERT INTO test VALUES (1, '%s');", time_buf);
		ESP_LOGI(TAG, "sql_buf=[%s]", sql_buf);
		rc = db_query(xMessageBufferQuery, db, sql_buf);
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		ESP_LOGW(pcTaskGetName(NULL), "Record inserted");
	} else {
		ESP_LOGW(pcTaskGetName(NULL), "Table already exists");
	}

	while(1) {
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		ESP_LOGI(pcTaskGetName(NULL), "ulTaskNotifyTake");

		// get local current time
		time_t now;
		struct tm timeinfo;
		time(&now);
		now = now + (CONFIG_LOCAL_TIMEZONE*60*60);
		localtime_r(&now, &timeinfo);
		char time_buf[64];
		strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);
		ESP_LOGI(TAG, "time_buf=[%s]", time_buf);

		// update record
		char sql_buf[128];
		sprintf(sql_buf, "UPDATE test SET datetime = '%s' where id = 1;", time_buf);
		ESP_LOGI(TAG, "sql_buf=[%s]", sql_buf);
		rc = db_query(xMessageBufferQuery, db, sql_buf);
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		ESP_LOGW(pcTaskGetName(NULL), "Record updated");

		// query record
		strcpy(sql_buf, "SELECT datetime FROM test where id = 1;");
		rc = db_query(xMessageBufferQuery, db, sql_buf);
		if (rc != SQLITE_OK) vTaskDelete(NULL);
		while (1) {
			readBytes = xMessageBufferReceive(xMessageBufferQuery, sqlmsg, sizeof(sqlmsg), 100);
			ESP_LOGD(pcTaskGetName(NULL), "readBytes=%d", readBytes);
			if (readBytes == 0) break;
			sqlmsg[readBytes] = 0;
			ESP_LOGI(pcTaskGetName(NULL), "sqlmsg=[%s]", sqlmsg);
		}

		// redirect
#if CONFIG_REDIRECT_TO_UDP
		ESP_LOGW(pcTaskGetName(NULL), "Redirect to UDP");
		rc = db_query(xMessageBufferRedirect, db, sql_buf);
		if (rc != SQLITE_OK) vTaskDelete(NULL);
#endif
#if CONFIG_REDIRECT_TO_MQTT
		ESP_LOGW(pcTaskGetName(NULL), "Redirect to MQTT");
		rc = db_query(xMessageBufferRedirect, db, sql_buf);
		if (rc != SQLITE_OK) vTaskDelete(NULL);
#endif
	} // end while

	sqlite3_close(db);
	printf("All Done\n");
	vTaskDelete(NULL);
}
