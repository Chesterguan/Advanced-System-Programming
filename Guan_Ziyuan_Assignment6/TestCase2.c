#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include <linux/ioctl.h>
#include <pthread.h>

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

#define ASP_DEVICE "/dev/MYCDEV"


pthread_t threads[3];
struct data
{
	int threadId;
	int  filedesc;
};

struct data data0,data1,data2;

void *mode_change_1(void *arg)
{
	struct data *i = (struct data*)arg;
	int id  = i->threadId;
	int fd1 = i->filedesc;
	
	printf("\n Thread id %d has started \n", id);
	
	printf("\n Thread id %d tries to change the mode of device to mode 1 \n", id);	
	ioctl(fd1,E2_IOCMODE1,1);
	
	pthread_exit(NULL);
 
}

void *mode_change_2(void *arg)
{
	struct data *i = (struct data*)arg;
	int id  = i->threadId;
	int fd1 = i->filedesc;
	
	printf("\n Thread id %d has started \n", id);
	
	printf("\n Thread id %d tries to change the mode of device to mode 2 \n", id);	
	ioctl(fd1,E2_IOCMODE2,2);
	
	pthread_exit(NULL);
 
}

void *write_data(void *arg)
{
	struct data *i = (struct data*)arg;
	int id  = i->threadId;
	int fd0 = i->filedesc;
	char buf[30];
	char* msg;
	
	printf("Thread id %d has started \n",id);
	fd0 = open(ASP_DEVICE, O_RDWR);
        if(fd0 == -1)
	{
		msg =   strerror(errno);
                printf("File %s either does not exist or has been locked by another process\n", ASP_DEVICE);
                fprintf(stderr, "open failed: %s\n", msg);
		exit(-1);
	}
         printf(" Device opened from thread id %d in mode 1 file descriptor =  %d \n",id,fd0);
	
	read(fd0,buf,sizeof(buf));
	strcpy(buf,"Helloworld");
	sleep(10);
	write(fd0,buf,sizeof(buf));
	
	printf("\n Thread id %d close device \n", id);	
	close(fd0);
	
	pthread_exit(NULL);
    
}

int main() 
{
	int fd0 = 0;
        int fd1;
	char* msg;
	int x=0;
	int y=1;
	int z=2;
	
	fd1 = open(ASP_DEVICE, O_RDWR);
        if(fd1 == -1)
	{
		msg =   strerror(errno);
                printf("File %s either does not exist or has been locked by another process\n", ASP_DEVICE);
                fprintf(stderr, "open failed: %s\n", msg);
		exit(-1);
	}
        printf(" Device opened from main thread - file descriptor =  %d \n",fd1);
	
	
	data0.threadId = 0;
	data0.filedesc = fd1;
	pthread_create(&(threads[x]), NULL, mode_change_1, (void*)&data0);
	
	data1.threadId = 1;
	data1.filedesc = fd1;
	pthread_create(&(threads[y]), NULL, mode_change_2, (void*)&data1);
	
	data2.threadId = 2;
	data2.filedesc = fd0;
	pthread_create(&(threads[z]), NULL, write_data, (void*)&data2);
	
	pthread_join(threads[x], NULL);
	pthread_join(threads[y], NULL);
	pthread_join(threads[z], NULL);
	
	close(fd1);
	pthread_exit(NULL);
}
