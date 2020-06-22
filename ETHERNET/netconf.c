/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Network connection configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "common.h"
#include "netconf.h"
#include "tcpip.h"
#include <stdio.h>
#include "lwip_comm.h"

#include "dp83848.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */
extern __IO uint32_t  EthStatus;
//#ifdef USE_DHCP
__IO uint8_t DHCP_state;
//#endif /* USE_DHCP */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
	
//#ifndef USE_DHCP
	if(lwipdev.dhcpenable == 0)
	{
//		uint8_t iptab[4] = {0};
//		uint8_t iptxt[20];
	}
 
//#endif
  /* Create tcp_ip stack thread */
  tcpip_init( NULL, NULL );	

  lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息

  /* IP address setting */
//#ifdef USE_DHCP
	if(lwipdev.dhcpenable == 1)
	{
		ipaddr.addr = 0;
		netmask.addr = 0;
		gw.addr = 0;
	}
	else
	{
//#else
		IP4_ADDR(&ipaddr, lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
		IP4_ADDR(&netmask, lwipdev.netmask[0], lwipdev.netmask[1] , lwipdev.netmask[2], lwipdev.netmask[3]);
		IP4_ADDR(&gw, lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);

		printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
		printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
		printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	}
//#endif  

  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
  struct ip_addr *netmask, struct ip_addr *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))

  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface.*/
  netif_set_default(&xnetif);

  if (EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG))
  { 
    /* Set Ethernet link flag */
    xnetif.flags |= NETIF_FLAG_LINK_UP;

    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&xnetif);
//#ifdef USE_DHCP
	if(lwipdev.dhcpenable == 1)
	{
		DHCP_state = DHCP_START;
	}
//#endif /* USE_DHCP */
  }
  else
  {
    /*  When the netif link is down this function must be called.*/
    netif_set_down(&xnetif);
//#ifdef USE_DHCP
	if(lwipdev.dhcpenable == 1)
	{
		DHCP_state = DHCP_LINK_DOWN;
	}
//#endif /* USE_DHCP */
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&xnetif, ETH_link_callback);
}

//#ifdef USE_DHCP

TaskHandle_t xHandleTaskDHCP = NULL;
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void LwIP_DHCP_task(void * pvParameters)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	uint32_t IPaddress;
	uint32_t NETmask;
	uint32_t GATEway;

	while(1)
	{
		switch (DHCP_state)
		{
			case DHCP_START:
			{
				dhcp_start(&xnetif);
				/* IP address should be set to 0 
				every time we want to assign a new DHCP address*/
				IPaddress = 0;
				DHCP_state = DHCP_WAIT_ADDRESS;
				
				printf("正在查找DHCP服务器,请稍等...........\r\n"); 
			}
			break;

			case DHCP_WAIT_ADDRESS:
			{
				printf("正在获取地址...\r\n");
				/* Read the new IP address */
				IPaddress=xnetif.ip_addr.addr;		//读取新IP地址
				NETmask=xnetif.netmask.addr;//读取子网掩码
				GATEway=xnetif.gw.addr;			//读取默认网关 

				if (IPaddress!=0) 
				{
					printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
					//解析出通过DHCP获取到的IP地址
					lwipdev.ip[3]=(uint8_t)(IPaddress>>24); 
					lwipdev.ip[2]=(uint8_t)(IPaddress>>16);
					lwipdev.ip[1]=(uint8_t)(IPaddress>>8);
					lwipdev.ip[0]=(uint8_t)(IPaddress);
					printf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
					//解析通过DHCP获取到的子网掩码地址
					lwipdev.netmask[3]=(uint8_t)(NETmask>>24);
					lwipdev.netmask[2]=(uint8_t)(NETmask>>16);
					lwipdev.netmask[1]=(uint8_t)(NETmask>>8);
					lwipdev.netmask[0]=(uint8_t)(NETmask);
					printf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
					//解析出通过DHCP获取到的默认网关
					lwipdev.gateway[3]=(uint8_t)(GATEway>>24);
					lwipdev.gateway[2]=(uint8_t)(GATEway>>16);
					lwipdev.gateway[1]=(uint8_t)(GATEway>>8);
					lwipdev.gateway[0]=(uint8_t)(GATEway);
					printf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);

					DHCP_state = DHCP_ADDRESS_ASSIGNED;	

					/* Stop DHCP */
					dhcp_stop(&xnetif);
				}
				else
				{
					/* DHCP timeout */
					if (xnetif.dhcp->tries > MAX_DHCP_TRIES)
					{
						/* Static address used */
						IP4_ADDR(&ipaddr, lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
						IP4_ADDR(&netmask, lwipdev.netmask[0], lwipdev.netmask[1] , lwipdev.netmask[2], lwipdev.netmask[3]);
						IP4_ADDR(&gw, lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);

						printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
						printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
						printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
						printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);

						netif_set_addr(&xnetif, &ipaddr , &netmask, &gw);
						
						DHCP_state = DHCP_TIMEOUT;

						/* Stop DHCP */
						dhcp_stop(&xnetif);
					}
				}
			}
			break;

			default: break;
		}

		/* wait 250 ms */
		delay_ms(250);
	}   
}
//#endif  /* USE_DHCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
