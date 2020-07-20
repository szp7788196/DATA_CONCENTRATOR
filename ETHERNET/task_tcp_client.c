#include "task_tcp_client.h"
#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip_comm.h"
#include "netconf.h"
#include "common.h"
#include "usart6.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "concentrator_conf.h"
#include "concentrator_comm.h"


u8 ETH_ConnectState = ETH_UNKNOW;
struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
unsigned portBASE_TYPE SatckTCP_CLIENT;

TaskHandle_t xHandleTaskTCP_CLIENT = NULL;

void vTaskTCP_CLIENT(void *pvParameters)
{
//	u16 i = 0;
	u8 *msg = NULL;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t server_port,loca_port;
	
	server_port = lwipdev.remoteport;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1)
	{
		LoginResponse = 0;
		ETH_ConnectState = ETH_UNKNOW;
		
//#ifdef USE_DHCP
		if(lwipdev.dhcpenable == 1)
		{
			WAIT_DHCP:
			if(DHCP_state != DHCP_ADDRESS_ASSIGNED &&
			   DHCP_state != DHCP_TIMEOUT)
			{
				delay_ms(1000);
				
				goto WAIT_DHCP;
			}
		}
//#endif
		
		tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		
		if(err != ERR_OK)  
		{
			netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
			
			delay_ms(1000);
		}
		else if(err == ERR_OK)    //���������ӵ�����
		{
			struct netbuf *recvbuf;
			
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			
			while(1)
			{
				ETH_ConnectState = ETH_CONNECTED;
				
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{
					for(q = recvbuf->p; q != NULL; q = q->next)  //����������pbuf����
					{
						msg = (u8 *)q->payload;
						
						fifo_put(dl_buf_id,q->len,msg);
					}

					netbuf_delete(recvbuf);
				}
				
				RecvNetFrameAndPushToRxQueue(MODE_ETH);
				
				PullEthTxQueueAndSendFrame();
				
				if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
				
				delay_ms(100);
				
				SatckTCP_CLIENT = uxTaskGetStackHighWaterMark(NULL);
			}
		}
	}
}

void PullEthTxQueueAndSendFrame(void)
{
	BaseType_t xResult;
	ServerFrame_S *tx_frame = NULL;

	xResult = xQueueReceive(xQueue_EthFrameTx,(void *)&tx_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		netconn_write(tcp_clientconn ,tx_frame->buf,tx_frame->len,NETCONN_COPY); //����tcp_server_sentbuf�е�����
		
		vPortFree(tx_frame->buf);
		tx_frame->buf = NULL;

		vPortFree(tx_frame);
		tx_frame = NULL;
	}
}

void tcp_client_init(void)
{
	xTaskCreate(vTaskTCP_CLIENT,    				/* ָʾ������  */
				"vTaskTCP_CLIENT",  				/* ��������    */
				configMINIMAL_STACK_SIZE * 8,       /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        						/* �������  */
				configMAX_PRIORITIES - 3,           /* �������ȼ�*/
				&xHandleTaskTCP_CLIENT); 			/* ������  */
}












