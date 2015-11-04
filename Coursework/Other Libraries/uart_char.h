#ifndef _UART_TXRX_CHAR
#define _UART_TXRX_CHAR

/*! \brief Init uart 1 at 115200bps, 8 data bits, 1 stop bit, Enable ISR for RX and TX */
void e_init_uart1(void);

/*! \brief Check if something is comming on uart 1
 * \return the number of characters available, 0 if none are available */
int  e_ischar_uart1();

/*! \brief If available, read 1 char and put it in pointer
 * \param car The pointer where the caracter will be stored if available
 * \return 1 if a char has been readed, 0 if no char is available
 */
int  e_getchar_uart1(char *car);

/*! \brief Send a buffer of char of size length
 * \param buff The top of the array where the data are stored
 * \param length The length of the array to send
 */
void e_send_uart1_char(const char * buff, int length);

/*! \brief  To check if the sending operation is done
 * \return 1 if buffer sending is in progress, return 0 if not
 */
int  e_uart1_sending(void);


/*! \brief Init uart 2 at 115200bps, 8 data bits, 1 stop bit, Enable ISR for RX and TX */
void e_init_uart2(void);

/*! \brief Check if something is comming on uart 2
 * \return the number of characters available, 0 if none are available */
int  e_ischar_uart2();

/*! \brief If available, read 1 char and put it in pointer
 * \param car The pointer where the caracter will be stored if available
 * \return 1 if a char has been readed, 0 if no char is available
 */
int  e_getchar_uart2(char *car);

/*! \brief Send a buffer of char of size length
 * \param buff The top of the array where the datas are stored
 * \param length The length of the array
 */
void e_send_uart2_char(const char * buff, int length);

/*! \brief  To check if the sending operation is done
 * \return 1 if buffer sending is in progress, return 0 if not
 */
int  e_uart2_sending(void);


extern void *e_uart1_int_clr_addr; //address to be clear on interrupt
extern int e_uart1_int_clr_mask; //mask to be use to clear on interrupt
extern void *e_uart2_int_clr_addr; //address to be clear on interrupt
extern int e_uart2_int_clr_mask; //mask to be use to clear on interrupt


#endif
