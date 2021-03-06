#ifndef HAL_HSUSB_IMPL_LEGACY
#define HAL_HSUSB_IMPL_LEGACY
/*
===========================================================================

FILE:         HAL_hsusb_impl_Legacy.h

DESCRIPTION:  
This is the external interface for the FS-USB Legacy core implementation.

===========================================================================

===========================================================================
Copyright � 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_Default.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImplLegacy(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_IMPL_LEGACY */
