#include "sys.h"
#include "remotecontrol.h"
#include "T200_motors.h"
#include "Usartlin.h"
#include "usart.h"
#include "delay.h"


extern reve_data rev_contrl_data;
u16 KEY_VAl, remoteVal_LX, remoteVal_LY, remoteVal_RX, remoteVal_RY;

void set_speed(uint16_t up_m1,uint16_t tr_m1);

void remotecontroller(void){

	static int speed_state = 0;
	 
	uint8_t first =1;
	// 档1  前进 后退
	uint16_t mode1_up_m1 ;
	uint16_t mode1_tl_m1 ; 

	// 档2  前进 后退
	uint16_t mode2_up_m1 ;
	uint16_t mode2_tl_m1 ; 

	// 档3  前进 后退
	uint16_t mode3_up_m1 ;
	uint16_t mode3_tl_m1 ; 
 
 //获取摇杆值、按键值
   remoteVal_LX = rev_contrl_data.resv_data[1];//左x
   remoteVal_LY = rev_contrl_data.resv_data[0];//左y	
   remoteVal_RX = rev_contrl_data.resv_data[2];//右x
   remoteVal_RY = rev_contrl_data.resv_data[3];//右y	
   KEY_VAl      = rev_contrl_data.resv_data[4];
//		printf("LX:%d\r\n",remoteVal_LX)	;
//		printf("LY:%d\r\n",remoteVal_LY)	;
//		printf("RX:%d\r\n",remoteVal_RX)	;
//    	printf("RY:%d\r\n",remoteVal_RY)	;
//		printf("K_VAL:%d\r\n",KEY_VAl)	    ;
 
 //挡位控制
		if( KEY_VAl == 1 &&  first ){
		  speed_state++;
			if( speed_state > SPEED_HIGH )
				speed_state = SPEED_LOW;
				first = 0;
		}
		 else if( KEY_VAl ==0){
			first=1;	 
		}
		// 档1  前进 后退
		 mode1_up_m1 = LineTrust(remoteVal_LY,0,1600,254,1400);
		 mode1_tl_m1 = LineTrust(remoteVal_LX,0,2200,254,800);
		// 档2  前进 后退
		 mode2_up_m1 = LineTrust(remoteVal_LY,0,1800,254,1200);
		 mode2_tl_m1 = LineTrust(remoteVal_LX,0,2200,254,800);
		// 档3  前进 后退
		 mode3_up_m1 = LineTrust(remoteVal_LY,0,2200,254,800);
		 mode3_tl_m1 = LineTrust(remoteVal_LX,0,2200,254,800);
//		printf("speed_state:%d\r\n",speed_state);
		switch (speed_state){
			case 1:
				 set_speed(mode1_up_m1,mode1_tl_m1);

				break;
			case 2:
				
				set_speed(mode2_up_m1,mode2_tl_m1);

				break;
			case 3:
				
				set_speed(mode3_up_m1,mode3_tl_m1);
			
				break;
		     }
			delay_ms(100);
 }


void set_speed(uint16_t up_m1,uint16_t tr_m1)
{
	if(remoteVal_LX!=254 && remoteVal_LX!=0){
	
            u16 tl1 ,tr2;
		//左转
		 if( remoteVal_LX >= 0x7F  ){
			  tr2  =  up_m1;
			  tl1  = 1500 + (tr2-1500)*(2200 - tr_m1)/700;
		   motorsSetRatio(MOTOR_M3,tl1);
		   motorsSetRatio(MOTOR_M4,tr2);
//		   printf("tl1:%d\r\n",tl1);
//		   printf("tr2:%d\r\n",tr2);			
		}
		 //右转
		 if( remoteVal_LX < 0x7F  ){
			
			    tl1 =  up_m1;
			    tr2 = 1500 + (tl1-1500)*(tr_m1-800)/700;
		   motorsSetRatio(MOTOR_M3,tl1);
		   motorsSetRatio(MOTOR_M4,tr2);
//			printf("tl1:%d\r\n",tl1);
//			printf("tr2:%d\r\n",tr2);			
		}
	}
		//停止
		else if( remoteVal_LX==0x7F && remoteVal_LY==0x7F ){
			
			 motorsSetRatio(MOTOR_M3,1500);
			 motorsSetRatio(MOTOR_M4,1500);
//			printf("停止\r\n");
		  }
	}





























