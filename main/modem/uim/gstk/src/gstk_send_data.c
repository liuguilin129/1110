/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   G E T   C H A N N E L   S T A T U S


GENERAL DESCRIPTION
  This source file contains functions to process select item
  proactive command


FUNCTIONS
  gstk_send_data_req
    This function parses the Send Data Command from the card
    and sends it to the client.

  gstk_send_data_cnf
    This function packs the Send Data terminal response and sends
    the command to UIM server.

  gstk_send_data_cnf_cleanup
    This function cleans up any allocated pointer on the stack.



INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_send_data.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
05/06/05   sk      Fixed Send Channel Data devide identity bug
03/05/05   tml     Added tag comprehension support
11/04/04   sk/tml  Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Task Definition Headers */
#include "tmc.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"


/*===========================================================================
FUNCTION gstk_send_data_cnf_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  send_data_cnf_ptr: [Input] Pointer to message that is required to
                             be processed and sent to UIM
  STK_response_ptr : [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_send_data_cnf_cleanup(
  gstk_send_data_cnf_type          *send_data_cnf_ptr,
  send_data_terminal_response_type *STK_response_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  if(send_data_cnf_ptr == NULL)
  {
    MSG_ERROR("send_data_cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free send data tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)send_data_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info = NULL;
  }
} /* gstk_send_data_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_data_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request_ptr : [Input] Pointer to gstk_cmd_from_card_type
  cmd_data_ptr: [Input] Pointer to cmd_data_ptr

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_send_data_req_cleanup(
  gstk_cmd_from_card_type *request_ptr, 
  uint8                   *cmd_data_ptr)
{
  if(request_ptr == NULL)
  {
    MSG_ERROR("request_ptr ERR:NULL",0,0,0);
    return;
  }
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_send_data_req_cleanup */

/*===========================================================================
FUNCTION gstk_send_data_parsing_err_term_rsp

DESCRIPTION
  This function send terminal response as a result of parsing error.

PARAMETERS
  cmd_details_ref_id : [Input]  command details reference id
  result             : [Input] GSTK result
  additional_info_ptr: [Input] Pointer to addtional info

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_send_data_parsing_err_term_rsp(
  uint32                              cmd_details_ref_id,
  gstk_general_result_enum_type       result,
  const gstk_additional_info_type     *additional_info_ptr)
{
  gstk_status_enum_type               gstk_status       = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param = {0};

  ERR("Send Data TLV parsing error", 0, 0, 0);

  /* Send Terminal Response to Card */
  /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID,     /* client_id */
                                            0,                  /* user_data */
                                            cmd_details_ref_id, /* command_details_ref */
                                            command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                            GSTK_SEND_DATA_CNF,   /* command response */
                                            result, /* general_result */
                                            additional_info_ptr,   /* additional result */
                                            &dummy_extra_param  /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    MSG_ERROR("TR fail, %d",gstk_status,0,0);
  }    
} /* gstk_send_data_parsing_err_term_rsp */

