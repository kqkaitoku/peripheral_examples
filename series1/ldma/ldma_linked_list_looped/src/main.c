/**************************************************************************//**
 * @file
 * @brief LDMA Linked List Looped Example
 * @version 1.0.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_ldma.h"

/* DMA channel used for the examples */
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        1 << LDMA_CHANNEL

/* Memory to memory transfer buffer size */
#define BUFFER_SIZE         4

/* Number of iterations of A and B. */
#define NUM_ITERATIONS      4

/* Constant for loop transfer */
/* NUM_SETS - 1 (for first iteration) */
#define LOOP_COUNT          NUM_ITERATIONS - 1

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink[3];

/* Buffer for memory to memory transfer */
uint8_t dstBuffer[BUFFER_SIZE];

uint8_t srcA[BUFFER_SIZE] = "AAaa";
uint8_t srcB[BUFFER_SIZE] = "BBbb";
uint8_t srcC[BUFFER_SIZE] = "CCcc";

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending;

  /* Read interrupt source */
  pending = LDMA_IntGet();

  /* Clear interrupts */
  LDMA_IntClear(pending);

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR ){
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }

  /* Request next transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for descriptor list with looping
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  /* Initialize buffers for memory transfer */
  for (i = 0; i < BUFFER_SIZE; i++){
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Use looped peripheral transfer configuration macro */
  LDMA_TransferCfg_t periTransferTx =
      LDMA_TRANSFER_CFG_MEMORY_LOOP(LOOP_COUNT);

  /* LINK descriptor macros for looping, SINGLE descriptor macro for single transfer */
  descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcA, &dstBuffer, BUFFER_SIZE, 1);
  descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcB, &dstBuffer, BUFFER_SIZE, -1);
  descLink[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(&srcC, &dstBuffer, BUFFER_SIZE);

  /* Enable looping */
  descLink[1].xfer.decLoopCnt = 1;

  /* Enable interrupts */
  descLink[0].xfer.doneIfs = true;
  descLink[1].xfer.doneIfs = true;
  descLink[2].xfer.doneIfs = true;

  /* Disable automatic triggers */
  descLink[0].xfer.structReq = false;
  descLink[1].xfer.structReq = false;
  descLink[2].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);

  /* Request first transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator if available */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  /* Initialize LDMA */
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
