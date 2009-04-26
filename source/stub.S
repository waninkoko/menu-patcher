	.rodata
	.globl bgData
	.balign 32

bgData:
	.incbin "../data/background"


#include "asm.h"

	.text
	.globl unstub_start

unstub_start:
	isync

	lis r3, 0
	ori r3, r3, 0x3400
	mtsrr0 r3
	isync

	mfmsr r3

	li r4, 0x30
	andc r3, r3, r4

	mtsrr1 r3
	isync

	rfi
