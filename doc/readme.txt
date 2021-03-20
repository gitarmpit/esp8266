Big one: AI Thinker 
Small one: WROOM-02 

Pull up: EN, RST, GPIO0, GPIO2 
Pull down:  GPIO15 

To flash: Pull down GPIO0

Min current 300-500mA,  3.0V ~ 3.6V


==============
Boot message is output at 74880 baud, then the chip switches to 115200
AT commands run at 115200 (boot message will be garbage, read and ignore) 
  
==============
https://www.esp8266.com/viewtopic.php?p=83631
 ets Jan  8 2013,rst cause:2, boot mode:(3,3)
 jump to run user1

 boot mode (x,y): x=3 Flash boot (normal run)  x=1 UART download,  y=?? seems random
 rst cause=2 : external reset, seems to work ok after it 
 lots of resets and crashes due to poor contacts, week power supply 
 solder everything and power from USB which gives at least 500mA 
 may need a 0.1uF cap to ground on EN pin
==============
Reset causes: 
    1: Power reboot or normal boot
    2: External reset using reset pin or wake up from deep sleep
    3: Software reset
    4: Hardware watchdog (WDT) reset

boot device:
    0:
    1: ram
    3: flash
 
https://www.espressif.com/sites/default/files/documentation/0c-esp-wroom-02_datasheet_en.pdf

================
UART download? OTA via network? 

================
esp8266 SDK:  Non-OS SDK : AT commands etc, RTOS SDK

Terminal: CoolTerm


Documents: 
ESP8266 AT Instruction Set
ESP8266 AT Command Examples
ESP8266EX Frequently Asked Questions
https://www.espressif.com/sites/default/files/documentation/Espressif_FAQ_EN.pdf

user1.bin user2.bin
eagle.irom0.text.bin 

==============
flashing: esptool.py 


https://hackaday.com/2015/03/18/how-to-directly-program-an-inexpensive-esp8266-wifi-module/


https://github.com/nodemcu

https://arduino-esp8266.readthedocs.io/en/latest/

https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process

https://www.espressif.com/en/support/documents/technical-documents

ESP-07 ESP-12F ESP-12E NodeMCU
