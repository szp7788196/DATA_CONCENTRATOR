#include "task_tcp_client.h"
#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip_comm.h"
#include "netconf.h"
#include "common.h"


struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 tcp_client_sendbuf[TCP_CLIENT_TX_BUFSIZE];	//TCP�ͻ��˷������ݻ�����
u8 tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

TaskHandle_t xHandleTaskTCP_CLIENT = NULL;

void vTaskTCP_CLIENT(void *pvParameters)
{
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;
	
	time_t time_heart_beat = SysTick1s;
	u8 heart_beat[24] = {0x68 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x04 ,0x68 ,0xE1 
,0x06 ,0x9C ,0xA5 ,0x25 ,0x8D ,0xEE ,0x46 ,0xE2 ,0x16 ,0xFE ,0xFD ,0xFC ,0xFB ,0xFA ,0xF9};

	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);

	while (1)
	{
#ifdef USE_DHCP
		WAIT_DHCP:
		if(DHCP_state != DHCP_ADDRESS_ASSIGNED &&
		   DHCP_state != DHCP_TIMEOUT)
		{
			delay_ms(500);
			
			goto WAIT_DHCP;
		}
#endif
		tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
		else if (err == ERR_OK)    //���������ӵ�����
		{
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				if(Usart1RecvEnd == 0xAA)
				{
					Usart1RecvEnd = 0;
					
					memcpy(tcp_client_sendbuf,Usart1RxBuf,Usart1FrameLen);
					
					data_len = Usart1FrameLen;
					
					tcp_client_flag |= LWIP_SEND_DATA;
				}
				else
				{
					if(SysTick1s - time_heart_beat >= 30)
					{
						time_heart_beat = SysTick1s;
						
						memcpy(tcp_client_sendbuf,heart_beat,24);
						
						data_len = 24;
					
						tcp_client_flag |= LWIP_SEND_DATA;
					}
				}
				
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					err = netconn_write(tcp_clientconn ,tcp_client_sendbuf,data_len,NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
					
					data_len=0;
				}

				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����
					}

//					data_len=0;  //������ɺ�data_lenҪ���㡣
//					memcpy(tcp_client_sendbuf,tcp_client_recvbuf,data_len);
//					printf("%s\r\n",tcp_client_recvbuf);
					
					memcpy(Usart1TxBuf,tcp_client_recvbuf,data_len);
					Usart1SendLen = data_len;
					data_len = 0;
					USART_ITConfig(USART1, USART_IT_TC, ENABLE);
					
					netbuf_delete(recvbuf);
//					tcp_client_flag |= LWIP_SEND_DATA;
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
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












