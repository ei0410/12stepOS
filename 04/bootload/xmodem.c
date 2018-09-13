#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "xmodem.h"

//control code
#define XMODEM_SOH 0x01
#define XMODEM_STK 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a //ctrl-z

#define XMODEM_BLOCK_SIZE 128

static int xmodem_wait(void)
{
    long cnt = 0;

    //send NAK while receive start
    while (!serial_is_recv_enable(SERIAL_DEFAULT_DEVICE)) {
        if (++cnt >= 2000000) {
            cnt = 0;
            serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_NAK);
        }
    }

    return 0;
}

//receive per block
static int xmodem_read_block(unsigned char block_number, char *buf)
{
    unsigned char c, block_num, check_sum;
    int i;

    //receive block number
    block_num = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if (block_num != block_number)
        return -1;

    //receive bitwised block number
    block_num ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if (block_num != 0xff)
        return -1;

    //receive 128byte data
    check_sum = 0;
    for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
        c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
        *(buf++) = c;
        check_sum += c;
    }

    //receive check_sum
    check_sum ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if (check_sum)
        return -1;

    return i;
}

long xmodem_recv(char *buf)
{
    int r, receiving = 0;
    long size = 0;
    unsigned char c, block_number = 1;

    while (1) {
        if (!receiving)
            xmodem_wait();//send request while receiving start

        c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);//receive a charactor

        if (c == XMODEM_EOT) { //end receiving 
            serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_ACK);
            break;
        } else if (c == XMODEM_CAN) { //cancel receiving
            return -1;
        } else if (c == XMODEM_SOH) { //start receiving
            receiving++;
            r = xmodem_read_block(block_number, buf);
            if (r < 0) { //error
                serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_NAK);
            } else { //add buffer pointer and return ACK
                block_number++;
                size += r;
                buf  += r;
                serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_ACK);
            }
        } else {
            if (receiving)
                return -1;
        }
    }
    
    return size;
}
