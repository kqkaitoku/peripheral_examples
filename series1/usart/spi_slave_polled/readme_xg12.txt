spi_slave_polled

This example demonstrates USART as a SPI slave.
Data received on the MOSI pin is processed by polling.
The TXDATA register is loaded right after receiving data 
to send data to the master each SPI frame on the MISO pin.

How to connect the master board to slave board:
Connect master CS to slave CS
Connect master SCLK to slave SCLK
Connect master MOSI to slave MOSI
Connect master MISO to slave MISO

How To Test:
1. Build the project and download to the Starter Kit
2. Build spi_master project and download to a Starter Kit
3. Place breakpoint in the main while loop when refreshing the RX buffer index as per comments
4. Run spi_slave 
5. Run spi_master 
6. After hitting the breakpoint, observe RxBuffer[] in IDE variables/expressions window. 
   RxBuffer[] should contain 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09.

Peripherals Used:
HFRCO  - 19 MHz
USART2 - Synchronous (SPI) mode, CLK = 1 MHz


Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10

Board:  Silicon Labs EFR32MG12 Radio Board (BRD4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10