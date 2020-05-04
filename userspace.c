#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

uint8_t read_buf[4];

int main()
{
        int fd;
        char option;

        fd = open("/dev/htu21df", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }

        read(fd, read_buf, 4);
        uint16_t raw_temp = (read_buf[1] << 8) | (read_buf[0]);

	printf("RAW VALUES\n");
        for(int i = 0; i < 4; i++){
            printf("%u ", read_buf[i]);
        }
	printf("\n");

        float temp = ((raw_temp * 175.72 / 65536) - 46.85);
        printf("Temperature: %.2f C\n", temp);

	uint16_t raw_hum = (read_buf[3] << 8) | (read_buf[0]);
	float hum = (raw_hum * 125.0 / 65536) - 6.0;
	printf("humidity: %.2f percent\n", hum);
        return 0;
}
