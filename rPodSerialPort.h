#ifndef rPodSerialPort_H
#define rPodSerialPort_H

int set_interface_attribs (int fd, int speed);
void set_blocking (int fd, int should_block);
int parse_baud(int baudrate);

#endif 
