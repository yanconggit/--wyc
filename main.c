#include <stc15.h>
#define Xtal 11059200  //������ֵ

void InterruptTimer1();

sbit BUZZ = P1^3;  //��������������
sbit KEY12 = P1^1;   //��������
sbit KEY4 = P1^0;   //��������
sbit KEY3= P3^7;   //��������
sbit KEY2 = P3^6;   //��������
sbit KEY1 = P3^5;   //��������
sbit KEY0 = P3^4;
sbit KEY = P3^3;
sbit KEY5 = P3^2;
bit enable = 0;



unsigned char T0RH = 0xFF;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0x00;  //T0����ֵ�ĵ��ֽ�

unsigned int code NoteFrequ[] = {  //����1-7����1-7�͸���1-7��ӦƵ���б�
    523,  587,  659,  698,  784,  880,  988,  //����1-7
    1047, 1175, 1319, 1397, 1568, 1760, 1976,  //����1-7
	  262, 294, 330, 349, 392, 440, 494,300//����1-7  ���һ����������
};

unsigned int code NoteReload[] = { //����1-7�͸���1-7��Ӧ�Ķ�ʱ������ֵ
		//65536 - (Xtal/12) / (300*2), //��������
    65536 - (Xtal/12) / (523*2),  //����1
    65536 - (Xtal/12) / (587*2),  //2
    65536 - (Xtal/12) / (659*2),  //3
    65536 - (Xtal/12) / (698*2),  //4
    65536 - (Xtal/12) / (784*2),  //5
    65536 - (Xtal/12) / (880*2),  //6
    65536 - (Xtal/12) / (988*2),  //7
    65536 - (Xtal/12) / (1047*2), //����1
    65536 - (Xtal/12) / (1175*2), //2
    65536 - (Xtal/12) / (1319*2), //3
    65536 - (Xtal/12) / (1397*2), //4
    65536 - (Xtal/12) / (1568*2), //5
    65536 - (Xtal/12) / (1760*2), //6
    65536 - (Xtal/12) / (1976*2), //7
	  65536 - (Xtal/12) / (262*2), //����1
    65536 - (Xtal/12) / (294*2), //2
    65536 - (Xtal/12) / (330*2), //3
    65536 - (Xtal/12) / (349*2), //4
    65536 - (Xtal/12) / (392*2), //5
    65536 - (Xtal/12) / (440*2), //6
    65536 - (Xtal/12) / (494*2), //7
		65536 - (Xtal/12) / (300*2), //��������
};

unsigned char code 	ZhiduanqingchangNote[] = { //ֽ���鳤
	      //8, 9, 10, 9, 8, 6, 22, 11, 10, 9, 22, 7, 8, 9, 7, 7, 5, 22, 7, 9, 8, 8, 3, 4, 2, 3, 1, 1, 5, 2,  //ǰ��
        19, 3, 3, 3, 2, 3, 1, 2, 2, 2, 1, 2, 5, 1, 1, 1, 20, 1, 20, 19,     2, 2, 2, 3, 20, 19, 4, 4, 4, 3, 4, 1, 2, 2, 2, 1, 2, 5,
       	1, 1, 1, 20, 1, 20, 3, 2, 2, 1, 1, 22, 22, 22, 1, 1, 2,             3, 3, 3, 22, 3, 2, 1, 21, 6, 5, 22, 5, 6, 7, 8, 3, 3, 22, 8, 7, 8,
      	7, 3, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,5, 3, 2, 1, 20, 1,               3, 2, 2, 1, 2, 5, 1, 2, 5, 22, 22, 22, 1, 1, 2, 
        3, 3, 2, 3, 22, 3, 2, 1, 5, 5, 3, 5, 22, 5, 6, 7,                   8, 8, 8, 8, 8, 7, 6, 7, 6, 3, 5, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4, 
        5, 3, 2, 1, 6, 1, 3, 2, 2, 1, 2, 1, 20, 1, 22,                      22, 20, 1, 3, 2, 2, 22, 2, 1, 2, 1, 1, 22, 20, 1, 1, 0xff};	
