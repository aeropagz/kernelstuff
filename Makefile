CFileName:=myFirstDriver

ifneq ($(KERNELRELEASE),)
	obj-m  := $(CFileName).o

else
KDIR := /lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all: $(CFileName).ko

$(CFileName).ko: $(CFileName).c
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	
clean:
	rm Module.symvers
	rm modules.order
	rm $(CFileName).ko
	rm $(CFileName).mod.c
	rm $(CFileName).mod.o
	rm $(CFileName).mod
	rm $(CFileName).o
	
endif
