/**
 * Created: 02.04.2024
 * Author: Lekhtin A.V.
 */

#ifndef HW__H_
#define HW__H_

#include <ina90.h>
#include <ioavr.h>          // __ATmega328P__

#define F_CPU 16000000

#define OUT1 PORTD_PORTD7
#define OUT2 PORTD_PORTD6
#define LED  PORTD_PORTD5

#define _1us (F_CPU / 1000000)
#define _1ms (F_CPU / 1000)

#define delay_1us()    __delay_cycles(_1us)
#define delay_3us()    __delay_cycles(_1us*3)
#define delay_5us()    __delay_cycles(_1us*5)
#define delay_10us()   __delay_cycles(_1us*10)
#define delay_12us()   __delay_cycles(_1us*12)
#define delay_55us()   __delay_cycles(_1us*55)
#define delay_53us()   __delay_cycles(_1us*53)
#define delay_65us()   __delay_cycles(_1us*65)
#define delay_250us()  __delay_cycles(_1us*250)
#define delay_500us()  __delay_cycles(_1us*500)

#define delay_1ms()    __delay_cycles(_1ms)
#define delay_2ms()    __delay_cycles(_1ms*2)
#define delay_10ms()   __delay_cycles(_1ms*10)
#define delay_20ms()   __delay_cycles(_1ms*20)
#define delay_100ms()  __delay_cycles(_1ms*100)
#define delay_200ms()  __delay_cycles(_1ms*200)
#define delay_400ms()  __delay_cycles(_1ms*400)
#define delay_800ms()  __delay_cycles(_1ms*800)

#define delay_1000ms() __delay_cycles(_1ms*1000)
#define delay_2000ms() __delay_cycles(_1ms*2000)
#define delay_5000ms() __delay_cycles(_1ms*5000);

#endif // HW__H_
