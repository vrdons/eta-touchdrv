################################################################

ccflags-y += -I$(src)/include

ccflags-y += -Wall -O


################################################################

obj-m += OpticalDrv.o

KVER ?= $(shell uname -r)
KDIR ?= /lib/modules/$(KVER)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

new rebuild:	clean all
