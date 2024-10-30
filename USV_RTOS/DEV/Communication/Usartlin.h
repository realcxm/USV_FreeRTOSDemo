#ifndef __USARTLINK__H
#define __USARTLINK__H
#include "sys.h"
#include "stdio.h"
//帧头、帧尾
#define DATA_HEAD   0xFF
#define DATA_END    0xAA
#define EN_CRC       1    /* CRC校验，0：关闭，1：开启 */

/*********************************** 上位机 ——> 开发板 *******************************************/

/*数据类别*/
typedef enum
{
  REMOTE_val      =  0x01   ,/* 摇杆值 */
  KEY_MODE_val    =  0x02   ,/* 按键模式转换 */	
	
}rev_type;

/* 参数存放结构体（上位机——>开发板） */
typedef struct
{
	
int16_t resv_data[5];           /* 摇杆数据 */
int16_t res_key[3];             /*按键值*/
			
}reve_data;

extern reve_data rev_contrl_data;     /* 接收变量 */



/************************************开发板——>上位机*********************************************/
/*数据类别*/
typedef enum
{

 TYPE_JOY_REMOTE   =  0x01   ,/* 摇杆值 */
 TYPE_KEY_MODE     =  0x02   ,/* 按键模式转换 */	
 TYPE_GPS          =  0x03   ,/* GPS数据*/
 TYPE_POSITION     =  0x04   ,/* 目标方位角、当前方位角、距离*/
}upload_type;

/* 参数存放结构体（开发板——>上位机） */
typedef struct
{
int16_t remote_data[3];         /* 摇杆数据 */
int16_t key[3];                 /*按键值*/
int32_t gps[8];                 /* GPS数据 */
int16_t position_data[3];		/*目标方位角、当前方位角、距离*/	
}send_data;

extern send_data type_data;     /* 发送变量 */

#define REV_MAX_LEN   17       /* 接收数据最大长度 */

void Send_data(send_data *data, uint8_t upload_type);
void send_remoteVal(float remoteX_val,float remoteY_val,float remoteK_val );
void revSolv_handle(uint8_t *data);





// 定义数据包结构体
typedef struct {
    uint8_t header1;  // 帧头1
    uint8_t header2;  // 帧头2
	uint32_t length;  // 长度
    uint8_t function; // 功能字
    uint8_t data[3];  // 数据
    uint16_t crc;     // CRC校验
} DataPacket;


//void packData(DataPacket *packet, uint8_t header1, uint8_t header2, uint8_t function, uint32_t length, uint8_t *data);
//void printDataPacket(const DataPacket *packet);

void packData(DataPacket *packet, uint8_t *data) ;
uint16_t calcCRC(const DataPacket *packet);
void packDataPacket(const DataPacket* packet, uint8_t* buffer);
void send_PositionVa3(float Val1,float Val2,float Val3);//位置信息
void send_PositionVa4(uint8_t bytes[8]);//gps数据

#endif





