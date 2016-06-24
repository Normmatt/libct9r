/*---------------------------------------------------------------------------------

	Interrupt registers and vector pointers
	
	Copyright (C) 2016 - Modified from libnds
		Normmatt

	Copyright (C) 2005
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.


---------------------------------------------------------------------------------*/

/*! \file interrupts.h
    \brief ctr9 interrupt support.
*/

#ifndef CTR_INTERRUPTS_INCLUDE
#define CTR_INTERRUPTS_INCLUDE

#include <ctr/ctrtypes.h>
#include <ctr/ipc.h>


//! values allowed for REG_IE and REG_IF
enum IRQ_MASKS {
	IRQ_DMAC1_0			=	BIT(0),		/*!< DMAC1 interrupt mask 0 */
	IRQ_DMAC1_1			=	BIT(1),		/*!< DMAC1 interrupt mask 1 */
	IRQ_DMAC1_2			=	BIT(2),		/*!< DMAC1 interrupt mask 2 */
	IRQ_DMAC1_3			=	BIT(3),		/*!< DMAC1 interrupt mask 3 */
	IRQ_DMAC1_4			=	BIT(4),		/*!< DMAC1 interrupt mask 4 */
	IRQ_DMAC1_5			=	BIT(5),		/*!< DMAC1 interrupt mask 5 */
	IRQ_DMAC1_6			=	BIT(6),		/*!< DMAC1 interrupt mask 6 */
	IRQ_DMAC1_7			=	BIT(7),		/*!< DMAC1 interrupt mask 7 */
	IRQ_TIMER0			=	BIT(8),		/*!< Timer 0 interrupt mask */
	IRQ_TIMER1			=	BIT(9),		/*!< Timer 1 interrupt mask */
	IRQ_TIMER2			=	BIT(10),	/*!< Timer 2 interrupt mask */
	IRQ_TIMER3			=	BIT(11),	/*!< Timer 3 interrupt mask */
	IRQ_IPC_SYNC		=	BIT(12),	/*!< IPC sync interrupt mask */
	IRQ_FIFO_EMPTY		=	BIT(13),	/*!< Send FIFO empty interrupt mask */
	IRQ_FIFO_NOT_EMPTY	=	BIT(14),	/*!< Receive FIFO not empty interrupt mask */
	IRQ_AES				=	BIT(15),	/*!< AES interrupt mask */
	IRQ_SDMMC			= 	BIT(16),	/*!< SDMMC interrupt mask */
	IRQ_RSA				= 	BIT(22),	/*!< RSA interrupt mask */
	IRQ_CTRCARD1		=	BIT(23),	/*!< CTR Card Slot 1 interrupt mask */
	IRQ_CTRCARD2		=	BIT(24),	/*!< CTR Card Slot 2 interrupt mask */
	IRQ_CGC				=	BIT(25),	/*!< CGC interrupt mask */
	IRQ_CGC_DET			=	BIT(26),	/*!< CGC_DET interrupt mask */
	IRQ_DSCARD			=	BIT(27),	/*!< DS Card Slot interrupt mask */
	IRQ_ALL				=	(~0)		/*!< 'mask' for all interrupt */
};

typedef enum IRQ_MASKS IRQ_MASK;

/*!
	\brief returns the mask for a given timer.

	\param n the timer.

	\return the bitmask.
*/
#define IRQ_TIMER(n) (1 << ((n) + 8))

//! maximum number of interrupts.
#define MAX_INTERRUPTS  25



/*! \def REG_IE

    \brief Interrupt Enable Register.

	This is the activation mask for the internal interrupts.  Unless
	the corresponding bit is set, the IRQ will be masked out.
*/
#define REG_IE	(*(vuint32*)0x10001000)

/*! \def REG_IF

    \brief Interrupt Flag Register.

	Since there is only one hardware interrupt vector, the IF register
	contains flags to indicate when a particular of interrupt has occured.
	To acknowledge processing interrupts, set IF to the value of the
	interrupt handled.

*/
#define REG_IF	(*(vuint32*)0x10001004)
	
/*! \def REG_IME

    \brief Interrupt Master Enable Register.

	When bit 0 is clear, all interrupts are masked.  When it is 1,
	interrupts will occur if not masked out in REG_IE.

*/
#define REG_IME	(*(vuint32*)0x10001008)


//! values allowed for REG_IME
enum IME_VALUE {
	IME_DISABLE = 0, 	/*!< Disable all interrupts. */
	IME_ENABLE = 1,		/*!< Enable all interrupts not masked out in REG_IE */
};

#ifdef __cplusplus
extern "C" {
#endif


extern VoidFn	__irq_vector[];
extern	vuint32	__irq_flags[];
extern	vuint32	__irq_flagsaux[];

#define INTR_WAIT_FLAGS  *(__irq_flags)
#define INTR_WAIT_FLAGSAUX  *(__irq_flagsaux)
#define IRQ_HANDLER             *(__irq_vector)

struct IntTable{IntFn handler; u32 mask;};

/*! \fn irqInit()
	\brief Initialise the libnds interrupt system.

	This function is called internally (prior to main()) to set up irqs
    on the ARM9.

*/
void irqInit();

/*! \fn irqSet(u32 irq, VoidFn handler)
	\brief Add a handler for the given interrupt mask.

	Specify the handler to use for the given interrupt. This only works with
	the default interrupt handler, do not mix the use of this routine with a
	user-installed IRQ handler.
	\param irq Mask associated with the interrupt.
	\param handler Address of the function to use as an interrupt service routine
	\note

	\warning Only one IRQ_MASK can be specified with this function.
*/
void irqSet(u32 irq, VoidFn handler);

/*! \fn irqClear(u32 irq)
	\brief remove the handler associated with the interrupt mask irq.
	\param irq Mask associated with the interrupt.
*/
void irqClear(u32 irq);

/*! \fn irqInitHandler(VoidFn handler)
	\brief Install a user interrupt dispatcher.

	This function installs the main interrupt function, all interrupts are serviced through this routine. For most
	purposes the libnds interrupt dispacther should be used in preference to user code unless you know *exactly* what you're doing.

	\param handler Address of the function to use as an interrupt dispatcher
	\note the function *must* be ARM code
*/
void irqInitHandler(VoidFn handler);

/*! \fn irqEnable(u32 irq)
	\brief Allow the given interrupt to occur.
	\param irq The set of interrupt masks to enable.
	\note Specify multiple interrupts to enable by ORing several IRQ_MASKS.
*/
void irqEnable(u32 irq);

/*! \fn irqDisable(u32 irq)
	\brief Prevent the given interrupt from occuring.
	\param irq The set of interrupt masks to disable.
	\note Specify multiple interrupts to disable by ORing several IRQ_MASKS.
*/
void irqDisable(u32 irq);

/*! \fn IntrWait(u32 waitForSet, uint32 flags)

	\brief wait for interrupt(s) to occur

	\param waitForSet
		0: Return if the interrupt has already occured
		1: Wait until the interrupt has been set since the call
	\param flags
		interrupt mask to wait for

*/
void IntrWait(u32 waitForSet, uint32 flags);

static inline int enterCriticalSection() {
	int oldIME = REG_IME;
	REG_IME = 0;
	return oldIME;
	
	//TODO: Implement this using CPSR I flag
	
	return 0;
}

static inline void leaveCriticalSection(int oldIME) {
	REG_IME = oldIME;
	
	//TODO: Implement this using CPSR I flag
}


#ifdef __cplusplus
}
#endif

#endif //CTR_INTERRUPTS_INCLUDE

