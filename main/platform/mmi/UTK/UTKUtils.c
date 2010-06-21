/*==============================================================================
// 文件：
//        UTKUtils.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        
==============================================================================*/


/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "uim.h"
#include "uimtk.h"
#include "UTK_priv.h"
#include "OEMHeap.h"


/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define UIM_TK_VALUE_OFFSET         3
#define UI_UIM_STATUS_SIG UI_RUIM_SIG


/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
// OTA format message, for conversion 
static wms_OTA_message_type    cdma_ota;
static wms_tl_message_type     cdma_tl;

// client message, for conversion
static wms_client_message_s_type    client_message;

// Buffer for storing the device id and cmd details tag
static uim_tk_command_details_tlv_type      cmd_details_tlv;

// Buffer to store return data for packing and parsing BER TLV 
static uim_tk_tlv_type                      parsed_tlv_buf;

static uim_tk_parse_ber_tlv_return_type         parse_ber_status;
static uim_tk_pack_ber_tlv_return_type          pack_ber_status;
static uim_tk_proc_simple_tlv_return_type       parse_tlv_status;

static uim_rpt_type                 ui_uim_rpt_buf;

// Buffer to store setup menu command data
static  byte        setup_info_buf[UIM_MAX_CHARS];

// Buffer to store parse data
static  byte        utk_info_buf[UIM_MAX_CHARS];

/*==============================================================================
                                 函数声明
==============================================================================*/
static void send_UTK_simple_resp(uim_tk_general_result_type result_type);

extern uim_cmd_type *start_terminal_response_buffer(
  byte *packed_offset,              /* Pointer to offset for packing */
  uim_tk_tlv_type *parsed_tlv_buf,  /* Points to parsed TLV buffer */
  uim_tk_command_details_tlv_type *cmd_details /* Points to cmd details */
);

extern void proactive_cmd_response(uim_cmd_type *cmd_ptr);

extern void ui_cmd (ui_cmd_type *cmd_ptr);

/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数: 
    UTK_parse_proactive_command
       
说明: 
    本函数只分析原语命令数据的命令描述部分，并保存原始数据到文件级全局缓冲区中。
       
参数: 
    cmd_data [in]：指向命令数据的指针。
    cmd_len [in]：命令数据长度。
       
返回值:
    原语命令
       
备注:
    
    
==============================================================================*/
byte UTK_parse_proactive_command(byte *cmd_data, byte cmd_len)
{
    /* Offset for the different TLVs */
    byte offset = 0;  /* offset to raw data */
#if 0
    {
        int i;
        
        for (i=0; i<cmd_len;i++)
        {
            ERR("%i = %02x",i, cmd_data[i],0);
        }
        
    }
#endif    
    if (cmd_data[0] == UIM_TK_END_PROACTIVE_SESSION)
    {
        return cmd_data[0];
    }
    // 分析 BER-TLV 以确定 TLV 是否有效
    parse_ber_status = uim_tk_parse_ber_tlv (cmd_data, cmd_len);
    if ((parse_ber_status.status == UIM_TK_MESSAGE_IS_VALID) &&
        (parse_ber_status.tag == UIM_TK_PROACTIVE_SIM_COMMAND_TAG))
    {
        /* Parse cmd details tag */
        parse_tlv_status = uim_tk_parse_simple_tlv_from_ber_tlv (
                cmd_data, offset, &parsed_tlv_buf );
        offset+= parse_tlv_status.tlv_size;
        
        cmd_details_tlv = parsed_tlv_buf.cmd_details;

        switch (cmd_details_tlv.cmd_type)
        {
            case UIM_TK_SETUP_MENU:
                memcpy(setup_info_buf, cmd_data, cmd_len);
                setup_info_buf[0] = cmd_details_tlv.cmd_type;
                send_UTK_simple_resp(UIM_TK_CMD_PERFORMED_SUCCESSFULLY);
                break;
                
            case UIM_TK_SELECT_ITEM:
            case UIM_TK_DISPLAY_TEXT:
            case UIM_TK_GET_INPUT:
            case UIM_TK_CDMA_SEND_SHORT_MSG:
            case UIM_TK_SETUP_CALL:
            case UIM_TK_PLAY_TONE:
            case UIM_TK_REFRESH:
            default:
                memcpy(utk_info_buf, cmd_data, cmd_len);
                utk_info_buf[0] = cmd_details_tlv.cmd_type;
                break;
        }
    }

    return cmd_details_tlv.cmd_type;
}

/*==============================================================================
函数: 
    UTK_GetCmddata
       
说明: 
    本函数用来获取指向与命令相关的数据的地址。
       
参数: 
    cmd_type [In]: uim_tk_proactive_cmd_enum_type 。
       
返回值:
    byte *
       
备注:
    注意：返回数据的第一字节本应为主动式RUIM 命令标签，为便于使用，在函数
    parse_UTK_proactive_command 中将其改为 proactive command 值。其实知道
    proactive command 值后，主动式RUIM 命令标签就没多大意义了。

==============================================================================*/
byte * UTK_GetCmddata(byte cmd_type)
{
    if (cmd_type == UIM_TK_SETUP_MENU && cmd_type == setup_info_buf[0])
    {
        return setup_info_buf;
    }
    else if (cmd_type == utk_info_buf[0])
    {
        return utk_info_buf;
    }
    else
    {
        return NULL;
    }
}

/*==============================================================================
函数: 
    Pack_gw_7_bit_chars
       
说明: 
    本函数用来将AECHAR字符串序列编码为GSM 7 bit packed format 字节序列。
       
参数: 
    pwszIn [In]: 待编码AECHAR字符串 。
    shift [In]: 输出偏移bit位数,小于7。
    pdata [In]: 指向输出缓冲区 。
    nlen [In]: 输出缓冲区大小,单位为字节 。
    
       
返回值:
    编码后所用字节数
       
备注:
    对于不在 GsmToUcs2 表中的字符将以空格代替。

==============================================================================*/
int Pack_gw_7_bit_chars(AECHAR *pwszIn, uint16 shift, byte *pdata, int  nlen)
{
    int    i=0;
    int    pos = 0;
    int    inLen;
    byte   cTep;

    if ((NULL == pwszIn) || (NULL == pdata) || (nlen<1))
    {
        return 0;
    }
    
    inLen = WSTRLEN(pwszIn);
    /* pack the ASCII characters */
    shift %= 7;

    if (shift != 0)
    {
        /* Pack the upper bytes of the first character */
        if (pwszIn[i]>=GSM_CHARACTER_SET_SIZE)
        {
            cTep = 0x20;// ' '
        }
        else
        {
            cTep = GsmToUcs2[pwszIn[i]];
        }
        pdata[pos] |=  (cTep << shift);
        shift = (7 - shift) + 1;
        if (shift == 7)
        {
            shift = 0;
            i++;
        }
        pos++;
    }

    for( ; pos < nlen && i < inLen; pos++, i++ )
    {
        if (pwszIn[i]>=GSM_CHARACTER_SET_SIZE)
        {
            cTep = 0x20;// ' '
        }
        else
        {
            cTep = GsmToUcs2[pwszIn[i]];
        }

        /* pack the low bits */
        pdata[pos] = cTep >> shift;
        
        if( i+1 < inLen )
        {
            if (pwszIn[i+1]>=GSM_CHARACTER_SET_SIZE)
            {
                cTep = 0x20;// ' '
            }
            else
            {
                cTep = GsmToUcs2[pwszIn[i+1]];
            }
            
            /* pack the high bits using the low bits of the next character */
            pdata[pos] |= (uint8) (cTep << (7-shift));
            
            shift ++;
            
            if( shift == 7 )
            {
                shift = 0;
                i ++;
            }
        }
    }
    
    /* done */
    return pos;
} 

