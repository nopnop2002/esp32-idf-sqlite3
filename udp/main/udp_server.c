/*
	BSD Socket UDP Broadcast Server

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"

#include "lwip/sockets.h"

#include "sqlite3.h"
#include "sqllib.h"

MessageBufferHandle_t xMessageBufferUDP;

// The total number of bytes (not messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 2048;
// The size, in bytes, required to hold each item in the message,
size_t xItemSize = 128;

static int callback(void *data, int argc, char **argv, char **azColName) {
	MessageBufferHandle_t *xMessageBuffer = (MessageBufferHandle_t *)data;
	ESP_LOGD(__FUNCTION__, "data=[%p] xMessageBuffer=[%p]", data, xMessageBuffer);
	int i;
	char tx_buffer[xItemSize];
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

int udp_db_exec(MessageBufferHandle_t xMessageBuffer, sqlite3 *db, const char *sql, char *errmsg) {
	ESP_LOGD(__FUNCTION__, "xMessageBuffer=[%p]", xMessageBuffer);
	char *zErrMsg = 0;
	printf("%s\n", sql);
	int rc = sqlite3_exec(db, sql, callback, xMessageBuffer, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sprintf(errmsg, "SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	return rc;
}

void udp_server(void *pvParameters)
{
	char *base_path = (char *)pvParameters;
	ESP_LOGI(pcTaskGetName(NULL), "Start UDP PORT=%d base_path=[%s]", CONFIG_UDP_PORT, base_path);

	// Create Message Buffer
	xMessageBufferUDP = xMessageBufferCreate(xBufferSizeBytes);
	configASSERT( xMessageBufferUDP );

	// Set up address to recvfrom
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CONFIG_UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); /* message from ANY */

	// Create the socket
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
	LWIP_ASSERT("sock >= 0", sock >= 0);

#if 0
	// Set option
	int broadcast=1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
	LWIP_ASSERT("ret >= 0", ret >= 0);
#endif

#if 0
	// Set timeout
	struct timeval tv;
	tv.tv_sec = 1; // 1sec
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif

	// Bind socket
	int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	LWIP_ASSERT("ret >= 0", ret >= 0);

	// Open database
	char db_name[32];
	snprintf(db_name, sizeof(db_name)-1, "%s/example.db", base_path);
	sqlite3 *db;
	sqlite3_initialize();
	if (db_open(db_name, &db)) {
		ESP_LOGE(pcTaskGetName(NULL), "db_open fail");
		vTaskDelete(NULL);
	}

	// Client Info data
	struct sockaddr_in clientInfo;
	socklen_t clientInfoLen = sizeof(clientInfo);
	char rx_buffer[128];
	while(1) {
		// Receive data
		int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr*)&clientInfo, &clientInfoLen);
		ESP_LOGI(pcTaskGetName(NULL), "recvfrom len=%d errno=%d", len, errno);
		if (len < 0) {
			ESP_LOGE(pcTaskGetName(NULL), "recvfrom failed: errno %d", errno);
			break;
		}
		if (len > 0) {
			ESP_LOGI(pcTaskGetName(NULL), "recvfrom rx_buffer=[%.*s]", len, rx_buffer);
			char clientStr[16];
			inet_ntop(AF_INET, &clientInfo.sin_addr, clientStr, sizeof(clientStr));
			ESP_LOGI(pcTaskGetName(NULL), "recvfrom : %s, port=%d", clientStr, ntohs(clientInfo.sin_port));

			// Remove the trailing newline code
			ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), rx_buffer, len, ESP_LOG_DEBUG);
			char str[3];
			str[0] = 0x0d;
			str[1] = 0x0a;
			str[2] = 0x0;
			char *pos = strstr(rx_buffer, str);
			if (pos != NULL) {
				len = pos - &rx_buffer[0];
				ESP_LOGI(pcTaskGetName(NULL), "pos=%p %d", pos, len);
			}
			rx_buffer[len] = 0;
			ESP_LOGD(pcTaskGetName(NULL), "[%.*s]", len, rx_buffer);
			ESP_LOGI(pcTaskGetName(NULL), "[%s]", rx_buffer);

			// Execute sql and send response
			char errmsg[128];
			int rc = udp_db_exec(xMessageBufferUDP, db, rx_buffer, errmsg);
			ESP_LOGI(pcTaskGetName(NULL), "udp_db_exec rc=%d", rc);
			if (rc != SQLITE_OK) {
				sendto(sock, errmsg, strlen(errmsg), 0, (const struct sockaddr *)&clientInfo, clientInfoLen);
			} else {
				char tx_buffer[xItemSize];
				while (1) {
					size_t received = xMessageBufferReceive(xMessageBufferUDP, tx_buffer, sizeof(tx_buffer), 100);
					ESP_LOGI(pcTaskGetName(NULL), "xMessageBufferReceive received=%d", received);
					if (received == 0) break;
					sendto(sock, tx_buffer, received, 0, (const struct sockaddr *)&clientInfo, clientInfoLen);
				}
			}

		}
	} // end while

	// close socket
	close(sock);
	vTaskDelete(NULL);
}

