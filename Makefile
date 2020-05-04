export ARCH :=arm
export CROSS_COMPILE:=/home/saturn/tools/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
obj-m += htu21df.o
KDIR = /home/saturn/pi/linux
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