/*==============================================================================
函数: 
    UTK_GiveResponse
       
说明: 
    本函数用来响应对当前 proactive command 的处理结果。
       
参数: 
    pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息。
    eCmd [in]: uim_tk_proactive_cmd_enum_type
    bForwad[in]: 指示是否需要卡发出下一步命令
    eResult [in]: 对当前 proactive command 的处理结果
       
返回值:
    none
       
备注:
    注意：当前 proactive command 就是 CUTK 成员变量cmd_type。eResult 仅是一个简单
    结果。若要以大量数据响应，请以 CUTK 成员变量 方式传入

==============================================================================*/
void UTK_GiveResponse(CUTK * pMe, 
                      uim_tk_proactive_cmd_enum_type eCmd,
                      boolean  bForwad, 
                      uim_tk_general_result_type eResult)
{
    uim_cmd_type *uim_cmd_ptr=NULL;
    byte packed_offset;
    
    if (pMe->m_pDisplay != NULL)
    {
        ShowBusyIcon(pMe->m_pShell, pMe->m_pDisplay, &pMe->m_rc,FALSE);
    }
    ERR("%02x   %02x", eCmd, cmd_details_tlv.cmd_type, 0);
    uim_power_control( UIM_PROACTIVE_UIM, TRUE);
    switch(eCmd)
    {// 需要特别处理的响应在此处理，处理后应立即返回
        case UIM_TK_SETUP_MENU:
            if (bForwad)
            {
                if ((uim_cmd_ptr = (uim_cmd_type*) q_get(&uim_free_q)) != NULL)
                {
                    packed_offset = 3;
                    
                    /* Build the Device Identities of the response */
                    parsed_tlv_buf.hdr.comprehension_required = TRUE;
                    parsed_tlv_buf.hdr.tag = UIM_TK_DEVICE_ID_TAG;
                    parsed_tlv_buf.device_id.source_dev_id = UIM_TK_KEYPAD;
                    parsed_tlv_buf.device_id.dest_dev_id   = UIM_TK_UIM;
                    parse_tlv_status = uim_tk_pack_simple_tlv(uim_cmd_ptr->envelope.data, 
                                            packed_offset, &parsed_tlv_buf);
                    packed_offset += parse_tlv_status.tlv_size;
                    
                    /* Build the Item Identities of the response */
                    parsed_tlv_buf.hdr.comprehension_required = TRUE;
                    parsed_tlv_buf.hdr.tag = UIM_TK_ITEM_ID_TAG;
                    parsed_tlv_buf.item_id.id = pMe->m_btCursel;// 当前菜单选择
                    
                    parse_tlv_status = uim_tk_pack_simple_tlv(uim_cmd_ptr->envelope.data,
                                             packed_offset, &parsed_tlv_buf);
                    packed_offset += parse_tlv_status.tlv_size;
                    
                    /* Pack the BER-TLV */
                    pack_ber_status = uim_tk_pack_ber_tlv (uim_cmd_ptr->envelope.data,
                                                      UIM_TK_MENU_SELECTION_TAG,
                                                      packed_offset - UIM_TK_VALUE_OFFSET);
                    /* Fill in the comand size */
                    uim_cmd_ptr->envelope.offset = pack_ber_status.tlv_offset;
                    uim_cmd_ptr->envelope.num_bytes = pack_ber_status.total_size;
        
                    /* Terminal Response */
                    uim_cmd_ptr->hdr.command = UIM_ENVELOPE_F;
                    uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
                    uim_cmd_ptr->hdr.cmd_hdr.sigs = 0;
                    uim_cmd_ptr->hdr.rpt_function = NULL;
                    uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
                    uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
                    
                    /* Send the command to the R-UIM */
                    uim_cmd( uim_cmd_ptr );
                    
                    return;
                }
                else /* The UIM free queue is empty */
                {
                    MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
                } /* end if - check if the UIM free queue is empty */
                
                return;
            }
            
            // 简单通用响应在后面执行
            break;
            
        case UIM_TK_SELECT_ITEM:
        case UIM_TK_GET_INPUT:
        case UIM_TK_PROVIDE_LOCAL_INFO:
            if (bForwad)
            {
                uim_cmd_ptr = start_terminal_response_buffer(&packed_offset, 
                                        &parsed_tlv_buf, &cmd_details_tlv);
                                
                if (uim_cmd_ptr != NULL)
                {
                    /* Set the result to indicate we do not understand the command */
                    parsed_tlv_buf.result.result = UIM_TK_CMD_PERFORMED_SUCCESSFULLY;
                    
                    /* Pack the result in the Terminal Response */
                    parse_tlv_status = uim_tk_pack_simple_tlv(uim_cmd_ptr->terminal_response.data, 
                                        packed_offset, &parsed_tlv_buf);
                                        
                    packed_offset += parse_tlv_status.tlv_size;
        
                    switch(cmd_details_tlv.cmd_type)
                    {
                        case UIM_TK_SELECT_ITEM:
                            /* Build the Item Identities of the response */
                            parsed_tlv_buf.hdr.comprehension_required = TRUE;
                            parsed_tlv_buf.hdr.tag = UIM_TK_ITEM_ID_TAG;
                            parsed_tlv_buf.item_id.id = pMe->m_btCursel;// 当前菜单选择;
                            
                            parse_tlv_status = uim_tk_pack_simple_tlv(uim_cmd_ptr->terminal_response.data, 
                                                    packed_offset, &parsed_tlv_buf);
                            packed_offset += parse_tlv_status.tlv_size;
                            break;
        
                        case UIM_TK_GET_INPUT:
                            {
                                byte * pBuf=NULL;
                                uint16 *pTep;
                                int nLen;
                                int i;	
                                
                                /* Build the Text String of the response */
                                parsed_tlv_buf.hdr.comprehension_required = TRUE;
                                parsed_tlv_buf.hdr.tag = UIM_TK_TEXT_STRING_TAG;  
                                nLen = (byte)WSTRLEN(pMe->m_wszInputText);
                                
                                pBuf = (byte *)MALLOC((nLen + 2) * sizeof(AECHAR));
                                if (NULL == pBuf)
                                {
                                    return;
                                }
                                
                                if (pMe->m_TextQS & 0x02)
                                {// UCS2 alphabet.
                                    pTep = (uint16 *)pBuf;
                                    
                                    for (i=0;i<nLen;i++)
                                    {
                                        *pTep = HTONS(pMe->m_wszInputText[i]);
                                        pTep++;
                                    }
                                    parsed_tlv_buf.txt.num_bytes = nLen*sizeof(uint16); 
                                    parsed_tlv_buf.txt.dcs = 0x08;// UCS2 编码
                                }
                                else
                                {// SMS default alphabet
                                    if (pMe->m_TextQS & 0x08)
                                    {// user input to be in SMS packed format.
                                        parsed_tlv_buf.txt.num_bytes = Pack_gw_7_bit_chars(pMe->m_wszInputText,
                                                    0, pBuf, (nLen + 2) * sizeof(AECHAR));
                                        parsed_tlv_buf.txt.dcs = 0x00;// SMS缺省压缩字母7比特的数据编码计划
                                    }
                                    else
                                    {// user input to be in unpacked format
                                        for (i=0;i<nLen;i++)
                                        {
                                            if (pMe->m_wszInputText[i]>=GSM_CHARACTER_SET_SIZE)
                                            {
                                                pBuf[i] = 0x20;// ' '
                                            }
                                            else
                                            {
                                                pBuf[i] = Ucs2ToGsm[pMe->m_wszInputText[i]];
                                            }
                                        }
                                        parsed_tlv_buf.txt.num_bytes = nLen;
                                        parsed_tlv_buf.txt.dcs = 0x04;// SMS default alphabet
                                    }
                                }
                                
                                if ((parsed_tlv_buf.txt.num_bytes > 0) && (parsed_tlv_buf.txt.data != NULL)) 
                                {
                                    if(parsed_tlv_buf.txt.data != NULL)
                                    {
                                        memcpy(parsed_tlv_buf.txt.data, pBuf, parsed_tlv_buf.txt.num_bytes);
                                    }
                                }  
                                
                                // 释放动态分配的内存                                 
                                FREE(pBuf);
                                pBuf = NULL;
                                
                                parse_tlv_status = uim_tk_pack_simple_tlv(
                                uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                                packed_offset += parse_tlv_status.tlv_size;  
                            }                                           
                            break;
                            
                        /* Build the local information of the response */    
                        case UIM_TK_PROVIDE_LOCAL_INFO:
                            {
                                Local_Information *pLocalInformation = (Local_Information *)MALLOC(sizeof(Local_Information));  
                                UTKApp_LocalInformation(pMe,pLocalInformation);                                                                  
                                parsed_tlv_buf.hdr.comprehension_required = TRUE;
                                parsed_tlv_buf.hdr.tag = pLocalInformation->command_rsp_tag;
                                switch (pLocalInformation->command_rsp_tag)
                                {
                                    case UIM_TK_LOCN_INFO_TAG: 
                                        parsed_tlv_buf.locn_info.mcc = pLocalInformation->locn_info.mcc; 
                                        parsed_tlv_buf.locn_info.imsi_11_12 = pLocalInformation->locn_info.imsi_11_12; 
                                        parsed_tlv_buf.locn_info.sid = pLocalInformation->locn_info.sid;  
                                        parsed_tlv_buf.locn_info.nid = pLocalInformation->locn_info.nid;   
                                        parsed_tlv_buf.locn_info.base_id = pLocalInformation->locn_info.base_id;  
                                        parsed_tlv_buf.locn_info.base_lat = pLocalInformation->locn_info.base_lat;  
                                        parsed_tlv_buf.locn_info.base_long = pLocalInformation->locn_info.base_long;                                                                                                                               
                                        break;   
                                         	
                                    case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:
                                        parsed_tlv_buf.date_time.year = pLocalInformation->date_time.year; 
                                        parsed_tlv_buf.date_time.month = pLocalInformation->date_time.month; 
                                        parsed_tlv_buf.date_time.day = pLocalInformation->date_time.day;
                                        parsed_tlv_buf.date_time.hour = pLocalInformation->date_time.hour;
                                        parsed_tlv_buf.date_time.minute = pLocalInformation->date_time.minute;
                                        parsed_tlv_buf.date_time.second = pLocalInformation->date_time.second;
                                        parsed_tlv_buf.date_time.zone = pLocalInformation->date_time.zone;                                                            
                                        break; 
                                    
                                    case UIM_TK_LANG_TAG:
                                        parsed_tlv_buf.lang.lang = pLocalInformation->lang.lang; 
                                        break;   
                                    
                                    case UIM_TK_ACCESS_TECHNOLOGY_TAG:
                                        parsed_tlv_buf.access_technology.access_technology = pLocalInformation->access_technology; 
                                        break;                                     
                                }                                                   
                                FREE(pLocalInformation);                              
                                parse_tlv_status = uim_tk_pack_simple_tlv(
                                uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                                packed_offset += parse_tlv_status.tlv_size; 
                            }                                                                    
                            break;                               
            
                        default:
                            break;
                    }
                        
                    /* Fill in the comand size */
                    uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                    
                    /* Send the Terminal Response */
                    proactive_cmd_response(uim_cmd_ptr);
                }
                else /* The UIM free queue is empty */
                {
                    MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
                } /* end if - check if the UIM free queue is empty */
                
                return;
            }
            break;
            
        case UIM_TK_END_PROACTIVE_SESSION:
            send_UTK_simple_resp(eResult);
            return;
            
        default:
            break;
    }
    
    // 简单通用响应
    send_UTK_simple_resp(eResult);
}

static void send_UTK_simple_resp(uim_tk_general_result_type result_type) 
{
    uim_cmd_type *uim_cmd_ptr;
    byte packed_offset;              /* Pointer to offset for packing */
    
    uim_cmd_ptr = start_terminal_response_buffer(&packed_offset, 
                            &parsed_tlv_buf, &cmd_details_tlv);
                            
    /* Make sure there was a UIM buffer available. */
    if (uim_cmd_ptr != NULL)
    {
        /* Set the result to indicate we do not understand the command */
        parsed_tlv_buf.result.result = result_type;
        
        /* Pack the result in the Terminal Response */
        parse_tlv_status = uim_tk_pack_simple_tlv(uim_cmd_ptr->terminal_response.data, 
                                packed_offset, &parsed_tlv_buf);
        packed_offset += parse_tlv_status.tlv_size;
        
        /* Fill in the comand size */
        uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
        
        /* Send the Terminal Response */
        proactive_cmd_response(uim_cmd_ptr);        
    }
}

static void ui_ruim_report(uim_rpt_type *report)
{
    ui_uim_rpt_buf = *report;
    
    rex_set_sigs( &ui_tcb, UI_UIM_STATUS_SIG );
} 


/*==============================================================================
函数: 
    UTK_SendTerminalProfile
       
说明: 
    Sends a TERMINAL PROFILE command to the R-UIM.  This command tells the R-UIM
    about what UTK facilities the mobile supports。
       
参数: 
    none
       
返回值:
    none
       
备注:
    ui_init_ruim_items 的替代函数。

==============================================================================*/
void UTK_SendTerminalProfile (void)
{
    uim_cmd_type   *uim_cmd_ptr;              /* UIM command pointer */
    byte uim_cdma_svc_table_buffer[UIM_CDMA_SVC_TABLE_SIZE];
    uim_svc_table_return_type svc;            /* service allocated and activated*/

    /* Initialize buffer for reading CDMA service table */
    if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
        /* Read CDMA service table */
        
        /* Fill up the command header info */
        uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
        
        /* Set the Protocol to CDMA */
        uim_cmd_ptr->hdr.protocol = UIM_CDMA;
        
        /* Fill in the callback function */
        uim_cmd_ptr->hdr.rpt_function = ui_ruim_report;
        
        /* Set the report option to always report upon completion of the cmd */
        uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
        
        /* The task and signal are set to NULL */
        uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
        uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
        
        /* Access info */
        uim_cmd_ptr->access_uim.item = UIM_CDMA_CDMA_SVC_TABLE;
        
        /* Read Access to UIM */
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        
        /* Indicates the offset for read */
        uim_cmd_ptr->access_uim.offset = 0;
        
        /* Buffer to return the data in */
        uim_cmd_ptr->access_uim.data_ptr = uim_cdma_svc_table_buffer;
        
        /* Number of bytes to access */
        uim_cmd_ptr->access_uim.num_bytes = UIM_CDMA_SVC_TABLE_SIZE;
        
        /* Clear the signal */
        (void) rex_clr_sigs( &ui_tcb, UI_UIM_STATUS_SIG );

        /* Send the command */
        uim_cmd( uim_cmd_ptr );

        /* Wait for the command to be done */
        (void) rex_wait( UI_UIM_STATUS_SIG );
        
        if ((ui_uim_rpt_buf.rpt_type == UIM_ACCESS_R) &&
            (ui_uim_rpt_buf.rpt_status == UIM_PASS))
        {
        	
            ERR("UTK_SendTerminalProfile 2",0,0,0);
            /* Check to see if UIM is Pro-Active */
            svc = uim_return_cdma_svc_availabililty ( UIM_CDMA_SVC_PROACTIVE_UIM,
            uim_cdma_svc_table_buffer);
            
            /* Check if Proactive uim has been allocated and activated */
            if ( svc.activated && svc.allocated )
            {
                /* The UIM is kept on for the proactive UIM. */
                uim_power_control( UIM_PROACTIVE_UIM, TRUE);
                
                /* Set the info on Pro-active UIM */
                uim_set_proactive_uim ( TRUE );
                
                /* Send TERMINAL PROFILE command */
                if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
                {
                    /* Fill up header info */
                    uim_cmd_ptr->hdr.command = UIM_TERMINAL_PROFILE_F;
                    /* Set the Protocol to CDMA */
                    uim_cmd_ptr->hdr.protocol = UIM_CDMA;
                    /* Fill in the callback function */
                    uim_cmd_ptr->hdr.rpt_function = ui_ruim_report;
                    /* Set the report option to always report upon completion of the cmd */
                    uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
                    /* The task and signal are set to NULL */
                    uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
                    uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
                    
                    /* Number of bytes sent in the command */
                    uim_cmd_ptr->terminal_profile.num_bytes =
                              UIM_TK_TERMINAL_PROFILE_DATA_LENGTH;
                    
                    /* Initialize buffer for sending terminal profile command */
                    memset (uim_cmd_ptr->terminal_profile.data, 0,
                        UIM_TK_TERMINAL_PROFILE_DATA_LENGTH);
                    
                    /* Fill bit fields for command data */
                    uim_cmd_ptr->terminal_profile.data[0]  |=
                        ( UIM_TK_B1_PROFILE_DOWNLOAD | UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD );
                    uim_cmd_ptr->terminal_profile.data[2]  |=
                        UIM_TK_B3_DISPLAY_TEXT;
                    
#ifdef FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS
                    /* Set terminal profile bytes specific to the use of our application.
                    ** Setting the bytes to 0xFF enables specific proactive commands, in
                    ** particular SET UP MENU to invoke CDMA Send SMS.
                    */
                    uim_cmd_ptr->terminal_profile.data[0]  = 0xFF;
                    uim_cmd_ptr->terminal_profile.data[1]  = 0xFF;
                    uim_cmd_ptr->terminal_profile.data[2]  = 0xFF;
                    uim_cmd_ptr->terminal_profile.data[3]  = 0xFF;
                    uim_cmd_ptr->terminal_profile.data[7]  = 0xFF;
                    uim_cmd_ptr->terminal_profile.data[8]  = 0xFF;
#endif /* FEATURE_UIM_DEBUG_TOOLKIT_SEND_SMS */
                    uim_cmd_ptr->terminal_profile.data[19] |=
                        ( UIM_TK_B20_CDMA_SEND_SMS | UIM_TK_B20_CDMA_SMS_PP_DOWNLOAD );
                    
                    /* Clear the signal */
                    (void) rex_clr_sigs( &ui_tcb, UI_UIM_STATUS_SIG );
                    
                    /* Send the command */
                    uim_cmd( uim_cmd_ptr );
                    
                    /* Wait for the command to be done */
                    (void) rex_wait( UI_UIM_STATUS_SIG );
                } /* buffer available */
            } /* svc allocated and activated */
        } /* UIM_PASS */
    }
    else
    {
        MSG_HIGH ("Buffer not available to read CDMA service table",0,0,0);
    }
} 

/*==============================================================================
函数: 
    UTK_decodetext
       
说明: 
    本函数用来解码按短信编码方式编码的字节序列到 AECHAR 字符串。
       
参数: 
    wszBuf [In/out]: 用于保存解码后文本的缓冲区 。
    nBufSize [In]: 输出缓冲区大小,单位为字节 。
    pdata [In]: 指向待解码字节流 。
    nlen [In]: 待解码字节流大小,单位为字节 。
    encode_schedule [In]: 待解码字节流的编码方式 。
    
       
返回值:
    none
       
备注:
    注意这里字节流的编码方式完全与 uasms_user_data_encoding_e_type 定义的不同。
    0x00: SMS缺省压缩字母7比特的数据编码计划
    0x04: GSM缺省字母8比特数据的数据编码计划
    0x08: 16比特UNICODE字母的数据编码计划
    本函数对不能识别的编码方式，按 0x04 处理
==============================================================================*/
void UTK_decodetext(AECHAR *wszBuf, int nBufSize, byte *pdata, int nlen, byte encode_schedule)
{
    int     i;
    
    if (NULL == wszBuf || NULL == pdata || nBufSize<=0 || nlen<=0)
    {
        return;
    }
    
    switch (encode_schedule)
    {
        case 0x00:// SMS缺省压缩字母7比特的数据编码计划
            {
                int      i=0;
                uint16   pos=0;
                byte     bTep;
                int      shift=0;
                int      nMaxAECHAR;
                
                nMaxAECHAR = nBufSize/sizeof(AECHAR);
                for( pos=0; (i<nMaxAECHAR) && (pos<nlen); pos++ )
                {
                    if (shift == 7)
                    {
                        bTep = pdata[pos-1] >> 1;
                        wszBuf[i] = (AECHAR)GsmToUcs2[bTep];
                        if (GSM_CHARACTER_SET_SIZE>bTep)
                        {
                            wszBuf[i] = (AECHAR)GsmToUcs2[bTep];
                            //ERR("i%d=%04x  %02x",i,wszBuf[i],bTep);
                            i++;
                        }
                        shift = 0;
                    }

                    bTep = (pdata[pos] << shift ) & 0x7F;
                    
                    if (shift != 0)
                    {
                        /* except the first byte, a character contains some bits
                        ** from the previous byte.
                        */
                        bTep = bTep | (pdata[pos-1] >> (8-shift));
                    }
                    if (GSM_CHARACTER_SET_SIZE>bTep)
                    {
                        wszBuf[i] = (AECHAR)GsmToUcs2[bTep];
                        i++;
                        //ERR("i%d=%04x  %02x",i,wszBuf[i],bTep);
                    }
                    
                    shift ++;
                }
                wszBuf[i] = 0;
            }
            break;
        
        case 0x08:// 16比特UNICODE字母的数据编码计划
            if (nBufSize>nlen)
            {
                uint16 *pUs;
                
                MEMCPY((void*)wszBuf, (void*)pdata, nlen);
                pUs = wszBuf;
                for (i=0; i<nlen/2; i++)
                {
                   *pUs = NTOHS(*pUs);
                   pUs++;
                }
                wszBuf[i] = 0;
            }
            break;
        
        default:  // 0x04 GSM缺省字母8比特数据的数据编码计划
            if (nBufSize/sizeof(AECHAR) > nlen)
            {
                for (i=0; i<nlen; i++)
                {
                    wszBuf[i] = (AECHAR)pdata[i];
                }
                wszBuf[i] = 0;
            }
            break;
    }
}

/*==============================================================================
函数: 
    DecodeDisplayTextData
       
说明: 
    本函数用来解码相应于 Display Text 命令的原始数据字节序列到结构 Display_text 中。
       
参数: 
    pdata [In]: 相应于 Display Text 命令的原始数据字节序列(BER-TLV 结构) 。
    pText [In/out]: 指向用于保存解码后数据的 Display_text 结构缓冲区 。
    
       
返回值:
    none
       
备注:

==============================================================================*/
void DecodeDisplayTextData(byte *pdata, Display_text *pText)
{
    int pos=0, nValLen,nTep;
    if (NULL == pdata || NULL == pText)
    {
        return;
    }
    
    pText->command_tag = pdata[pos];
    pos++;
    if (pdata[pos] == 0x81)
    {
        pText->length = pdata[pos+1];
        pos+=2;
        nTep = 3;
    }
    else
    {
        pText->length = pdata[pos];
        pos++;
        nTep = 2;
    }
    
    while (pos<(pText->length+nTep))
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case 0x01:// UIM_TK_COMMAND_DETAILS_TAG
            case 0x81:
                pText->cmd_describe.describe_id = pdata[pos];
                pText->cmd_describe.length = pdata[pos+1];
                pText->cmd_describe.command_num = pdata[pos+2];
                pText->cmd_describe.command_type = pdata[pos+3];
                pText->cmd_describe.command_restricttag = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case 0x02:// UIM_TK_DEVICE_ID_TAG
            case 0x82:
                pText->dev_identity.speciality_id = pdata[pos];
                pText->dev_identity.length = pdata[pos+1];
                pText->dev_identity.source_speciality = pdata[pos+2];
                pText->dev_identity.target_speciality = pdata[pos+3];
                pos+=4;
                break;
                
            // 正文串识
            case 0x0D:// UIM_TK_TEXT_STRING_TAG
            case 0x8D:
                pText->text.text_id = pdata[pos];
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pText->text.length = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pText->text.length = pdata[pos];
                    pos++;
                }
                pText->text.encode_schedule = pdata[pos];
                pos++;
                // 解码字符串文本
                UTK_decodetext(pText->text.data, sizeof(pText->text.data), 
                        &pdata[pos], pText->text.length-1, pText->text.encode_schedule);
                pos+=(pText->text.length-1);
                break;
                
            // 图标标识符
            case 0x1E:// UIM_TK_ICON_ID_TAG
            case 0x9E:
                // 不支持，跳过
                pos+=4;
                break;
                
            // 立即响应标识
            case 0x2B:// UIM_TK_IMMEDIATE_RSP_TAG
            case 0xAB:
                // 不支持，跳过
                pos+=2;
                break;
                
            // 持续时间标识
            case 0x04:// UIM_TK_DURATION_TAG
            case 0x84:
                // 不支持，跳过
                pos+=4;
                break;
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
}

