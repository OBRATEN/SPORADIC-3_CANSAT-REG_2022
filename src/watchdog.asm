.include "tn13def.inc"

.def temp = r16
.def time = r17
.def flag = r18

.dseg

.eseg
teamname: .dw 0x7303 ; LOW+HIGH = "S3" = SPORADIC-3

.cseg

.org 0x0000
    rjmp _main
.org 0x0001
    rjmp INT0_V
.org 0x0003
    rjmp T0_OVF

_main:
    cli
    ldi temp, RAMEND
    out SPL, temp
    ldi temp, 0b00000110
    out DDRB, temp
    ldi temp, 0x00
    out PORTB, temp
    ldi temp, 0b00000101
    out TCCR0B, temp
    ldi temp, 0b00000010
    out TIMSK0, temp
    sei
    rjmp _cycle

_cycle:
    wdr
    cpi time, 0x14
    breq _reboot
    rjmp _cycle

_reboot:
    ldi temp, 0b00000100
    out DDRB, temp
    break
    rjmp _cycle

_clear:
    clr flag
    clr time
    ldi temp, 0b00000001
    out PORTB, temp
    ret

T0_OVF:
    push temp
    in temp, SREG
    push temp
    ldi temp, 0x00
    out TCNT0, temp
    inc time
    in temp, PORTB
    andi temp, 0b00000001
    out PORTB, temp
    tst flag
    rcall _clear
    pop temp
    out SREG, temp
    pop temp
    reti

INT0_V:
    push temp
    in temp, SREG
    push temp
    ser flag
    pop temp
    out SREG, temp
    pop temp
    reti