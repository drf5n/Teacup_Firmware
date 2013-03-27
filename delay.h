#ifndef	_DELAY_H
#define	_DELAY_H

#include	<stdint.h>
#if defined (__AVR__)
#include	<util/delay_basic.h> // AVR: /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/avr/include/util/delay_basic.h
#else  // ARM / teensy 3.0
#endif
#include	"watchdog.h"

#define		WAITING_DELAY		100

#if F_CPU < 4000000UL
#error Delay functions only work with F_CPU >= 4000000UL 
#endif

// microsecond delay, does NOT reset WDT if feature enabled
void delay_us(uint16_t delay);

// microsecond delay, does reset WDT if feature enabled
void _delay(uint32_t delay);

// millisecond delay, does reset WDT if feature enabled
void _delay_ms(uint32_t delay);


// microsecond timer, does reset WDT if feature enabled
// 0 results in no real delay, but the watchdog
// reset is called if the feature is enabled
static void delay_us_w(uint32_t) __attribute__ ((always_inline));
inline void delay_us_w(uint32_t d) {
	if (d > (65536L / (F_CPU / 4000000L))) {
		_delay(d);
	}
	else {
		wd_reset();
		if( d ) {
		  //			_delay_loop_2(d * (F_CPU / 4000000L));
		  delayMicroseconds(d);
			wd_reset();
		}
	}
}

// millisecond timer, does reset WDT if feature enabled
// 0 results in no real delay, but the watchdog
// reset is called if the feature is enabled
static void delay_ms(uint32_t) __attribute__ ((always_inline));
inline void delay_ms(uint32_t d) {
	if (d > 65)
		_delay_ms(d);
	else
		delay_us_w(d * 1000);
 }
#endif	/* _DELAY_H */
