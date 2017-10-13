obj-m:=sysfs_eeprom.o

KBUILD_EXTRA_SYMBOLS+= /home/debian/lkm/spi_interface/Module.symvers

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
	sudo depmod



