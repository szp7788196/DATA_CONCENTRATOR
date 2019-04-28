#include "task_tcp_client.h"
#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip_comm.h"


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

	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);

	while (1)
	{
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
					memcpy(tcp_client_sendbuf,tcp_client_recvbuf,data_len);
//					printf("%s\r\n",tcp_client_recvbuf);
					netbuf_delete(recvbuf);
					tcp_client_flag |= LWIP_SEND_DATA;
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
	xTaskCreate(vTaskTCP_CLIENT,    					/* ָʾ������  */
				"vTaskTCP_CLIENT",  					/* ��������    */
				configMINIMAL_STACK_SIZE * 8,       /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        						/* �������  */
				10,           						/* �������ȼ�*/
				&xHandleTaskTCP_CLIENT); 			/* ������  */	
}












