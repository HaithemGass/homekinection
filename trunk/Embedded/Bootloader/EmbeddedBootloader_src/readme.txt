Bootloader for mega family. Revision 1.7

Supported devices:
         MCU                                           external flash memory          RF4CE boot func  
atxmega128a1  (not tested):                                  nothing                         no 
atxmega256a3  (both compilers):                              at45db041                       no 
atmega1281    (both compilers):                              at25f2048, at45db041            yes
atmega2561    (both compilers):                              nothing                         no 
atmega128rfa1 (both compilers):                              at25f2048, at45db041            yes
atxmega256d3  (both compilers):                              at45db041                       no

Fuse bytes for xmega:            (with and without OTAU)
FUSEBYTE0                                 0xFF
FUSEBYTE1                                 0x00
FUSEBYTE2                                 0xBF
FUSEBYTE4                                 0xFE
FUSEBYTE5                                 0xFF

Fuse bytes for atmega:       (without OTAU)     (with OTAU)     (with RF4CE boot functions)
EXTENDED                        0xFF              0xFF                    0xFF  
HIGH                            0x9C              0x9A                    0x9A 
LOW                             0x62              0x62                    0x62

Fuse bytes for atmega128rfa1:(without OTAU)     (with OTAU)     (with RF4CE boot functions)
EXTENDED                        0xFE              0xFE                    0xFE   
HIGH                            0x9C              0x9A                    0x9A
LOW                             0x62              0x62                    0x62

Revision history:

1.0 - initial revision.
1.1 - atmega1281\2561 are supported.
1.2 - SPI serializer is supported for xMega target. 
SPI parameteres: 
frequency less or equal 200 kHz;
start of SPI clock after slave select greater or equal 100 ns;
delay between consecutive transfers greater or equal 100 us.
spiFlasher is added for test of SPI part of bootloader. spiFlasher is actual for sam7x evaluation board target.
Hardware sources are used by spiFlasher:
IRQ1 - interrupt line for slave data ready station. (A15 on S3)
SPI1 - SPI interface (clock - A23, mosi - A24, miso - A25, ss - A26 on S3)
1.3 - Loading from external flash is supported for atmega1281. External SPI flash at25f2048 is used. 
USART0 is used in MSPI mode, pin F3 is used as chip select. Size of boot section is 4 kbytes.
1.4 - Service area are increased(1 byte) for external flash loader.
1.5 - atmega128rfa1 is supported.
1.6 - atxmega256d3 is supported. at45db041 can be used for bootloader with OTAU supporting. 
xMega(atxmega256a\d3) support usage of the external flash. Both MCU use SPI on port D to flash connect.
atmega1281, atmega128rfa1 support at25f2058 and at45db041.
atxmega256a3, atxmega256d3 support at45db041.
Workaround has been realized for the problem from xmega errata:
"Writing EEPROM or Flash while reading any of them will not work". 
1.7 - Functions for RF4CE OTAU have been added. It was added only for atmega1281 and atmega128rfa1 (only GCC).
1.8 - Start procedure was refactored, every interface try to scan activity 200 ms. Some interfaces 
are supported in the same time. IAR is not supported anymore.