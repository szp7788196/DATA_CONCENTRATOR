#ifndef __COMMON_H
#define __COMMON_H

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"
#include <time.h>

/*---------------------------------------------------------------------------*/
/* Type Definition Macros                                                    */
/*---------------------------------------------------------------------------*/
#ifndef __WORDSIZE
  /* Assume 32 */
  #define __WORDSIZE 32
#endif

    typedef unsigned char   uint8;
    typedef char            int8;
    typedef unsigned short  uint16;
    typedef short           int16;
    typedef unsigned int    uint32;
    typedef int             int32;

#ifdef WIN32
    typedef int socklen_t;
#endif

    typedef unsigned long long int  uint64;
    typedef long long int           int64;


#define DEBUG_LOG


#define SOFT_WARE_VRESION						101			//软件版本号
#define HARD_WARE_VRESION						101			//硬件版本号

#define DEFAULT_TIME_SEC						86400

 
#define CONCENTRATOR_BASIC_CONF_ADD				0			//集控器基础参数配置EEPROM存储地址
#define CONCENTRATOR_BASIC_CONF_LEN				127

#define CONCENTRATOR_RUN_MODE_ADD				151			//集控器运行模式EEPROM存储地址
#define CONCENTRATOR_RUN_MODE_LEN				3

#define CONCENTRATOR_ALARM_CONF_ADD				155			//集控器告警参数配置EEPROM存储地址
#define CONCENTRATOR_ALARM_CONF_LEN				24

#define CONCENTRATOR_GATE_WAY_ADD				184			//网关ID存储地址
#define CONCENTRATOR_GATE_WAY_LEN				6

#define CONCENTRATOR_LOCATION_CONF_ADD			201			//集控器经纬度年表参数配置EEPROM存储地址
#define CONCENTRATOR_LOCATION_CONF_LEN			1506

#define CONCENTRATOR_LOCAL_NET_CONF_ADD			1800		//集控器本地网络参数配置存储地址
#define CONCENTRATOR_LOCAL_NET_CONF_LEN			26

#define FW_STATE_ADD							1721		//固件更新状态EEPROM存储地址
#define FW_STATE_LEN							79

#define RELAY_MODULE_APPOINTMENT_NUM_ADD		1874		//继电器模块策略配置数量地址
#define RELAY_MODULE_APPOINTMENT_NUM_LEN		6

#define RELAY_MODULE_STRATEGY_NUM_ADD			1880		//继电器模块预约配置数量地址
#define RELAY_MODULE_STRATEGY_NUM_LEN			6

#define RELAY_MODULE_CONF_NUM_ADD				1886		//继电器模块配置数量地址
#define RELAY_MODULE_CONF_NUM_LEN				6

#define RELAY_MODULE_BASIC_CONF_ADD				1892		//继电器模块基础配置
#define RELAY_MODULE_BASIC_CONF_LEN				9

#define RELAY_MODULE_CONF_ADD					1901		//继电器模块配置地址
#define RELAY_MODULE_CONF_LEN					277

#define RELAY_MODULE_ALARM_CONF_ADD				3301		//继电器模块告警参数配置
#define RELAY_MODULE_ALARM_CONF_LEN				5

#define RELAY_MODULE_APPOINTMENT_ADD			3351		//继电器模块预约控制配置
#define RELAY_MODULE_APPOINTMENT_LEN			95

#define RELAY_MODULE_STRATEGY_ADD				4351		//继电器模块策略配置
#define RELAY_MODULE_STRATEGY_LEN				38

#define RELAY_MODULE_STRATEGY_SWITCH_ADD		1848		//策略组(模式)切换配置
#define RELAY_MODULE_STRATEGY_SWITCH_LEN		29

#define INPUT_COLLECTOR_BASIC_CONF_ADD			15790		//输入量采集模块基础配置
#define INPUT_COLLECTOR_BASIC_CONF_LEN			5

#define INPUT_COLLECTOR_CONF_NUM_ADD			15795		//输入量采集模块配置数量地址
#define INPUT_COLLECTOR_CONF_NUM_LEN			6

#define INPUT_COLLECTOR_CONF_ADD				15801		//输入量采集模块配置地址
#define INPUT_COLLECTOR_CONF_LEN				259

#define INPUT_COLLECTOR_ALARM_CONF_ADD			18391		//输入量采集模块告警参数配置
#define INPUT_COLLECTOR_ALARM_CONF_LEN			4

#define ELECTRICITY_IN_METER_RATIO_ADD			190			//内置电表变比
#define ELECTRICITY_IN_METER_RATIO_LEN			10

#define ELECTRICITY_METER_BASIC_CONF_ADD		17551		//电表基础配置
#define ELECTRICITY_METER_BASIC_CONF_LEN		5

