obj-m :=usbkbd.o
KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
app:
	gcc -o userapp userapp.c 
clean:
	sudo rmmod usbkbd.ko
	rm -rf *.o *.ko *.mod.* *.symvers *.order *-