unsigned char code ZhiduanqingchangBeat[] = {
	      //2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 8, 12, 4, 5, 4, 8, 8, 4,   //ǰ��
        8, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 4, 2,       2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 4, 5, 2, 2, 2, 2, 2, 5, 
        2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 8, 4, 4, 2, 2, 2, 2,                4, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 
        4, 2, 5, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4, 2, 5, 1, 1,             2, 2, 2, 2, 2, 4, 2, 16, 16,4, 4, 2, 2, 2, 2, 
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,                   2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 2, 2, 2, 2, 
        2, 4, 2, 5, 1, 1, 2, 2, 2, 2, 2, 4, 1, 12, 4,                     2, 1, 1, 2, 2, 8, 2, 1, 1, 2, 2, 8, 2, 2, 12, 16};
unsigned char code 	piaoyangguohaiNote[] = {
	2,3,3,3,   3,3,2,1,   5,3,3,   3,2,3,   2,2,3,   2,1,6,6,                 1,1,1,1 ,  2,1,3,3,   1,1,1,1,   2,1,1,1,   2,2,   2,2,3 ,   2,
	2,3,3,3,   3,3,5,   5,5,3,   2,3,2,3,   2,2,3,   2,1,6,6,                 1,1,1,1 ,  1,2,3,3,   1,1,1,6,   2,1,   2,2,   2,2,3 ,   2, 22,6,
	6,5,5,   5,5,6,   6,8,6,   5,6,5,1,   2,2,3,   2,3,2,20,   20,						1,1,1,6,   2,1,3,3,   1,1,1,6,   2,1,  6,6,6,   6,3,6,   5,   5,   
	5,3,2,3,   3,5,   3,   2,2,3,   2,20,2,   2,  														1,1,1,1 ,  1,20,1,     1,1,1,1 ,   1,1,1,20,		6,5,5,5,   5,5,5,3,   6,5,5,22,5,
	6,5,6,5,   6,22,5,  5,3,5,3,   5,22,5,5,   6,6,6,   5,6,5,3,3,22,3,				2,1,2,1,  2,6,22,1,  1,21,1,21,  1,6,  8,8,7,8,7,6,6,0xff
};

unsigned char code piaoyangguohaiBeat[] ={
	1,1,1,1,   1,1,1,1,   1,2,1,   2,1,1,	  1,2,1,   2,1,1,8,									1,1,1,1,   1,2,1,1,   1,1,1,1,   1,1,1,1,   3,1,   2,1,1, 	 8,
	1,1,1,1,	 1,2,1,     1,2,1,   1,1,1,1,	2,1,1,   2,1,1,8,									1,1,1,1,   1,2,1,1,   1,1,1,1,   1,3,   2,1,  2,1,1,  4,   3,1,
	1,2,1,   2,1,1,   1,2,1 ,  1,1,1,1,  2,1,1,   1,1,1,1, 8,									1,1,1,1,   1,2,1,1,  	1,1,1,1,   1,3,   2,1,1,   2,1,1,   8,  16,
	1,1,1,1,	 1,3,   8,   1,2,1,   2,1,1,  8,   															1,1,1,1,   1,2.1,   1,1,1,1,   1,1,1,1,   1,1,1,1,   1,1,1,1,   3,1,   2,1,1,
	1,1,1,1,   2,1,1,   1,1,1,1,   1,1,1,1,   2,1,1,   1,1,1,1,   4,   3,1,		1,1,1,1,   1,1,1,1,   1,1,1,1,   1,3,    2,2,   1,1,1,1,   8, 1
};
unsigned char code XiaoXingXingNote[] = {   //С����
        1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1,  
        5, 5, 4, 4,    3, 3, 2,    5, 5, 4, 4,    3, 3, 2 ,
        1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1, 0xff    };