/*==============================================================================
函数: 
    CUTK_SetUTKMenu
       
说明: 
    本函数针对命令 Setup Menu 和 Select Item 命令, 分析相应于命令的字节数据流，
    建立菜单列表。
       
参数: 
    pMe [in]：指向UTK Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in]：IMenuCtl 接口指针。
    pwszTitle [out]： 菜单标题(需保存时传入)
    cmd_type： uim_tk_proactive_cmd_enum_type
       
返回值:
    菜单项数
       
备注:
    本函数只处理 Setup Menu 和 Select Item 命令的菜单的初始化。
==============================================================================*/
int CUTK_SetUTKMenu(CUTK *pMe, IMenuCtl *pMenu, 
    AECHAR *pwszTitle, 
    uim_tk_proactive_cmd_enum_type cmd_type
)
{
    AECHAR * wszBuf;
    byte *utk_ptr;
    int nItemCount = 0;
    CtlAddItem  ai;
    int pos=0;
    int nLen,nValLen,nSize,nTep;
    command_describe cmd_describe;
	AECHAR wsFmt[5] = {0};

    if (pMe == NULL)
    {
        return 0;
    }
    
    nSize = 256 * sizeof(AECHAR);
    wszBuf = MALLOC(nSize);
    if (NULL == wszBuf)
    {
        return 0;
    }
    (void)IMENUCTL_SetTitle(pMenu, NULL, 0, pMe->m_wszTitle);
    utk_ptr = UTK_GetCmddata(cmd_type);
    ERR("miaoxiaoming: UTK_GetCmddata utk_ptr=%x",utk_ptr,0,0);
    if (utk_ptr != NULL)
    {
        // 跳过主动式RUIM 命令标签
        pos++;
        
        // 获取命令数据长度
        if (utk_ptr[pos] == 0x81)
        {
            nLen = utk_ptr[pos+1];
            pos+=2;
            nTep = 3;
        }
        else
        {
            nLen = utk_ptr[pos];
            pos++;
            nTep = 2;
        }
        
        while (pos<(nLen+nTep))
        {
            ERR("miaoxiaoming utk_ptr.value=%x",utk_ptr[pos],0,0);
            switch (utk_ptr[pos])
            {
                // 命令说明标识
                case 0x01:// UIM_TK_COMMAND_DETAILS_TAG
                case 0x81:
                    cmd_describe.describe_id = utk_ptr[pos];
                    cmd_describe.length = utk_ptr[pos+1];
                    cmd_describe.command_num = utk_ptr[pos+2];
                    cmd_describe.command_type = utk_ptr[pos+3];
                    cmd_describe.command_restricttag = utk_ptr[pos+4];
                    pos+=5;
                    break;
                    
                // 设备特性标识
                case 0x02:// UIM_TK_DEVICE_ID_TAG
                case 0x82:
                    pos+=4;
                    break;
                    
                // 条目标识
                case 0x0F:// UIM_TK_ITEM_TAG
                case 0x8F:
                    // 跳过条目标识
                    pos++;
                    
                    // 长度
                    if (utk_ptr[pos] == 0x81)
                    {
                        nValLen = utk_ptr[pos+1];
                        pos+=2;
                    }
                    else
                    {
                        nValLen = utk_ptr[pos];
                        pos++;
                    }
                    if (nValLen > 0)
                    {
                        ai.wItemID = utk_ptr[pos];
                        ai.pszResText = NULL;
                        ai.wText = 0;
                        ai.pImage = NULL;
                        ai.wImage = IDB_UTKMENU;
                        ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                        ai.wFont = AEE_FONT_NORMAL;
                        ai.dwData = 0;
                        ai.pText = wszBuf;
                        MEMSET(wszBuf, 0, nSize);
                        DecodeAlphaString(&utk_ptr[pos+1], nValLen-1, wszBuf, 256);
                        (void)IMENUCTL_AddItemEx(pMenu, &ai );
                        nItemCount++;
                    }
                    pos+=nValLen;
                    break;
                
                // Alpha标识符
                case 0x05:// UIM_TK_ALPHA_ID_TAG
                case 0x85:
                    // 跳过Alpha标识
                    pos++;
                    
                    // 长度
                    if (utk_ptr[pos] == 0x81)
                    {
                        nValLen = utk_ptr[pos+1];
                        pos+=2;
                    }
                    else
                    {
                        nValLen = utk_ptr[pos];
                        pos++;
                    }
                    if (nValLen>0)
                    {
                        MEMSET(wszBuf, 0, nSize);
                        DecodeAlphaString(&utk_ptr[pos], nValLen, wszBuf, 256);
                        (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wszBuf);
                    }
                    if (pwszTitle)
                    {
                        WSTRCPY(pwszTitle, wszBuf);
                    }
                    pos+=nValLen;
                    break;
                    
                    
                // 条目图标标识符列表
                case 0x1F:// 
                case 0x9F:
                    
                // 图标标识符
                case 0x1E:// UIM_TK_ICON_ID_TAG
                case 0x9E:
                    
                default:
                    // 命令不支持的项
                    ERR("---Unsupport!----",0,0,0);
                    // 跳过标识
                    pos++;
                    
                    if (utk_ptr[pos] == 0x81)
                    {
                        nValLen = utk_ptr[pos+1];
                        pos+=2;
                    }
                    else
                    {
                        nValLen = utk_ptr[pos];
                        pos++;
                    }
                    
                    // 跳过值部分
                    pos+=nValLen;
                    break;
            }
        }
    }  
      
    FREE(wszBuf);        
        
    return nItemCount;
}

