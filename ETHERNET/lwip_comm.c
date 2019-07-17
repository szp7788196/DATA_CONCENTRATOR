#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "delay.h"
#include "usart.h"  
#include <stdio.h>
#include "common.h"

   
  
__lwip_dev lwipdev;						//lwip控制结构体 



//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0 = *(vu32*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节
	//默认远端IP为:192.168.1.115
//	lwipx->remoteip[0] = 183;	
//	lwipx->remoteip[1] = 3;
//	lwipx->remoteip[2] = 129;
//	lwipx->remoteip[3] = 44;
	
	lwipx->remoteip[0] = 192;	
	lwipx->remoteip[1] = 168;
	lwipx->remoteip[2] = 1;
	lwipx->remoteip[3] = 10;
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0] = 2;//高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
	lwipx->mac[1] = 0;
	lwipx->mac[2] =0;
	lwipx->mac[3] = (sn0 >> 16) & 0XFF;//低三字节用STM32的唯一ID
	lwipx->mac[4] = (sn0 >> 8) & 0XFFF;;
	lwipx->mac[5] = sn0 & 0XFF; 
	//默认本地IP为:192.168.1.30
	lwipx->ip[0] = 192;	
	lwipx->ip[1] = 168;
	lwipx->ip[2] = 1;
	lwipx->ip[3] = 10;
	//默认子网掩码:255.255.255.0
	lwipx->netmask[0] = 255;	
	lwipx->netmask[1] = 255;
	lwipx->netmask[2] = 255;
	lwipx->netmask[3] = 0;
	//默认网关:192.168.1.1
	lwipx->gateway[0] = 192;	
	lwipx->gateway[1] = 168;
	lwipx->gateway[2] = 1;
	lwipx->gateway[3] = 1;	
	lwipx->dhcpstatus = 0;//没有DHCP	
}



