unsigned char code XiaoXingXingBeat[] = {
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,};
unsigned char code TwoTigerNote[] = {    //��ֻ�ϻ�������
        1,   2,   3, 1,    1,   2,   3, 1,   3, 4, 5,   3, 4, 5,
        5,6, 5,4, 3, 1,    5,6, 5,4, 3, 1,   1, 5, 1,   1, 5, 1,0xff};

unsigned char code TwoTigerBeat[] = {    //��ֻ�ϻ����ı�4��ʾһ�ģ�1����1/4�ģ�8����2��
        4,   4,   4, 4,    4,   4,   4, 4,   4, 4, 8,   4, 4, 8,
        3,1, 3,1, 4, 4,    3,1, 3,1, 4, 4,   4, 4, 8,   4, 4, 8,
    };
unsigned char code HappybirthdayNote[] = {  //���տ��ָ�
        5, 5, 6, 5, 8, 7, 22, 5, 5, 6, 5, 9, 8, 5, 5, 12, 10, 8, 7, 6, 11, 11,  10, 8, 9, 8,0xff };
unsigned char code HappybirthdayBeat[] = {
        2, 2, 4, 4, 4, 4, 4, 2, 2,  4, 4, 4, 8, 2, 2, 4, 4, 4, 4, 8, 2, 2, 4, 4, 4, 8};	
unsigned char *SongNote, *SongBeat;//ָ��	
unsigned char sizeofNote;//���ĵĶ���
bit tmrflag = 0;  //��ʱ���ж���ɱ�־
bit breakflage = 0;
unsigned char keybuf = 0xFF; 
unsigned char keybuf0 = 0xFF; 
unsigned char keybuf1 = 0xFF; 
unsigned char keybuf2 = 0xFF; 
unsigned char keybuf3 = 0xFF; 
unsigned char keybuf4 = 0xFF;
unsigned char keybuf5 = 0xFF;
unsigned char keybuf12 = 0xFF;
bit playflage = 1;
void Play(void);
void main()
{ 
	EA = 1;       //ʹ��ȫ���ж�
	TMOD = 0x11;  //����T0��T1������ģʽ1
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
//	AUXR &= 0xFB;		//��ʱ��ʱ��12Tģʽ
//	T2L = 0x00;		//���ö�ʱ��ֵ
//	T2H = 0x94;		//���ö�ʱ��ֵ
//	IE2 |= 0x04;
//	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
	IT0 =1;
	EX1 = 1;
	


	TH0 = T0RH;   
	TL0 = T0RL;
	ET0 = 1;      //ʹ��T0�ж�
	TR0 = 1;

	while (1)
	{
		InterruptTimer1();
	}
}


void Play()//���ຯ�� 
{
    unsigned char beat;   //��ǰ��������
    unsigned char note;   //��ǰ���Ķ�Ӧ������
    unsigned int time = 0;      //��ǰ���ļ�ʱ
    unsigned int beatTime = 0;  //��ǰ������ʱ��
    unsigned int soundTime = 0; //��ǰ�����跢��ʱ��

    for (beat=0; beat<sizeofNote; )  //�ý���������Ϊѭ������
    {
				if(breakflage == 1)
						break;
        while (!tmrflag);  //ÿ�ζ�ʱ���ж���ɺ󣬼�Ⲣ�������
        tmrflag = 0;
        if (time == 0)  //��ǰ���Ĳ���������һ���½���
        {
						if(breakflage == 1)
							break;
            note = SongNote[beat] - 1;
					  if(note == 0xfe)
						{
							break;
						}
            T0RH = NoteReload[note] >> 8;
            T0RL = NoteReload[note];
            beatTime = (SongBeat[beat] * NoteFrequ[note]) >> 2;//���������ʱ�䣬����2λ�൱�ڳ�4����λ����������Լӿ�ִ���ٶ�
            soundTime = beatTime - (beatTime >> 2);//���㷢��ʱ�䣬
            enable = 1;  //ָʾ��������ʼ����
						if (note == 21)//���׵�0 �ȴ�
							enable =0;
            time++;
        }
        else  //��ǰ����δ��������ǰ����
        {
					if(breakflage == 1)
						break;
            if (time >= beatTime)  //��ǰ����ʱ�䵽�������ʱ��ʱ���㣬
            {                      //������������������׼�������½���
                time = 0;
                beat++;
            }
            else  //��ǰ����ʱ��δ�ﵽ��ʱ��ʱ��
            {
                time++;   //�ۼ�ʱ�����
                if (time == soundTime)  //���﷢��ʱ���ָʾ�رշ�������
                {                       //����0.25*��ʱ��ľ��������
                    enable = 0;         //����������������������
                }
            }
        }
    }
		playflage = 1;
}