/*===========================================================================
FUNCTION gstk_send_data_req

DESCRIPTION
  This function parses the Send Data Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_data_req (
  gstk_proactive_cmd_type *command_ptr
)
{
  gstk_status_enum_type               gstk_status         = GSTK_SUCCESS;
  uint8                               *cmd_data_ptr       = NULL;
  int32                               cmd_data_len        = 0;
  uint32                              cmd_details_ref_id  = 0;
  int32                               curr_offset         = 0;
  int32                               new_offset          = 0;
  boolean                             more_tlv            = FALSE;
  gstk_additional_info_type           additional_info     = {0};
  boolean                             is_valid_ch_id      = TRUE;

  /* Cannot do {0x00} in declaration because of compilation warning 
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  MSG_HIGH(" IN GSTK_SEND_DATA_REQ",0,0,0);

  if(!command_ptr)
  {
    MSG_ERROR("command_ptr is NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize request to 0 */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  if(gstk_status != GSTK_SUCCESS)
  {
    if (gstk_status == GSTK_INVALID_DEVICE_COMMAND) {
      additional_info.length = 1;
      additional_info.additional_info[0] = 0x03;  /* Channel Identifier not valid, can't use enum because
                                                    there are multiple meaning for value 0x03 */
      gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                          GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                          &additional_info);
    }
    else {
      gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    }
    /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
    gstk_send_data_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
    
  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_SEND_DATA_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
      command_details_ref_table[cmd_details_ref_id].command_details.command_number;
    
  /* channel id */
  is_valid_ch_id = gstk_util_is_valid_channel_id_range(command_details_ref_table[cmd_details_ref_id].destination_dev_id);
  if ( is_valid_ch_id != TRUE) {
    additional_info.length = 1;
    additional_info.additional_info[0] = 0x03;  /* Channel Identifier not valid, can't use enum because
                                                   there are multiple meaning for value 0x03 */
    gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                        GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                        &additional_info);
    /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
    gstk_send_data_req_cleanup(&request, cmd_data_ptr);
    return GSTK_INVALID_COMMAND;
  }

  request.cmd.send_data_pro_cmd_req.ch_id = (int)command_details_ref_table[cmd_details_ref_id].destination_dev_id 
                                      - GSTK_CHANNEL_ID_BASE;

  /* send immediately */
  /* Command details, qualifier tells us the priority */
  if(command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x01)
  {
    request.cmd.send_data_pro_cmd_req.send_data_immediately = TRUE;
    MSG_MED(" .... Send Data Immediately", 0, 0, 0);    
  }
  else
  {
    request.cmd.send_data_pro_cmd_req.send_data_immediately = FALSE;
    MSG_MED(" .... Send Data Store in TX", 0, 0, 0);
  }

  /* optional alpha TLV */
  gstk_status = gstk_parser_alpha_identifier_tlv_ext( GSTK_ALPHA_IDENTIFIER_TAG,
                                                      cmd_data_ptr,
                                                      cmd_data_len,
                                                      curr_offset,
                                                      &(request.cmd.send_data_pro_cmd_req.alpha),
                                                      &new_offset,
                                                      &more_tlv,
                                                      OPTIONAL,
                                                      &cmd_details_ref_id);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
    gstk_send_data_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  else {
    if (!more_tlv) {
      gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                          GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                          &additional_info);
      /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
      gstk_send_data_req_cleanup(&request, cmd_data_ptr);
      return GSTK_INVALID_COMMAND;
    }
    curr_offset = new_offset;
  }
      
  /* optional ICON TLV */
  if(gstk_valid_tag(cmd_data_ptr[curr_offset],GSTK_ICON_IDENTIFIER_TAG)) {
    /* there is icon TLV */
    /* Icon ID */
    gstk_status = gstk_parser_icon_id_tlv_ext(
        &(request.cmd.send_data_pro_cmd_req.alpha),
        cmd_data_ptr,
        cmd_data_len,
        curr_offset,
        &(request.cmd.send_data_pro_cmd_req.icon),
        &new_offset,
        &more_tlv,
        OPTIONAL,
        &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS) {
      gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                          GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                          &additional_info);
      /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
      gstk_send_data_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    else {
      if (!more_tlv) {
        gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                            GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                            &additional_info);
        /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
        gstk_send_data_req_cleanup(&request, cmd_data_ptr);
        return GSTK_INVALID_COMMAND;
      }
      curr_offset = new_offset;
    }
  } /* optional icon tlv */

  /* Continue with Channel data command parsing: Mandatory */
  gstk_status = gstk_parser_ch_data_tlv(cmd_data_ptr,
                                        cmd_data_len,
                                        curr_offset,
                                        &(request.cmd.send_data_pro_cmd_req.ch_data),
                                        &new_offset,
                                        &more_tlv,
                                        MANDATORY,
                                        &cmd_details_ref_id);

  if(gstk_status != GSTK_SUCCESS) {
    gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id,
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
    gstk_send_data_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  else {
    curr_offset = new_offset;
  }

  if (more_tlv) {
    /* There are more tlvs after the last spec specified tlv, ignore */
    MSG_HIGH("Card Provide more TLV after final TLV element, Ignore", 0, 0, 0);
  }

  /* Send command to clients */
  gstk_status = gstk_util_send_message_to_clients(&request);

  if(gstk_status != GSTK_SUCCESS)
  {
    /* Send Terminal Response to Card */
    /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
    MSG_HIGH("Display Text not able to send to client", 0, 0, 0);

    /* Send Terminal Response to Card */
    gstk_set_additional_info(&additional_info,GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

    gstk_send_data_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                        &additional_info);
  }
  else
  {
    /* Start Timer */
    gstk_util_start_client_response_timer(cmd_details_ref_id);
    /* reset gstk_status to SUCCESS since there has been no error*/
    /* This is needed since some of the TLVs are optional, which might
    ** cause the various parsing functions to return invalid_command */
    gstk_status = GSTK_SUCCESS;
  }
  
  /* release memory allocation for send data request info data and cmd_data_ptr accordingly */
  gstk_send_data_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_send_data_req */

/*===========================================================================
FUNCTION gstk_send_data_cnf

DESCRIPTION
  This function packs the get channel status terminal response and sends
  the command to UIM server.

PARAMETERS
  send_data_cnf_ptr: [Input] Pointer to message that is required to
                            be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_data_cnf (
  gstk_send_data_cnf_type* send_data_cnf_ptr
)
{
  gstk_status_enum_type                  gstk_status   = GSTK_SUCCESS;
  send_data_terminal_response_type       STK_response  = {0};
  uim_cmd_type                           *uim_cmd_ptr  = NULL;
  int                                    offset        = 0;
  int                                    i             = 0;
  uint32                                 ref_id        = 0;

  MSG_HIGH("IN GSTK send data cnf ",0,0,0);

  if(send_data_cnf_ptr == NULL)
  {
    MSG_ERROR("send_data_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(send_data_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
     send_data_cnf_ptr->cmd_details_ref_id,
     send_data_cnf_ptr->message_header.command_id,
     send_data_cnf_ptr->command_number,
     &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) { 
    gstk_send_data_cnf_cleanup(send_data_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_data_cnf_cleanup(send_data_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
     send_data_cnf_ptr->command_result,
     send_data_cnf_ptr->result_additional_info,
     &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_data_cnf_cleanup(send_data_cnf_ptr, &STK_response);
    return gstk_status;
  }

  if (send_data_cnf_ptr->data_present) {
    /* Channel data length */  
    gstk_status = gstk_packer_ch_data_len_tlv(
                send_data_cnf_ptr->ch_data_remain_buf_len,
                &STK_response.ch_data_len);
    if (gstk_status != GSTK_SUCCESS) {
      gstk_send_data_cnf_cleanup(send_data_cnf_ptr, &STK_response);
      return gstk_status;
    }
  }

  /* total header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                            STK_response.header.device_id.device_tag_length +
                            STK_response.header.result.result_tag_length + 6;

  if (send_data_cnf_ptr->data_present) {
    STK_response.num_bytes = STK_response.num_bytes +
                             STK_response.ch_data_len.ch_data_len_length +
                             2;
  }


  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info( UIM_TERMINAL_RESPONSE_F,
                                                        &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS)
    {
      if (uim_cmd_ptr != NULL) {
        ref_id = send_data_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for clienr response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
           the card */
        gstk_util_cmd_details_ref_table_free_slot(send_data_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        memcpy(uim_cmd_ptr->terminal_response.data,
               &STK_response.header.command_details,
               (int)(STK_response.header.command_details.length + 2));
        offset = (int)(offset + STK_response.header.command_details.length + 2);

        /* copy device */
        memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
               &STK_response.header.device_id,
               (int)(STK_response.header.device_id.device_tag_length + 2));
        offset = (int)(offset + STK_response.header.device_id.device_tag_length + 2);

        /* copy result */
        memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
               &STK_response.header.result,
               3); /* result tag +  length + general result */

        offset = offset + 3;

        if ( STK_response.header.result.additional_info != NULL )
        {
          /* copy Additional Information */
          memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                 STK_response.header.result.additional_info,
                 (int)(STK_response.header.result.result_tag_length - 1));
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* Need to add channel status */
        if (send_data_cnf_ptr->data_present) {
          /* copy Channel data length */
          memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
                 &STK_response.ch_data_len,
                 (int)(STK_response.ch_data_len.ch_data_len_length + 2));
          offset = (int)(offset + STK_response.ch_data_len.ch_data_len_length + 2);
        }

        MSG_MED("Send Data: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
        for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
          MSG_MED("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
        }

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
      else {
        MSG_MED("Never here", 0, 0, 0);
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    MSG_ERROR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_send_data_cnf_cleanup(send_data_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_send_data_cnf */

