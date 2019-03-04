#include <stdio.h>
#include "log_printf.h"
#include <string.h>
#include <stdarg.h>

// #define PSDK_PRINTF_OPEN  

void LOG_Init(void)
{
    UART_Init(UART_TEST_NUM, 115200); //yanly
}

void UART_Printf(const char *fmt, ...)
{
#ifdef PSDK_PRINTF_OPEN
	char buffer[LOG_STR_LEN_MAX];
	va_list args;
	va_start (args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	UART_Write(UART_TEST_NUM, (uint8_t *)buffer, strlen(buffer) + 1);
	va_end(args);
#endif
}

/************************** (C) COPYRIGHT DJI Innovations************ END OF FILE ***/