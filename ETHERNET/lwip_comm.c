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

   
  
__lwip_dev lwipdev;						//lwip���ƽṹ�� 



//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0 = *(vu32*)(0x1FFF7A10);//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	//Ĭ��Զ��IPΪ:192.168.1.115
//	lwipx->remoteip[0] = 183;	
//	lwipx->remoteip[1] = 3;
//	lwipx->remoteip[2] = 129;
//	lwipx->remoteip[3] = 44;
	
	lwipx->remoteip[0] = 192;	
	lwipx->remoteip[1] = 168;
	lwipx->remoteip[2] = 1;
	lwipx->remoteip[3] = 10;
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0] = 2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
	lwipx->mac[1] = 0;
	lwipx->mac[2] =0;
	lwipx->mac[3] = (sn0 >> 16) & 0XFF;//�����ֽ���STM32��ΨһID
	lwipx->mac[4] = (sn0 >> 8) & 0XFFF;;
	lwipx->mac[5] = sn0 & 0XFF; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0] = 192;	
	lwipx->ip[1] = 168;
	lwipx->ip[2] = 1;
	lwipx->ip[3] = 10;
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0] = 255;	
	lwipx->netmask[1] = 255;
	lwipx->netmask[2] = 255;
	lwipx->netmask[3] = 0;
	//Ĭ������:192.168.1.1
	lwipx->gateway[0] = 192;	
	lwipx->gateway[1] = 168;
	lwipx->gateway[2] = 1;
	lwipx->gateway[3] = 1;	
	lwipx->dhcpstatus = 0;//û��DHCP	
}



























