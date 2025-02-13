#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>     //POSIX中断控制定义
#include <sys/select.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#define COUNT 37


 int fd = 0;
 char buff[100] = {0};


int uart_set(int fd)
{
    struct termios options;
    if(tcgetattr(fd,&options) < 0) {
    printf("tcgetattr error\n");
    return -1;
    }
    //设置波特率
    cfsetispeed(&options,B115200);
    cfsetospeed(&options,B115200);
    //设置数据位
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    //设置校验位
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~INPCK;
    //关闭流控
    options.c_cflag &= ~CRTSCTS;
    //设置停止位
    options.c_cflag &= ~CSTOPB;
    if(tcsetattr(fd,TCSANOW,&options) < 0) {
        printf("tcsetattr failed\n");
        return -1;
    }
        return 0;
}


int uart_write(int fd, char *buf, int len)
{
    int ret;
    int write_num, left_num;
    char *ptr;
    
    left_num = len;
    while(left_num > 0)
        {
            write_num = write(fd,buf,left_num);
            if (write_num > 0) 
            {
                left_num -= write_num;
                ptr += write_num;
            } else {
                    printf("write fail!\n");
                    return -1;
                    }
        }
    return 0;
}

int uart_read(int fd, char *buf, int len)
{
    int ret;
    int read_num, left_num;
    fd_set rfds;
    char *ptr;

    FD_ZERO(&rfds);
    FD_SET(fd,&rfds);

    left_num = len;

    ret = select(fd+1,&rfds,NULL,NULL,NULL);
    if (ret > 0) 
    {
        while(left_num > 0)
        {
            read_num = read(fd,buf,left_num);
            if (read_num > 0) 
            {
                left_num -= read_num;
                ptr += read_num;
            } 
            else 
            {
                printf("read fail!\n");
                return -1;
            }
        }
    }
    return 0;
}


int main()
{
    int w = 0;
    int i = 0;
    fd = open("/dev/ttyAMA1",O_RDWR|O_NOCTTY|O_NONBLOCK);
    uart_set(fd);
    printf("fd=%d\n",fd);
    unsigned char buf[] = {0xAA,0x05,0x01,0xA3,0x01,0x00,0x54,0xEB,0xAA};
    int len = sizeof(buf) / sizeof(buf[0]);
    int ret = 0;   
    w = write(fd, buf, len);   
    printf("w = %d\n",w);
    if( w != len)
    {
        printf("write error!!! write_num = %d\n",w);
        return -1;
    };             
    ret = read(fd,buff,10);
    printf("ret = %d\n",ret);
    if(ret == -1)
    {
        printf("read error!!!\n");
        printf("errno = %d\n",errno);
        return -1;
    }
    
    printf("read = %#x",buff[0]);
    return 0 ;
}