void exint1() interrupt 2
{
	breakflage = 1;
}

/*���Ʒ��������� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //���¼�������ֵ
    TL0 = T0RL;
		tmrflag = 1;
    if (enable)   //ʹ��ʱ��ת���������Ƶ�ƽ
        BUZZ = ~BUZZ;
    else          //δʹ��ʱ�رշ�����
        BUZZ = 1;
}

/*����ɨ��*/
void InterruptTimer1() //interrupt 12 using 1 //�������ֲο�֮ǰ������ ���мƴι��ܵ����
{
	static char scale = 1;
	bit KeySta = 1;  //��ǰ����״̬
	bit KeySta0 = 1;  //��ǰ����״̬
	bit KeySta1 = 1;  //��ǰ����״̬
	bit KeySta2 = 1;  //��ǰ����״̬
	bit KeySta3 = 1;  //��ǰ����״̬
	bit KeySta4 = 1;  
	bit KeySta5 = 1; 
	bit KeySta12 = 1; 
	
//	T2L = 0x00;		//���ö�ʱ��ֵ
//	T2H = 0x41;		//���ö�ʱ��ֵ
	
	keybuf = (keybuf<<1) | KEY; 
	if (keybuf == 0xff)
	{ 
		KeySta = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[0] >> 8;T0RL = NoteReload[0];break;
			case 2:T0RH = NoteReload[7] >> 8;T0RL = NoteReload[7];break;
			case 3:T0RH = NoteReload[14] >> 8;T0RL = NoteReload[14];break;
		}
	}
	else if (keybuf == 0x00)
			KeySta = 1;
	else{} 
		
	keybuf0 = (keybuf0<<1) | KEY0; 
	if (keybuf0 == 0XFF)
	{ 
		KeySta0 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[1] >> 8;T0RL = NoteReload[1];break;
			case 2:T0RH = NoteReload[8] >> 8;T0RL = NoteReload[8];break;
			case 3:T0RH = NoteReload[15] >> 8;T0RL = NoteReload[15];break;
		}
	}
	else if (keybuf0 == 0x00)
			KeySta0 = 1;
	else{} 
		
		
	keybuf1 = (keybuf1<<1) | KEY1; 
	if (keybuf1 == 0XFF)
	{ 
		KeySta1 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[2] >> 8;T0RL = NoteReload[2];break;
			case 2:T0RH = NoteReload[9] >> 8;T0RL = NoteReload[9];break;
			case 3:T0RH = NoteReload[16] >> 8;T0RL = NoteReload[16];break;
		}
	}
	else if (keybuf1 == 0x00)
			KeySta1 = 1;
	else{} 
		
	keybuf2 = (keybuf2<<1) | KEY2; 
	if (keybuf2 == 0XFF)
	{ 
		KeySta2 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[3] >> 8;T0RL = NoteReload[3];break;
			case 2:T0RH = NoteReload[10] >> 8;T0RL = NoteReload[10];break;
			case 3:T0RH = NoteReload[17] >> 8;T0RL = NoteReload[17];break;
		}
	}
	else if (keybuf2 == 0x00)
			KeySta2 = 1;
	else{} 
		
	keybuf3 = (keybuf3<<1) | KEY3; 
	if (keybuf3 == 0XFF)
	{ 
		KeySta3 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[4] >> 8;T0RL = NoteReload[4];break;
			case 2:T0RH = NoteReload[11] >> 8;T0RL = NoteReload[11];break;
			case 3:T0RH = NoteReload[18] >> 8;T0RL = NoteReload[18];break;
		}
	}
	else if (keybuf3 == 0x00)
			KeySta3 = 1;
	else{} 
		
		
		
	keybuf4 = (keybuf4<<1) | KEY4; 
	if (keybuf4 == 0XFF)
	{ 
		KeySta4 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[5] >> 8;T0RL = NoteReload[5];break;
			case 2:T0RH = NoteReload[12] >> 8;T0RL = NoteReload[12];break;
			case 3:T0RH = NoteReload[19] >> 8;T0RL = NoteReload[19];break;
		}
	}
	else if (keybuf4 == 0x00)
			KeySta4 = 1;
	else{} 
		
