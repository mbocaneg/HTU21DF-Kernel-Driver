# HTU21DF-Kernel-Driver
A Kenel space driver for the I2C variant of the HTU21DF temperature and humidity sensor that targets Embedded Linux systems. Driver was built on a Raspberry Pi, but should work on any Linux system that has access to an I2C port.

# Building
Run the included makefile in this directory to build the driver:
```
make
```

# Usage
To use the driver first load the driver:
```
sudo insmod htu21df.ko
```
Then grant the necessary permissions to the driver and sysfs:
```
sudo chmod 666 /dev/htu21df
sudo chmod 666 /sys/bus/i2c/devices/i2c-1/new_device
```
...and finally assign the driver an I2C address:
```
echo htu21df 0x40 > /sys/bus/i2c/devices/i2c-1/new_device
```
A sample program is provided to show how to access and read from the sensor from user space.