/*==============================================================================
函数: 
    UTK_ProcessSendCDMASMSCmd
       
说明: 
    本函数处理 CDMA SEND SMS 命令。
       
参数: 
    pMe [In]: 指向UTK Applet对象结构的指针,该结构包含小程序的特定信息。
    pCltMsg [in/out]: 指向 wms_client_message_s_type 结构的指针，用于返回待发
                      消息的 wms_client_message_s_type 结构地址。
    wszAlpha [in/out]: 用于返回 Alpha 标识字符串的buffer
    nAlphabufLen [in]: 返回 Alpha 标识字符串的buffer 的大小，单位为 AECHAR                    
       
返回值:
    none
       
备注:
    如返回值为 SUCCESS , 并且 *pCltMsg 等于 NULL , 表示消息不需要发送(特殊要求)

==============================================================================*/
int UTK_ProcessSendCDMASMSCmd(CUTK *pMe, 
    wms_client_message_s_type ** pCltMsg,
    AECHAR *wszAlpha,
    int nAlphabufLen
)
{
    int pos=0,nValLen,nCmdvalLen,nTep;
    byte *pdata=NULL;
    command_describe  cmd_des;
    boolean           bNeedPacking = FALSE;
    
    if ((NULL == pMe) || (NULL == wszAlpha) || (NULL == pCltMsg))
    {
        return EFAILED;
    }
    
    pdata = UTK_GetCmddata(pMe->cmd_type);
    
    if (NULL == pdata)
    {
        return EFAILED;
    }
    
    //  跳过主动式UIM 命令标签
    pos++;
    
    if (pdata[pos] == 0x81)
    {
        nCmdvalLen = pdata[pos+1];
        pos+=2;
        nTep = nCmdvalLen+3;
    }
    else
    {
        nCmdvalLen = pdata[pos];
        pos++;
        nTep = nCmdvalLen+2;
    }
    
    // 开始分析各子项
    while (pos<nTep)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case 0x01:// UIM_TK_COMMAND_DETAILS_TAG
            case 0x81:
                cmd_des.describe_id = pdata[pos];
                cmd_des.length = pdata[pos+1];
                cmd_des.command_num = pdata[pos+2];
                cmd_des.command_type = pdata[pos+3];
                cmd_des.command_restricttag = pdata[pos+4];
                if (cmd_des.command_restricttag & 0x01)
                { // SMS packing by the ME required.
                    bNeedPacking = TRUE;
                }
                pos+=5;
                break;
                
            // 设备特性标识
            case 0x02:// UIM_TK_DEVICE_ID_TAG
            case 0x82:
                pos+=4;
                break;
                
            // Alpha标识符
            case 0x05:// UIM_TK_ALPHA_ID_TAG
            case 0x85:
                // 跳过Alpha标识
                pos++;
                
                // 长度
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                if (nValLen>0)
                {
                    MEMSET(wszAlpha, 0, nAlphabufLen*sizeof(AECHAR));
                    DecodeAlphaString(&pdata[pos], nValLen, wszAlpha, nAlphabufLen);
                }

                pos+=nValLen;
                break;
                
            // CDMA SMS TDPU
            case 0x48:// UIM_TK_CDMA_SMS_TPDU_TAG
            case 0xC8:
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                {
                    wms_status_e_type st;
                    int i;
                    
                    cdma_ota.format      = WMS_FORMAT_CDMA;
                    cdma_ota.data_len    = MIN (nValLen, WMS_MAX_LEN-2);
                    memcpy((uint8*)cdma_ota.data, &pdata[pos], cdma_ota.data_len);
                    
                    st = wms_ts_decode_CDMA_tl(&cdma_ota,
                                        &cdma_tl,
                                        &client_message.u.cdma_message.raw_ts);
                    if (st != WMS_OK_S) 
                    {
                        MSG_HIGH("UTK: DECODE CDMA TL FAILURE", 0, 0, 0);
                        return EFAILED;
                    }
                    if (cdma_tl.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
                    {
                        for (i=0; i<cdma_tl.address.number_of_digits; i++)
                        {
                            if (cdma_tl.address.digits[i]==0)
                            {
                                cdma_tl.address.digits[i] = 0xA;
                            }
                        }
                    }
                    wms_ts_convert_tl2cl(&cdma_tl, &client_message.u.cdma_message);
                    
                    *pCltMsg = &client_message;
#ifdef FEATURE_CARRIER_CHINA_TELCOM
                    // 检查消息是否是注册信息，若是，按联通要求不发送
                    if (cdma_tl.mask & WMS_MASK_TL_BEARER_DATA)
                    {
                        wms_client_ts_data_s_type *pCltBd = NULL;
                        wms_client_bd_s_type *info_data = NULL;
                        wms_raw_ts_data_s_type   *raw_bd_ptr = &client_message.u.cdma_message.raw_ts;
                        wms_cdma_user_data_s_type *puserdata;
                        int nlen;
                        AECHAR wstrESN[4] = {0};
                        extern void OEMWMS_ConvertFromUnicode(wms_client_bd_s_type *bd_data);
                        
                        (void)STRTOWSTR("ESN", wstrESN, sizeof(wstrESN));
                        pCltBd = sys_malloc(sizeof(wms_client_ts_data_s_type));
                        info_data = sys_malloc(sizeof(wms_client_bd_s_type));
                        
                        if ((NULL != pCltBd) && (NULL != info_data))
                        {
                            if (wms_ts_decode(raw_bd_ptr, pCltBd) == WMS_OK_S)
                            {
                                if (pCltBd->u.cdma.mask & WMS_MASK_BD_USER_DATA) 
                                {
                                    switch (pCltBd->u.cdma.user_data.encoding)
                                    {
                                        case WMS_ENCODING_ASCII:
                                        case WMS_ENCODING_IA5:
                                            puserdata = (wms_cdma_user_data_s_type*)&pCltBd->u.cdma.user_data;
                                            nlen = wms_ts_unpack_ascii(puserdata,
                                                        pCltBd->u.cdma.user_data.number_of_digits+1,
                                                        info_data->user_data.data);

                                            info_data->user_data.data[nlen] = 0;
                                            if ((nlen >= 3) &&
                                                (STRNCMP((char *)(info_data->user_data.data), "ESN", 3) == 0))
                                            {
                                                *pCltMsg = NULL;
                                            }
                                            break;
                                            
                                        case WMS_ENCODING_UNICODE:
                                            OEMWMS_ConvertFromUnicode(&pCltBd->u.cdma);
                                            nlen = pCltBd->u.cdma.user_data.data_len;
                                            
                                            if ((nlen >= 3) &&
                                                (WSTRNCMP((AECHAR *)(pCltBd->u.cdma.user_data.data), wstrESN, 3) == 0))
                                            {
                                                *pCltMsg = NULL;
                                            }
                                            break;
                                            
                                        // just copy the user data for other encodings
                                        default:
                                            nlen = pCltBd->u.cdma.user_data.data_len;
                                            
                                            if ((nlen >= 3) &&
                                                (STRNCMP((char *)(pCltBd->u.cdma.user_data.data), "ESN", 3) == 0))
                                            {
                                                *pCltMsg = NULL;
                                            }
                                            break;
                                    }
                                }
                            }
                        }
                        
                        if (NULL != pCltBd)
                        {
                            sys_free(pCltBd);
                        }
                        if (NULL != info_data)
                        {
                            sys_free(info_data);
                        }
                    }
#endif                    
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
                
            // 地址标识
            case 0x06:// UIM_TK_ADDRESS_TAG
            case 0x86:
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
    
    return SUCCESS;
}

/*==============================================================================
函数: 
    UTK_Get_Setup_Call_Buf
       
说明: 
    本函数用来获取存储相应于 Setup Call 命令原始数据字节序列解码后数据的结构 
    Setup_Call 缓冲区地址。
       
参数: 
    none
       
返回值:
    Setup_Call 结构指针
       
备注:
    因为 Setup Call 命令解码信息会跨模块使用，故使用一个全局静态变量来保存。但
    必须通过本函数来访问。
==============================================================================*/
Setup_Call *UTK_Get_Setup_Call_Buf(void)
{
    static Setup_Call stCall={0};
    
    return &stCall;
}

/*==============================================================================
函数: 
    UTK_DecodeSetupCallCmdData
       
说明: 
    本函数用来解码相应于 Setup Call 命令的原始数据字节序列到结构 Setup_Call 中。
       
参数: 
    pdata [In]: 相应于 Setup Call 命令的原始数据字节序列(BER-TLV 结构) 。
    pstCall [in/out]: 指向用于保存解码后数据的 Setup_Call 结构缓冲区 。
       
返回值:
    SUCCESS : 成功
    EFAILED : 失败
       
备注:

==============================================================================*/
int UTK_DecodeSetupCallCmdData(byte *pdata, Setup_Call *pstCall)
{
    int pos=0, nValLen, nTep, i, j;
    byte hig,low;
    
    if ((NULL == pdata) || (NULL == pstCall))
    {
        return EFAILED;
    }
    
    //  跳过主动式UIM 命令标签
    pos++;
    
    // 命令长度
    if (pdata[pos] == 0x81)
    {
        nTep = pdata[pos+1]+3;
        pos+=2;
    }
    else
    {
        nTep = pdata[pos]+2;
        pos++;
    }
    
    // 开始分析各子项
    while (pos<nTep)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case 0x01:// UIM_TK_COMMAND_DETAILS_TAG
            case 0x81:
                pstCall->cmd_describe.describe_id = pdata[pos];
                pstCall->cmd_describe.length = pdata[pos+1];
                pstCall->cmd_describe.command_num = pdata[pos+2];
                pstCall->cmd_describe.command_type = pdata[pos+3];
                pstCall->cmd_describe.command_restricttag = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case 0x02:// UIM_TK_DEVICE_ID_TAG
            case 0x82:
                pos+=4;
                break;
                
            // Alpha标识符
            case 0x05:// UIM_TK_ALPHA_ID_TAG
            case 0x85:
                // 跳过Alpha标识
                pos++;
                
                // 长度
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                if (nValLen>0)
                {
                    if (WSTRLEN(pstCall->wszAlpha)==0)
                    {// 先用户证实阶段
                        DecodeAlphaString(&pdata[pos], nValLen, pstCall->wszAlpha, 
                                sizeof(pstCall->wszAlpha)/sizeof(AECHAR));
                    }
                    else
                    {// 后呼叫建立阶段
                        DecodeAlphaString(&pdata[pos], nValLen, pstCall->wszAlphaSetupCall, 
                                sizeof(pstCall->wszAlphaSetupCall)/sizeof(AECHAR));
                    }
                }

                pos+=nValLen;
                break;
                
            // 地址标识
            case 0x06:// UIM_TK_ADDRESS_TAG
            case 0x86:
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过 TONNPI
                pos++;
                
                // 解析号码
                j=0;
                for (i=0; (i<nValLen-1) && (j<sizeof(pstCall->wszDialNum)/sizeof(AECHAR)); i++)
                {
                    low = ((pdata[pos+i] & 0x0F) + '0');
                    hig = (((pdata[pos+i]>>4) & 0x0F) + '0');
                    
                    
                    if('0'<=low && low<='9')
                    {
                        pstCall->wszDialNum[j] = (AECHAR)low;
                        j++;
                    }
                    else if('0'+0x0A==low)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'*';
                        j++;
                    }
                    else if('0'+0x0B==low)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'#';
                        j++;
                    }
                    else if('0'+0x0C==low)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'+';
                        j++;
                    }
                    else
                    {
                        break;
                    } 
                    
                    if('0'<=hig && hig<='9')
                    {
                        pstCall->wszDialNum[j] = (AECHAR)hig;
                        j++;
                    }
                    else if('0'+0x0A==hig)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'*';
                        j++;
                    }
                    else if('0'+0x0B==hig)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'#';
                        j++;
                    }
                    else if('0'+0x0C==hig)
                    {
                        pstCall->wszDialNum[j] = (AECHAR)'+';
                        j++;
                    }
                    else
                    {
                        break;
                    } 
                }
                
                // 回补 TONNPI
                pos--;
                
                // 跳过值部分
                pos+=nValLen;
                break;
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
    
    return SUCCESS;
}

