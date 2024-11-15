#include "nmea_gps.h" 
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ATK-NEO-6M GPSģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/10/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved							  
////////////////////////////////////////////////////////////////////////////////// 	   


//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx)
{	 		    
	uint8_t *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n����
//����ֵ:m^n�η�.
uint32_t NMEA_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(uint8_t *buf,uint8_t*dx)
{
	uint8_t *p=buf;
	uint32_t ires=0,fres=0;
	uint8_t ilen=0,flen=0,i;
	uint8_t mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*'))break;//����������
		if(*p=='.'){mask|=0X01;p++;}//����С������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 
//����GPGSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p,*p1,dx;
	uint8_t len,i,j,slx=0;
	uint8_t posx;   	 
	p=buf;
	p1=(uint8_t*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';								//�õ�GPGSV������
	posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ���������
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(uint8_t*)strstr((const char *)p,"$GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//�õ����Ǳ��
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//�õ��������� 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//�õ����������
			else break;
			slx++;	   
		}   
 		p=p1+1;//�л�����һ��GPGSV��Ϣ
	}   
}
//����GPGGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;
  uint32_t temp;	   
	float rs;	
	p1=(uint8_t*)strstr((const char *)buf,"$GNGGA");
	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,2);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,7)+(rs*NMEA_Pow(10,7-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,3);								//��γ���Ǳ�γ 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,4);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,7)+(rs*NMEA_Pow(10,7-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p1,5);								//������������
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//�õ����ڶ�λ��������
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//�õ����θ߶�
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx); 
  posx=NMEA_Comma_Pos(p1,11);
  if(posx!=0XFF)gpsx->sep=NMEA_Str2num(p1+posx,&dx);	
}
//����GPGSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx; 
	uint8_t i;   
	p1=(uint8_t*)strstr((const char *)buf,"$GNGSA");
	posx=NMEA_Comma_Pos(p1,2);								//�õ���λ����
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
	for(i=0;i<12;i++)										//�õ���λ���Ǳ��
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);								//�õ�PDOPλ�þ�������
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);								//�õ�HDOPλ�þ�������
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);								//�õ�VDOPλ�þ�������
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}
//����GPRMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
		uint8_t *p1,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
//	float rs;  
	p1=(uint8_t*)strstr((const char *)buf,"$GNRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
		posx=NMEA_Comma_Pos(p1,2);
	if(posx!=0XFF)gpsx->pvi=*(p1+posx);         //��λ��Ч�Ա�־
	posx=NMEA_Comma_Pos(p1,12);
	if(posx!=0XFF)gpsx->pmf=*(p1+posx);       //��λģʽ��־
	
	
//	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
//		gpsx->utc.hour=temp/10000;
//		gpsx->utc.min=(temp/100)%100;
//		gpsx->utc.sec=temp%100;	 	 
//	}	
//	posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
//		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
//		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,7)+(rs*NMEA_Pow(10,7-dx))/60;//ת��Ϊ�� 
//	}
//	posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ 
//	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
// 	posx=NMEA_Comma_Pos(p1,5);								//�õ�����
//	if(posx!=0XFF)
//	{												  
//		temp=NMEA_Str2num(p1+posx,&dx);		 	 
//		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
//		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
//		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,7)+(rs*NMEA_Pow(10,7-dx))/60;//ת��Ϊ�� 
//	}
//	posx=NMEA_Comma_Pos(p1,6);								//������������
//	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
}
//����GPVTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;    
	p1=(uint8_t*)strstr((const char *)buf,"$GNVTG");							 
	posx=NMEA_Comma_Pos(p1,7);								//�õ���������
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//ȷ������1000��
	}
}  
//��ȡNMEA-0183��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSV����
	NMEA_GNGGA_Analysis(gpsx,buf);	//GPGGA���� 	
	NMEA_GNGSA_Analysis(gpsx,buf);	//GPGSA����
	NMEA_GNRMC_Analysis(gpsx,buf);	//GPRMC����
	NMEA_GNVTG_Analysis(gpsx,buf);	//GPVTG����
}



/*************����ϵת��***********/

#define pi   3.1415926535897932384626
#define ee   0.00669342162296594323//����������˹������ϵ��
#define aa   6378245.0
#define x_pi (3.14159265358979324 * 3000.0 / 180.0)

/*
  * ��γ��ƫ��ת��
 */
double transform_lat(double lng, double lat)
{

    double ret;
    ret = -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 *  sqrt( fabs(lng));
    ret += (20.0 *  sin(6.0 * lng * pi) + 20.0 *  sin(2.0 * lng *   pi)) * 2.0 / 3.0;
    ret += (20.0 *  sin(lat *   pi) + 40.0 *  sin(lat / 3.0 *   pi)) * 2.0 / 3.0;
    ret += (160.0 *  sin(lat / 12.0 *   pi) + 320 *  sin(lat *   pi / 30.0)) * 2.0 / 3.0;

    return ret;    
}


double transform_lng(double lng, double lat)
{
    double ret;
    ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 *  sqrt( fabs(lng));
    ret += (20.0 *  sin(6.0 * lng *   pi) + 20.0 *  sin(2.0 * lng *   pi)) * 2.0 / 3.0;
    ret += (20.0 *  sin(lng *   pi) + 40.0 *  sin(lng / 3.0 *   pi)) * 2.0 / 3.0;
    ret += (150.0 *  sin(lng / 12.0 *   pi) + 300.0 *  sin(lng / 30.0 *   pi)) * 2.0 / 3.0;
    return ret;

}


/*
"""
  * WGS84תGCj02 ƥ��ߵ¡���Ѷ��ͼ
  * @param lng
  *  @param lat
  *  @returns {*[]}
"""
*/

void wgs84_to_gcj02(double lng, double lat, double *gcj_lng, double *gcj_lat) {
    double dLat = transform_lat(lng - 105.0, lat - 35.0);
    double dLng = transform_lng(lng - 105.0, lat - 35.0);
    double radLat = lat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((aa * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLng = (dLng * 180.0) / (aa / sqrtMagic * cos(radLat) * pi);
    *gcj_lat = lat + dLat;
    *gcj_lng = lng + dLng;
}


/*
"""
  * WGS84תBD09LL ƥ��ٶȵ�ͼ
  * @param lng
  *  @param lat
  *  @returns {*[]}
"""
*/

// GCJ-02 to BD09LL
void gcj02_to_bd09ll(double gcj_lng, double gcj_lat, double *bd_lng, double *bd_lat) {
    double z = sqrt(gcj_lng * gcj_lng + gcj_lat * gcj_lat) + 0.00002 * sin(gcj_lat * pi);
    double theta = atan2(gcj_lat, gcj_lng) + 0.000003 * cos(gcj_lng * pi);
    *bd_lng = z * cos(theta) + 0.0065;
    *bd_lat = z * sin(theta) + 0.006;
}

// WGS-84 to BD09LL directly
void wgs84_to_bd09ll(double wgs_lng, double wgs_lat, double *bd_lng, double *bd_lat) {
    double gcj_lng, gcj_lat;
    wgs84_to_gcj02(wgs_lng, wgs_lat, &gcj_lng, &gcj_lat);
    gcj02_to_bd09ll(gcj_lng, gcj_lat, bd_lng, bd_lat);
}




