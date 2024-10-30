#include "Usartlin.h"
#include "sys.h"
#include "string.h"
#include "usart2.h"
#include "uart5.h"
#include "usart.h"


/*************************************** 第一部分 CRC校验 ********************************************/

/* CRC 高位字节值表 */ 
static const uint8_t s_crchi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC 低位字节值表 */ 
const uint8_t s_crclo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/**
 * @brief       CRC16校验
 * @param       *_pBuf：数据地址
 * @param       _usLen：数据长度
 * @note        采用crc16-modbus，多项式hex：8005
 * @retval      校验结果
 */
uint16_t crc16_calc(uint8_t *_pBuf, uint16_t _usLen)
{
    uint8_t ucCRCHi = 0xFF;                         /* 高CRC字节初始化 */
    uint8_t ucCRCLo = 0xFF;                         /* 低CRC 字节初始化 */
    uint16_t usIndex;                               /* CRC循环中的索引 */

    while (_usLen--)
    {
        usIndex = ucCRCLo ^ *_pBuf++;               /* 计算CRC */
        ucCRCLo = ucCRCHi ^ s_crchi[usIndex];
        ucCRCHi = s_crclo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);      /* 返回校验结果，高字节在高位 */
}



/*************************************** 第二部分 底层函数 ********************************************/
__IO uint8_t rev_data[REV_MAX_LEN];     /* 存放接收数据的数组（环形缓冲区） */
__IO uint8_t rev_p = 0;                       /* 地址偏移量 */
reve_data rev_contrl_data;      /* 接收变量 */
send_data type_data;          /*发送变量*/


/**
 * @brief       内存初始化
 * @param       *data：内存起始地址
 * @retval      无
 */
void debug_obj_init(send_data *data)
{
    size_t obj_size = sizeof(send_data);
    memset(data, 0, (size_t)obj_size);             /* 把指定范围内存清零 */
}


/**
 * @brief       上位机数据解析
 * @param       *data：接收的数据（地址）
 * @note        利用环形缓冲区来接收数据，再存进相应的结构体成员中
 * @retval      无
 */
void revSolv_handle(uint8_t *data){
    uint8_t temp[REV_MAX_LEN];
    uint8_t i;
    if (rev_p >= REV_MAX_LEN)          /* 超过缓冲区（数组）最大长度 */
    {
        rev_p = 0;                           /* 地址偏移量清零 */
    }
	rev_data[rev_p] = *(data);         /* 取出数据，存进数组 */
	if (*data == DATA_END)                   /* 判断是否收到帧尾 */
     {
		if (rev_data[(rev_p + REV_MAX_LEN - 9) % REV_MAX_LEN] == DATA_HEAD)                        /* 数据包长度为10个字节，判断第一个字节是否为帧头 */
         {
            for (i = 0; i < 7; i++)
            {
                temp[i] = rev_data[(rev_p + REV_MAX_LEN - 9 + i) % REV_MAX_LEN];                   /* 取出帧头、数据类别、数据域 */
            }
			
#if EN_CRC                                         /* 进行CRC校验 */
            if (crc16_calc(temp, 7) == ((rev_data[(rev_p + REV_MAX_LEN - 9 + 7) % REV_MAX_LEN] << 8) | \
                                         rev_data[(rev_p + REV_MAX_LEN - 9 + 8) % REV_MAX_LEN]))
#endif 
			{
				switch (rev_data[(rev_p + REV_MAX_LEN - 9 + 1) % REV_MAX_LEN])                           /* 判断数据类别 */
				{
					case REMOTE_val: 																						/* 接收摇杆值 */
						for(i = 0;i < 5;i++ )
						{
						 rev_contrl_data.resv_data[i]=temp[i+2];
						}
						break;

					case KEY_MODE_val:                                                                                        /* 接收按键值 */
						for(i = 0;i < 3;i++ )
						{
							 rev_contrl_data.res_key[i]=temp[i+2];
						}
						break;
				}
			  }
       }
    }
	  rev_p ++;
//	printf("数据：%d\r\n",rev_data[1]);
}



/**
 * @brief       数据发送
 * @param       *data：发送的数据（地址）
 * @param       upload_type：发送的数据类别
 * @retval      无
 */
