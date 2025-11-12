/*
 * rs485.c
 *
 *  Created on: May 4, 2025
 *      Author: vinh
 */
#include "rs485.h"
#include "usart.h"
#include "sys_app.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;
char buffer[128];
uint8_t rs485_buffer[MODBUS_BUFFER_SIZE];
uint8_t rs485_cnt = 0;
uint8_t sensor_is_update = 0;
uint8_t sensor_addr_present = 0;
uint16_t rs485_type[MODBUS_NODE_SIZE] = {RS485_SENSOR_PH_N01, RS485_SENSOR_NPK, RS485_SENSOR_EC_TEMM_HUM};
uint16_t rs485_res_size[] = {7, 11, 11};
uint8_t sensor_uplink_data[MODBUS_DATA_SIZE];

void UART1_SendData(uint8_t *data, uint8_t len);
uint16_t crc16(uint8_t *data, uint8_t length);

void get_state_modbus_device(uint16_t type, uint8_t id);

void get_status_modbus_devices(){
	static uint8_t event_update_cnt = 0;
	sensor_addr_present = event_update_cnt;
	sprintf(buffer, "read status: addr %d, type %d\r\n",
			sensor_addr_present, rs485_type[sensor_addr_present]);
	UART_SendString(buffer);
	event_update_cnt++;
	if(event_update_cnt >= MODBUS_NODE_SIZE){
		event_update_cnt = 0;
	}
	if(sensor_addr_present >= MODBUS_NODE_SIZE){
		sensor_addr_present = 0;
	}	
	get_state_modbus_device(rs485_type[sensor_addr_present], sensor_addr_present);
}

void get_state_modbus_device(uint16_t type, uint8_t id){
	uint8_t addr = id + 1;
	rs485_cnt = 0;
	switch(type){
	case RS485_SENSOR_PH:
		read_rs485_sensor_state(addr, 0x03, 0x03, 1);
		break;
	case RS485_SENSOR_NPK:
		read_rs485_sensor_state(addr, 0x03, 0x1e, 3);
		break;
	case RS485_SENSOR_EC_TEMM_HUM:
		read_rs485_sensor_state(addr, 0x03, 0x00, 3);
		break;
	case RS485_SENSOR_PH_N01:
		read_rs485_sensor_state(addr, 0x03, 0x00, 1);
		break;
	default:
		break;
	}
}

void read_rs485_sensor_state(uint8_t slave_addr, uint8_t code, uint16_t register_add, uint8_t len){
	uint8_t data[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	data[0] = slave_addr;
	data[1] = code;
	data[2] = register_add >> 8;
	data[3] = register_add & 0xff;
	data[5] = len;
	uint16_t crc = crc16(data, sizeof(data)-2);
	data[6] = crc & 0xff;
	data[7] = crc >> 8;
	__HAL_RCC_USART1_CLK_ENABLE();
	MX_USART1_UART_Init();

	HAL_UART_Receive_IT(&huart1, &rs485_buffer[0], rs485_res_size[sensor_addr_present]);
	HAL_Delay(100);
	UART1_SendData(data, 8);
	char* rs485_data = hex2str(data, 8);
	sprintf(buffer, "rs485 write: %s, size %d\r\n", rs485_data, rs485_res_size[sensor_addr_present]);
	UART_SendString(buffer);
}

void receive_response_modbus_device(){
	rs485_cnt = rs485_res_size[sensor_addr_present];
	if(rs485_cnt >= MODBUS_BUFFER_SIZE){
		rs485_cnt = 0;
	}
	// Xử lý dữ liệu nhận được ở đây (Rx_data)
	if(rs485_cnt == rs485_res_size[sensor_addr_present]){
		decode_rs485_data();		
		if(rs485_type[sensor_addr_present] != RS485_SENSOR_EC_TEMM_HUM){			
		 	get_status_modbus_devices();
		}
	}
}

void print_response_rs485(){
	if(rs485_cnt > 0){
		sprintf(buffer, "rs485 log: %s\r\n", hex2str(rs485_buffer, rs485_cnt));
		UART_SendString(buffer);
	}
}

uint8_t decode_rs485_data(void){
	if(rs485_cnt == 0) return 0;
//	sprintf(buffer, "rs485 res: %s\r\n", hex2str(rs485_buffer, rs485_cnt));
//	UART_SendString(buffer);
	uint16_t type = rs485_type[sensor_addr_present];
	switch(type){
	case RS485_SENSOR_PH:
	case RS485_SENSOR_PH_N01:	
	  sprintf(buffer, "rs485 PH: %d\r\n", rs485_buffer[4]);
	  UART_SendString(buffer);
	  if(rs485_buffer[4] > 140) break;	
	  sensor_uplink_data[7] = rs485_buffer[4];		
		break;
	case RS485_SENSOR_NPK:
		memcpy (&sensor_uplink_data[8], &rs485_buffer[3], 6);
	  uint16_t n,p,k;
	  n = rs485_buffer[3] << 8 | rs485_buffer[4];
	  p = rs485_buffer[5] << 8 | rs485_buffer[6];
	  k = rs485_buffer[7] << 8 | rs485_buffer[8];
	  sprintf(buffer, "rs485 NPK: %d %d %d\r\n", n, p, k);
	  UART_SendString(buffer);
		break;
	case RS485_SENSOR_EC_TEMM_HUM:
		memcpy (&sensor_uplink_data[1], &rs485_buffer[7], 2);//ec
		memcpy (&sensor_uplink_data[3], &rs485_buffer[5], 2);//temp
		memcpy (&sensor_uplink_data[5], &rs485_buffer[3], 2);//humi
	  uint16_t ec,temp,humi;
	  ec = rs485_buffer[7] << 8 | rs485_buffer[8];
	  temp = rs485_buffer[5] << 8 | rs485_buffer[6];
	  humi = rs485_buffer[3] << 8 | rs485_buffer[4];
	  sprintf(buffer, "rs485 ec %d, temp %d, humi %d\r\n", ec, temp, humi);
	  UART_SendString(buffer);
		break;
	default:
		break;
	}
	return 1;
}

void UART1_SendData(uint8_t *data, uint8_t len) {
    HAL_UART_Transmit(&huart1, data, len, HAL_MAX_DELAY); // Replace huart2 with your UART instance
}

uint16_t crc16(uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;

    for (uint8_t i = 0; i < length; i++) {
		crc ^= data[i];
		for (uint8_t j = 8; j > 0; j--) {
			if (crc & 0x0001) {
			crc = (crc >> 1) ^ 0xA001;
			} else {
			crc >>= 1;
			}
		}
    }

    return crc;
}

char *hex2str(uint8_t *data, uint8_t len)
{
    static char str[101];
    const char hex[] = "0123456789abcdef";
    if (len > 50) {
        len = 50;
    }
    if (data  == NULL) {
    	APP_LOG(TS_ON, VLEVEL_L, "hex2str NULL");
        return str;
    }
    for (uint8_t i = 0; i < len; i++)
    {
        str[i * 2] = hex[data[i] >> 4];
        str[i * 2 + 1] = hex[data[i] & 0x0F];
    }
    str[len * 2] = 0;
    return str;
}