//	keybuf5 = (keybuf5<<1) | KEY5; 
//	if (keybuf5 == 0XFF)
//	{ 
//		scale+=7;
//		if(scale>=21)
//			scale=0;
//	}
//	else if (keybuf5 == 0x00)
//			KeySta5 = 1;
//	else{}
		
		
 keybuf12= (keybuf12<<1) | KEY12; 
	if (keybuf12 == 0XFF)
	{ 
		KeySta12 = 0;
		enable = 1;
		switch(scale)
		{
			case 1:T0RH = NoteReload[6] >> 8;T0RL = NoteReload[6];break;
			case 2:T0RH = NoteReload[13] >> 8;T0RL = NoteReload[13];break;
			case 3:T0RH = NoteReload[20] >> 8;T0RL = NoteReload[20];break;
		}
	}
	else if (keybuf12 == 0x00)
			KeySta12 = 1;
	else{} 
	if(KeySta&&KeySta0&&KeySta1&&KeySta2&&KeySta3&&KeySta4&&KeySta12)
	{
		enable = 0;
	}
	if(KeySta2==0&&KeySta0==0&&KeySta1==0)
	{	
		breakflage = 0;
		SongNote = ZhiduanqingchangNote;
		SongBeat =ZhiduanqingchangBeat;
		sizeofNote = sizeof(ZhiduanqingchangNote);
		Play();
	}
	if(KeySta2==0&&KeySta3==0&&KeySta4==0)
	{	
		breakflage = 0;
		SongNote = piaoyangguohaiNote;
		SongBeat =piaoyangguohaiBeat;
		sizeofNote = sizeof(piaoyangguohaiNote);
		Play();
	}
	if(KeySta2==0&&KeySta3==0&&KeySta1==0)
	{	
		breakflage = 0;
		SongNote = XiaoXingXingNote;
		SongBeat =XiaoXingXingBeat;
		sizeofNote = sizeof(XiaoXingXingNote);
		Play();
	}
	if(KeySta12==0&&KeySta3==0&&KeySta4==0)
	{	
		breakflage = 0;
		SongNote = TwoTigerNote;
		SongBeat =TwoTigerBeat;
		sizeofNote = sizeof(TwoTigerNote);
		Play();
	}
	if(KeySta1==0&&KeySta3==0&&KeySta12==0)
	{	
		breakflage = 0;
		SongNote = HappybirthdayNote;
		SongBeat = HappybirthdayBeat;
		sizeofNote = sizeof(HappybirthdayNote);
		Play();
	}
	if(KeySta12==0&&KeySta4==0)
	{	
		scale++;
		if(scale == 4)
			scale = 1;
	}
	
}


















