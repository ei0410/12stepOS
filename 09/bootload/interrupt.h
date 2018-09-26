#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

//both are simbols by defined link script
extern char softvec;
#define SOFTVEC_ADDR (&softvec)

typedef short softvec_type_t; //define type of a kind of software interrupt vector

//define type of interrupt handler
typedef void (*softvec_handler_t)(softvec_type_t type, unsigned long sp);

//position of software interrupt vector
#define SOFTVECS ((softvec_handler_t *)SOFTVEC_ADDR)

#define INTR_ENABLE  asm volatile ("andc.b #0x3f,ccr")
#define INTR_DISABLE asm volatile ("orc.b  #0xc0,ccr")

int softvec_init(void);

int softvec_setintr(softvec_type_t type, softvec_handler_t handler);

//common handler
void interrupt(softvec_type_t type, unsigned long sp);

#endif
