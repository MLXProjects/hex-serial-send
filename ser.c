#define TERM "/dev/ttyUSB0"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//typedef byte unsigned char;
#define byte unsigned char

int ser_init(int fd, int speed){
    struct termios tty={0};
    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return 0;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return 0;
    }
    return 1;
}

int main(int argc, char **argv){
	byte dev_specified = (argc>1) && (argv[1][0] == '/');
	int first_arg = dev_specified?2:1;
	byte cmd;
	int fd = open(dev_specified?argv[1]:TERM, O_RDWR|O_NOCTTY|O_SYNC);
	if (fd < 0){
		printf("cannot open\n");
		return 1;
	}
	if (!ser_init(fd, B2400)){
		printf("cannot init serial\n");
		return 2;
	}
	int done;
	unsigned char buf[2] = {0xA5, 0x10};
	for (cmd = first_arg; cmd < argc; cmd++){
		if (strlen(argv[cmd]) > 2){
			char temp_str[3] = {argv[cmd][0], argv[cmd][1], 0};
			buf[0] = strtol(temp_str, NULL, 16);
			char temp_str2[3] = {argv[cmd][2], argv[cmd][3], 0};
			buf[1] = strtol(temp_str2, NULL, 16);
		}
		else buf[1] = strtol(argv[cmd], NULL, 16);
		done = write(fd, &buf, 2);
		if (done != 2){
			printf("written 0x%X%s%X - (%d != 2) bytes\n", buf[0], (buf[1]>>4)>0?"":"0", buf[1], done);
		}
		else printf("written 0x%X%s%X - 2 bytes\n", buf[0], (buf[1]>>4)>0?"":"0", buf[1]);
	}
	tcdrain(fd);
}
