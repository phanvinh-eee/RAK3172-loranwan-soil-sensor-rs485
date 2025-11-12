/*
 * rs485.h
 *
 *  Created on: May 4, 2025
 *      Author: vinh
 */

#ifndef INC_RS485_H_
#define INC_RS485_H_
#include <stdint.h>
#define MODBUS_NODE_SIZE 3
#define MODBUS_DATA_SIZE 14
#define MODBUS_BUFFER_SIZE 128
#define RS485_SENSOR_PH 10001
#define RS485_SENSOR_NPK 10002
#define RS485_SENSOR_EC_TEMM_HUM 10003
#define RS485_SENSOR_PH_N01 10004

void receive_response_modbus_device();
void get_status_modbus_devices();
void check_response_modbus_device();
void read_rs485_sensor_state(uint8_t slave_addr, uint8_t code, uint16_t register_add, uint8_t len);
char *hex2str(uint8_t *data, uint8_t len);
void print_response_rs485();
uint8_t decode_rs485_data(void);
#endif /* INC_RS485_H_ */
