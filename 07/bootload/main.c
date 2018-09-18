#include "defines.h"
#include "interrupt.h"
#include "serial.h"
#include "xmodem.h"
#include "elf.h"
#include "lib.h"

static int init(void)
{
    extern int erodata, data_start, edata, bss_start, ebss;//the simbles are defined by link script.

    memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
    memset(&bss_start, 0, (long)&ebss - (long)&bss_start);
    //initialize data areas and bss areas. If you forget the process, global variant will not be init.

	softvec_init();

    serial_init(SERIAL_DEFAULT_DEVICE);

    return 0;
}

//hex dump output of memories
static int dump(char *buf, long size)
{
    long i;

    if (size < 0) {
        puts("no data.\n");
        return -1;
    }
    for (i = 0; i < size; i++) {
        putxval(buf[i], 2);
        if ((i & 0xf) == 15) {
            puts("\n");
        } else {
            if ((i & 0xf) == 7) puts(" ");
            puts(" ");
        }
    }
    puts("\n");

    return 0;
}

//service function for wait
static void wait()
{
    volatile long i;
    for (i = 0; i < 300000; i++) 
        ;
}

int main(void)
{
    static char buf[16];
    static long size = -1;
    static unsigned char *loadbuf = NULL;
	char *entry_point;
	void (*f)(void);
    extern int buffer_start; //defined buffer by link script

	INTR_DISABLE;

    init();

    puts("kzload (kozos boot loader) started.\n");

    while (1) {
        puts("kzload> "); //show prompt
        gets(buf); //receive command from serial

        if (!strcmp(buf, "load")) { //download xmodem files
            loadbuf = (char *)(&buffer_start);
            size = xmodem_recv(loadbuf);
            wait(); //wait while send app finish and return terminal
            if (size < 0) {
                puts("\nXMODEM receive error!\n");
            } else {
                puts("\nXMODEM receive succeeded.\n");
            }
        } else if (!strcmp(buf, "dump")) { //hex dump output of memories
            puts("size: ");
            putxval(size, 0);
            puts("\n");
            dump(loadbuf, size);
        } else if (!strcmp(buf, "run")) { //run elf file
			entry_point = elf_load(loadbuf); //load and spread on memories
			if (!entry_point) {
				puts("run over!\n");
			} else {
				puts("starting from entry point: ");
				putxval((unsigned long)entry_point, 0);
				puts("\n");
				f = (void (*)(void))entry_point;
				f(); //bring process to loaded program
			}
		} else {
            puts("unknown.\n");
        }
    }
    
    return 0;
}
