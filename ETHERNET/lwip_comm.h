#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H
#include "common.h"
#include "dp83848.h"
  
//#define USE_DHCP	

   

//lwip���ƽṹ��
typedef struct  
{
	u8 mac[6];      //MAC��ַ
	u8 remoteip[4];	//Զ������IP��ַ 
	u16 remoteport;	//Զ�������˿ں�
	u8 ip[4];       //����IP��ַ
	u8 netmask[4]; 	//��������
	u8 gateway[4]; 	//Ĭ�����ص�IP��ַ
	
	u8 dhcpenable;	//DHCP������־
	
	vu8 dhcpstatus;	//dhcp״̬ 
					//0,δ��ȡDHCP��ַ;
					//1,����DHCP��ȡ״̬
					//2,�ɹ���ȡDHCP��ַ
					//0XFF,��ȡʧ��.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip���ƽṹ��



void lwip_comm_default_ip_set(__lwip_dev *lwipx);



#endif













