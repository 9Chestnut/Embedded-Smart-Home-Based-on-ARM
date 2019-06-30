#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "demo.h"



int main(void)
{
	
	int fd;
	fd = open("/dev/fb0",O_RDWR);
	if(fd == -1)
	{
		printf("open fbo error!");
		return ;
	}
	int i,j;
	buf = mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);//映射

 	init();
 	pthread_t id1,id2,id3;  //线程id
	pthread_create(&id1,NULL,display_beam_temp,NULL);
	pthread_create(&id2,NULL,display_gas,NULL);
	pthread_create(&id3,NULL,slide_picture,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	pthread_join(id3,NULL);

  	munmap(buf,480*800*4);//释放映射
	close(fd);
	return 0;
}



/**
 * HCT
 * 显示所有信息
 */
void init()
{
	
	//打印底图
	pic_num(3);
	//字体-温度
	display_world(wen,0xff0000,50,472,24,21);
	display_world(du,0xff0000,50,508,24,21);
	//字体-气压
	display_world(qi,0xff0000,174,472,24,21);
	display_world(ya,0xff0000,174,508,24,21);
	//字体-湿度
	display_world(shi,0xff0000,50,618,24,21);
	display_world(du,0xff0000,50,654,24,21);
	//字体-烟雾
	display_world(yan,0xff0000,174,618,24,21);
	display_world(wu,0xff0000,174,654,24,21);
	//字体-海拔
	display_world(hai,0xff0000,290,450,24,21);
	display_world(ba,0xff0000,290,474,24,21);
	//字体-光照
	display_world(guang,0xff0000,360,450,24,21);
	display_world(zhao,0xff0000,360,474,24,21);
	//字体-贺传腾
    display_world(he,0xff,290,60,24,21);
    display_world(chuan,0xff,290,84,24,21);
	display_world(teng,0xff,290,108,24,21);
	display_number(1640820238, 290,138,number);
	//字体-黄琳江
	display_world(huang,0xff,320,60,24,21);
    display_world(lin,0xff,320,84,24,21);
	display_world(jiang,0xff,320,108,24,21);
	display_number(1640820237, 320,138,number);
	//字体-曾博洋
    display_world(zeng,0xff,355,60,24,21);
    display_world(bo,0xff,355,84,24,21);
	display_world(yang,0xff,355,108,24,21);
	display_number(1640820219, 355,138,number);
	//字体-段君健
    display_world(duan,0xff,390,60,24,21);
    display_world(jun,0xff,390,84,24,21);
	display_world(jian,0xff,390,108,24,21);
	display_number(1640820220, 390,138,number);
	//字体-熊宇
	display_world(xiong,0xff,425,60,24,21);
	display_world(yu,0xff,425,108,24,21);
	display_number(1640820215, 425,138,number);
	//字体-智能家居
	display_world(zhi,0xff,100,740,40,35);
	display_world(neng,0xff,150,740,40,35);
	display_world(jia,0xff,200,740,40,35);
	display_world(ju,0xff,250,740,40,35);
}

/**
 * HCT
 * 触摸屏滑动函数
 */
