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

int fd,fp;

pthread_t threads[2];

void *ioctl_change_mode(void *arg)
{
	int *i = (int*)arg;
	int id  = *i;
	
	printf("\n Thread id %d has started \n", id);
	printf("\n Ioctl Operation.Mode is being changed to 2 ");
	ioctl(fd, E2_IOCMODE2,2);
   
	pthread_exit(NULL);
 
}

void *open_fp(void *arg)
{
	int *i = (int*)arg;
	int id  = *i;
	char* msg;
	
	printf("\n Thread id %d has started \n", id);
	
	fp = open(ASP_DEVICE, O_RDWR);
    if(fp == -1)
	{
		msg =   strerror(errno);
                printf("File %s either does not exist or has been locked by another process\n", ASP_DEVICE);
                fprintf(stderr, "open failed: %s\n", msg);
		exit(-1);
	}
    printf(" Device opened from thread id %d - file descriptor =  %d \n",id,fp);

	pthread_exit(NULL);
 
}

void *open_fd(void *arg)
{
	int *i = (int*)arg;
	int id  = *i;
	char* msg;
	
	printf("\n Thread id %d has started \n", id);
	
	fd = open(ASP_DEVICE, O_RDWR);
    if(fd == -1)
	{
		msg =   strerror(errno);
                printf("File %s either does not exist or has been locked by another process\n", ASP_DEVICE);
                fprintf(stderr, "open failed: %s\n", msg);
		exit(-1);
	}
    printf(" Device opened from thread id %d - file descriptor =  %d \n",id,fd);

	pthread_exit(NULL);
 
}


int main() 
{
	//int i = 0, j = 1, k = 2;
	int x=0;
	int y=1;
	int z=2;
	
	pthread_create(&(threads[x]), NULL, ioctl_change_mode, (void*)&x);
	pthread_create(&(threads[y]), NULL, open_fp, (void*)&y);
	pthread_create(&(threads[z]), NULL, open_fd, (void*)&z);
	
	pthread_join(threads[x], NULL);
	pthread_join(threads[y], NULL);
	pthread_join(threads[z], NULL);
	
	close(fd);
	close(fp);
	
	printf("\n Main thread closed device files \n");	
	
	pthread_exit(NULL);
}
