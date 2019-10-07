#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/

#define MAXPROC 20
#ifndef MAXINT
#define MAXINT 0xEFFFFFFF
#endif
#define QUANTUM 5000     /* time for cpu job to run */
#define MAGICNUM 49      /* magic num is the number of devices we have (49) */ 

/* creating syscall processor state areas */
#define SYSCALLBREAKNEW 0x200003D4
#define SYSCALLBREAKOLD 0x20000348

/*creating program trap processor state areas */
#define PROGRAMTRAPNEW 0x200002BC
#define PROGRAMTRAPOLD 0x20000230

/*creating the TLB management state areas */
#define TLBMANAGEMENTNEW 0x200001A4
#define TLBMANAGEMENTOLD 0x20000118

/*creating the interrupt processor state areas */
#define INTERRUPTNEW 0x2000008C
#define INTERRUPTOLD 0x20000000

/* creating and defining status options */
#define ALLOFF 0x00000000     /*turn everything off */ 
#define IECON 0x00000001	  /*turn exernal interrupts current to be set by Status.IM */
#define IEPON 0x00000004	  /*set interrupts previous on*/
#define IEPOFF 0xFFFFFFFB	  /*set interrupts previous off */
#define KERPON 0xFFFFFFF7	  /*set kernal previous mode on */
#define KERPOFF 0x00000008	  /*set kernal previous mode off and now in user mode */	
#define IMASKON 0x0000FF00	  /*turn interrupt mask on */
#define IMASKOFF 0xFFFF00FF       /*turn interrupt mask off */
#define VMPON 0x02000000	  /*turn virtual memory previous on */
#define VMPOFF 0xFDFFFFFF	  /*turn virtual memory previous off*/
#define TEON 0x08000000		  /*turn local timer on */
#define TEOFF 0xF7FFFFFF	  /*turn local timer off */


/* Hardware & software constants */
#define PAGESIZE		4096	/* page size in bytes */
#define WORDLEN			4		/* word size in bytes */
#define PTEMAGICNO		0x2A


#define ROMPAGESTART	0x20000000	 /* ROM Reserved Page */


/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR	0x10000000
#define TODLOADDR	0x1000001C
#define INTERVALTMR	0x10000020	
#define TIMESCALEADDR	0x10000024

/*Exception Calls - Phase 2*/
#define CREATEPROC      1
#define TERMINATEPROC   2
#define VERHOGEN        3
#define PASSERN         4
#define SPECTRAPVEC     5
#define GETCPUTIME      6
#define WAITCLOCK       7
#define WAITIO          8

/* utility constants */
#define	TRUE		1
#define	FALSE		0
#define ON              1
#define OFF             0
#define HIDDEN		static
#define EOS		'\0'

#define NULL ((void *)0xFFFFFFFF)


/* vectors number and type */
#define VECTSNUM	4

#define TLBTRAP		0
#define PROGTRAP	1
#define SYSTRAP		2

#define TRAPTYPES	3


/* device interrupts */
#define DISKINT		3
#define TAPEINT 	4
#define NETWINT 	5
#define PRNTINT 	6
#define TERMINT		7

#define DEVREGLEN	4	/* device register field length in bytes & regs per dev */
#define DEVREGSIZE	16 	/* device register size in bytes */

/* device register field number for non-terminal devices */
#define STATUS		0
#define COMMAND		1
#define DATA0		2
#define DATA1		3

/* device register field number for terminal devices */
#define RECVSTATUS      0
#define RECVCOMMAND     1
#define TRANSTATUS      2
#define TRANCOMMAND     3


/* device common STATUS codes */
#define UNINSTALLED	0
#define READY		1
#define BUSY		3

/* device common COMMAND codes */
#define RESET		0
#define ACK		1

/* operations */
#define	MIN(A,B)	((A) < (B) ? A : B)
#define MAX(A,B)	((A) < (B) ? B : A)
#define	ALIGNED(A)	(((unsigned)A & 0x3) == 0)

/* Useful operations */
#define STCK(T) ((T) = ((* ((cpu_t *) TODLOADDR)) / (* ((cpu_t *) TIMESCALEADDR))))
#define LDIT(T)	((* ((cpu_t *) INTERVALTMR)) = (T) * (* ((cpu_t *) TIMESCALEADDR))) 


#endif
