obj-m := driver_testcase4.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
app:
	gcc -o TestCase4 TestCase4.c -lpthread
#	gcc -o TestCase3 TestCase3.c -lpthread
# 	gcc -o TestCase2 TestCase2.c -lpthread
#	gcc -o TestCase1 TestCase1.c -lpthread

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
	rm -rf TestCase4
	
