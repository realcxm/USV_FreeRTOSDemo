#ifndef __USARTLINK__H
#define __USARTLINK__H
#include "sys.h"
#include "stdio.h"
//֡ͷ��֡β
#define DATA_HEAD   0xFF
#define DATA_END    0xAA
#define EN_CRC       1    /* CRCУ�飬0���رգ�1������ */

/*********************************** ��λ�� ����> ������ *******************************************/

/*�������*/
typedef enum
{
  REMOTE_val      =  0x01   ,/* ҡ��ֵ */
  KEY_MODE_val    =  0x02   ,/* ����ģʽת�� */	
	
}rev_type;

/* ������Žṹ�壨��λ������>�����壩 */
typedef struct
{
	
int16_t resv_data[5];           /* ҡ������ */
int16_t res_key[3];             /*����ֵ*/
			
}reve_data;

extern reve_data rev_contrl_data;     /* ���ձ��� */



/************************************�����塪��>��λ��*********************************************/
/*�������*/
typedef enum
{

 TYPE_JOY_REMOTE   =  0x01   ,/* ҡ��ֵ */
 TYPE_KEY_MODE     =  0x02   ,/* ����ģʽת�� */	
 TYPE_GPS          =  0x03   ,/* GPS����*/
 TYPE_POSITION     =  0x04   ,/* Ŀ�귽λ�ǡ���ǰ��λ�ǡ�����*/
}upload_type;

/* ������Žṹ�壨�����塪��>��λ���� */
typedef struct
{
int16_t remote_data[3];         /* ҡ������ */
int16_t key[3];                 /*����ֵ*/
int32_t gps[8];                 /* GPS���� */
int16_t position_data[3];		/*Ŀ�귽λ�ǡ���ǰ��λ�ǡ�����*/	
}send_data;

extern send_data type_data;     /* ���ͱ��� */

#define REV_MAX_LEN   17       /* ����������󳤶� */

void Send_data(send_data *data, uint8_t upload_type);
void send_remoteVal(float remoteX_val,float remoteY_val,float remoteK_val );
void revSolv_handle(uint8_t *data);





// �������ݰ��ṹ��
typedef struct {
    uint8_t header1;  // ֡ͷ1
    uint8_t header2;  // ֡ͷ2
	uint32_t length;  // ����
    uint8_t function; // ������
    uint8_t data[3];  // ����
    uint16_t crc;     // CRCУ��
} DataPacket;


//void packData(DataPacket *packet, uint8_t header1, uint8_t header2, uint8_t function, uint32_t length, uint8_t *data);
//void printDataPacket(const DataPacket *packet);

void packData(DataPacket *packet, uint8_t *data) ;
uint16_t calcCRC(const DataPacket *packet);
void packDataPacket(const DataPacket* packet, uint8_t* buffer);
void send_PositionVa3(float Val1,float Val2,float Val3);//λ����Ϣ
void send_PositionVa4(uint8_t bytes[8]);//gps����

#endif





