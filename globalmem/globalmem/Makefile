TARGET=globalmem
ifneq ($(KERNELRELEASE),)
	obj-m := $(TARGET).o
else
	KERNELDIR := /lib/modules/5.0.2-050002-generic/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
endif