void * slide_picture(void * arg)
{

	int fd;
	int cout=0;
	fd = open("/dev/input/event0",O_RDWR);//打开文件,读写方式
	if (fd == -1)
	{
		perror("open event0 error");
		return ;
	}
	display_picture(p1,30,15);//展示第一张图片
	struct input_event ev;
	int x1,y1,x2,y2,_x,_y;
	int temp1 = 0;
	int temp2 = 0;
	int i = 0;
	while(1)
	{
		usleep(5000);
		int r = read(fd,&ev,sizeof(ev)); //read函数是以阻塞的方式去读取内容，也就是没有内容可读的话，它就会阻塞在这里，不会向下执行；
		printf("r = %d\n",r);
		if (r != sizeof(ev))//如果读取错误，退出本次循环
		{
			continue;
		}
		//通过type，code来确定，x轴、y轴坐标
		if(ev.type == EV_ABS && ev.code == ABS_X)
		{
			if (temp1 == 0)
			{
				x1 = ev.value;//x轴起始点
				temp1++;
			}
			x2 = ev.value;//x轴终点
			
		}
		if(ev.type == EV_ABS && ev.code == ABS_Y)

		{
			if (temp2 == 0)
			{
				y1 = ev.value;//y轴起始点
				temp2++;
			}
			y2 = ev.value;//y轴终点			
		}
		//滑动结束，判断终点是否为抬起的点
		if (ev.type == 0x01 && ev.code == 0x14a && ev.value == 0) 
		{
			
			printf("x1 = %d , y1 = %d\n",x1,y1 );
			printf("x2 = %d , y2 = %d\n",x2,y2 );
			if (x1>15&&x2<415&&y1>30&&y2<267)//判断只有在相册区域才能识别左右滑动
			{
				_x = abs(x2-x1); //求x2-x1的绝对值
				_y = abs(y2-y1);
				if (_x > _y) //在斜方向滑动时判断为左右滑动
				{
					if (x2 > x1)
					{
						printf("right\n");				
						cout++;
						if(cout>2)
						{
							cout=0;
						}	
					pic_num(cout);				
					}
					else if (x2 < x1)
					{
						printf("left\n");				
						cout--;
						if(cout<0)
						{
							cout=2;
						}			
						pic_num(cout);					
					}
				}
			}
			if (x1>579&&x2<715&&y1>275&&y2<475) //判断只有在灯泡图片区域才能识别上下滑动
			{
				_x = abs(x2-x1); //求x2-x1的绝对值
				_y = abs(y2-y1);
				if(_x < _y) //在斜方向滑动时判断为上下滑动
				{
					if (y2 > y1)
					{
						printf("down\n");
						pic_num(5);
						open_light = 0;
					}
					else if (y2 < y1)
					{
						printf("up\n");
						//pic_num(5);
						open_light = 1;
					}
				}
			}
			
			
			temp1 = 0;
			temp2 = 0;
		}
	
	}
	close(fd);
	return ;
}
/**
 * HCT
 * 打印数字
 */
void display_number(int in_number , int x0,int y0,char number[][42])
{
	int i , j ;
	int x , y;
	char bit_num[10];
	int s=0;
	int z=0;
	while(1)
	{
		bit_num[s]=in_number%10;
		s++;
		in_number=in_number/10;
		if(in_number==0)
		{
			break;
		}
	}
	int bit=0;
	for(s=s-1;s>=0;s--)
	{
		
		for(i=0;i<42;i++)
		{
			for(j=0;j<8;j++)
			{
				if(number[bit_num[s]][i]>>(7-j)&1==1)
				{
					x=i/2;
					y=16*bit+(i%2)*8+j;
					display_point(0xff, x+x0, y+y0);
				}
			}
		}
		bit++;
	}
}

/**
 * HCT
 * 图片映射
 */
void pic_num(int num)
{
	if(pic[num]==0){
		display_picture(p1,30,15);
	}
	else if(pic[num]==1){
		display_picture(p2,30,15);
	}
	else if(pic[num]==2){
		display_picture(p3,30,15);
	}
	else if(pic[num]==3){
		display_picture(p4,0,0);
	}
	else if(pic[num]==4){
		display_picture(p5,275,579);
	}
	else if(pic[num]==5){
		display_picture(p6,275,579);
	}	
}

/**
 * HCT
 * 打印图片
 */
void display_picture(char src[],int x0,int y0)
{

	int fd;
	fd = open(src,O_RDWR);
	if(fd == -1)
	{
		printf("open 2.bmp error!");
		return ;
	}
	//图片宽度
	int w;
	lseek(fd,0x12,SEEK_SET);
    read(fd,&w,4);
    printf("%d\n",w);
    //图片高度
    int h;
    lseek(fd,0x16,SEEK_SET);
    read(fd,&h,4);
    printf("%d\n",h);
    lseek(fd,54,SEEK_SET);

    int n = 0;
    int i,j;
    char buf[w*h*3];
    read(fd,buf,w*h*3);
    int color ;
    for (i = 0; i < h; i++)
    {
    	for (j = 0; j < w; j++)
    	{
            color = buf[n*3+2]<<16 | buf[n*3+1]<<8 | buf[n*3];
    		display_point(color,h-i-1+x0,j+y0);
    		n++;
    	}
    }
	close(fd);
}