void Send_data(send_data *data, uint8_t upload_type){
	uint8_t cur_data, i;
    uint8_t send_data1[37];                                            /* 数据上传数组 */
    send_data1[0] = DATA_HEAD;                                   /* 数据包第1个字节（数组第0个元素），固定为帧头 */
    cur_data = 2;                                                       /* 数据域从第3个字节（数组第2个元素）开始 */
    switch (upload_type)												/* 判断数据类别 */
	{
		case(TYPE_JOY_REMOTE):                          /*摇杆值*/
	    send_data1[1] = upload_type;                    /* 数据包第2个字节（数组第1个元素），固定为数据类别 */	                  
	    for(int i = 0;i < 3;i++)
	    {
		send_data1[cur_data++] = data->remote_data[i];
		}
		break;
		
        case(TYPE_KEY_MODE):                           /*按键值*/
	    send_data1[1] = upload_type;
		for(int i = 0;i < 3;i++)
	    {
		send_data1[cur_data++] = data->key[i];
		}
		break;
		
		case(TYPE_GPS):                              /*GPS数据*/
	    send_data1[1] = upload_type;
		for(int i = 0;i < 8;i++)
	    {
		send_data1[cur_data++] = data->gps[i];
		}
		break;
		case(TYPE_POSITION):                           /*位置信息*/
	    send_data1[1] = upload_type;
		for(int i = 0;i < 3;i++)
	    {
		send_data1[cur_data++] = data->position_data[i];
		}
		break;
	default :
       send_data1[1] =  0xFE;                                             /* 数据类别错误，存入错误码0xFE */
       break;
	}
	if (send_data1[1] == 0xFE)                                                /* 数据类别错误，直接跳出 */
    {
        return;
    }
	 else                                                                       /* 数据类别正确 */
    {
        uint16_t crc_res = crc16_calc(&(send_data1[0]), cur_data);            /* 进行CRC校验 */
        send_data1[cur_data++] = (crc_res >> 8) & 0xFF;                       /* 存入校验结果高8位 */
        send_data1[cur_data++] = (crc_res) & 0xFF;                            /* 存入校验结果低8位 */
        send_data1[cur_data++] = DATA_END;                                    /* 存入帧尾 */
		    uart5_send(send_data1, cur_data);/* 发送数据到上位机 */	
			
//			   printf("gps已发送\r\n");
//			  usart1_send(send_data1, cur_data);
//		usart2_send(send_data1, cur_data);/* 发送数据到上位机 */	
    }
}




/*************************************** 第三部分 应用层函数 ********************************************/
/**
 * @brief       初始化调试
 * @param       无
 * @retval      无
 */
void debug_init(void)
{
    debug_obj_init(&type_data);            /* 初始化所需内存 */
}

/**
 * @brief       摇杆数据上传
 * @param       remoteVal ：遥控数据
 * @retval      无
 */
void send_remoteVal(float remoteX_val,float remoteY_val,float remoteK_val )
{
    type_data.remote_data[0] = (int16_t)(remoteX_val);               /* 传入摇杆y值 */
	type_data.remote_data[1] = (int16_t)(remoteY_val);               /* 传入摇杆x值 */
	type_data.remote_data[2] = (int16_t)(remoteK_val);               /* 传入按键值 */
	Send_data(&type_data, TYPE_JOY_REMOTE);    	                   /* 发送遥控数据 */

}

/**
 * @brief       按键数据上传
 * @param     
 * @retval      无
 */
void send_KeyVa2(float Val1,float Val2,float Val3 )
{
    type_data.key[0] = (int16_t)(Val1);               /* 传入摇杆y值 */
	type_data.key[1] = (int16_t)(Val2);               /* 传入摇杆x值 */
	type_data.key[2] = (int16_t)(Val3);               /* 传入按键值 */
	Send_data(&type_data, TYPE_KEY_MODE);    	                   /* 发送遥控数据 */

}
/**
 * @brief       位置数据上传
 * @param     
 * @retval      无
 */
void send_PositionVa3(float Val1,float Val2,float Val3)
{

	type_data.position_data[0] = (int16_t)(Val1);               /* 目标方位角值 */
	type_data.position_data[1] = (int16_t)(Val2);               /* 当前方位角值 */
	type_data.position_data[2] = (int16_t)(Val3);               /* 距离值 */
	Send_data(&type_data, TYPE_POSITION);    	                /* 发送位置数据 */ 

}

/**
 * @brief       gps数据上传
 * @param     
 * @retval      无
 */
void send_PositionVa4(uint8_t bytes[8])
{
   for(int i =0;i<8;++i)
 {
	 type_data.gps[i] = bytes[i];
 }
 
	Send_data(&type_data, TYPE_GPS);    	                    /* 发送位置数据 */ 
// printf("111\r\n");
}