#define ELECTRICITY_METER_CONF_NUM_ADD			17556		//电表配置数量地址
#define ELECTRICITY_METER_CONF_NUM_LEN			6

#define ELECTRICITY_METER_CONF_ADD				17562		//电表配置地址
#define ELECTRICITY_METER_CONF_LEN				994

#define ELECTRICITY_METER_ALARM_CONF_ADD		23526		//电表告警参数配置
#define ELECTRICITY_METER_ALARM_CONF_LEN		3

#define LAMP_LIST_NUM_ADD						23533		//灯具配置数量表
#define LAMP_LIST_NUM_LEN						68

#define LAMP_NODE_LOSS_ALARM_CONF_ADD			23601		//灯具节点丢失告警配置地址
#define LAMP_NODE_LOSS_ALARM_CONF_LEN			3

#define LAMP_APPOINTMENT_NUM_ADD				23608		//灯具策略配置数量地址
#define LAMP_APPOINTMENT_NUM_LEN				6

#define LAMP_STRATEGY_NUM_ADD					23614		//灯具预约配置数量地址
#define LAMP_STRATEGY_NUM_LEN					6

#define LAMP_GROUP_LIST_NUM_ADD					23620		//灯具每组配置数量表
#define LAMP_GROUP_LIST_NUM_LEN					22

#define LAMP_BASIC_CONG_ADD						23642		//灯具基础配置
#define LAMP_BASIC_CONG_LEN						18

#define LAMP_FW_STATE_ADD						23660		//灯具固件更新状态EEPROM存储地址
#define LAMP_FW_STATE_LEN						79

#define LAMP_APPOINTMENT_ADD					23739		//单灯预约控制配置
#define LAMP_APPOINTMENT_LEN					95

#define LAMP_CONF_ADD							24689		//单灯基础配置
#define LAMP_CONF_LEN							31

#define LAMP_STRATEGY_ADD						40561		//单灯策略配置
#define LAMP_STRATEGY_LEN						51

#define LAMP_STRATEGY_LIST_NUM_ADD				66673		//单灯任务配置数量列表
#define LAMP_STRATEGY_LIST_NUM_LEN				68

#define LUMETER_CONF_NUM_ADD					70000		//光照计配置数量地址
#define LUMETER_CONF_NUM_LEN					6

#define LUMETER_BASIC_CONF_ADD					70006		//光照计基础配置
#define LUMETER_BASIC_CONF_LEN					25

#define LUMETER_ALARM_CONF_ADD					70031		//光照计告警参数配置
#define LUMETER_ALARM_CONF_LEN					4

#define LUMETER_CONF_ADD						70035		//光照计配置地址
#define LUMETER_CONF_LEN						42




static const uint32_t crc32tab[] = 
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
	0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
	0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
	0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
	0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
	0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
	0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
	0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
	0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
	0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
	0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
	0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
	0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
	0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
	0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
	0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
	0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
	0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
	0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
	0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
	0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
	0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
	0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
	0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
	0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
	0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
	0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL 
};

static const u8 auchCRCHi[] = 
{
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
    0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,
    0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,
    0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
    0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
    0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
    0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
    0x80,0x41,0x00,0xC1,0x81,0x40
    } ;
    /* CRC低位字节值表*/
static const u8 auchCRCLo[] = 
{
    0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,
    0x07,0xC7,0x05,0xC5,0xC4,0x04,0xCC,0x0C,0x0D,0xCD,
    0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
    0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,
    0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,0x14,0xD4,
    0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
    0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,
    0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
    0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,
    0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,
    0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,
    0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
    0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,
    0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,
    0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
    0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
    0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,
    0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
    0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,
    0x70,0xB0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
    0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,
    0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,
    0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4B,0x8B,
    0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,
    0x43,0x83,0x41,0x81,0x80,0x40
};


typedef struct	Uint32TypeNumber
{
	u32 number;
	
	u16 crc16;
	
}__attribute__((packed))Uint32TypeNumber_S;

typedef struct	TimeRange					//时间段
{
	u8 s_month;								//起始月
	u8 s_date;								//起始日
	u8 s_hour;								//起始时
	u8 s_minute;							//起始分
	
	u8 e_month;								//终止月
	u8 e_date;								//终止日
	u8 e_hour;								//终止时
	u8 e_minute;							//终止分
	
	u8 week_enable;							//星期限制

}__attribute__((packed))TimeRange_S;


