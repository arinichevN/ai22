#define PRINT_NL serial->write('\n')
#define PRINT_TAB serial->write('\t')
#define PRINT_VAR(V) serial->print(V)
#define PRINT_CELL(V) PRINT_VAR(V); PRINT_TAB

void print_var(HardwareSerial *serial){
	;
}

