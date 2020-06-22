#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H
#include "common.h"
#include "dp83848.h"
  
//#define USE_DHCP	

   

//lwip控制结构体
typedef struct  
{
	u8 mac[6];      //MAC地址
	u8 remoteip[4];	//远端主机IP地址 
	u16 remoteport;	//远端主机端口号
	u8 ip[4];       //本机IP地址
	u8 netmask[4]; 	//子网掩码
	u8 gateway[4]; 	//默认网关的IP地址
	
	u8 dhcpenable;	//DHCP开启标志
	
	vu8 dhcpstatus;	//dhcp状态 
					//0,未获取DHCP地址;
					//1,进入DHCP获取状态
					//2,成功获取DHCP地址
					//0XFF,获取失败.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip控制结构体



void lwip_comm_default_ip_set(__lwip_dev *lwipx);



#endif













