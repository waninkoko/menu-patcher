#ifndef _ASM_H_
#define _ASM_H_

/* Registers */
#define	r0	0
#define	r1	1
#define	r2	2
#define	r3	3
#define	r4	4
#define	r5	5
#define	r6	6
#define	r7	7
#define	r8	8
#define	r9	9
#define	r10	10
#define	r11	11
#define	r12	12
#define	r13	13
#define	r14	14
#define	r15	15
#define	r16	16
#define	r17	17
#define	r18	18
#define	r19	19
#define	r20	20
#define	r21	21
#define	r22	22
#define	r23	23
#define	r24	24
#define	r25	25
#define	r26	26
#define	r27	27
#define	r28	28
#define	r29	29
#define	r30	30
#define	r31	31 


/* BATs */
#define	IBAT0U		528
#define	IBAT0L		529	
#define	IBAT1U		530	
#define	IBAT1L		531	
#define	IBAT2U		532	
#define	IBAT2L		533	
#define	IBAT3U		534	
#define	IBAT3L		535	
#define	IBAT4U		560
#define	IBAT4L		561
#define	IBAT5U		562
#define	IBAT5L		563
#define	IBAT6U		564
#define	IBAT6L		565
#define	IBAT7U		566
#define	IBAT7L		567

#define	DBAT0U		536
#define	DBAT0L		537	
#define	DBAT1U		538	
#define	DBAT1L		539	
#define	DBAT2U		540	
#define	DBAT2L		541	
#define	DBAT3U		542	
#define	DBAT3L		543
#define DBAT4U		568
#define DBAT4L		569
#define DBAT5U		570
#define DBAT5L		571
#define DBAT6U		572
#define DBAT6L		573
#define DBAT7U		574
#define DBAT7L		575


/* HIDs */
#define HID0		1008
#define HID1		1009
#define HID2		920	
#define HID4		1011


/* MSRs */
#define MSR_RI		0x00000002
#define MSR_DR		0x00000010
#define MSR_IR		0x00000020
#define MSR_IP		0x00000040
#define MSR_SE		0x00000400
#define MSR_ME		0x00001000
#define MSR_FP		0x00002000
#define MSR_POW		0x00004000
#define MSR_EE		0x00008000

#endif
