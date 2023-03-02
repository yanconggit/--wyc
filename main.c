#include <stc15.h>
#define Xtal 11059200  //赋晶振值

void InterruptTimer1();

sbit BUZZ = P1^3;  //蜂鸣器控制引脚
sbit KEY12 = P1^1;   //按键定义
sbit KEY4 = P1^0;   //按键定义
sbit KEY3= P3^7;   //按键定义
sbit KEY2 = P3^6;   //按键定义
sbit KEY1 = P3^5;   //按键定义
sbit KEY0 = P3^4;
sbit KEY = P3^3;
sbit KEY5 = P3^2;
bit enable = 0;



unsigned char T0RH = 0xFF;  //T0重载值的高字节
unsigned char T0RL = 0x00;  //T0重载值的低字节

unsigned int code NoteFrequ[] = {  //中音1-7低音1-7和高音1-7对应频率列表
    523,  587,  659,  698,  784,  880,  988,  //中音1-7
    1047, 1175, 1319, 1397, 1568, 1760, 1976,  //高音1-7
	  262, 294, 330, 349, 392, 440, 494,300//低音1-7  最后一个纯属凑数
};

unsigned int code NoteReload[] = { //中音1-7和高音1-7对应的定时器重载值
		//65536 - (Xtal/12) / (300*2), //纯属凑数
    65536 - (Xtal/12) / (523*2),  //中音1
    65536 - (Xtal/12) / (587*2),  //2
    65536 - (Xtal/12) / (659*2),  //3
    65536 - (Xtal/12) / (698*2),  //4
    65536 - (Xtal/12) / (784*2),  //5
    65536 - (Xtal/12) / (880*2),  //6
    65536 - (Xtal/12) / (988*2),  //7
    65536 - (Xtal/12) / (1047*2), //高音1
    65536 - (Xtal/12) / (1175*2), //2
    65536 - (Xtal/12) / (1319*2), //3
    65536 - (Xtal/12) / (1397*2), //4
    65536 - (Xtal/12) / (1568*2), //5
    65536 - (Xtal/12) / (1760*2), //6
    65536 - (Xtal/12) / (1976*2), //7
	  65536 - (Xtal/12) / (262*2), //低音1
    65536 - (Xtal/12) / (294*2), //2
    65536 - (Xtal/12) / (330*2), //3
    65536 - (Xtal/12) / (349*2), //4
    65536 - (Xtal/12) / (392*2), //5
    65536 - (Xtal/12) / (440*2), //6
    65536 - (Xtal/12) / (494*2), //7
		65536 - (Xtal/12) / (300*2), //纯属凑数
};

unsigned char code 	ZhiduanqingchangNote[] = { //纸短情长
	      //8, 9, 10, 9, 8, 6, 22, 11, 10, 9, 22, 7, 8, 9, 7, 7, 5, 22, 7, 9, 8, 8, 3, 4, 2, 3, 1, 1, 5, 2,  //前奏
        19, 3, 3, 3, 2, 3, 1, 2, 2, 2, 1, 2, 5, 1, 1, 1, 20, 1, 20, 19,     2, 2, 2, 3, 20, 19, 4, 4, 4, 3, 4, 1, 2, 2, 2, 1, 2, 5,
       	1, 1, 1, 20, 1, 20, 3, 2, 2, 1, 1, 22, 22, 22, 1, 1, 2,             3, 3, 3, 22, 3, 2, 1, 21, 6, 5, 22, 5, 6, 7, 8, 3, 3, 22, 8, 7, 8,
      	7, 3, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4,5, 3, 2, 1, 20, 1,               3, 2, 2, 1, 2, 5, 1, 2, 5, 22, 22, 22, 1, 1, 2, 
        3, 3, 2, 3, 22, 3, 2, 1, 5, 5, 3, 5, 22, 5, 6, 7,                   8, 8, 8, 8, 8, 7, 6, 7, 6, 3, 5, 5, 5, 6, 8, 5, 6, 22, 6, 5, 4, 
        5, 3, 2, 1, 6, 1, 3, 2, 2, 1, 2, 1, 20, 1, 22,                      22, 20, 1, 3, 2, 2, 22, 2, 1, 2, 1, 1, 22, 20, 1, 1, 0xff};	
