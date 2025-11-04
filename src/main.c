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

void myADC_Init(void);
void myDAC_Init(void);
void myGPIOA_Init(void);
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
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_PLL;
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
    myADC_Init();		/* Initialize ADC */
    myDAC_Init();		/* Initialize DAC */
    myGPIOA_Init();		/* Initialize I/O port PA */
	myGPIOB_Init();		/* Initialize I/O port PB */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */


	while (1)
	{
		// Nothing is going on here...


	}

	return 0;

}

// CONFIGURE ADC AND DAC PROPERLY

void myADC_Init() {
	/* Enable clock for ADC */
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
}

void myDAC_Init() {
	/* Enable clock for DAC */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
}

void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);

	/* Configure PA4 as put */
	GPIOA->MODER |= 0x300;

	/* Configure PA1 as put */
	GPIOA->MODER |= 0xC;

	/* Set PA0 to pull-down*/
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0;
}

void myGPIOB_Init()
{
	/* Enable clock for GPIOB peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB2 as input */
	GPIOB->MODER &= ~(GPIO_MODER_MODER2);

	/* Configure PB3 as input */
	GPIOB->MODER &= ~(GPIO_MODER_MODER3);

	/* Ensure no pull-up/pull-down for PB2 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

	/* Ensure no pull-up/pull-down for PB3 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3);
}


void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 |= TIM_CR1_ARPE; // buffer auto reload enabled
	TIM2->CR1 &= ~(TIM_CR1_DIR); // count up
	TIM2->CR1 |= TIM_CR1_OPM; // stop on next update event
	TIM2->CR1 &= ~(TIM_CR1_UDIS); // enable update events
	TIM2->CR1 |= TIM_CR1_URS; // update interrupt on overflow only

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	TIM2->EGR |= TIM_EGR_UG;


	/* Assign TIM2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER |= TIM_DIER_TIE;

}


void myEXTI_Init()
{
	/* Map EXTI0 line to PA0 */
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA);

	/* Map EXTI2 line to PB2 */
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

	/* Map EXTI3 line to PB3 */
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PB);

	/* EXTI0 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR0;

	/* EXTI2 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR2;

	/* EXTI3 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR3;

	/* Unmask interrupts from EXTI0 line */
	EXTI->IMR |= EXTI_IMR_MR0;

	/* Unmask interrupts from EXTI2 line */
	EXTI->IMR |= EXTI_IMR_MR2;

	/* Mask interrupts from EXTI3 line */
	EXTI->IMR &= ~(EXTI_IMR_MR3);

	/* Assign EXTI0 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	/* Assign EXTI2 & EXTI3 interrupt priority = 1 in NVIC */
	NVIC_SetPriority(EXTI2_3_IRQn, 1);

	/* Enable EXTI0 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	/* Enable EXTI2 & EXTI3 interrupts in NVIC */
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
		TIM2->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}

void EXTI0_1_IRQHandler()
{
	/* Check if EXTI0 interrupt pending flag is set */
	if ((EXTI->PR & EXTI_PR_PR0) != 0)
		{
			/* Flip masking interrupts for EXTI2 and EXTI3 */
			EXTI->IMR ^= (EXTI_IMR_MR2 | EXTI_IMR_MR3);

			/* Clear flag */
			EXTI->PR = EXTI_PR_PR0;
		}
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...
	float timer;
	float frequency;

	/* Check if EXTI2 interrupt pending flag is set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{

		if (first_edge) {
			TIM2->CNT = 0x00;
			TIM2->CR1 |= TIM_CR1_CEN;
			first_edge = 0;
		} else {
			TIM2->CR1 &= ~(TIM_CR1_CEN);
			timer = TIM2->CNT;

			frequency = 1/(timer / 48000000);

			trace_printf("Signal Frequency: %.3f Hz\n", frequency);
			first_edge = 1;
		}
		EXTI->PR = EXTI_PR_PR2;
	}

	/* Check if EXTI3 interrupt pending flag is set */
	if ((EXTI->PR & EXTI_PR_PR3) != 0)
	{
		if (first_edge) {
			TIM2->CNT = 0x00;
			TIM2->CR1 |= TIM_CR1_CEN;
			first_edge = 0;
		} else {
			TIM2->CR1 &= ~(TIM_CR1_CEN);
			timer = TIM2->CNT;

			frequency = 1/(timer / 48000000);

			trace_printf("Signal Frequency: %.3f Hz\n", frequency);
			first_edge = 1;
		}
		EXTI->PR = EXTI_PR_PR3;
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
