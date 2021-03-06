/**********************************************************************
* sdcc_bsp.c
*
* SDCC(Secure Digital Card Controller) driver BSP.
*
* This file implements the SDCC driver BSP for the board in use.
*
* Copyright (c) 2008
* Qualcomm Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header:
//source/qcom/qct/core/storage/sdcc/rel/04/src/bsp/surf/6240/sdcc_bsp.c#1 $
$DateTime: 2008/11/12 10:46:30 $ $Author: vink $

when         who     what, where, why
----------------------------------------------------------------------
2008-10-31   vin     Initial version
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/

#include "sdcc_bsp.h"
#include "pm.h"
#include "tramp.h"
#include "gpio_int.h"
#include "gpio_1100.h"
#include "hw.h"

/*==============================================================================
================================================================================
                           S D C C    B S P
==============================================================================*/

/* Check if slot interrupt has been enabled*/
static boolean slot_int_enabled = FALSE;

/*=============================================================================
 * FUNCTION      sdcc_bsp_vdd_control
 *
 * DESCRIPTION   This function turns ON/OFF the Vdd source to the card in
 *               use.
 * 
 * PARAMETERS    [IN] state : Desired power state: on or off
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_vdd_control (sdcc_bsp_vdd_ctl_type state)
{
   if(SDCC_BSP_VDD_ON == state)
   {

   }
   else if (SDCC_BSP_VDD_OFF == state)
   {

   }
   else
   {
      return SDCC_BSP_ERR_INVALID_PARAM;
   }
   return SDCC_BSP_NO_ERROR;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_slot_interrupt_exists
 *
 * DESCRIPTION   This function returns TRUE if hardware supports detection
 *               of card removal / insertion.
 *
 * PARAMETERS    None
 *
 * RETURN VALUE  TRUE if supported, FALSE otherwise
 *
 *===========================================================================*/
boolean
sdcc_bsp_slot_interrupt_exists()
{
   return (qsc11x0_hw_rev.pmic_hw_version >= 3) ? TRUE : FALSE;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_enable_slot_int
 *
 * DESCRIPTION   Setup the mechanism for detecting the card insertion/removal
 *               and associate an ISR to be called when a card is inserted
 *               or removed.
 *
 * PARAMETERS    [IN]: Pointer to the ISR function
 *
 * RETURN VALUE  None
 *
 *===========================================================================*/
void
sdcc_bsp_enable_slot_int(void *isr)
{
   if(FALSE == slot_int_enabled)
   {
      if( NULL == isr)
         return;
#ifndef CUST_EDITION
      gpio_tlmm_config(GPIO_INPUT_32);
      gpio_int_set_detect(GPIO_INT_32, DETECT_EDGE);
      gpio_int_set_handler(GPIO_INT_32, ACTIVE_LOW,
                           (gpio_int_handler_type)isr);
#endif
      slot_int_enabled = TRUE;
   }   
} /* sdcc_bsp_enable_slot_int */

