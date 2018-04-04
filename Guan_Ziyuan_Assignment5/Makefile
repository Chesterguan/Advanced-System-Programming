obj-m := char_driver.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
	
app: 
	gcc -o userapp userapp.c

clean:
	rm -rf *.o *.ko .*.mod.o.cmd .*.o.cmd .*.ko.cmd *.symvers *.mod.c .tmp_versions *.order *~
clean-app:
	rm -rf userapp
