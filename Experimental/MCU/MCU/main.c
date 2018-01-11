#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Defines.h"
#include "CAN.h"
#include "Data.h"
#include "Timing.h"

int main()
{	
	init_timers();
	can_init();
	can_rx(MASTERID);
	
	set_timefunc(TPER_1024HZ, mainloop);
	
	//Set CPU into sleep mode(while simultaneously enabling interrupts)
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();
	
	while(1);
}

void mainloop()
{
	
}