unsigned char code ZhiduanqingchangBeat[] = {
	      //2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 8, 8, 12, 4, 5, 4, 8, 8, 4,   //前奏
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
unsigned char code XiaoXingXingNote[] = {   //小星星
        1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1,  
        5, 5, 4, 4,    3, 3, 2,    5, 5, 4, 4,    3, 3, 2 ,
        1, 1, 5, 5,    6, 6, 5,    4, 4, 3, 3,    2, 2, 1, 0xff    };

unsigned char code XiaoXingXingBeat[] = {
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,
        4, 4, 4, 4,    4, 4, 8,    4, 4, 4, 4,    4, 4, 8,};
unsigned char code TwoTigerNote[] = {    //两只老虎音符表
        1,   2,   3, 1,    1,   2,   3, 1,   3, 4, 5,   3, 4, 5,
        5,6, 5,4, 3, 1,    5,6, 5,4, 3, 1,   1, 5, 1,   1, 5, 1,0xff};

unsigned char code TwoTigerBeat[] = {    //两只老虎节拍表，4表示一拍，1就是1/4拍，8就是2拍
        4,   4,   4, 4,    4,   4,   4, 4,   4, 4, 8,   4, 4, 8,
        3,1, 3,1, 4, 4,    3,1, 3,1, 4, 4,   4, 4, 8,   4, 4, 8,
    };
unsigned char code HappybirthdayNote[] = {  //生日快乐歌
        5, 5, 6, 5, 8, 7, 22, 5, 5, 6, 5, 9, 8, 5, 5, 12, 10, 8, 7, 6, 11, 11,  10, 8, 9, 8,0xff };
unsigned char code HappybirthdayBeat[] = {
        2, 2, 4, 4, 4, 4, 4, 2, 2,  4, 4, 4, 8, 2, 2, 4, 4, 4, 4, 8, 2, 2, 4, 4, 4, 8};	
unsigned char *SongNote, *SongBeat;//指针	
unsigned char sizeofNote;//节拍的多少
bit tmrflag = 0;  //定时器中断完成标志
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
	EA = 1;       //使能全局中断
	TMOD = 0x11;  //配置T0，T1工作在模式1
	AUXR &= 0x7F;		//定时器时钟12T模式
//	AUXR &= 0xFB;		//定时器时钟12T模式
//	T2L = 0x00;		//设置定时初值
//	T2H = 0x94;		//设置定时初值
//	IE2 |= 0x04;
//	AUXR |= 0x10;		//定时器2开始计时
	IT0 =1;
	EX1 = 1;
	


	TH0 = T0RH;   
	TL0 = T0RL;
	ET0 = 1;      //使能T0中断
	TR0 = 1;

	while (1)
	{
		InterruptTimer1();
	}
}


void Play()//演奏函数 
{
    unsigned char beat;   //当前节拍索引
    unsigned char note;   //当前节拍对应的音符
    unsigned int time = 0;      //当前节拍计时
    unsigned int beatTime = 0;  //当前节拍总时间
    unsigned int soundTime = 0; //当前节拍需发声时间

    for (beat=0; beat<sizeofNote; )  //用节拍索引作为循环变量
    {
				if(breakflage == 1)
						break;
        while (!tmrflag);  //每次定时器中断完成后，检测并处理节拍
        tmrflag = 0;
        if (time == 0)  //当前节拍播完则启动一个新节拍
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
            beatTime = (SongBeat[beat] * NoteFrequ[note]) >> 2;//计算节拍总时间，右移2位相当于除4，移位代替除法可以加快执行速度
            soundTime = beatTime - (beatTime >> 2);//计算发声时间，
            enable = 1;  //指示蜂鸣器开始发声
						if (note == 21)//简谱的0 等待
							enable =0;
            time++;
        }
        else  //当前节拍未播完则处理当前节拍
        {
					if(breakflage == 1)
						break;
            if (time >= beatTime)  //当前持续时间到达节拍总时间时归零，
            {                      //并递增节拍索引，以准备启动新节拍
                time = 0;
                beat++;
            }
            else  //当前持续时间未达到总时间时，
            {
                time++;   //累加时间计数
                if (time == soundTime)  //到达发声时间后，指示关闭蜂鸣器，
                {                       //插入0.25*总时间的静音间隔，
                    enable = 0;         //用以区分连续的两个节拍
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

/*控制蜂鸣器发声 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //重新加载重载值
    TL0 = T0RL;
		tmrflag = 1;
    if (enable)   //使能时反转蜂鸣器控制电平
        BUZZ = ~BUZZ;
    else          //未使能时关闭蜂鸣器
        BUZZ = 1;
}

/*按键扫描*/
void InterruptTimer1() //interrupt 12 using 1 //按键部分参看之前的帖子 带有计次功能的秒表
{
	static char scale = 1;
	bit KeySta = 1;  //当前按键状态
	bit KeySta0 = 1;  //当前按键状态
	bit KeySta1 = 1;  //当前按键状态
	bit KeySta2 = 1;  //当前按键状态
	bit KeySta3 = 1;  //当前按键状态
	bit KeySta4 = 1;  
	bit KeySta5 = 1; 
	bit KeySta12 = 1; 
	
//	T2L = 0x00;		//设置定时初值
//	T2H = 0x41;		//设置定时初值
	
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


















