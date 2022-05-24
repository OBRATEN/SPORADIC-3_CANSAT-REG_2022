.include "tn2313def.inc"

.def temp = r16
.def calc = r17
.def tmr  = r18
.def cmpr = r19

.dseg

.cseg

.org 0x0000

_main:
	ldi temp, 0b00000101
	out DDRB, temp

	ldi temp, 0x1E
	out TCNT1H, temp
	ldi temp, 0x14
	out TCNT1L, temp
	ldi temp, 0
	out TCCR1A, temp
	ldi temp, 0b00001010
	out TCCR1B, temp

	rjmp _cycle

_tmradd:
	ldi temp, 0x00
	out TIFR, temp
	inc tmr
	ret

_startOut:
	clr tmr
	ldi temp, 0b00000001
	out PORTB, temp
	inc cmpr
	ret

_stopOut:
	clr cmpr
	ldi temp, 0x00
	out PORTB, temp
	ret

_reset:
	ldi temp, 0b00000100
	out PORTB, temp
	ret

_cycle:
	wdr
	inc cmpr
	in temp, TIFR
	sbrs temp, 6
	rcall _tmradd
	in temp, PORTB
	sbrs temp, 1
	rcall _startOut
	cpi cmpr, 0x10
	rcall _stopOut
	cpi calc, 0x3
	rcall _reset
	rjmp _cycle
