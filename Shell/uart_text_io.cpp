#include <stdio.h>
#include <avr/interrupt.h>
#include "uart_text_io.h"
#include "../fifo.h"

/*Code module from AVR command shell project.
 See appropriate header file for detais.*/
FIFO( 512 ) uart_tx_fifo;
FIFO( 64 ) uart_rx_fifo;
volatile int i_intxt = 0;

#define FDEV_SETUP_STREAM(p, g, f) { 0, 0, f, 0, 0, p, g, 0 }
FILE uart_stream = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

void TIO_Init(void)
{
	//UCSR0B=(1<<RXEN0) | (1<<TXEN0);
	//разрешить прием, передачу данных и прерывание по приёму байта
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UBRR0 = 16;//103; //9600BPS

	stdout = stdin = &uart_stream;
	i_intxt = 0;
	FIFO_FLUSH(uart_tx_fifo);
	FIFO_FLUSH(uart_rx_fifo);
}

ISR( USART_RX_vect )
{
	unsigned char rxbyte = UDR0;
	if (!FIFO_IS_FULL(uart_rx_fifo))
	{
		FIFO_PUSH(uart_rx_fifo, rxbyte);
	}
}

ISR( USART_UDRE_vect )
{
	if (FIFO_IS_EMPTY(uart_tx_fifo))
	{
		//если данных в fifo больше нет то запрещаем это прерывание
		UCSR0B &= ~(1 << UDRIE0);
	}
	else
	{
		//иначе передаем следующий байт
		char txbyte = FIFO_FRONT(uart_tx_fifo);
		FIFO_POP(uart_tx_fifo);
		UDR0 = txbyte;
	}
}

int uart_putc(  char c, FILE *file )
{
  int ret;
  cli(); //запрещаем прерывания
  if( !FIFO_IS_FULL( uart_tx_fifo ) ) {
    //если в буфере есть место, то добавляем туда байт
    FIFO_PUSH( uart_tx_fifo, c );
    //и разрешаем прерывание по освобождению передатчика
    UCSR0B |= ( 1 << UDRIE0 );
    ret = 0;
  }
  else {
    ret = -1; //буфер переполнен
  }
  sei(); //разрешаем прерывания
  //UCSR0B |= ( 1 << UDRIE0 );
  return ret;
}

int uart_getc( FILE* file )
{
  int ret;
  cli(); //запрещаем прерывания
  if( !FIFO_IS_EMPTY( uart_rx_fifo ) ) {
    //если в буфере есть данные, то извлекаем их
    ret = FIFO_FRONT( uart_rx_fifo );
    FIFO_POP( uart_rx_fifo );
  }
  else {
    ret = _FDEV_EOF; //данных нет
  }
  sei(); //разрешаем прерывания
  return ret;
}

int TIO_CharInput(void)
{
	//while (!(UCSR0A & (1<<RXC0)));
//  return UDR0;
	return getchar();

}

void TIO_CharOutput(uint8_t ch)
{
 // UDR0=ch;
	putchar(ch);
//  while (!(UCSR0A & (1<<UDRE0)));
}

void TIO_TextOutput(char *outtxt)
{
	uint8_t i = 0;

	puts(outtxt);

	/*while (outtxt[i] != 0)
	{
		TIO_CharOutput(outtxt[i]);
		i++;
	}*/
}

boolean TIO_TextInput(uint8_t *intxt)
{
	int ch;

	ch = TIO_CharInput();

	if ((ch != _FDEV_EOF) && (ch != EOF) )
	{

		if (i_intxt < MAX_TXT_LEN)
		{
			if ((i_intxt) || (ch != CHR_BS))
				//TIO_CharOutput(ch); //Echo

			switch (ch)
			{
			case CHR_BS:

				if (i_intxt > 0)
					i_intxt--;

				intxt[i_intxt] = 0;

				TIO_CharOutput(' ');
				TIO_CharOutput(CHR_BS);
				break;

			case CHR_ENTER:
				intxt[i_intxt] = 0;
				i_intxt = 0;
				return true;
				break;

			default:
				intxt[i_intxt] = ch;
				i_intxt++;
				break;
			}
		}
		else
		{
			if (ch == CHR_ENTER)
			{
				intxt[i_intxt - 1] = 0;
				i_intxt = 0;
				return true;
			}
		}
	}
	return false;
}

void TIO_PrintPgmMsg(uint8_t* pgm_msg)
{
	uint16_t i = 0;
	uint8_t ch;

	do
	{
		ch = pgm_read_byte(&(pgm_msg[i]));
		i++;

		if (ch)
			TIO_CharOutput(ch);

	} while (ch != 0);
}