/**
 * HCT
 * 打印字
 */
void display_world(char word[],int color,int x0,int y0,int w,int h)
{
	int i,j;
	int a,b;
 	for (i = 0; i < (w*h/8); i++)
	{
  		for (j = 0; j < 8; j++)
  		{
  			if (word[i] >> (7-j) & 1 == 1)
  			{
  				a = i/(w/8);
  				b = (i%(w/8))*8+j;
  		  		display_point(color,a+x0,b+y0);
  			}
  		}

   	}
	return ;
}

/**
 * HCT
 * 打印点
 */
void display_point(int color,int x,int y)
{
	*(buf+x*800+y) = color;
}

/**
 * HCT
 * 显示光照，温度，湿度
 */
void * display_beam_temp(void * arg)
{
	int fd = serial_init(COM2,9600);
	if(fd == -1)
    {
        perror("open serial error");
        return ;
    }

 	int lux1,tmp1,pre1,hum1,alt;
 	int lux2,tmp2,pre2,hum2;

 	int fd1;
	fd1 = open("/dev/fb0",O_RDWR);
	if(fd1 == -1)
	{
		printf("open fbo error!");
		return ;
	}
	int i,j;
	buf = mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd1,0);//映射
    while(1)
    {
 		//写命令
    	char w_buf[3] = {0xa5,0x83,0x28};
	    int w = write(fd,w_buf,3);
        printf("w = %d\n", w);//w表示命令字节数
	    usleep(5000); //延时5000微秒
	    //读数据
	    char r_buf[24];
	    int r = read(fd,r_buf,24);//r表示实际接收到的字节数
	    printf("r = %d\n", r);
	    //通过协议判断接收到的数据是否正确
	 	if (r == 24 && r_buf[2] == 0x15 && r_buf[11] == 0x45)
	 	{
	 		//光照
	 		lux1 = (r_buf[4]<<24) | (r_buf[5]<<16) | (r_buf[6]<<8) | r_buf[7];
	 		lux2=lux1/100;
	 		//清屏，刷新底色，防止数据覆盖
	 		for(i=350;i<405;i++)
			{
				for(j=500;j<579;j++)
				{		

					*(buf+i*800+j) = 0xF3C9A1;
				}
			}
			//把光照打印到屏幕
	        display_number(lux2,360,510,number);
	 		printf("lux = %d\n",lux2);
			
	 		tmp1 = (r_buf[13]<<8) |  r_buf[14];
	 		tmp2 = tmp1/100;
	 		//清屏，刷新底色，防止数据覆盖
	 		for(i=80;i<110;i++)
			{
				for(j=480;j<530;j++)
				{		

					*(buf+i*800+j) = 0xF2E9E0;
				}
			}
	 		display_number(tmp2, 90,487,number);
	 		printf("tem = %d\n",tmp2);
	 		//气压
	 		pre1 = (r_buf[15]<<24) | (r_buf[16]<<16) | (r_buf[17]<<8) | r_buf[18];
	 		pre2 = pre1/100;
	 		//清屏，刷新底色，防止数据覆盖
	 		for(i=210;i<240;i++)
			{
				for(j=465;j<568;j++)
				{		

					*(buf+i*800+j) = 0xF2E9E0;
				}
			}
	 		display_number(pre2, 214,470,number);
	 		printf("pre = %d\n",pre2);
	 		//湿度
	 		hum1 = (r_buf[19]<<8) | r_buf[20];
	 	    hum2 = hum1/100;
	 		//清屏，刷新底色，防止数据覆盖
	 		for(i=80;i<110;i++)
			{
				for(j=620;j<680;j++)
				{		

					*(buf+i*800+j) = 0xF2E9E0;
				}
			}
	 		display_number(hum2, 90,628,number);
	 		printf("hum = %d\n",hum2);
	 		//海拔
	 		alt = (r_buf[21]<<8) |  r_buf[22];
	 		//清屏，刷新底色，防止数据覆盖
	 		for(i=280;i<310;i++)
			{
				for(j=500;j<579;j++)
				{		

					*(buf+i*800+j) = 0xF3C9A1;
				}
			}
	 		display_number(alt, 290,510,number);
	 		printf("alt = %d\n",alt);
	 	}
	 	//通过判断光照强度和开关（open_light == 1）控制灯泡亮灭
	 	if (lux1<50 && open_light == 1)
	 	{
	 		pic_num(4);
	 	}
	 	else
	 	{
	 		pic_num(5);
	 	}
		usleep(100*5000);
		
    }
    munmap(buf,480*800*4);//释放映射
    close(fd1);
	close(fd);  
}

