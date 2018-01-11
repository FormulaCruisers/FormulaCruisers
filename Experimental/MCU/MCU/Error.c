#include <avr/interrupt.h>
#include "Defines.h"
#include "Data.h"

#include "Error.h"

void e_throw(char* error)
{
	//Log the error
	log_print("ERROR: ");
	log_println(error);
}