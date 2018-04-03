KDIR	:= /usr/src/linux-headers-$(shell uname -r)
obj-m := asp_mycdev.o

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
#	gcc -Wall -Werror -02 -o rw_test rw_test.c 
#	gcc -Wall -Werror -02 -o lseek_test lseek_test.c
#	gcc -Wall -Werror -02 -o ioctl_test ioctl_test.c

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	rm -f *.o.cmd *.symvers *.order *.gch rw_test lseek_test ioctl_test