//烟雾函数
void * display_gas(void * arg)
{
	//初始化串口
	int fd = serial_init(COM3,9600);
	if(fd == -1)
    {
        perror("open serial error");
        return ;
    }
    int fd1 = open("/dev/beep",O_RDWR);
    if(fd1 == -1)
    {
        perror("open beep error");
        return ;
    }
    int fd2;
	fd2 = open("/dev/fb0",O_RDWR);
	if(fd2 == -1)
	{
		printf("open fbo error!");
		return ;
	}
	int i,j;
	buf = mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);//映射
    while(1)
    {
    	 //进行通信，写命令
	    char w_buf[9] = {0xFF,0x01,0x86,0x00,0,0,0,0,0x79};
	    write(fd,w_buf,9);
	    usleep(5000); //延时5000微秒
	    //读数据
	    char r_buf[9];
	    int r = read(fd,r_buf,9);
	    //通过协议判断接收到的数据是否正确
	    if (r == 9 && r_buf[0] == 0xFF && r_buf[1] == 0x86)
	    {
	    	int n = r_buf[2] << 8 | r_buf[3];//浓度值，即第3和第4个字节
			//清屏，刷新底色，防止数据覆盖
			for(i=210;i<240;i++)
			{
				for(j=580;j<700;j++)
				{		

					*(buf+i*800+j) = 0xF2E9E0;
				}
			}
			display_number(n, 214,624,number);
			printf("n = %d \n",n);
			//通过判断烟雾浓度控制蜂鸣器是否响
			if (n > 90)
			{
				ioctl(fd1,0,1);//蜂鸣器响
				sleep(2);
				ioctl(fd1,1,1);//蜂鸣器不响
			}
			else
			{
				ioctl(fd1,1,1);//蜂鸣器不响
			}
	    }
		usleep(100*5000);
    }
    munmap(buf,480*800*4);//释放映射
    close(fd1);
    close(fd);
    close(fd2);
	return 0;
}


/**
 * HCT
 * 串口初始化函数
 */
int serial_init(char *file,int peed)
{
	/*
	210串口所对应的文件名
	#define COM1 "/dev/s3c2410_serial1"
	#define COM2 "/dev/s3c2410_serial2"
	#define COM3 "/dev/s3c2410_serial3"
	file:串口所对应的文件名
	peed:波特率,是9600
	返回值是初始化之后的串口文件的文件描述符
	*/
	
	//打开串口文件
    int fd = open(file,O_RDWR);
    if(fd == -1)
    {
        perror("open serial error");
        return -1;
    }
	
    /*定义串口属性结构体*/
    struct termios myserial;
    memset(&myserial,0,sizeof(myserial));//清零
	
	/*设置控制模式 本地连接  使能接收*/
    myserial.c_cflag |= (CLOCAL | CREAD); 
    myserial.c_cflag &=  ~CSIZE; //清空数据位
    myserial.c_cflag &= ~CRTSCTS; //无硬件控制流
    myserial.c_cflag |= CS8; //数据位 8
    myserial.c_cflag &= ~CSTOPB; //停止位 1
    myserial.c_cflag &= ~PARENB; //不要校验

    /*设置波特率*/
	switch(peed)
	{
		case 9600:
			cfsetospeed(&myserial,B9600);
			cfsetispeed(&myserial,B9600);
			break;
		case 57600:
			cfsetospeed(&myserial,B57600);
			cfsetispeed(&myserial,B57600);
			break;
		case 115200:
			cfsetospeed(&myserial,B115200);
			cfsetispeed(&myserial,B115200);
			break;
	}
	
    /*刷新输出队列，清除正接收的数据*/
    tcflush(fd,TCIFLUSH);

    /*更改配置*/
    tcsetattr(fd,TCSANOW,&myserial);
	
    return fd;
}
