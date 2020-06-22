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
#include "concentrator_conf.h"

   
  
__lwip_dev lwipdev;						//lwip控制结构体 

//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0 = *(vu32*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节
	//默认远端IP为:192.168.1.115
	lwipx->remoteip[0] = ConcentratorLocalNetConfig.remote_ip[0];	
	lwipx->remoteip[1] = ConcentratorLocalNetConfig.remote_ip[1];
	lwipx->remoteip[2] = ConcentratorLocalNetConfig.remote_ip[2];
	lwipx->remoteip[3] = ConcentratorLocalNetConfig.remote_ip[3];
	
	lwipx->remoteport = ConcentratorLocalNetConfig.remote_port;
	
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0] = 2;//高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
	lwipx->mac[1] = 0;
	lwipx->mac[2] =0;
	lwipx->mac[3] = (sn0 >> 16) & 0XFF;//低三字节用STM32的唯一ID
	lwipx->mac[4] = (sn0 >> 8) & 0XFFF;;
	lwipx->mac[5] = sn0 & 0XFF; 
	//默认本地IP为:192.168.1.30
	lwipx->ip[0] = ConcentratorLocalNetConfig.local_ip[0];	
	lwipx->ip[1] = ConcentratorLocalNetConfig.local_ip[1];
	lwipx->ip[2] = ConcentratorLocalNetConfig.local_ip[2];
	lwipx->ip[3] = ConcentratorLocalNetConfig.local_ip[3];
	//默认子网掩码:255.255.255.0
	lwipx->netmask[0] = ConcentratorLocalNetConfig.local_msak[0];	
	lwipx->netmask[1] = ConcentratorLocalNetConfig.local_msak[1];
	lwipx->netmask[2] = ConcentratorLocalNetConfig.local_msak[2];
	lwipx->netmask[3] = ConcentratorLocalNetConfig.local_msak[3];
	//默认网关:192.168.1.1
	lwipx->gateway[0] = ConcentratorLocalNetConfig.local_gate[0];	
	lwipx->gateway[1] = ConcentratorLocalNetConfig.local_gate[1];
	lwipx->gateway[2] = ConcentratorLocalNetConfig.local_gate[2];
	lwipx->gateway[3] = ConcentratorLocalNetConfig.local_gate[3];	
	
	lwipx->dhcpenable = ConcentratorLocalNetConfig.dhcp_enable;//没有DHCP	
}



























