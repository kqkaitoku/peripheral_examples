/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates interrupt-driven use of the USART in
 * synchronous (SPI) master mode.  The main loop transmits the specified
 * number of bytes and receives the byte that is shifted in with each
 * outgoing one.
 *
 * The pins used in this example are defined below and are described in
 * the accompanying readme.txt file.
 *
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_usart.h"

// Size of the data buffers
#define BUFLEN  10

// Ports and pins for SPI interface
#define US0MISO_PORT  gpioPortA
#define US0MISO_PIN   5
#define US0MOSI_PORT  gpioPortA
#define US0MOSI_PIN   6
#define US0CLK_PORT   gpioPortC
#define US0CLK_PIN    3
#define US0CS_PORT    gpioPortC
#define US0CS_PIN     1

// Outgoing data
uint8_t outbuf[BUFLEN];

// Incoming data
uint8_t inbuf[BUFLEN];

// Position in the buffer
uint32_t bufpos;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure RX pin as an input
  GPIO_PinModeSet(US0MISO_PORT, US0MISO_PIN, gpioModeInput, 0);

  // Configure TX pin as an output
  GPIO_PinModeSet(US0MOSI_PORT, US0MOSI_PIN, gpioModePushPull, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(US0CLK_PORT, US0CLK_PIN, gpioModePushPull, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet(US0CS_PORT, US0CS_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    USART0 initialization
 *****************************************************************************/
void initUsart0(void)
{
  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf = true;   // MSB first transmission for SPI compatibility

  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[0].RXROUTE = (US0MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (US0MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].TXROUTE = (US0MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (US0MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (US0CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |    // MISO
                                GPIO_USART_ROUTEEN_TXPEN |    // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN;

  // Configure and enable USART0
  USART_InitSync(USART0, &init);

  // Enable transmit complete interrupt
  USART_IntEnable(USART0, USART_IEN_TXC);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(USART0_TX_IRQn);
  NVIC_EnableIRQ(USART0_TX_IRQn);
}

/**************************************************************************//**
 * @brief
 *    USART0 transmit interrupt handler.
 *****************************************************************************/
void USART0_TX_IRQHandler(void)
{
  /*
   * Save byte received concurrent with the transmission of the last bit of
   * the previous outgoing byte, and increment the buffer position to the
   * next byte.
   */
  inbuf[bufpos++] = USART0->RXDATA;

  // If there are still bytes left to send, transmit the next one
  if (bufpos < BUFLEN)
    USART0->TXDATA = outbuf[bufpos];

  // Clear the requesting interrupt before exiting the handler
  USART_IntClear(USART0, USART_IF_TXC);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Chip errata
  CHIP_Init();

  // Initialize GPIO and USART0
  initGpio();
  initUsart0();

  while (1)
  {
    // Zero incoming buffer and populate outgoing data array
    for (i = 0; i < BUFLEN; i++)
    {
      inbuf[i] = 0;
      outbuf[i] = (uint8_t)i;
    }

    // Start at the beginning of the buffer
    bufpos = 0;

    // Assert chip select (drive low)
    GPIO_PinOutClear(US0CS_PORT, US0CS_PIN);

    /*
     * Transmit the first byte, then go into EM1.  The IRQ handler will
     * receive each incoming byte and transmit the next outgoing byte.
     */
    USART0->TXDATA = outbuf[bufpos];

    while (bufpos < BUFLEN)
      EMU_EnterEM1();

    // De-assert chip select upon transfer completion (drive high)
    GPIO_PinOutSet(US0CS_PORT, US0CS_PIN);
  }
}