extern SemaphoreHandle_t  xMutex_USART2;
extern SemaphoreHandle_t  xMutex_SPI2;
extern SemaphoreHandle_t  xMutex_RTC;
extern SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameRx;
extern SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameTx;
extern SemaphoreHandle_t  xMutex_SPI_FLASH;
extern SemaphoreHandle_t  xMutex_Push_xQueue_AlarmReportSend;
extern SemaphoreHandle_t  xMutex_Push_xQueue_AlarmReportStore;
extern SemaphoreHandle_t  xMutex_TransServerFrameStruct;
extern SemaphoreHandle_t  xMutex_RelayStrategy;
extern SemaphoreHandle_t  xMutex_RelayAppointment;
extern SemaphoreHandle_t  xMutex_Rs485Rs485Frame;


extern QueueHandle_t xQueue_ServerFrameRx;
extern QueueHandle_t xQueue_4gFrameTx;
extern QueueHandle_t xQueue_WifiFrameTx;
extern QueueHandle_t xQueue_EthFrameTx;
extern QueueHandle_t xQueue_NB_IoTFrameTx;
extern QueueHandle_t xQueue_ConcentratorFrameStruct;
extern QueueHandle_t xQueue_LampFrameStruct;
extern QueueHandle_t xQueue_RelayFrameStruct;
extern QueueHandle_t xQueue_InputCollectorFrameStruct;
extern QueueHandle_t xQueue_ElectricityMeterFrameStruct;
extern QueueHandle_t xQueue_LumeterFrameStruct;
extern QueueHandle_t xQueue_AlarmReportSend;
extern QueueHandle_t xQueue_AlarmReportStore;
extern QueueHandle_t xQueue_AlarmReportRead;
extern QueueHandle_t xQueue_HistoryRecordRead;
extern QueueHandle_t xQueue_RelayModuleState;
extern QueueHandle_t xQueue_InputCollectorState;
extern QueueHandle_t xQueue_ElectricityMeterState;
extern QueueHandle_t xQueue_Rs485Rs485Frame;
extern QueueHandle_t xQueue_RelayRs485Frame;
extern QueueHandle_t xQueue_InputCollectorRs485Frame;
extern QueueHandle_t xQueue_ElectricityMeterRs485Frame;
extern QueueHandle_t xQueue_LumeterRs485Frame;
extern QueueHandle_t xQueue_PlcFrame;
extern QueueHandle_t xQueue_LampPlcFrame;
extern QueueHandle_t xQueue_LampPlcExecuteTaskToPlc;
extern QueueHandle_t xQueue_LampPlcExecuteTaskFromPlc;
extern QueueHandle_t xQueue_LampState;
extern QueueHandle_t xQueue_LampPlcExecuteTaskState;
extern QueueHandle_t xQueue_TransTransFrame;


extern time_t SysTick1s;
extern time_t SysTick10ms;

void bubbleSort(u32 *arr,int n);
time_t GetSysTick1s(void);
time_t GetSysTick10ms(void);
u8 char_upper(u8 c);
u8 myisspace(int x);
u8 myisdigit(int x);
int myatoi(const char *nptr);
void myitoa(int num,char *str,int radix);
u8 GetDatBit(u32 dat);
u32 GetADV(u8 len);
void IntToString(u8 *DString,u32 Dint,u8 zero_num);
void TimeToString(u8 *str,u16 year, u8 month, u8 date, u8 hour, u8 minute, u8 second);
void Int4BitToString(u8 *str,u16 num);
void HexToStr(char *pbDest, u8 *pbSrc, u16 len);
int my_toupper( int ch);
void StrToHex(u8 *pbDest, char *pbSrc, u16 len);

u32 CRC32(const u8 *buf, u32 size);
u32 CRC32Extend(const u8 *buf, u32 size, u32 temp,u8 flag);
u16 CRC16(u8 *puchMsgg,u16 usDataLen);
u8 CalCheckSum(u8 *buf, u16 len);
u8 leap_year_judge(u16 year);
u32 get_days_by_calendar(u16 year,u8 month,u8 date);
u32 get_minutes_by_calendar(u8 month,u8 date,u8 hour,u8 minute);

u16 MyStrstr(u8 *str1, u8 *str2, u16 str1_len, u16 str2_len);
unsigned short find_str(unsigned char *s_str, unsigned char *p_str, unsigned short count, unsigned short *seek);
int search_str(unsigned char *source, unsigned char *target);
unsigned short get_str1(unsigned char *source, unsigned char *begin, unsigned short count1, unsigned char *end, unsigned short count2, unsigned char *out);
unsigned short get_str2(unsigned char *source, unsigned char *begin, unsigned short count, unsigned short length, unsigned char *out);
unsigned short get_str3(unsigned char *source, unsigned char *out, unsigned short length);

u8 GetMemoryForSpecifyPointer(u8 **str,u16 size, u8 *memory);


















void ReadTotalConfigurationParameters(void);










#endif