void DecodeGetInputData(byte *pdata, Get_input *pGetinput)
{
    int pos=0, nValLen;
    if (NULL == pdata || NULL == pGetinput)
    {
        return;
    }
    
    pGetinput->command_tag = pdata[pos];
    pos++;
    if (pdata[pos] == 0x81)
    {
        pGetinput->length = pdata[pos+1];
        pos+=2;
    }
    else
    {
        pGetinput->length = pdata[pos];
        pos++;
    }
    
    while (pos<pGetinput->length)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case UIM_TK_COMMAND_DETAILS_TAG:
            case 0x81:
                pGetinput->cmd_describe.describe_id = pdata[pos];
                pGetinput->cmd_describe.length = pdata[pos+1];
                pGetinput->cmd_describe.command_num = pdata[pos+2];
                pGetinput->cmd_describe.command_type = pdata[pos+3];
                pGetinput->cmd_describe.command_restricttag = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case UIM_TK_DEVICE_ID_TAG:
            case 0x82:
                pGetinput->dev_identity.speciality_id = pdata[pos];
                pGetinput->dev_identity.length = pdata[pos+1];
                pGetinput->dev_identity.source_speciality = pdata[pos+2];
                pGetinput->dev_identity.target_speciality = pdata[pos+3];
                pos+=4;
                break;
                
            // 正文串识
            case UIM_TK_TEXT_STRING_TAG:
            case 0x8D:
                pGetinput->text.text_id = pdata[pos];
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pGetinput->text.length = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pGetinput->text.length = pdata[pos];
                    pos++;
                }
                pGetinput->text.encode_schedule = pdata[pos];
                pos++;
                // 解码字符串文本
                UTK_decodetext(pGetinput->text.data, sizeof(pGetinput->text.data), 
                        &pdata[pos], pGetinput->text.length-1, pGetinput->text.encode_schedule);
                pos+=(pGetinput->text.length-1);
                break;
                
            // 响应长度标识
            case UIM_TK_RSP_LEN_TAG:
            case 0x91:
                pGetinput->response.response_id = pdata[pos];          
                pGetinput->response.length = 2;               
                pGetinput->response.min_len_rsp = pdata[pos+2];        
                pGetinput->response.max_len_rsp = pdata[pos+3];      
                pos+=4;                  
                break;
                
            // 缺省正文
            case UIM_TK_DEFAULT_TEXT_TAG:
            case 0x97:  
                pGetinput->defaulttext.text_id = pdata[pos];
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pGetinput->defaulttext.length = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pGetinput->defaulttext.length = pdata[pos];
                    pos++;
                }
                pGetinput->defaulttext.encode_schedule = pdata[pos];
                pos++;
                // 解码字符串文本
                UTK_decodetext(pGetinput->defaulttext.data, sizeof(pGetinput->defaulttext.data), 
                        &pdata[pos], pGetinput->defaulttext.length-1, pGetinput->defaulttext.encode_schedule);
                pos+=(pGetinput->defaulttext.length-1);                
                break;
                
            // 图标标识符
            case UIM_TK_ICON_ID_TAG:
            case 0x9E:
                pGetinput->icon.icon_id = pdata[pos];          
                pGetinput->icon.length = 2;               
                pGetinput->icon.qual = pdata[pos+2];        
                pGetinput->icon.id = pdata[pos+3];      
                pos+=4;                                        
                break;
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
}

