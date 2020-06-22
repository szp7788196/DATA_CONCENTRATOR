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

   
  
__lwip_dev lwipdev;						//lwip���ƽṹ�� 

//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0 = *(vu32*)(0x1FFF7A10);//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	//Ĭ��Զ��IPΪ:192.168.1.115
	lwipx->remoteip[0] = ConcentratorLocalNetConfig.remote_ip[0];	
	lwipx->remoteip[1] = ConcentratorLocalNetConfig.remote_ip[1];
	lwipx->remoteip[2] = ConcentratorLocalNetConfig.remote_ip[2];
	lwipx->remoteip[3] = ConcentratorLocalNetConfig.remote_ip[3];
	
	lwipx->remoteport = ConcentratorLocalNetConfig.remote_port;
	
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0] = 2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
	lwipx->mac[1] = 0;
	lwipx->mac[2] =0;
	lwipx->mac[3] = (sn0 >> 16) & 0XFF;//�����ֽ���STM32��ΨһID
	lwipx->mac[4] = (sn0 >> 8) & 0XFFF;;
	lwipx->mac[5] = sn0 & 0XFF; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0] = ConcentratorLocalNetConfig.local_ip[0];	
	lwipx->ip[1] = ConcentratorLocalNetConfig.local_ip[1];
	lwipx->ip[2] = ConcentratorLocalNetConfig.local_ip[2];
	lwipx->ip[3] = ConcentratorLocalNetConfig.local_ip[3];
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0] = ConcentratorLocalNetConfig.local_msak[0];	
	lwipx->netmask[1] = ConcentratorLocalNetConfig.local_msak[1];
	lwipx->netmask[2] = ConcentratorLocalNetConfig.local_msak[2];
	lwipx->netmask[3] = ConcentratorLocalNetConfig.local_msak[3];
	//Ĭ������:192.168.1.1
	lwipx->gateway[0] = ConcentratorLocalNetConfig.local_gate[0];	
	lwipx->gateway[1] = ConcentratorLocalNetConfig.local_gate[1];
	lwipx->gateway[2] = ConcentratorLocalNetConfig.local_gate[2];
	lwipx->gateway[3] = ConcentratorLocalNetConfig.local_gate[3];	
	
	lwipx->dhcpenable = ConcentratorLocalNetConfig.dhcp_enable;//û��DHCP	
}



























