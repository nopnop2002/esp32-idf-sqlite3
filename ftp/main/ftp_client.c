/*	BSD Socket FTP Client

	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"
#include "esp_log.h"
#include "cJSON.h"

#include "FtpClient.h"

extern MessageBufferHandle_t xMessageBufferRedirect;
extern size_t xItemSize;

static const char *TAG = "FTP";

void ftp_client(void *pvParameters) {
	char *base_path = (char *)pvParameters;
	ESP_LOGI(TAG, "Start base_path=[%s]", base_path);

	// Open FTP server
	ESP_LOGI(TAG, "ftp server:%s", CONFIG_FTP_SERVER);
	ESP_LOGI(TAG, "ftp user  :%s", CONFIG_FTP_USER);
	static NetBuf_t* ftpClientNetBuf = NULL;
	FtpClient* ftpClient = getFtpClient();
	int connect = ftpClient->ftpClientConnect(CONFIG_FTP_SERVER, CONFIG_FTP_PORT, &ftpClientNetBuf);
	ESP_LOGI(TAG, "connect=%d", connect);
	if (connect == 0) {
		ESP_LOGE(TAG, "FTP server connect fail");
		vTaskDelete(NULL);
	}

	// Login FTP server
	int login = ftpClient->ftpClientLogin(CONFIG_FTP_USER, CONFIG_FTP_PASSWORD, ftpClientNetBuf);
	ESP_LOGI(TAG, "login=%d", login);
	if (login == 0) {
		ESP_LOGE(TAG, "FTP server login fail");
		vTaskDelete(NULL);
	}

	char buffer[xItemSize];
	TickType_t xTicksToWait = portMAX_DELAY;

	char localFileName[64];
	char remoteFileName[64];
#if CONFIG_FTP_FILE_FORMAT_JSON
	sprintf(localFileName, "%s/local.json", base_path);
	sprintf(remoteFileName, "sqlite.json");
	cJSON *root = NULL;
	cJSON *object = NULL;
#elif CONFIG_FTP_FILE_FORMAT_CSV
	sprintf(localFileName, "%s/local.csv", base_path);
	sprintf(remoteFileName, "sqlite.csv");
	char headerBuffer[256];
	char dataBuffer[256];
	bool needHeader = true;
#endif
	FILE* f_local = NULL;
	bool inRecord = false;
	char itemName[128];
	char itemValue[128];
	int itemCounter;
	while(1) {
		size_t received = xMessageBufferReceive(xMessageBufferRedirect, buffer, sizeof(buffer), xTicksToWait);
		ESP_LOGD(TAG, "xMessageBufferReceive received=%d", received);
		if (received > 0) {
			ESP_LOGI(TAG, "xMessageBufferReceive buffer=[%.*s]",received, buffer);
			buffer[received] = 0;
			if (strcmp(buffer, "<sql>") == 0) {
				ESP_LOGI(TAG, "Start SQL");
				xTicksToWait = 10;

				f_local = fopen(localFileName, "w");
				if (f_local == NULL) {
					ESP_LOGE(TAG, "Failed to open local file");
					break;
				}
#if CONFIG_FTP_FILE_FORMAT_JSON
				root = cJSON_CreateArray();
#endif
			} else if (strcmp(buffer, "<record>") == 0) {
				inRecord = true;
				itemCounter = 0;
#if CONFIG_FTP_FILE_FORMAT_JSON
				object = cJSON_CreateObject();
#elif CONFIG_FTP_FILE_FORMAT_CSV
				memset(headerBuffer, 0, sizeof(headerBuffer));
				memset(dataBuffer, 0, sizeof(dataBuffer));
#endif
			} else if (strcmp(buffer, "</record>") == 0) {
				inRecord = false;
#if CONFIG_FTP_FILE_FORMAT_JSON
				//object are deleted in cJSON_AddItemToArray
				cJSON_AddItemToArray(root, object);
				//cJSON_Delete(object);
#elif CONFIG_FTP_FILE_FORMAT_CSV
				if (needHeader) {
					fprintf(f_local, "%s\n", headerBuffer);
					needHeader = false;
				}
				fprintf(f_local, "%s\n", dataBuffer);
#endif
			}

			if (inRecord) {
				char *pos = strstr(buffer, " = ");
				if (pos == NULL) continue;
				itemCounter++;
				ESP_LOGD(TAG, "pos=[%p] buffer=[%p] length=[%d] pos+3=[%s]", pos, buffer, pos - buffer, pos+3);
				memset(itemName, 0, sizeof(itemName));
				strncpy(itemName, buffer, (int)(pos - buffer));
				memset(itemValue, 0, sizeof(itemValue));
				strcpy(itemValue, pos+3);
				ESP_LOGD(TAG, "itemName=[%s] itemValue=[%s]", itemName, itemValue);
#if CONFIG_FTP_FILE_FORMAT_JSON
				cJSON_AddStringToObject(object, itemName, itemValue);
#elif CONFIG_FTP_FILE_FORMAT_CSV
				if (itemCounter == 1) {
					strcat(headerBuffer,"\"");;
					strcat(dataBuffer,"\"");;
				} else {
					strcat(headerBuffer,",\"");;
					strcat(dataBuffer,",\"");;
				}
				strcat(headerBuffer,itemName);
				strcat(headerBuffer,"\"");;
				strcat(dataBuffer,itemValue);
				strcat(dataBuffer,"\"");
#endif
			}
		} else {
			ESP_LOGI(TAG, "End SQL");
			xTicksToWait = portMAX_DELAY;
#if CONFIG_FTP_FILE_FORMAT_JSON
			//char *my_json_string = cJSON_PrintUnformatted(root);
			char *my_json_string = cJSON_Print(root);
			ESP_LOGD(TAG, "my_json_string ----------\n%s\n----------",my_json_string);
			fprintf(f_local, "%s", my_json_string);
			cJSON_free(my_json_string);
			cJSON_Delete(root);
#endif
			if (f_local != NULL) fclose(f_local);
			ftpClient->ftpClientPut(localFileName, remoteFileName, FTP_CLIENT_TEXT, ftpClientNetBuf);
#if CONFIG_FTP_FILE_FORMAT_JSON
#elif CONFIG_FTP_FILE_FORMAT_CSV
			needHeader = true;
#endif
		}
	}

	// close ftp
	ftpClient->ftpClientQuit(ftpClientNetBuf);
	vTaskDelete( NULL );
}