/*==============================================================================
函数: 
    DecodeRefreshData
       
说明: 
    本函数用来解码相应于 Refresh 命令的原始数据字节序列到结构 RefreshCmdType 中。
       
参数: 
    pdata [In]: 相应于 Refresh 命令的原始数据字节序列(BER-TLV 结构) 。
    pCmd [In/out]: 指向用于保存解码后数据的 RefreshCmdType 结构缓冲区 。
    
       
返回值:
    none
       
备注:

==============================================================================*/
void DecodeRefreshData(byte *pdata, RefreshCmdType *pCmd)
{
    int pos=0, nValLen,nTep;
    if (NULL == pdata || NULL == pCmd)
    {
        return;
    }
    
    pos++;
    if (pdata[pos] == 0x81)
    {
        nTep = pdata[pos+1];
        pos+=2;
        nTep += 3;
    }
    else
    {
        nTep = pdata[pos];
        pos++;
        nTep += 2;
    }
    
    while (pos<nTep)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case 0x01:// UIM_TK_COMMAND_DETAILS_TAG
            case 0x81:
                pCmd->cmd_qualifier = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case 0x02:// UIM_TK_DEVICE_ID_TAG
            case 0x82:
                pos+=4;
                break;
                
            // 文件列表标识
            case 0x12:// UIM_TK_FILE_LIST_TAG
            case 0x92:
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pCmd->fileList.numBytes = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pCmd->fileList.numBytes = pdata[pos];
                    pos++;
                }
                pCmd->fileList.numFiles = pdata[pos];
                pos++;
                if (pCmd->fileList.numBytes>0)
                {
                    pCmd->fileList.numBytes-=1;
                    pCmd->fileList.isValid = 1;
                }
                memcpy(pCmd->fileList.fileData, &pdata[pos], pCmd->fileList.numBytes);

                pos+=(pCmd->fileList.numBytes+1);
                break;
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
}

