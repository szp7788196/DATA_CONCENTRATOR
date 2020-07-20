#ifndef __EC20_H
#define __EC20_H

#include "sys.h"


#define EC20_PWRKEY		PCout(8)
#define EC20_RESET		PCout(0)
#define EC20_STATUS		PCin(2)
#define EC20_NETMOD		PCout(3)



#define TIMEOUT_1S 100
#define TIMEOUT_2S 200
#define TIMEOUT_3S 300
#define TIMEOUT_4S 400
#define TIMEOUT_5S 500
#define TIMEOUT_6S 600
#define TIMEOUT_7S 700
#define TIMEOUT_10S 1000
#define TIMEOUT_11S 1000
#define TIMEOUT_12S 1000
#define TIMEOUT_13S 1000
#define TIMEOUT_14S 1000
#define TIMEOUT_15S 1500
#define TIMEOUT_20S 2000
#define TIMEOUT_25S 2500
#define TIMEOUT_30S 3000
#define TIMEOUT_35S 3500
#define TIMEOUT_40S 4000
#define TIMEOUT_45S 4500
#define TIMEOUT_50S 5000
#define TIMEOUT_55S 5500
#define TIMEOUT_60S 6000
#define TIMEOUT_65S 6500
#define TIMEOUT_70S 7000
#define TIMEOUT_75S 7500
#define TIMEOUT_80S 8000
#define TIMEOUT_85S 8500
#define TIMEOUT_90S 9000
#define TIMEOUT_95S 9500
#define TIMEOUT_100S 10000
#define TIMEOUT_105S 10500
#define TIMEOUT_110S 11000
#define TIMEOUT_115S 11500
#define TIMEOUT_120S 12000
#define TIMEOUT_125S 12500
#define TIMEOUT_130S 13000
#define TIMEOUT_135S 13500
#define TIMEOUT_140S 14000
#define TIMEOUT_145S 14500
#define TIMEOUT_150S 15000
#define TIMEOUT_155S 15500
#define TIMEOUT_160S 16000
#define TIMEOUT_165S 16500
#define TIMEOUT_170S 17000
#define TIMEOUT_175S 17500
#define TIMEOUT_180S 18000


#define EC20_MAX_RECV_SEND_BUF_LEN	1024


typedef enum
{
    UNKNOW_STATE 	= 255,	//获取连接状态失败
	INITIAL			= 0,	//connection has not been established
	OPENING			= 1,	//client is connecting or server is trying to listen
	CONNECTED		= 2,	//已连接
	LISTENING		= 3,	//server is listening
	CLOSING			= 4,	//连接断开
	GOT_IP			= 5,	//已经分配到IP地址
	
} CONNECT_STATE_E;

typedef struct EC20Info					//EC20运行信息
{
	u8 *apn;							//apn
	u8 *iccid;							//iccid
	u8 *imsi;							//imsi
	u8 *imei;							//imei
	u8 *cnum;							//用户号码
	s8 csq;								//信号强度
}__attribute__((packed))EC20Info_S;


extern EC20Info_S EC20Info;
extern CONNECT_STATE_E EC20ConnectState;



void EC20_Init(void);
void ec20_soft_init(void);
u8 ec20_force_pwr_on(void);
u8 ec20_pwr_on(void);
u8 ec20_pwr_off(void);
void ec20_reset(void);
void ec20_send_data(unsigned char *buf,unsigned short len);
unsigned char ec20_wait_pwr_on_ready(u16 time_out_s);
unsigned char ec20_get_AT_ATI(void);
unsigned char ec20_set_AT_ATE0(void);
unsigned char ec20_get_AT_CPIN(void);
unsigned char ec20_get_AT_CGREG(void);
unsigned char ec20_set_AT_QICSGP(void);
unsigned char ec20_get_AT_CGSN(void);
unsigned char ec20_get_AT_QCCID(void);
unsigned char ec20_get_AT_CIMI(void);
unsigned char ec20_get_AT_CGDCONT(void);
unsigned char ec20_get_AT_CNUM(void);
unsigned char ec20_set_AT_QIACT(void);
unsigned char ec20_get_AT_QIACT(void);
unsigned char ec20_set_AT_QIOPEN(char *addr, char *port);
unsigned char ec20_set_AT_QICLOSE(void);
CONNECT_STATE_E ec20_get_AT_QISTATE(void);
unsigned char ec20_get_AT_CSQ(char *csq);
unsigned char ec20_get_AT_QISEND(unsigned char *buf,unsigned short len);
unsigned char ec20_get_AT_CCLK(char *buf);

























#endif
