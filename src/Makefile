################################################################

ccflags-y += -I$(src)/include

ccflags-y += -Wall -O


################################################################

obj-m += OpticalDrv2.o
obj-m += OpticalDrv4.o

OpticalDrv2-y := touch2/OpticalDrv2.o
OpticalDrv4-y := touch4/OpticalDrv4.o

KVER ?= $(shell uname -r)
KDIR ?= /lib/modules/$(KVER)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

new rebuild:	clean all