void UTKApp_LocalInformation(CUTK *pMe,Local_Information *pLocalInformation)
{
    extern cdma_bs_type *cur_bs_ptr; 	
    int pos=0, nValLen;
    byte   *pdata;
    JulianType  jDate;    
    
    if (NULL == pMe)
    {
        return;
    }       
    pdata = UTK_GetCmddata(pMe->cmd_type);        
    
    if (NULL == pdata || NULL == pLocalInformation)
    {
        return;
    }   
      
    pLocalInformation->command_tag = pdata[pos];
    pos++;
    if (pdata[pos] == 0x81)
    {
        pLocalInformation->length = pdata[pos+1];
        pos+=2;
    }
    else
    {
        pLocalInformation->length = pdata[pos];
        pos++;
    }
    
    while (pos<pLocalInformation->length)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case UIM_TK_COMMAND_DETAILS_TAG:
            case 0x81:
                pLocalInformation->cmd_describe.describe_id = pdata[pos];
                pLocalInformation->cmd_describe.length = pdata[pos+1];
                pLocalInformation->cmd_describe.command_num = pdata[pos+2];
                pLocalInformation->cmd_describe.command_type = pdata[pos+3];
                pLocalInformation->cmd_describe.command_restricttag = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case UIM_TK_DEVICE_ID_TAG:
            case 0x82:
                pLocalInformation->dev_identity.speciality_id = pdata[pos];
                pLocalInformation->dev_identity.length = pdata[pos+1];
                pLocalInformation->dev_identity.source_speciality = pdata[pos+2];
                pLocalInformation->dev_identity.target_speciality = pdata[pos+3];
                pos+=4;
                break;

            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
    switch (pLocalInformation->cmd_describe.command_restricttag)
    {                                                                         
         case UIM_TK_LOCAL_INFO_ACCORD: // Location Information according to current NAA
           {      
             char   txt[4];     	
             word   mcc;
             word   imsi_11_12;             
             static const char mintab[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};         
             
             pLocalInformation->command_rsp_tag = UIM_TK_LOCN_INFO_TAG;                             
             {
             	 mcc = cur_bs_ptr->csp.esp.mcc; 
                 txt[0] = mintab[ mcc/100 ];
                 mcc %= 100;
                 txt[1] = mintab[ mcc/10 ];
                 txt[2] = mintab[ mcc%10 ];
                 txt[3] = 0;
                 pLocalInformation->locn_info.mcc = (uint16) ATOI(txt);
             }
             {   
                 imsi_11_12 = cur_bs_ptr->csp.esp.imsi_11_12;                  
                 imsi_11_12 %= 100;
                 imsi_11_12 = 10 *(((imsi_11_12 / 10) + 1)%10)+(((imsi_11_12 % 10) + 1)%10);
                 pLocalInformation->locn_info.imsi_11_12 = imsi_11_12;
             }
             { 
             	pLocalInformation->locn_info.sid = cur_bs_ptr->csp.sp.sid;
             	pLocalInformation->locn_info.nid = cur_bs_ptr->csp.sp.nid;   
             	pLocalInformation->locn_info.base_id = cur_bs_ptr->csp.sp.base_id;             	          	
             	pLocalInformation->locn_info.base_lat = cur_bs_ptr->csp.sp.base_lat;
             	pLocalInformation->locn_info.base_long = cur_bs_ptr->csp.sp.base_long;
             }                                                              
           }                           
            break;

        case UIM_TK_LOCAL_INFO_DATE_TIME: // Date, time and time zone  
            pLocalInformation->command_rsp_tag = UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG;                                                
            // Get current date and time
            GetJulianDate(GETTIMESECONDS(), &jDate);                   
            pLocalInformation->date_time.year = jDate.wYear; 
            pLocalInformation->date_time.month = jDate.wMonth; 
            pLocalInformation->date_time.day = jDate.wDay; 
            pLocalInformation->date_time.hour = jDate.wHour; 
            pLocalInformation->date_time.minute = jDate.wMinute; 
            pLocalInformation->date_time.second = jDate.wSecond; 
            pLocalInformation->date_time.zone = 0xff; 
            break;                                          
            
        case UIM_TK_LOCAL_INFO_LANGUAGE: // Language setting     
            pLocalInformation->command_rsp_tag = UIM_TK_LANG_TAG;  
            pLocalInformation->lang.lang = (word)((GetLngCode()& 0xffff));
            break;        
            
        case UIM_TK_LOCAL_INFO_ACCESS_TECH: // Access Technology
            pLocalInformation->command_rsp_tag = UIM_TK_ACCESS_TECHNOLOGY_TAG;
            pLocalInformation->access_technology = UIM_TK_ACCESS_TECH_TIA_EIA_IS2000; 
            break;                                         
            
        default:
            break;              
    }                     
}

