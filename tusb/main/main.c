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
#include <time.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"

#include "sqlite3.h"
#include "sqllib.h"

static const char *TAG = "MAIN";

QueueHandle_t xQueueTinyUsb;

// The total number of bytes (not single messages) the message buffer will be able to hold at any one time.
size_t xBufferSizeBytes = 1024;

uint8_t prompt[] = "sqlite> ";
int lprompt = 8;

uint8_t crlf[2] = { 0x0d, 0x0a };
uint8_t cr[1] = { 0x0d };

// callback for rx
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
	size_t rx_size = 0;
	ESP_LOGD(TAG, "CONFIG_TINYUSB_CDC_RX_BUFSIZE=%d", CONFIG_TINYUSB_CDC_RX_BUFSIZE);
	uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
	esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
	if (ret == ESP_OK) {
		ESP_LOGD(TAG, "Data from channel=%d rx_size=%d", itf, rx_size);
		//ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
		for(int i=0;i<rx_size;i++) {
			xQueueSendFromISR(xQueueTinyUsb, &buf[i], NULL);
		}
#if CONFIG_LOCAL_ECHO
		/* write back */
		tinyusb_cdcacm_write_queue(itf, buf, rx_size);
		tinyusb_cdcacm_write_flush(itf, 0);
#endif

	} else {
		ESP_LOGE(TAG, "tinyusb_cdcacm_read error");
	}
}

// callback for line state changed
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
	int dtr = event->line_state_changed_data.dtr;
	int rts = event->line_state_changed_data.rts;
	ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

// callback for sql execute
static int callback(void *data, int argc, char **argv, char **azColName) {
	uint8_t tx_buffer[128];
	for (int i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		int tx_length = sprintf((char *)tx_buffer, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, tx_buffer, tx_length);
		tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, cr, 1);
	}
	tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
	printf("\n");
	return 0;
}

int tusb_db_exec(sqlite3 *db, const char *sql, char *errmsg) {
	char *zErrMsg = 0;
	printf("%s\n", sql);
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

void sqlite(void *pvParameters)
{
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

	uint8_t buffer[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
	int index = 0;
	while(1) {
		char ch;
		if(xQueueReceive(xQueueTinyUsb, &ch, portMAX_DELAY)) {
			ESP_LOGD(pcTaskGetName(NULL), "ch=0x%x",ch);
			if (ch == 0x0d) {
				ESP_LOGI(pcTaskGetName(NULL), "buffer=[%.*s]", index, buffer);
				if (index == 0) {
					tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, crlf, 2);
					tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, prompt, lprompt);
					tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
				} else {
					// Execute sql
					tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, crlf, 2);
					char errmsg[128];
					int rc = tusb_db_exec(db, (char *)buffer, errmsg);
					ESP_LOGI(pcTaskGetName(NULL), "rc=%d", rc);
					if (rc != SQLITE_OK) {
						uint8_t reply[128];
						int sendBytes = sprintf((char *)reply, "%s", errmsg);
						tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, reply, sendBytes);
						tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, crlf, 2);
						tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, prompt, lprompt);
						tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
					} else {
						tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, cr, 1);
						tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, prompt, lprompt);
						tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
					}
					index = 0;
				}
			} else if (ch == 0x0a) {

			} else {
				if (index < CONFIG_TINYUSB_CDC_RX_BUFSIZE) {
					buffer[index++] = ch;
					buffer[index] = 0;
				} else {
					ESP_LOGW(pcTaskGetName(NULL), "buffer overflow");
				}
			}
		} // end xQueueReceive
	} // end while
	vTaskDelete(NULL);
}

esp_err_t mountSPIFFS(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed = true
	};

	// Use settings defined above to initialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

#if 0
	ESP_LOGI(TAG, "Performing SPIFFS_check().");
	ret = esp_spiffs_check(conf.partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		return ret;
	} else {
			ESP_LOGI(TAG, "SPIFFS_check() successful");
	}
#endif

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Mount %s to %s success", path, label);
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	return ret;
}

void app_main(void)
{
	// Initialize spiffs
	char *base_path = "/spiffs";
	ESP_ERROR_CHECK(mountSPIFFS(base_path, "storage", 2));

	// Initialize tinyusb driver
	const tinyusb_config_t tusb_cfg = {
		.device_descriptor = NULL,
		.string_descriptor = NULL,
		.external_phy = false,
		.configuration_descriptor = NULL,
	};
	ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

	// Enable USB Serial Device
	// first way to register a callback
	tinyusb_config_cdcacm_t acm_cfg = {
		.usb_dev = TINYUSB_USBDEV_0,
		.cdc_port = TINYUSB_CDC_ACM_0,
		.rx_unread_buf_sz = 64,
		.callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
		.callback_rx_wanted_char = NULL,
		.callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
		.callback_line_coding_changed = NULL
	};
	ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

#if 0
	// second way to register a callback
	ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
		TINYUSB_CDC_ACM_0,
		CDC_EVENT_LINE_STATE_CHANGED,
		&tinyusb_cdc_line_state_changed_callback));
#endif

	// Create Queue
	xQueueTinyUsb = xQueueCreate(100, sizeof(char));
	configASSERT( xQueueTinyUsb );

	// Start sqlite execute tasks
	xTaskCreate(&sqlite, "SQLITE", 1024*6, (void *)base_path, 5, NULL);

	while(1) {
		vTaskDelay(1);
	}
}
