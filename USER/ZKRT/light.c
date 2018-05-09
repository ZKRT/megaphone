#include "light.h"
#include "zkrt.h"	//������Ҫ
#include "can.h"	//����CAN���ֽ�

uint32_t stand_count = 0XFFFFFFFF;
uint8_t can_value;
zkrt_packet_t zkrt_packet_buffer;

u8 zkrt_decode(zkrt_packet_t *packet) {
	while (CAN1_rx_check() == 1) {
		can_value = CAN1_rx_byte();
		if (zkrt_decode_char(&zkrt_packet_buffer, can_value) == 1) {
			return 1;
		}
	}
	return 0;
}
