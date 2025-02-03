/*	BSD Socket TCP Server Example

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lwip/sockets.h"

#include "sqlite3.h"
#include "sqllib.h"

// Connect with timeout
esp_err_t waitForConnect (int listen_sock, int sec, int usec) {
	fd_set master_set, working_set;
	int max_sd = listen_sock;
	FD_ZERO(&master_set);
	FD_SET(listen_sock, &master_set);

	struct timeval timeout;
	timeout.tv_sec	= sec;
	timeout.tv_usec = usec;
	memcpy(&working_set, &master_set, sizeof(master_set));
	ESP_LOGD(pcTaskGetName(NULL), "Waiting on connect...");
	int err = select(max_sd + 1, &working_set, NULL, NULL, &timeout);
	ESP_LOGI(pcTaskGetName(NULL), "select err=%d", err);
	if (err < 0) {
		ESP_LOGE(pcTaskGetName(NULL), "waitForConnect select() fail");
		return ESP_FAIL;
	}
	if (err == 0) {
		ESP_LOGW(pcTaskGetName(NULL), "waitForConnect select() timed out");
		return ESP_ERR_TIMEOUT;
	}
	ESP_LOGI(pcTaskGetName(NULL), "waitForConnect select() success");
	return ESP_OK;
}

static int callback(void *data, int argc, char **argv, char **azColName) {
	int *sock = (int *)data;
	ESP_LOGD(pcTaskGetName(NULL), "data=%p %p %d", data, sock, *sock);
	int i;
	char tx_buffer[128];
	for (i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		sprintf(tx_buffer, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		int err = send(*sock, tx_buffer, strlen(tx_buffer), 0);
		if (err < 0) {
			ESP_LOGE(pcTaskGetName(NULL), "Error occurred during sending: errno %d", errno);
		}
	}
	printf("\n");
	return 0;
}

int tcp_db_exec(int sock, sqlite3 *db, const char *sql, char *errmsg) {
	ESP_LOGD(pcTaskGetName(NULL), "sock=%p %d", &sock, sock);
	char *zErrMsg = 0;
	printf("%s\n", sql);
	int rc = sqlite3_exec(db, sql, callback, &sock, &zErrMsg);
	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sprintf(errmsg, "SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("Operation done successfully\n");
	}
	return rc;
}

void tcp_server(void *pvParameters)
{
	char *base_path = (char *)pvParameters;
	ESP_LOGI(pcTaskGetName(NULL), "Start TCP PORT=%d base_path=[%s]", CONFIG_TCP_PORT, base_path);

	// set up address to recv
	char addr_str[128];
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	//dest_addr.sin_port = htons(PORT);
	dest_addr.sin_port = htons(CONFIG_TCP_PORT);
	int addr_family = AF_INET;
	int ip_protocol = IPPROTO_IP;
	inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (listen_sock < 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Unable to create socket: errno %d", errno);
		return;
	}
	ESP_LOGI(pcTaskGetName(NULL), "Socket created");

	int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Socket unable to bind: errno %d", errno);
		return;
	}
	ESP_LOGI(pcTaskGetName(NULL), "Socket bound, port %d", CONFIG_TCP_PORT);

	err = listen(listen_sock, 1);
	if (err != 0) {
		ESP_LOGE(pcTaskGetName(NULL), "Error occurred during listen: errno %d", errno);
		return;
	}
	ESP_LOGI(pcTaskGetName(NULL), "Socket listening");

	while (1) {
		// Wait for connect
		ESP_LOGI(pcTaskGetName(NULL), "Waiting for connect");
		err = waitForConnect (listen_sock, 5, 0);
		if (err == ESP_ERR_TIMEOUT) continue;
		if (err == ESP_FAIL) break;

		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0) {
			ESP_LOGE(pcTaskGetName(NULL), "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(pcTaskGetName(NULL), "Socket accepted. sock=%d", sock);

		// Open database
		char db_name[32];
		snprintf(db_name, sizeof(db_name)-1, "%s/example.db", base_path);
		sqlite3 *db;
		sqlite3_initialize();
		if (db_open(db_name, &db)) {
			ESP_LOGE(pcTaskGetName(NULL), "db_open fail");
			vTaskDelete(NULL);
		}

		while (1) {
			char rx_buffer[128];
			int len = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
			ESP_LOGI(pcTaskGetName(NULL), "recv len=%d errno=%d", len, errno);

			// Error occurred during receiving
			if (len < 0) {
				ESP_LOGE(pcTaskGetName(NULL), "recv failed: errno %d", errno);
				break;
			}
			// Connection closed by client
			else if (len == 0) {
				ESP_LOGI(pcTaskGetName(NULL), "Connection closed by client");
				break;
			}
			// Data received
			else {
				// Get the sender's ip address as string
				if (source_addr.sin6_family == PF_INET) {
					inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
				} else if (source_addr.sin6_family == PF_INET6) {
					inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
				}
				ESP_LOGI(pcTaskGetName(NULL), "Received %d bytes from %s:", len, addr_str);

				// Remove the trailing newline code
				ESP_LOG_BUFFER_HEXDUMP(pcTaskGetName(NULL), rx_buffer, len, ESP_LOG_DEBUG);
				char str[3];
				str[0] = 0x0d;
				str[1] = 0x0a;
				str[2] = 0x0;
				char *pos = strstr(rx_buffer, str);
				if (pos != NULL) {
					len = pos - &rx_buffer[0];
					ESP_LOGD(pcTaskGetName(NULL), "pos=%p %d", pos, len);
				}
				rx_buffer[len] = 0;
				ESP_LOGI(pcTaskGetName(NULL), "[%.*s]", len, rx_buffer);
				ESP_LOGI(pcTaskGetName(NULL), "[%s]", rx_buffer);

				// Execute sql 
				char tx_buffer[128];
				char errmsg[128];
				int rc = tcp_db_exec(sock, db, rx_buffer, errmsg);
				ESP_LOGI(pcTaskGetName(NULL), "rc=%d", rc);
				if (rc != SQLITE_OK) {
					strcpy(tx_buffer, errmsg);
					int err = send(sock, tx_buffer, strlen(tx_buffer), 0);
					if (err < 0) {
						ESP_LOGE(pcTaskGetName(NULL), "Error occurred during sending: errno %d", errno);
						break;
					}
				}
			}
		} // end while

		ESP_LOGI(pcTaskGetName(NULL), "Close socket");
		sqlite3_close(db);
		printf("Closed database\n");
		close(sock);
	} // end while

	// Never reach here
	vTaskDelete(NULL);
}
