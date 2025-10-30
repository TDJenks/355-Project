//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOB_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);


// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/

int first_edge = 1;

int main(int argc, char* argv[])
{

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;  /* Enable SYSCFG clock */

	myGPIOB_Init();		/* Initialize I/O port PB */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */


	while (1)
	{
		// Nothing is going on here...


	}

	return 0;

}


void myGPIOB_Init()
{
	/* Enable clock for GPIOB peripheral */
	// Relevant register: RCC->AHBENR
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB2 as input */
	// Relevant register: GPIOB->MODER
	GPIOB->MODER &= ~(GPIO_MODER_MODER2_Msk);

	/* Ensure no pull-up/pull-down for PB2 */
	// Relevant register: GPIOB->PUPDR
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR2_Msk);
}


void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	// Relevant register: TIM2->CR1
	TIM2->CR1 |= TIM_CR1_ARPE_Msk; // buffer auto reload enabled
	TIM2->CR1 &= ~(TIM_CR1_DIR_Msk); // count up
	TIM2->CR1 |= TIM_CR1_OPM_Msk; // stop on next update event
	TIM2->CR1 &= ~(TIM_CR1_UDIS_Msk); // enable update events
	TIM2->CR1 |= TIM_CR1_URS_Msk; // update interrupt on overflow only

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM2->EGR |= TIM_EGR_UG_Msk;

	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
	TIM2->DIER |= TIM_DIER_TIE_Msk;
}


void myEXTI_Init()
{
	/* Map EXTI2 line to PB2 */
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

	/* EXTI2 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2_Msk;

	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	EXTI->IMR |= EXTI_IMR_MR2_Msk;

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(EXTI2_3_IRQn);

}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR
		TIM2->SR &= ~(TIM_SR_UIF_Msk);

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN_Msk;
	}
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...
	float timer;
	float period;
	float frequency;

	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{

		if (first_edge) {
			TIM2->CNT = 0x00;
			TIM2->CR1 |= TIM_CR1_CEN_Msk;
			first_edge = 0;
		} else {
			TIM2->CR1 &= ~(TIM_CR1_CEN_Msk);
			timer = TIM2->CNT;

			period = timer / 48000000;
			frequency = 1/(timer / 48000000);

			trace_printf("Signal Period: %.3f seconds\n", period);
			trace_printf("Signal Frequency: %.3f Hz\n", frequency);
			first_edge = 1;
		}
		EXTI->PR = EXTI_PR_PR2;

		//
		// 1. If this is the first edge:
		//	- Clear count register (TIM2->CNT).
		//	- Start timer (TIM2->CR1).
		//    Else (this is the second edge):
		//	- Stop timer (TIM2->CR1).
		//	- Read out count register (TIM2->CNT).
		//	- Calculate signal period and frequency.
		//	- Print calculated values to the console.
		//	  NOTE: Function trace_printf does not work
		//	  with floating-point numbers: you must use
		//	  "unsigned int" type to print your signal
		//	  period and frequency.
		//
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		//
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
