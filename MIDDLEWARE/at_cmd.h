#ifndef AT_CMD_H
#define AT_CMD_H
#include <stdint.h>
#include "ringbuf.h"


extern RingBuf *result_ptr;

typedef void (*sendmsg)(uint8_t * buf, uint32_t len);
void register_cmd_handler(sendmsg func,void *result_buf,volatile char *flag);
char AT_SendCmd(char* cmd, uint8_t *result,uint16_t waittime,uint8_t retry,uint16_t timeout);
char AT_SendData(uint8_t* data, uint16_t len,uint8_t *result,uint16_t waittime,uint8_t retry,uint16_t timeout);
#endif