void DecodePlayToneData(byte *pdata, Play_Tone *pPlayTone)
{
    int pos=0, nValLen;
    if (NULL == pdata || NULL == pPlayTone)
    {
        return;
    }
    
    pPlayTone->command_tag = pdata[pos];
    pos++;
    if (pdata[pos] == 0x81)
    {
        pPlayTone->length = pdata[pos+1];
        pos+=2;
    }
    else
    {
        pPlayTone->length = pdata[pos];
        pos++;
    }
    
    while (pos<pPlayTone->length)
    {
        switch (pdata[pos])
        {
            // 命令说明标识
            case UIM_TK_COMMAND_DETAILS_TAG:
            case 0x81:
                pPlayTone->cmd_describe.describe_id = pdata[pos];
                pPlayTone->cmd_describe.length = pdata[pos+1];
                pPlayTone->cmd_describe.command_num = pdata[pos+2];
                pPlayTone->cmd_describe.command_type = pdata[pos+3];
                pPlayTone->cmd_describe.command_restricttag = pdata[pos+4];
                pos+=5;
                break;
                
            // 设备特性标识
            case UIM_TK_DEVICE_ID_TAG:
            case 0x82:
                pPlayTone->dev_identity.speciality_id = pdata[pos];
                pPlayTone->dev_identity.length = pdata[pos+1];
                pPlayTone->dev_identity.source_speciality = pdata[pos+2];
                pPlayTone->dev_identity.target_speciality = pdata[pos+3];
                pos+=4;
                break;        
                        
            //Alpha标识符标识
            case UIM_TK_ALPHA_ID_TAG:
            case 0x85:  
                pPlayTone->alpha_id.alpha_id_id = pdata[pos];
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pPlayTone->alpha_id.length = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pPlayTone->alpha_id.length = pdata[pos];
                    pos++;
                }                
                MEMSET(pPlayTone->alpha_id.data, 0, 128 * sizeof(AECHAR));
                DecodeAlphaString(&pdata[pos], pPlayTone->alpha_id.length, pPlayTone->alpha_id.data, 128);              
                pos+=(pPlayTone->alpha_id.length);                                        
                break;     
                                
            // 音调标识
            case UIM_TK_TONE_TAG:
            case 0x8E:
                pPlayTone->tone.tone_id = pdata[pos];
                pPlayTone->tone.length = pdata[pos+1];
                pPlayTone->tone.tone = pdata[pos+2]; 
                pos+=3;                                             
                break;

            // 持续时间标识
            case UIM_TK_DURATION_TAG:
            case 0x84:
                // 不支持，跳过
                pPlayTone->duration.duration_id = pdata[pos];
                pPlayTone->duration.length = pdata[pos+1];
                pPlayTone->duration.time_unit = pdata[pos+2];     
                pPlayTone->duration.time_interval = pdata[pos+3];                   
                pos+=4;
                break;
                
            // 图标标识符
            case UIM_TK_ICON_ID_TAG:
            case 0x9E:
                pPlayTone->icon.icon_id = pdata[pos];          
                pPlayTone->icon.length = 2;               
                pPlayTone->icon.qual = pdata[pos+2];        
                pPlayTone->icon.id = pdata[pos+3];      
                pos+=4;                                        
                break;                
                
            // 正文串识
            case UIM_TK_TEXT_STRING_TAG:
            case 0x8D:
                pPlayTone->text.text_id = pdata[pos];
                pos++;
                if (pdata[pos] == 0x81)
                {
                    pPlayTone->text.length = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    pPlayTone->text.length = pdata[pos];
                    pos++;
                }
                pPlayTone->text.encode_schedule = pdata[pos];
                pos++;
                // 解码字符串文本
                UTK_decodetext(pPlayTone->text.data, sizeof(pPlayTone->text.data), 
                        &pdata[pos], pPlayTone->text.length-1, pPlayTone->text.encode_schedule);
                pos+=(pPlayTone->text.length-1);
                break;                
                
            default:
                // 命令不支持的项
                ERR("---Unsupport!----",0,0,0);
                // 跳过标识
                pos++;
                
                if (pdata[pos] == 0x81)
                {
                    nValLen = pdata[pos+1];
                    pos+=2;
                }
                else
                {
                    nValLen = pdata[pos];
                    pos++;
                }
                
                // 跳过值部分
                pos+=nValLen;
                break;
        }
    }
}

