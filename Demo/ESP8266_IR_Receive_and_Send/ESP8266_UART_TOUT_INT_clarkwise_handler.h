/*
 ESP8266_UART_TOUT_INT_clarkwise_handler.h - handling 
 returned data from clarkwise touch screen.
 Copyright (c) 2021 clarkwise.cc  All right reserved.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the Apache License 2.0

 Usage:
 Call install_uart_tout(); in setup.
 Rewrite clarkwise_returns_handler() in your code. The returned data is stored as rMsg
 */

#ifndef ESP8266_UART_TOUT_INT_clarkwise_handler_h
#define ESP8266_UART_TOUT_INT_clarkwise_handler_h

#include "uart_register.h"

struct clarkwiseMsg { 
    unsigned short control_id;
    int varInt = 0;
    float varFloat = 0.0;
    String varString = "";
} rMsg;

void clarkwise_returns_handler(); //rewrite this function in you code to process with the rMsg;
void uart0_rx_intr_handler(void *para);
void install_uart_tout();

void uart0_rx_intr_handler(void *para){
  uint8_t RcvChar;
  uint8_t uart_no = UART0;
  uint8_t fifo_len = 0;
  uint8_t buf_idx = 0;
  uint32_t uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no));//get uart intr status
    if (UART_RXFIFO_TOUT_INT_ST == (uart_intr_status & UART_RXFIFO_TOUT_INT_ST)) { //if it is caused by a time_out interrupt
      fifo_len = (READ_PERI_REG(UART_STATUS(uart_no)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT; //read rx fifo length
      buf_idx = 0;
      char rType = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
      fifo_len--;
      if(rType!='@' && rType!='#' && rType!='$'){
        while(buf_idx<fifo_len){
          READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
          buf_idx++;
        }
      } else {
        rMsg.varInt = 0;
        rMsg.varFloat = 0.0;
        rMsg.varString = "";
        buf_idx=0;
        char rCID[6];
        bool stopSignal = false;
        while ((buf_idx < fifo_len) && !stopSignal) {
          char rChar = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
          if(rChar==' '){
            rCID[buf_idx]='\0';
            rMsg.control_id=atoi(rCID);
            fifo_len = fifo_len - buf_idx - 1;
            buf_idx=0;
            stopSignal = true;
          } else {
            rCID[buf_idx]=rChar;
            buf_idx++;
          }
        }
        char rData[fifo_len];
        while (buf_idx < fifo_len) {
          rData[buf_idx] = READ_PERI_REG(UART_FIFO(uart_no)) & 0xFF;
          buf_idx++;
        }
        rData[fifo_len] = '\0';
        switch (rType){
          case '@':
            rMsg.varInt = atoi(rData);
            break;
          case '#':
            rMsg.varFloat = atof(rData);
            break;
          case '$':
            rMsg.varString = rData;
            break;
        }
        clarkwise_returns_handler(); 
      }
      WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_TOUT_INT_CLR); //clear full interrupt state
    }
    uart_intr_status = READ_PERI_REG(UART_INT_ST(uart_no)); //update interrupt status
}

void install_uart_tout(){
  ETS_UART_INTR_DISABLE(); //Disable UART Interrupt
  ETS_UART_INTR_ATTACH(uart0_rx_intr_handler, NULL); //attach handler function to uart0_rx_intr_handler
  
  WRITE_PERI_REG(UART_CONF1(0), UART_RX_TOUT_EN |
    ((0x2 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S)); //Enable UART RX Timeour function and set the timeout period as the time transmitting 2 bits 

  WRITE_PERI_REG(UART_INT_CLR(0), 0xffff); //Clear UART Interrupt flags
  SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_TOUT_INT_ENA); //Enable UART RX Timeout interrupt
  CLEAR_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA); //Disable UART RX Full interrupt
  ETS_UART_INTR_ENABLE();
}

#endif
