/*==============================================================================
//           W M S   A P P L E T  M O D U L E
// 文件:
//        WMSApp.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息模块主文件
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
#include "WmsApp_priv.h"      // 主Applet头文件
#include "WMSUtil.h"          // Applet工具模块头文件
#include "OEMClassIDs.h"

#ifdef WIN32
void wms_cacheinfolist_getcounts(wms_box_e_type box, 
                                 uint16 *pNew, 
                                 uint16 *pOnUIM,
                                 uint16 *pTotal)
{
	return;
}
void wms_cacheinfolist_enumbegin(wms_box_e_type ebox)
{
	return;
}
wms_cache_info_node *wms_cacheinfolist_enumnext(wms_box_e_type ebox)
{
	return NULL;
}
void wms_cacheinfolist_enumtoxuhao(wms_box_e_type ebox, int xuhao)
{
	return;
}
wms_cache_info_list * wms_get_cacheinfolist(wms_box_e_type ebox)
{
	return NULL;
}
uint8 wms_ts_pack_ascii
(
  const char        * ascii_ptr,       /* IN */
  uint8              * data,            /* OUT */
  uint8              * data_len_ptr,    /* OUT */
  uint8              * padding_bits_ptr /* OUT */
)
{
	return 0;
}
void wms_ts_decode_relative_time
(
  uint8                     v,
  wms_timestamp_s_type    * timestamp
)
{
	return;
}
wms_status_e_type wms_ts_decode
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr
)
{
	return 0;
}
uint8 wms_ts_dtmf2ascii
(
  uint8            len,
  const uint8      *dtmf,
  uint8            *ascii
)
{
	return 0;
}
uint8 wms_ts_unpack_ascii
(
  const wms_cdma_user_data_s_type    *ud,        /* IN */
  uint8    buf_len,                /* IN */
  uint8    *buf                    /* OUT */
)
{
	return 0;
}
wms_cache_info_node *wms_cacheinfolist_getnode(wms_box_e_type  eBox,
                                               wms_memory_store_e_type mem_store, 
                                               uint16 index)
{
	return NULL;
}
void wms_cacheinfolist_updatexuhao(wms_box_e_type ebox)
{
	return;
}
#endif
/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/


/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/


/*==============================================================================

                              本地函数声明

==============================================================================*/
int CWMSMod_Load(IShell  *pIShell, void  *ph, IModule **ppMod);

int CWMSMod_New(int16               nSize, 
                IShell              *pIShell,
                void                *ph,   
                IModule             **ppMod,
                PFNMODCREATEINST    pfnMC, 
                PFNFREEMODDATA      pfnMF);

static uint32 CWMSMod_AddRef(IModule *po);
static uint32 CWMSMod_Release(IModule *po);
static int CWMSMod_CreateInstance(IModule *po, 
                                  IShell  *pIShell,
                                  AEECLSID ClsId, 
                                  void   **ppObj);

static void CWMSMod_FreeResources(IModule  *po, IHeap    *ph, IFileMgr *pfm);

// Applet创建和释放函数
static int CWmsApp_New(IShell *ps, IModule *pIModule, IWmsApp **ppObj);

static uint32  CWmsApp_AddRef  (IWmsApp *pi);
static uint32  CWmsApp_Release (IWmsApp *pi);

// 事件处理函数
static boolean CWmsApp_HandleEvent(IWmsApp  *pi, 
                                   AEEEvent eCode,
                                   uint16   wParam, 
                                   uint32   dwParam);

static int CWmsApp_MessageService(IWmsApp *p, 
                                  WMSServiceType eStype, 
                                  AECHAR *toAddrStr, 
                                  AECHAR *pwstrText);
static int CWmsApp_GetLeftSendListNodeNum(IWmsApp *p, int *leftNum);
static int CWmsApp_DeleteAllNvCdmaSms(IWmsApp *p);

// 内部Applet方法声明
static int CWmsApp_InitAppData(WmsApp *pMe);
static void CWmsApp_FreeAppData(WmsApp *pMe);

// Applet主状态机处理方法
static void CWmsApp_RunFSM(WmsApp *pMe);

// 各种外来通知事件的处理函数
static boolean WmsApp_HandleNotifyEvent(WmsApp *pMe, AEENotify *pN);

// 处理来自 AEECLSID_CM_NOTIFIER 通知事件的函数
static boolean WmsApp_HandleCMNotify(WmsApp * pMe, AEENotify *pNotify);

// 获取群发消息链表
static IVector * WmsApp_GetSendList(void);

static IVector * WmsApp_GetUserDataMOList(void);


static char * CWmsApp_BuildStartArgs(WMSServiceType eStype);

static void WmsApp_ArgsStartInit(WmsApp *pMe, const char *pszArgs);

void WmsApp_CfgCb(wms_cfg_event_e_type event, wms_cfg_event_info_s_type *pInfo);

void WmsApp_MsgCb(wms_msg_event_e_type       event,
                  wms_msg_event_info_s_type  *pInfo,
                  boolean                    *shared);

#ifdef FEATURE_BROADCAST_SMS
void WmsApp_BcCb(wms_bc_event_e_type  event, wms_bc_event_info_s_type  *pInfo);
#endif

#ifdef  FEATURE_CDSMS
void WmsApp_DcCb(wms_dc_event_e_type  event,wms_dc_event_info_s_type  *info_ptr);
#endif

static void WmsApp_Init(WmsApp *pMe);

static void WmsApp_UpdateMemoryStatus(WmsApp *pMe, wms_memory_status_s_type *ptr);

static uint16 WmsApp_GetMsgICONID(wms_cache_info_node * pNode);

static void WmsApp_ReservedMsgTimer(void * pUser);

/*==============================================================================
                              全局数据
==============================================================================*/
// 重要提示::
//        g_dwAEEStdLibEntry全局变量仅用于SDK !
//        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点，不
//        要移动下列三行代码。
#ifdef AEE_SIMULATOR
   uint32 g_dwAEEStdLibEntry;
#ifndef WIN32
   #error The WMS applet was never intended to be used with the AEE Simulator.
#endif
#endif

/*==============================================================================
                              
                              本地（静态）数据
                              
==============================================================================*/
static CWMSMod gWMSMod;

static const VTBL(IModule) gModFuncs =
{
   CWMSMod_AddRef,
   CWMSMod_Release,
   CWMSMod_CreateInstance,
   CWMSMod_FreeResources
};

// 只允许一个 WmsApp 实例。每次创建 WMS Applet 时，返回同一结构指针给 BREW 层。
static WmsApp gWmsApp={0};

static const VTBL(IWmsApp) gWmsAppMethods =
{
   CWmsApp_AddRef,
   CWmsApp_Release,
   CWmsApp_HandleEvent,
   CWmsApp_MessageService,
   CWmsApp_GetLeftSendListNodeNum,
   CWmsApp_DeleteAllNvCdmaSms
};

/*==============================================================================
函数:
    CWMSMod_Load

说明:
    模块装载函数。

参数:
    pIShell [in]:IShell接口指针。
    ph :没有使用
    ppMod [out]:

返回值:
    int

备注:

==============================================================================*/
#ifndef WIN32
#ifdef AEE_SIMULATOR
__declspec(dllexport) int AEEMod_Load(IShell   *pIShell,
                                      void     *ph,
                                      IModule **ppMod)
#else
int CWMSMod_Load(IShell   *pIShell,
                        void     *ph,
                        IModule **ppMod)
#endif
#else
int CWMSMod_Load(IShell   *pIShell,
                        void     *ph,
                        IModule **ppMod)
#endif
{
    // 对于动态Applet，验证AEE STD库版本，以确保库与建立本模块时使用的库版本匹配
    // 。在AEE_SIMULATOR下不必进行本检查。
#if !defined(BREW_STATIC_APP) && !defined(AEE_SIMULATOR)
    if (GET_HELPER_VER() != AEESTDLIB_VER)
    {
        return EVERSIONNOTSUPPORT;
    }
#endif

    return CWMSMod_New(sizeof(CWMSMod), pIShell, ph, ppMod, NULL, NULL);
}

/*==============================================================================
函数:
    CWMSMod_New

说明:
    用来初始化 WMS 模块虚拟表（也增加引用计数）的助手函数。

参数:
    nSize: 没有使用
    pIShell [in]: IShell 接口指针。
    ph : 没有使用
    ppMod [out]:
    pfnMC: 没有使用
    pfnMF: 没有使用

返回值:
    int

备注:

==============================================================================*/
int CWMSMod_New(int16           nSize,
                IShell          *pIShell,
                void            *ph,
                IModule         **ppMod,
                PFNMODCREATEINST pfnMC,
                PFNFREEMODDATA   pfnMF)
{
    if (!ppMod)
    {
        return EFAILED;
    }

    *ppMod = NULL;

    // 重要提示:
    //        g_dwAEEStdLibEntry全局变量仅用于SDK !
    //        BREW SDK用户不能改写和直接使用本变量。它用作AEEStdLib的一个入口点
    //        。
#ifdef AEE_SIMULATOR
    g_dwAEEStdLibEntry = (uint32)ph;
    if (!pIShell || !ph)
    {
        return EFAILED;
    }
#else
    if (!pIShell)
    {
        return EFAILED;
    }
#endif

    MEMSET(&gWMSMod,0,sizeof(CWMSMod));

    INIT_VTBL(&gWMSMod,IModule,gModFuncs);

    gWMSMod.m_nRefs = 1;

    *ppMod = (IModule *)&gWMSMod;

    return AEE_SUCCESS;
}

/*==============================================================================
函数:
    CWMSMod_AddRef

说明:
    WMS 模块引用计数加1，并返回当前的引用数。

参数:
    po [in/out]:IModule接口对象指针。

返回值:
    当前对SounMenu 模块的引用数

备注:

==============================================================================*/
static uint32 CWMSMod_AddRef(IModule *po)
{
    return(++((CWMSMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
    CWMSMod_Release

说明:
    WMS 模块引用计数减1，并返回当前的引用数。

参数:
    po [in/out]:IModule接口对象指针。

返回值:
    当前对SounMenu 模块的引用数

备注:

==============================================================================*/
static uint32 CWMSMod_Release(IModule *po)
{
    if (((CWMSMod *) po)->m_nRefs == 0)
    {
        return 0;
    }

    return(--((CWMSMod *)po)->m_nRefs);
}

/*==============================================================================
函数:
    CWMSMod_CreateInstance

说明:
    通过创建(初始化)WMS对象来创建模块实例。

参数:
    po [in]:IModule接口对象指针。
    pIShell [in]:IShell接口对象指针。
    ClsId [in]:要创建的实例的类ID。
    ppObj [out]::用于保存对象实例的指针的指针变量。

返回值:
    none

备注:
    不需用户去调用。

==============================================================================*/
static int CWMSMod_CreateInstance(IModule   *po,
                                  IShell    *pIShell,
                                  AEECLSID  ClsId,
                                  void      **ppObj)
{
    *ppObj = NULL;

    if (ClsId != AEECLSID_WMSAPP)
    {
        return EFAILED;
    }

    if (CWmsApp_New(pIShell, po, (IWmsApp**)ppObj) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*==============================================================================
函数:
    CWMSMod_FreeResources

说明:
    释放WMS模块资源的函数。

参数:
    po :没有使用。
    IHeap :没有使用。
    pfm ::没有使用。

返回值:
    none

备注:
    不需用户去调用。

==============================================================================*/
static void CWMSMod_FreeResources(IModule *po, IHeap  *ph, IFileMgr *pfm)
{
    ;
}

/*==============================================================================
函数:
    CWmsApp_New

说明:
    初始化 WMS Applet 虚拟表和内部数据变量，同时 WMS Applet 引用计数加1。

参数:
    ps [in]: IShell接口对象指针。
    pIModule [in/out]: pIModule接口对象指针。
    ppObj [out]: 指向 IWmsApp 对象指针的指针。

返回值:
    EFAILED: 失败。
    SUCCESS: 成功。
       
备注:
    不需用户去调用。
    关于Applet结构对象可使用动态分配内存的方式创建，本处采用的是使用静态
    变量的方式。建议对结构比较大的Applet使用静态变量的方式。对于动态分配
    内存的方式需要注意对所分配空间的释放。

==============================================================================*/
static int CWmsApp_New(IShell *ps, IModule *pIModule, IWmsApp **ppObj)
{
    int retVal = SUCCESS;

    if (0 == gWmsApp.m_nRefs)
    {
        char *pszArgs = gWmsApp.m_pszArgs;
        MEMSET(&gWmsApp, 0, sizeof(gWmsApp));
        gWmsApp.m_pszArgs = pszArgs;
        INIT_VTBL(&gWmsApp, IWmsApp, gWmsAppMethods);

        gWmsApp.m_nRefs     = 0;
        gWmsApp.m_pShell    = ps;
        gWmsApp.m_pModule   = pIModule;

        (void) ISHELL_AddRef(ps);
        (void) IMODULE_AddRef(pIModule);

        retVal = CWmsApp_InitAppData(&gWmsApp);
    }

    ++gWmsApp.m_nRefs;

    *ppObj = (IWmsApp*)&gWmsApp;

    return retVal;
}

/*==============================================================================
函数:
    CWmsApp_AddRef

说明:
    WMS Applet 引用计数加1，并返回当前的引用数。

参数:
    pi [in/out]: IWmsApp 接口对象指针。

返回值:
    当前对 WMS Applet 的引用数

备注:

==============================================================================*/
static uint32 CWmsApp_AddRef(IWmsApp *p)
{
    register WmsApp *pMe = (WmsApp*)p;

    return (++pMe->m_nRefs);
}

/*==============================================================================
函数:
    CWmsApp_Release

说明:
    释放对 WMS Applet 的引用，引用计数减1，并返回当前的引用数。
    如果当前的引用数为零，还将释放 Applet 及相关资源。

参数:
    pi [in/out]: IWmsApp 接口对象指针。

返回值:
    当前对 WMS Applet 的引用数

备注:

==============================================================================*/
static uint32 CWmsApp_Release(IWmsApp *p)
{
    register WmsApp *pMe = (WmsApp*)p;

    if (pMe->m_nRefs == 0)
    {
        return 0;
    }

    if (--pMe->m_nRefs)
    {
        return pMe->m_nRefs;
    }

    CWmsApp_FreeAppData(pMe);

    (void) ISHELL_Release(pMe->m_pShell);
    (void) IMODULE_Release(pMe->m_pModule);

    // 注意:pMe是静态分配空间，因此不需要释放。FREE()
    return 0;
}

/*==============================================================================
函数:
    CWmsApp_InitAppData

说明:
    初始化 WMS Applet 结构数据缺省值。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    EFAILED: 初始化失败。
    SUCCESS: 初始化成功。

备注:

==============================================================================*/
static int CWmsApp_InitAppData(WmsApp *pMe)
{
	int rt;
    if (NULL == pMe)
    {
        return EFAILED;
    }
    
    // 初始化各成员变量
    pMe->m_prevState = WMSST_NONE;
    pMe->m_currState = WMSST_MAIN;
    pMe->m_STSwitchToEditMsg = WMSST_NONE;
    pMe->m_eDlgReturn = DLGRET_CREATE;
    pMe->m_eAppStatus = WMSAPP_RUNNING;
    pMe->m_eMBoxType = WMS_MB_NONE;
    pMe->m_eCreateWMSType = SEND_MSG_NEW;
    pMe->m_SendStatus = WMS_RPT_OK;
    pMe->m_bNaturalStart = TRUE;
    pMe->m_CopyType = NONECOPY;
    pMe->m_eMakeListMode = MAKEMSGLIST_INIT;
    pMe->m_eInsertType = INSERT_NONE; 
    pMe->m_strPhonePWD = NULL;
    
    // 取保存的配置信息
    WmsApp_GetPrefs(pMe);
    if (pMe->m_cfg.nInitIndictor != WMS_CFG_VERSION) // 当前版本未初始化
    {
       // 设置初始化标记
       pMe->m_cfg.nInitIndictor = WMS_CFG_VERSION;
       
       pMe->m_cfg.m_bAutoSave = TRUE;
       
       pMe->m_cfg.m_bAutoDelete = FALSE;
       
#ifdef FEATURE_SMS_UDH
       pMe->m_cfg.m_msg_ref = 1;
#endif    
    
       pMe->m_cfg.m_msgid = 1;
       // 保存配置
       WmsApp_StorePrefs(pMe);
    }
    
    // 获取群发链表
    pMe->m_pSendList = WmsApp_GetSendList();
    if (NULL == pMe->m_pSendList)
    {
        MSG_ERROR ("m_pSendList could not be created", 0, 0, 0);
        return EFAILED;
    }
    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_SOUND, (void**)&pMe->m_pSound) != SUCCESS)
    {
        return EFAILED;
    }
    // 获取消息用户数据链表
    pMe->m_pUserDataMOList = WmsApp_GetUserDataMOList();
    if (NULL == pMe->m_pUserDataMOList)
    {
        MSG_ERROR ("m_pUserDataMOList could not be created", 0, 0, 0);
        return EFAILED;
    }
    
    // 接口创建及相关初始化
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG,
            (void **)&pMe->m_pConfig) != SUCCESS)
    {
        ERR("Failed to create AEECLSID_CONFIG",0,0,0);
        CWmsApp_FreeAppData(pMe);
        return EFAILED;
    }

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_VECTOR,
            (void**)&pMe->m_pSaveNumList) != SUCCESS )
    {
        ERR("Failed to Create AEECLSID_IVECTOR",0,0,0);
        pMe->m_pSaveNumList = NULL;
        CWmsApp_FreeAppData(pMe);
        return EFAILED;
    }
    (void) IVector_EnsureCapacity(pMe->m_pSaveNumList, 60);
    
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_WMS, 
            (void **) &pMe->m_pwms) != SUCCESS)
    {
        MSG_ERROR ("pMe->m_pwms could not be created", 0, 0, 0);
        return EFAILED;
    }
    
#ifdef FEATURE_ICM
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CALLMANAGER, 
            (void **) &pMe->m_pICM) != SUCCESS)
    {
        MSG_ERROR ("pMe->m_pICM could not be created", 0, 0, 0);
        return EFAILED;
    }
#endif    

    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, 
            (void **) &pMe->m_pIAnn) != SUCCESS)
    {
        MSG_ERROR ("pMe->m_pIAnn could not be created", 0, 0, 0);
        return EFAILED;
    }
    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
	DBGPRINTF("IANNUNCIATOR_SetFieldIsActiveEx::::wmsapp111:::");
	MSG_FATAL("IANNUNCIATOR_SetFieldIsActiveEx::::wmsapp111:::",0,0,0);
    //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    // Create IAlert instance
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ALERT, 
            (void **) &pMe->m_pAlert) != SUCCESS)
    {
        MSG_ERROR ("pMe->m_pAlert could not be created", 0, 0, 0);
        return EFAILED;
    }
    
    // Create IDisplay instance
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_DISPLAY, 
            (void **) &pMe->m_pDisplay) != SUCCESS)
    {
        MSG_ERROR ("pMe->m_pDisplay could not be created", 0, 0, 0);
        return EFAILED;
    }

    if( ISHELL_CreateInstance( pMe->m_pShell, AEECLSID_BACKLIGHT, (void **)&pMe->m_pBacklight)!=AEE_SUCCESS)
    {
        CWmsApp_FreeAppData(pMe);
        return EFAILED;
    }
    CALLBACK_Init(&pMe->m_callback, 
                  (PFNNOTIFY) WmsApp_wmscommandreport,
                  (void*)pMe);
    
    // Register Callbacks
    (void)IWMS_Init(pMe->m_pwms, WMS_CLIENT_TYPE_WMS_APP, &pMe->m_clientId);
    (void)IWMS_RegisterCfgCb(pMe->m_pwms, pMe->m_clientId, WmsApp_CfgCb );
    (void)IWMS_RegisterMsgCb(pMe->m_pwms, pMe->m_clientId, WmsApp_MsgCb );

#ifdef FEATURE_BROADCAST_SMS
    (void)IWMS_RegisterBcCb(pMe->m_pwms, pMe->m_clientId, WmsApp_BcCb);
#endif

#ifdef FEATURE_CDSMS
    (void)IWMS_RegisterDcCb(pMe->m_pwms, pMe->m_clientId, WmsApp_DcCb);
#endif

    (void)IWMS_Activate(pMe->m_pwms,pMe->m_clientId);
    

    return SUCCESS;
}

/*==============================================================================
函数:
    CWmsApp_FreeAppData

说明:
    释放 WMS Applet 相关资源。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
static void CWmsApp_FreeAppData(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return;
    }
    
    pMe->m_eAppStatus = WMSAPP_STOP;
    FREEIF(pMe->m_strPhonePWD);

    if (NULL != pMe->m_pConfig)
    {
        ICONFIG_Release(pMe->m_pConfig);
        pMe->m_pConfig = NULL;
    }

    //------------------------------------------------------------------------------------------------------
    if (NULL != pMe->m_pwms)
    {
        IWMS_Deactivate(pMe->m_pwms,pMe->m_clientId);
        IWMS_Release(pMe->m_pwms);
        pMe->m_pwms = NULL;
    }
    
#ifdef FEATURE_ICM
    if (NULL != pMe->m_pICM)
    {
        ICM_Release(pMe->m_pICM);
        pMe->m_pICM = NULL;
    }
#endif  

    if (pMe->m_pIAnn) 
    {
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
    if(pMe->m_pSound != NULL)
    {
        ISOUND_Release(pMe->m_pSound);
        pMe->m_pSound = NULL;
    }
    if (pMe->m_pAlert != NULL)
    {
        IALERT_Release(pMe->m_pAlert);
        pMe->m_pAlert = NULL;
    }
    
    if (pMe->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pMe->m_pDisplay);
        pMe->m_pDisplay = NULL;
    }
    
    // 注意顺序
    WmsApp_FreeSaveNumList(pMe);
    if (NULL != pMe->m_pSaveNumList)
    {
        IVector_Release(pMe->m_pSaveNumList);
        pMe->m_pSaveNumList = NULL;
    }
    
    WmsApp_FreeMultiSendList(pMe->m_pSendList);
    if(pMe->m_pBacklight)
    {
        IBACKLIGHT_Release(pMe->m_pBacklight);
        pMe->m_pBacklight=NULL;
    }
    FREEIF(pMe->m_msSend.m_szMessage);
}

/*==============================================================================
函数:
    CWmsApp_HandleEvent

说明:
    WMS Applet事件处理主函数。所有属于本Applet的事件，都将经由该函数处理。

参数:
    pi [in]:指向IWmsApp接口t对象的指针。
    eCode:BREW事件代码。
    wParam:事件参数
    dwParam [in]:与wParam关联的数据。。

返回值:
    TRUE:传入事件被处理。
    FALSE:传入事件没被处理。

备注:
    该函数供BREW调用，用户不得调用。
        
==============================================================================*/
static boolean CWmsApp_HandleEvent(IWmsApp  *pi,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    WmsApp *pMe = (WmsApp*)pi;
    AEEAppStart *as=NULL;
    IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    switch (eCode)
    {
        case EVT_APP_START:
            // 此事件dwParam为指针，不应为0
            if (dwParam == 0) 
            {
                return FALSE;
            }
            as = (AEEAppStart*)dwParam;
    
            pMe->m_eAppStatus = WMSAPP_RUNNING;
    
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }
            
            pMe->m_currState = WMSST_MAIN;
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_bNaturalStart = TRUE;

#ifdef WIN32
			pMe->m_bCdmaWmsReady = TRUE;
			pMe->m_refresh_in_progress = 0;
#endif
            
            if (((pMe->m_bCdmaWmsReady == FALSE) || pMe->m_refresh_in_progress))
            {
                // 消息没初始化完毕,不允许进行相关操作.
                pMe->m_eDlgReturn = DLGRET_CREATE;
                
                pMe->m_currState = WMSST_INIT;
                if (pMe->m_pszArgs != NULL)
                {
                    if (STARTARGPREFIX_POWERONSTART == *pMe->m_pszArgs)
                    {
                        pMe->m_currState = WMSST_EXIT;
                    }
                    sys_free(pMe->m_pszArgs);
                    pMe->m_pszArgs = NULL;
                }
            }
            else if (pMe->m_pszArgs != NULL)
            {
                pMe->m_bNaturalStart = FALSE;
                
                WmsApp_ArgsStartInit(pMe, pMe->m_pszArgs);
                
                sys_free(pMe->m_pszArgs);
                pMe->m_pszArgs = NULL;
            }
            else if (pMe->m_bNeedContinueSend)
            {
                MOVE_TO_STATE(WMSST_CONTINUESEND_QUERY)
                pMe->m_stcontinuesendbk = WMSST_MAIN;
            }
            
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            if (pMe->m_currState != WMSST_EXIT)
            {
                pMe->m_currState = WMSST_MAIN;
            }
#endif
            if(pMe->m_bNaturalStart == TRUE)
            {
                boolean bsmslock;
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_SMS_LOCK_CHECK,
                                       &bsmslock,
                                       sizeof(bsmslock));
                bsmslock = FALSE;
                if(bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    pMe->m_stchkpwdbk = WMSST_MAIN;                    
                }
            }
            // 开始跑WMS状态机
            CWmsApp_RunFSM(pMe);
            return TRUE;

        case EVT_APP_STOP:
            pMe->m_bNaturalStart = TRUE;
            pMe->m_eAppStatus = WMSAPP_STOP;
    
            if(pMe->m_pImage)
            {
                IIMAGE_Release(pMe->m_pImage);
                pMe->m_pImage = NULL;
            }
        
            // 转为后台模式
            *((boolean *)dwParam) = FALSE;
            
            // 在这里作必要的变量重置工作
            WmsApp_MemberReset(pMe);
            pMe->m_wMainMenuSel = 0;//将主菜单的重置工作分割出来，第一次进入短信模块使光标停留在第一项上
            return TRUE;

        case EVT_APP_SUSPEND:
            pMe->m_eAppStatus = WMSAPP_SUSPEND;
            return TRUE;
    
        case EVT_APP_RESUME:
            if (dwParam == 0) 
            {
                return FALSE;
            }
            as = (AEEAppStart*)dwParam;
    
            pMe->m_eAppStatus = WMSAPP_RUNNING;
            
            pMe->m_rc = as->rc;
            {
                AEEDeviceInfo di; 
                
                ISHELL_GetDeviceInfo(pMe->m_pShell,&di);
                pMe->m_rc.dy = di.cyScreen;
            }
            pMe->m_bNaturalStart = TRUE;
            
            if ((pMe->m_bCdmaWmsReady == FALSE) || pMe->m_refresh_in_progress)
            {
                // 消息没初始化完毕,不允许进行相关操作.
                pMe->m_eDlgReturn = DLGRET_CREATE;
                
                pMe->m_currState = WMSST_INIT;
                if (pMe->m_pszArgs != NULL)
                {
                    sys_free(pMe->m_pszArgs);
                    pMe->m_pszArgs = NULL;
                    if (STARTARGPREFIX_POWERONSTART == *pMe->m_pszArgs)
                    {
                        pMe->m_currState = WMSST_EXIT;
                    }
                }
            }
            else if (pMe->m_pszArgs != NULL)
            {
                pMe->m_bNaturalStart = FALSE;
                WmsApp_ArgsStartInit(pMe, pMe->m_pszArgs);
                
                sys_free(pMe->m_pszArgs);
                pMe->m_pszArgs = NULL;
            }
            else if (pMe->m_bNeedContinueSend)
            {
                MOVE_TO_STATE(WMSST_CONTINUESEND_QUERY)
                pMe->m_stcontinuesendbk = WMSST_MAIN;
            }
            else if (pMe->m_currState == WMSST_WRITEMSG)
            {
                if (pMe->m_bTextFullAlert)
                {
                    pMe->m_eDlgReturn = DLGRET_TEXTFULL;
                    pMe->m_bTextFullAlert = FALSE;
                }
            }
            
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
            pMe->m_currState = WMSST_MAIN;
#endif
            if(pMe->m_isPopMenu)
            {
                pMe->m_eDlgReturn = DLGRET_CANCELED;
            }
            
            // 开始跑WMS状态机
            CWmsApp_RunFSM(pMe); 
            return TRUE;

        case EVT_DIALOG_INIT:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            pMe->m_pActiveIDlg = (IDialog*)dwParam;
            pMe->m_wActiveDlgID = wParam;
            WmsApp_SetDialogHandler(pMe);
            
            // Route the received event to the current active dialog handler.
            return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_DIALOG_START:
			if(OEM_IME_DIALOG == wParam)
			{
				return TRUE;
			}
            (void) WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);

            return TRUE;
            
        case EVT_DIALOG_END:
			if(OEM_IME_DIALOG == wParam)
			{
				return ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER_REDRAW,0,0);
			}
            if (wParam == 0)
            {
                return TRUE;
            }
    
            (void) WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    
            pMe->m_pActiveIDlg = NULL;
            pMe->m_wActiveDlgID = 0;
            pMe->m_pDialogHandler = NULL;
    
            // 事件EVT_APP_SUSPEND先于EVT_DIALOG_END事件发出，因此
            // ，对于Applet处于挂起状态时，不跑状态机。
            if (pMe->m_eAppStatus == WMSAPP_RUNNING)
            {                
                // 开始跑WMS状态机
                CWmsApp_RunFSM(pMe);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            return TRUE;

        case EVT_UPDATE:
            (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            return TRUE;
            
        case EVT_KEY:
#if MIN_BREW_VERSION(3,0)
            // do not want to handle au
            if (((dwParam & KB_AUTOREPEAT) != 0) &&
                (wParam != AVK_UP) &&
                (wParam != AVK_DOWN) &&
                (wParam != AVK_LEFT) &&
                (wParam != AVK_RIGHT))
            {
                return TRUE;
            }
#endif
            return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            
        case EVT_COMMAND:        	
        case EVT_KEY_PRESS:
        case EVT_KEY_RELEASE:
            return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);

        case EVT_BUSY:
            // The applet is never 'busy'
            break;
            
        case EVT_NOTIFY:
            return WmsApp_HandleNotifyEvent(pMe, (AEENotify*)dwParam);
       
        case EVT_WMS_CFG_MEMORY_STATUS:
            WmsApp_UpdateMemoryStatus(pMe,&((wms_cfg_event_info_s_type *)dwParam)->memory_status);
            WMSAPPU_FREE(dwParam);
            WmsApp_UpdateAnnunciators(pMe);
            return TRUE;
        
        // 消息初始化完成
        case EVT_WMS_CFG_CDMA_READY:
            pMe->m_mode = WMS_MESSAGE_MODE_CDMA;
            WmsApp_Init(pMe);
            WMSAPPU_FREE(dwParam)
            return TRUE;
            
        case EVT_WMS_CFG_REFRESH_DONE:
            pMe->m_refresh_in_progress = FALSE;
            WMSAPPU_FREE(dwParam)
            return TRUE;
            
        case EVT_WMS_CFG_ROUTES:
            {
                wms_cfg_event_info_s_type *Info = (wms_cfg_event_info_s_type*)dwParam;
                
                (void)MEMCPY(&pMe->m_routes, &Info->routes, sizeof(pMe->m_routes));
            }
            WMSAPPU_FREE(dwParam)
            return TRUE;
            
        case EVT_WMS_MSG_WRITE:
        case EVT_WMS_MSG_READ:
        case EVT_WMS_MSG_WRITE_TEMPLATE:
        case EVT_WMS_MSG_READ_TEMPLATE:
            (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            WMSAPPU_FREE(dwParam)

            return TRUE;
            
        case EVT_WMS_MSG_MODIFY_TAG:
            WMSAPPU_FREE(dwParam)

            // 更新图标
            WmsApp_UpdateAnnunciators(pMe);
            
            return TRUE;
            
        case EVT_WMS_MSG_SUBMIT_REPORT:
        case EVT_WMS_MSG_SEND:
            if (((wms_msg_event_info_s_type *)dwParam)->submit_report_info.client_id == WMS_CLIENT_TYPE_WMS_APP)
            {
                WmsApp_ProcessStatus(pMe, &((wms_msg_event_info_s_type *)dwParam)->submit_report_info);
            }
            FREE((wms_msg_event_info_s_type*)dwParam);
            return TRUE;
            
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            {
                wms_msg_event_info_s_type *info = ((wms_msg_event_info_s_type*)dwParam);
                boolean bRet;
#ifdef FEATURE_SMS_UDH
                boolean bUDHPortPresent = FALSE;
                int nRet;
#endif
                
                MSG_MED("WMSApp received new message!",0,0,0);
                
                // 非 CDMA 模式消息不予受理
                if (info->mt_message_info.message.msg_hdr.message_mode != WMS_MESSAGE_MODE_CDMA)
                {
                    WMSAPPU_FREE(dwParam);
                    return TRUE;
                }
                
                bRet = IWMS_CfgCheckWapPushMsg(pMe->m_pwms, 
                            &info->mt_message_info.message);
                if (bRet == TRUE)
                {
                    // Wms Applet 不处理此消息
                    MSG_HIGH("WAP PUsh Message Recieved and Dropped", 0, 0, 0);
                    WMSAPPU_FREE(dwParam);
                    return TRUE;
                }
                
#ifdef FEATURE_SMS_UDH
                // 检查消息是否带 Port 头部结构，待该结构的消息归特定应用处理
                (void)MEMSET(&pMe->m_CltTsdata, 0 , sizeof(pMe->m_CltTsdata));
                    
                nRet = IWMS_TsDecode(pMe->m_pwms, 
                             &info->mt_message_info.message.u.cdma_message.raw_ts, 
                             &pMe->m_CltTsdata);
                             
                if (nRet == SUCCESS)
                {
                    if (pMe->m_CltTsdata.u.cdma.message_id.udh_present)
                    {
                        uint8 i=0;
                        
                        for (i=0; i<pMe->m_CltTsdata.u.cdma.user_data.num_headers; i++)
                        {
                            if ((pMe->m_CltTsdata.u.cdma.user_data.headers[i].header_id == WMS_UDH_PORT_8) ||
                                (pMe->m_CltTsdata.u.cdma.user_data.headers[i].header_id == WMS_UDH_PORT_16))
                            {
                                bUDHPortPresent = TRUE;
                                break;
                            }
                        }
                    }
                }
                    
                if (bUDHPortPresent)
                {
                    // 丢弃不属于受理范围内的消息
                    MSG_HIGH("Message with UDH Port Header Recieved and Dropped", 0, 0, 0);
                    WMSAPPU_FREE(dwParam);
                    return TRUE;
                }
#endif // FEATURE_SMS_UDH
            }
            // 注意：接着在 EVT_WMS_MSG_STATUS_REPORT 处理
        case EVT_WMS_MSG_STATUS_REPORT:
            {
                wms_msg_event_info_s_type *info = ((wms_msg_event_info_s_type*)dwParam);
                
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM                
                if (info->mt_message_info.message.u.cdma_message.teleservice == 
                    WMS_TELESERVICE_CHINAUNICOMREG)
                {
                    int nRet;
                    
                    (void)MEMSET(&pMe->m_CltTsdata, 0 , sizeof(pMe->m_CltTsdata));
                    
                    nRet = IWMS_TsDecode(pMe->m_pwms, 
                                 &info->mt_message_info.message.u.cdma_message.raw_ts, 
                                 &pMe->m_CltTsdata);
                                 
                    if ((nRet == SUCCESS) &&
                        (pMe->m_CltTsdata.u.cdma.mask & WMS_MASK_BD_USER_DATA))
                    {
                        int len = 0;
                        uint8 temp[8], *buf = temp;
                        wms_cdma_user_data_s_type *user_data = &pMe->m_CltTsdata.u.cdma.user_data;

                        if ((user_data->encoding == WMS_ENCODING_IA5) || 
                            (user_data->encoding == WMS_ENCODING_ASCII))
                        {
                            len = wms_ts_unpack_ascii(user_data, 6, (byte *)buf);
                        }
                        else if (user_data->encoding == WMS_ENCODING_OCTET)
                        {
                            len = user_data->data_len;
                            buf = user_data->data;
                        }

                        if ((len == 2) && (buf[0] == 0x01) && (buf[1] == 0x04))
                        {// 注册成功确认消息
                        
                             // 发送事件给 Idle，令其更新注册状态
                            (void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_CORE_APP,
                                        EVT_UPDATEREGSTATUS,
                                        1,
                                        0);    
                             ERR("ISHELL_PostEvent EVT_UPDATEREGSTATUS",0,0,0);
                        }
                    }
                    
                    FREE(info);
                    return TRUE;
                }
#endif

                if (info->mt_message_info.route == WMS_ROUTE_STORE_AND_NOTIFY)
                {
                    boolean bSendEvt = TRUE;

                    
                    if (info->mt_message_info.message.u.cdma_message.teleservice == WMS_TELESERVICE_VMN_95 ||
                        info->mt_message_info.message.u.cdma_message.teleservice == WMS_TELESERVICE_MWI)
                    {
                        uint16 nVmNews = 0;
                        
                        wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nVmNews, NULL, NULL);
                        if (nVmNews == 0)
                        {
                            bSendEvt = FALSE;
                        }
                    }
                    if (bSendEvt)
                    {
#ifdef FEATURE_FM_RADIO
                        // 通知收音机关闭声音
                        (void) ISHELL_PostEvent( pMe->m_pShell,
                                                 AEECLSID_APP_FMRADIO,
                                                 EVT_WMS_PROCESS_NEW_MSG,
                                                 1,
                                                 0);
#endif                                             

                        // 语音提示用户有消息
                        WmsApp_PlaySMSAlert(pMe, TRUE);
                        
                        // 通知 CoreApp 需要进行短信提示
                        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                                 EVTFLG_ASYNC, 
                                                 AEECLSID_CORE_APP, 
                                                 EVT_WMS_MSG_RECEIVED_MESSAGE,
                                                 0, 
                                                 0);
                    }
                    
                    (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
                }
                FREE(info);
            }
            
            // 更新图标
            WmsApp_UpdateAnnunciators(pMe);
            return TRUE;
            
        // 删除消息
        case EVT_WMS_MSG_DELETE:
        case EVT_WMS_MSG_DELETE_TEMPLATE:
        case EVT_WMS_MSG_DELETE_BOX:
            (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            WMSAPPU_FREE(dwParam)

            // 更新图标
            WmsApp_UpdateAnnunciators(pMe);
            
            return TRUE;
            
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsapp_cmd_status_type  *pStatus = (wmsapp_cmd_status_type *)dwParam;
                
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                if ((pStatus->cmd == WMS_CMD_MSG_SEND) && (WMS_CMD_ERR_NONE != pStatus->cmd_err))
                {
                    wms_submit_report_info_s_type *pInfo = MALLOC(sizeof(wms_submit_report_info_s_type));
                    
                    ERR("Command WMS_CMD_MSG_SEND Failed because of %d!", pStatus->cmd_err, 0, 0);
                    
                    if (NULL != pInfo)
                    {
                        pInfo->report_status = WMS_RPT_GENERAL_ERROR;
                        
                        pInfo->cause_info.tl_status = WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_LAST_S;
                        WmsApp_ProcessStatus(pMe, pInfo);
                        FREE(pInfo);
                    }
                    
                    return TRUE;
                }
            }
            
            (void)WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
            WMSAPPU_FREE(dwParam)

            return TRUE;
            
        case EVT_WMS_CFG_MEMORY_FULL:
            if (pMe->m_pwms)
            {
#ifdef FEATURE_FM_RADIO
                // 通知收音机关闭声音
                (void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_APP_FMRADIO,
                                        EVT_WMS_PROCESS_NEW_MSG,
                                        2,
                                        0);
#endif                                        
 
                // 语音提示用户有消息
                WmsApp_PlaySMSAlert(pMe, TRUE);
            }
            WMSAPPU_FREE(dwParam)
            return TRUE;
            
        case EVT_WMS_DC_DISCONNECTED:
            if (pMe->m_bSending)
            {
                pMe->m_bDCDisconnectedInSending = TRUE;
            }
            else
            {
                pMe->m_bDCDisconnectedInSending = FALSE;
            }
            WMSAPPU_FREE(dwParam)
            return TRUE;

        // 预约短信时间到通知事件
        case EVT_RESERVEDMSGALERT:
            {
                uint16 nNews=0;
                
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, NULL);
                
                if (nNews == 0)
                {
                    return TRUE;
                }
                
                if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_WMSAPP)
                {
                    char *str = NULL;
                    
                    str = (char *)sys_malloc(2);
                    if (str)
                    {
                        *str = STARTARGPREFIX_RESERVEDMSGSTART;
                        
                        if (NULL != pMe->m_pszArgs)
                        {
                            sys_free(pMe->m_pszArgs);
                        }
                        pMe->m_pszArgs = str;
                        
                        (void)ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
                    }
                }
#ifdef FEATURE_CDSMS_RUIM                
                else if (pMe->m_currState == WMSST_MSGCOPY) // 根据spec定义，如果在复制中，就延时10秒再提示 
                {
                    ISHELL_SetTimer(pMe->m_pShell, 10000, WmsApp_ReservedMsgTimer, pMe);
                    return TRUE;
                }
#endif                    
                else if (NULL != pMe->m_pActiveIDlg)
                {
                    MOVE_TO_STATE(WMSST_RESERVEDMSGALERT)
                    CLOSE_DIALOG(DLGRET_CREATE)
                }
                
                IBACKLIGHT_Enable(pMe->m_pBacklight);
                
#ifdef FEATURE_FM_RADIO
                // 通知收音机关闭声音
                (void) ISHELL_PostEvent(pMe->m_pShell,
                                        AEECLSID_APP_FMRADIO,
                                        EVT_WMS_PROCESS_NEW_MSG,
                                        3,
                                        0);
#endif                                        
            }
            return TRUE;
        
#ifdef FEATURE_CARRIER_TAIWAN_APBW    
        case EVT_CALLVOICEMAILNUM:
            if ((pMe->m_bCdmaWmsReady == FALSE) || pMe->m_refresh_in_progress)
            {
                // 消息没初始化完毕,不允许进行相关操作.
                return TRUE;
            }
            else
            {
                wms_cache_info_node  *pnode = NULL;
                
                pnode = wms_cacheinfolist_getnode(WMS_MB_VOICEMAIL, WMS_MEMORY_STORE_NV_CDMA, 0);
                if ((NULL != pnode) && (pnode->msg_tag == WMS_TAG_MT_NOT_READ))
                {
                    (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                            pMe->m_clientId,
                                            &pMe->m_callback,
                                            (void *)pMe,
                                            pnode->mem_store,
                                            pnode->index,
                                            WMS_TAG_MT_READ);
                }
            }
            
            return TRUE;
#endif
        
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch

		case EVT_PEN_UP:
		case EVT_PEN_DOWN:
			{
				return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
			}

		case EVT_USER:
			{
				if((wParam == AVK_SELECT) || (wParam == AVK_INFO))
				{
					if(dwParam != 0)
					{
						eCode = EVT_COMMAND;
						wParam = dwParam;
						dwParam = 0;
					}
					else
					{
						eCode = EVT_KEY;
					}
				}
				else if(wParam == AVK_CLR)
				{
					eCode = EVT_KEY;
				}
				else if(wParam == AVK_DOWN)//wlh 换桌面图片
				{
					eCode = EVT_KEY;
				}
				else if (wParam == AVK_POUND)
				{
					eCode = EVT_KEY_PRESS;
				}
				return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
			}
#endif              
        default:
            // 将事件路由至当前活动对话框处理函数
            return WmsApp_RouteDialogEvt(pMe,eCode,wParam,dwParam);
    }

    return FALSE;
} // CWmsApp_HandleEvent()

//------------------------------------------------------------------------------
// 公共助手函数定义 (用于WMS其它模块 )
//------------------------------------------------------------------------------
/*==============================================================================
函数:
    WmsApp_UpdateMenuList

说明:
    更新当前 pMe->m_eMBoxType 指向的邮箱的消息菜单列表。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in/out]: IMenuCtl接口指针。

返回值:
    none

备注:
    New
        
==============================================================================*/
void WmsApp_UpdateMenuList(WmsApp *pMe, IMenuCtl *pMenu)
{
    wms_cache_info_list   *pList = NULL;
    wms_cache_info_node  *pnode = NULL;
    int i, nItems;
    uint16  wItemCount;
    uint16  wTotalPages;
    AECHAR  wszFmt[10]={0};
    AECHAR  wszName[MAX_TITLE_LEN];
    AECHAR  wszTitle[MAX_TITLE_LEN + 10];
    AECHAR  wstrNum[MAX_PH_DIGITS+1];
    CtlAddItem  mai;
    uint16      wSelectItemID=0;
    boolean     bFindCurxuhao = FALSE;
    
    if ((NULL == pMe) || (NULL == pMenu))
    {
        return;
    }
    
    switch (pMe->m_eMBoxType)
    {
        case WMS_MB_OUTBOX:
        case WMS_MB_INBOX:
        case WMS_MB_DRAFT:
        case WMS_MB_RESERVE:
        case WMS_MB_RSVFAILED:
            pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
            break;
            
        default:
            return;
    }

    if (NULL == pList)
    {
        return;
    }
    
    // 更新链表序号
    wms_cacheinfolist_updatexuhao(pMe->m_eMBoxType);
    
    // 取链表项数
    if (pMe->m_eMBoxType == WMS_MB_RSVFAILED)
    {
        wItemCount = pList->nOnUIMS;
    }
    else
    {
        wItemCount = pList->nBranches;
    }
    
    if (wItemCount == 0)
    {
        return;
    }
    
    ERR("wItemCount = %d",wItemCount,0,0);

    // 计算需要的总页数
    wTotalPages = wItemCount / (MAXITEMS_ONEPAGE);
    if ((wItemCount % MAXITEMS_ONEPAGE) != 0)
    {
        wTotalPages++;
    }
    if (wTotalPages < 2)
    {
        nItems = wItemCount;
    }
    else
    {
        nItems = MAXITEMS_ONEPAGE;
    }
    
    // 消息总数不大于一页显示的菜单项数时，不考虑下两种要求产生菜单列表的模式
    if (((pMe->m_eMakeListMode == MAKEMSGLIST_NEXTONE) ||
         (pMe->m_eMakeListMode == MAKEMSGLIST_BACKONE) ||
         (pMe->m_eMakeListMode == MAKEMSGLIST_PREPAGE) ||
         (pMe->m_eMakeListMode == MAKEMSGLIST_NEXTPAGE)) && 
        (wTotalPages < 2))
    {
        int nCount;
        uint16 wItemID;
        
        nCount = IMENUCTL_GetItemCount(pMenu);
        
        if (nCount > 1)
        {
            if (pMe->m_eMakeListMode == MAKEMSGLIST_BACKONE)
            {
                wItemID = IMENUCTL_GetItemID(pMenu, nCount-1);
            }
            else
            {
                wItemID = IMENUCTL_GetItemID(pMenu, 0);
            }
            
            // 将当前选中项置为末尾，体现移动菜单项的连贯性
            IMENUCTL_SetSel(pMenu, wItemID);
            
            (void)IMENUCTL_Redraw(pMenu);
        }
        return;
    }
    
    // 确定建立菜单列表时，对应消息列表的起点位置
    switch (pMe->m_eMakeListMode)
    {
        case MAKEMSGLIST_INIT:
            pMe->m_wCurPageStarxuhao = 1;
            pMe->m_wSelItemxuhao = 1;
            break;
            
        case MAKEMSGLIST_NEXTONE:
            if (pMe->m_wCurPageStarxuhao+MAXITEMS_ONEPAGE>wItemCount)
            {
                pMe->m_wCurPageStarxuhao = 1;
                pMe->m_wSelItemxuhao = 1;
            }
            else
            {
                pMe->m_wSelItemxuhao = pMe->m_wCurPageStarxuhao+MAXITEMS_ONEPAGE;
                pMe->m_wCurPageStarxuhao++;
            }
            
            break;
            
        case MAKEMSGLIST_BACKONE:
            if (pMe->m_wCurPageStarxuhao>1)
            {
                pMe->m_wCurPageStarxuhao--;
                pMe->m_wSelItemxuhao = pMe->m_wCurPageStarxuhao;
            }
            else
            {// 移至最后一页，选中最后一项
                pMe->m_wCurPageStarxuhao = wItemCount+1-MAXITEMS_ONEPAGE;
                pMe->m_wSelItemxuhao = wItemCount;
            }
            
            break;
            
        case MAKEMSGLIST_PREPAGE:
            if (pMe->m_wCurPageStarxuhao==1)
            {// 移至最后一页，选中最后一项
                pMe->m_wCurPageStarxuhao = wItemCount+1-MAXITEMS_ONEPAGE;
                pMe->m_wSelItemxuhao = wItemCount;
            }
            else if (pMe->m_wCurPageStarxuhao<=MAXITEMS_ONEPAGE)
            {// 移至首页，选中第一项
                pMe->m_wCurPageStarxuhao = 1;
                pMe->m_wSelItemxuhao = 1;
            }
            else
            {
                pMe->m_wCurPageStarxuhao = pMe->m_wCurPageStarxuhao-MAXITEMS_ONEPAGE;
                pMe->m_wSelItemxuhao = pMe->m_wCurPageStarxuhao;
            }
            break;
            
        case MAKEMSGLIST_NEXTPAGE:
            if ((pMe->m_wCurPageStarxuhao+2*MAXITEMS_ONEPAGE-1)<=wItemCount)
            {// 移至首页，选中第一项
                pMe->m_wCurPageStarxuhao += MAXITEMS_ONEPAGE;
            }
            else if (pMe->m_wCurPageStarxuhao<=(wItemCount-MAXITEMS_ONEPAGE))
            {// 移至末页，选中第一项
                pMe->m_wCurPageStarxuhao = wItemCount-MAXITEMS_ONEPAGE+1;
            }
            else
            {// 移至首页，选中第一项
                pMe->m_wCurPageStarxuhao = 1;
            }
            
            pMe->m_wSelItemxuhao = pMe->m_wCurPageStarxuhao;
            
            break;
            
        case MAKEMSGLIST_RESUMECURPAGE:
            if (wTotalPages<2)
            {
                pMe->m_wCurPageStarxuhao = 1;
            }
            else
            {
                if (pMe->m_wCurPageStarxuhao>wItemCount)
                {
                    pMe->m_wCurPageStarxuhao = 1;
                }
                else if ((pMe->m_wCurPageStarxuhao+MAXITEMS_ONEPAGE)>wItemCount)
                {
                    pMe->m_wCurPageStarxuhao = wItemCount-MAXITEMS_ONEPAGE+1;
                }
            }
            break;
            
        default:
            return;
    }
    
    // 将游标置于第一个菜单项节点
    wms_cacheinfolist_enumtoxuhao(pMe->m_eMBoxType, pMe->m_wCurPageStarxuhao);
        
    // 建立菜单项列表
    MEMSET(&mai, 0, sizeof(mai));
    mai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    mai.pszResText = NULL;
    mai.pImage = NULL;
    mai.wFont = AEE_FONT_NORMAL;
    mai.dwData = 0;
    
    // 先清除旧有消息列表
    (void)IMENUCTL_DeleteAll(pMenu);

    (void)STRTOWSTR("%s", wszFmt, sizeof(wszFmt));
    for (i=0; i<nItems; i++)
    {
        wszTitle[0] = 0;
        pnode = wms_cacheinfolist_enumnext(pMe->m_eMBoxType);
        MEMSET(wszName, 0, sizeof(wszName));
        
        if (NULL == pnode)
        {
            wms_cacheinfolist_enumbegin(pMe->m_eMBoxType);
            pnode = wms_cacheinfolist_enumnext(pMe->m_eMBoxType);
            
            if (NULL == pnode)
            {
                break;
            }
        }
        else if (pnode->pNext == NULL)
        {
            wms_cacheinfolist_enumbegin(pMe->m_eMBoxType);
        }
        if (pnode->msg_type == WMS_BD_TYPE_DELIVERY_ACK)
        {
            (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                        IDS_REPORTTITLE, wszTitle,sizeof(wszTitle));
        }
        else
        {
            wstrNum[0] = 0;
            
            // 从电话本中取人名
            if (NULL != pnode->pszNum)
            {
                (void)STRTOWSTR(pnode->pszNum, wstrNum, sizeof(wstrNum));
                
                WMSUtil_GetContactName(pMe, wstrNum, wszName, MAX_TITLE_LEN);
            }
                                        
            // 格式化菜单项文本
            if (WSTRLEN(wszName) > 0)
            {
                WSPRINTF(wszTitle, sizeof(wszTitle), wszFmt, wszName);
            }
            else
            {
                if (NULL != pnode->pwszTitle)
                {
                    WSPRINTF(wszTitle, sizeof(wszTitle), wszFmt, pnode->pwszTitle);
                }
                else if (NULL != pnode->pszNum)
                {
                    WSPRINTF(wszTitle, sizeof(wszTitle), wszFmt, wstrNum);
                }
                else
                {
                    (void)STRTOWSTR("**********", wstrNum, sizeof(wstrNum));
                    WSPRINTF(wszTitle, sizeof(wszTitle), wszFmt, wstrNum);
                }
            }
        }

        mai.pText = wszTitle;
        mai.wItemID = MSG_CMD_BASE + pnode->index;
        if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
        {
            mai.wItemID += RUIM_MSGINDEX_BASE;
        }
        //mai.dwData = mai.wItemID;
        mai.dwData = pnode->xuhao;
        
        mai.wImage = WmsApp_GetMsgICONID(pnode);
            
        // 添加带图标菜单项到菜单
        (void)IMENUCTL_AddItemEx(pMenu, &mai);
        
        if (pnode->xuhao == pMe->m_wSelItemxuhao)
        {
            bFindCurxuhao = TRUE;
            wSelectItemID = mai.wItemID;
        }
    }
    if (bFindCurxuhao == FALSE)
    {
        wSelectItemID = mai.wItemID;
        pMe->m_wSelItemxuhao = mai.dwData;
    }
    
    if (wSelectItemID != 0)
    {
        uint16 nTitleID=0;
        int    nLen;
        
        (void)STRTOWSTR(" (%d/%d)", wszFmt, sizeof(wszFmt));
        wszTitle[0] = 0;
        
        switch (pMe->m_eMBoxType)
        {
            case WMS_MB_INBOX:
                nTitleID = IDS_INBOX;
                break;

            case WMS_MB_OUTBOX:
                nTitleID = IDS_OUTBOX;
                break;

            case WMS_MB_DRAFT:
                nTitleID = IDS_DRAFT;
                break;
                
            case WMS_MB_RESERVE:
                nTitleID = IDS_RESERVEDMSGS;
                break;

            case WMS_MB_RSVFAILED:
                nTitleID = IDS_FAILEDRSVDMSGS;
                break;
                
            default:
                break;
        }
        
        if (nTitleID != 0)
        {
            (void) ISHELL_LoadResString(pMe->m_pShell,
                    AEE_WMSAPPRES_LANGFILE,
                    nTitleID,
                    wszTitle,
                    sizeof(wszTitle));
            nLen = WSTRLEN(wszTitle);
            WSPRINTF(&wszTitle[nLen], (sizeof(wszTitle) - nLen*sizeof(AECHAR)), wszFmt, pMe->m_wSelItemxuhao, wItemCount);
			#if 0
            (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wszTitle);
			#else
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
			#endif
        }
        if(wItemCount > MAXITEMS_ONEPAGE)
        {
            AEERect rc;

            IMENUCTL_GetRect(pMenu, &rc);
            rc.dx = pMe->m_rc.dx - SCROLLBAR_WIDTH;
            IMENUCTL_SetRect(pMenu, &rc);
        
            rc.x = pMe->m_rc.dx - SCROLLBAR_WIDTH;
            rc.dx = SCROLLBAR_WIDTH;
            rc.y = 0;
            rc.dy = pMe->m_rc.dy- BOTTOMBAR_HEIGHT;
        
            Appscommon_DrawScrollBar(pMe->m_pDisplay, pMe->m_wCurPageStarxuhao + MAXITEMS_ONEPAGE - 1, wItemCount, MAXITEMS_ONEPAGE, &rc);
            IDISPLAY_Update(pMe->m_pDisplay);
        }        
        IMENUCTL_SetSel(pMenu, wSelectItemID);
    }

    (void)IMENUCTL_Redraw(pMenu);
} //WmsApp_UpdateMenuList()

//------------------------------------------------------------------------------
//   局部函数定义(仅在WmsApp.c文件中使用)
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                    Applet状态方法定义
//-----------------------------------------------------------------------------

/*==============================================================================
函数:
    CWmsApp_RunFSM

说明:
    运行有限状态机引擎。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
static void CWmsApp_RunFSM(WmsApp *pMe)
{
    NextFSMAction nextFSMAction = NFSMACTION_WAIT;
    for ( ; ; )
    {
        nextFSMAction = WmsApp_ProcessState(pMe);

        // 这里不需要复杂的状态机，因此为简化状态机设计这里仅用一个布尔变量来
        // 指示dlgResult是否应该被改写。在某些特殊场合（见状态机设计），这种作
        // 法，避免了不必要的复杂性，简化了设计。
        if (pMe->m_bDoNotOverwriteDlgResult)
        {
            // We allow only one iteration without touching this variable. So,
            // let's reset it back to indicate that we are going to overwrite the
            // result for all subsequent iterations.
            pMe->m_bDoNotOverwriteDlgResult = FALSE;
        }
        else
        {
            // Overwrite the dlgResult to its default value. The next dialog that
            // we are going to display will set and return the dlgResult based on
            // the users action.
            pMe->m_eDlgReturn = DLGRET_CREATE;
        }

        // If the state machine processing engine indicates that it created a
        // dialog and requesting us to exit this loop, then we quit.
        if (nextFSMAction == NFSMACTION_WAIT)
        {
            break;
        }
    }
}

/*==============================================================================
函数:
    WmsApp_HandleNotifyEvent

说明:
    处理通知事件。

参数:
    pMe [in]：指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pN [in]：AEENotify对象指针。
       
返回值:
    TRUE:  事件被处理。
    FALSE: 事件没处理。

备注:


==============================================================================*/
static boolean WmsApp_HandleNotifyEvent(WmsApp *pMe, AEENotify *pN)
{
    if ((NULL == pMe) || (NULL == pN))
    {
        return FALSE;
    }
    
    switch (pN->cls)
    {
        case AEECLSID_CM_NOTIFIER:
            return WmsApp_HandleCMNotify(pMe, pN);

        case AEECLSID_SHELL:
            if (pN->dwMask == NMASK_SHELL_START_STATUS)
            {
                (void)ISHELL_RegisterNotify(pMe->m_pShell, 
                                            AEECLSID_WMSAPP,
                                            AEECLSID_SHELL,
                                            0);
                IWmsApp_AddRef((IWmsApp *)pMe);
            }
            if (pN->dwMask & NMASK_SHELL_KEY)
            {
                IALERT_StopMp3Alert(pMe->m_pAlert);
                (void)ISHELL_RegisterNotify(pMe->m_pShell, 
                                            AEECLSID_WMSAPP,
                                            AEECLSID_SHELL,
                                            0);
            }
            
            return TRUE;
        default:
            break;
    }
    
    return FALSE;
}

/*==============================================================================
函数: 
    WmsApp_HandleCMNotify
       
说明: 
    函数处理来自 AEECLSID_CM_NOTIFIER 的通知事件。
       
参数: 
    pMe [in]：指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pNotify [in]: 指向 AEENotify 结构体变量的指针。
    
    
返回值:
    TRUE: 事件得到处理
    FALSE:事件没被处理
       
备注:

==============================================================================*/
static boolean WmsApp_HandleCMNotify(WmsApp * pMe, AEENotify *pNotify)
{
    AEECMNotifyInfo *pEvtInfo;
    
    if (NULL == pNotify)
    {
        return FALSE;
    }
    
    pEvtInfo = pNotify->pData;
    
    switch (pNotify->dwMask)
    {
        case NMASK_CM_PHONE:
            switch(pEvtInfo->event)
            {
                // Phone information is now available
                case AEECM_EVENT_PH_INFO_AVAIL:
                    return TRUE;

                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    
    return TRUE;
}

/*==============================================================================
函数:
    WmsApp_MultSendMsgTimer

说明:
    群发信息时，消息发送间的间隔时间定时器，用于启动下一条消息的发送。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_MultSendMsgTimer(void *pme)
{
    WmsApp* pMe = (WmsApp*)pme;
    CMultiSendItemInfo *pItem = NULL;
    int   nRet;

#ifdef WIN32
	pMe->m_bSending = TRUE;
	goto END_SENDING;
#endif

    if (NULL == pMe)
    {
        return;
    }

    pMe->m_bSaveFailedMsg = FALSE;
    
    if (pMe->m_idxCurSend >= pMe->m_nSendItems)
    {// 整条消息全部数据包已发出
        // 从接收地址列表中删除当前地址
        pItem = (CMultiSendItemInfo *)IVector_RemoveElementAt(pMe->m_pSendList, 0);
        if (NULL != pItem)
        {
            FREE(pItem);
        }
        
        // 建立客户消息列表
        WmsApp_BuildSendClentMsgList(pMe);
        
        if (NULL == pMe->m_pCurSendCltMsg)
        {
            goto END_SENDING;
        }
        
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_WMSAPP, 
                                 EVT_USER_REDRAW,
                                 0, 
                                 0);
                                 
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
        // 运营商要求为群发的每个目标保存短信
        if (pMe->m_SendOPT != SENDOPT_SEND)
        {
            // 保存消息
            nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                pMe->m_clientId, 
                                &pMe->m_callback,
                                (void*)pMe,
                                WMS_WRITE_MODE_INSERT,
                                pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
                                
            if (nRet == SUCCESS)
            {
                return;
            }
            pMe->m_idxCurSend = 0;
        }
#endif                                 
    }
    
    pMe->m_bSending = FALSE;
    pMe->m_bDCDisconnectedInSending = FALSE;
    pMe->m_nDisconnectedInSendingRetryTimes = 0;
    
    // 发送当前消息包
    nRet = IWMS_MsgSend(pMe->m_pwms, 
                        pMe->m_clientId, 
                        &pMe->m_callback,
                        (void*)pMe,
                        WMS_SEND_MODE_CLIENT_MESSAGE,
                        pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
                        
    if (nRet == SUCCESS)
    {
        pMe->m_bSending = TRUE;
        return;
    }
                             
END_SENDING:
    if ((pMe->m_wActiveDlgID == IDD_SENDING) &&
        (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP))
    {
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_WMSAPP, 
                                 EVT_SENDSMSEND,
                                 0, 
                                 0);
    }
    
    // 释放用户数据列表
    WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
}

/*==============================================================================
函数:
    WmsApp_ReSendMsgTimer

说明:
    重发信息时，消息发送间的间隔时间定时器，用于启动下一消息数据包的发送。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_ReSendMsgTimer(void *pme)
{
    WmsApp* pMe = (WmsApp*)pme;
    CMultiSendItemInfo *pItem = NULL;
    wms_cache_info_node  *pnode = NULL;
    int   nRet;

    if (NULL == pMe)
    {
        return;
    }
    
    // 确定当前发送消息节点
    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
    {
        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
        {
            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
            break;
        }
    }
    
    if (pMe->m_idxCur >= LONGSMS_MAX_PACKAGES)
    {// 整条消息全部数据包已发出
        // 从接收地址列表中删除当前地址
        pItem = (CMultiSendItemInfo *)IVector_RemoveElementAt(pMe->m_pSendList, 0);
        if (NULL != pItem)
        {
            FREE(pItem);
        }
        
        goto END_SENDING;
    }
    
    // 读取当前节点消息                             
    nRet = IWMS_MsgRead(pMe->m_pwms,
                       pMe->m_clientId,
                       &pMe->m_callback,
                       (void *)pMe,
                       pnode->mem_store,
                       pnode->index);
                        
    if (nRet == SUCCESS)
    {
        return;
    }
    
END_SENDING:
    if ((pMe->m_wActiveDlgID == IDD_SENDING) &&
        (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP))
    {
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_WMSAPP, 
                                 EVT_SENDSMSEND,
                                 0, 
                                 0);
    }
    
    // 释放临时分配数据
    WmsApp_FreeSendClentMsgList(pMe);
    
    pMe->m_idxCur = LONGSMS_MAX_PACKAGES;
}

/*==============================================================================
函数:
    WmsApp_SendReservedMsgTimer

说明:
    发送预约信息时，消息发送间的间隔时间定时器，用于启动下一消息数据包的发送。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    无。

备注:

==============================================================================*/
void WmsApp_SendReservedMsgTimer(void *pme)
{
    WmsApp* pMe = (WmsApp*)pme;
    CMultiSendItemInfo *pItem = NULL;
    wms_cache_info_node  *pnode = NULL;
    int   nRet;

    if (NULL == pMe)
    {
        return;
    }
    
    // 确定当前发送消息节点
    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
    {
        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
        {
            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
            break;
        }
    }
    
    if (pMe->m_idxCur >= LONGSMS_MAX_PACKAGES)
    {// 整条消息全部数据包已发出
        // 从接收地址列表中删除当前地址
        pItem = (CMultiSendItemInfo *)IVector_RemoveElementAt(pMe->m_pSendList, 0);
        if (NULL != pItem)
        {
            FREE(pItem);
        }
        
        goto END_SENDING;
    }
    
    // 读取当前节点消息                             
    nRet = IWMS_MsgRead(pMe->m_pwms,
                       pMe->m_clientId,
                       &pMe->m_callback,
                       (void *)pMe,
                       pnode->mem_store,
                       pnode->index);
                        
    if (nRet == SUCCESS)
    {
        return;
    }
    
END_SENDING:
    if ((pMe->m_wActiveDlgID == IDD_SENDING) &&
        (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP))
    {
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_WMSAPP, 
                                 EVT_SENDSMSEND,
                                 0, 
                                 0);
    }
    else
    {
        WmsApp_ReservedMsgSetTimer(pMe);
    }
    
    // 释放临时分配数据
    WmsApp_FreeSendClentMsgList(pMe);
    
    pMe->m_idxCur = LONGSMS_MAX_PACKAGES;
}


/*==============================================================================
函数:
    WmsApp_GetSendList

说明:
    函数用于获取群发链表。

参数:
    pMe [in]:指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    指定邮箱类型相应消息链表。

备注:
       
==============================================================================*/
static IVector * WmsApp_GetSendList(void)
{
    // 收件箱消息基本信息列表
    static IVector  *gMultiSendList = NULL;
    int             nRet;
    
    IShell * pShell = AEE_GetShell();
    
    // 先判断链表接口是否创建，若未创建再创建
    if (NULL != gMultiSendList)
    {
        return gMultiSendList;
    }
    
    if (NULL == pShell)
    {
        return NULL;
    }
    
    nRet = ISHELL_CreateInstance(pShell,AEECLSID_VECTOR,(void**)&gMultiSendList);
    if (nRet != SUCCESS )
    {
        ERR("Failed to Create gMultiSendList !",0,0,0);
        gMultiSendList = NULL;
    }
    else
    {
        (void) IVector_EnsureCapacity(gMultiSendList, MAXNUM_MULTISEND);
    }
    
    return gMultiSendList;
} // WmsApp_GetSendList

/*==============================================================================
函数：
    WmsApp_GetAddItem

说明：
    从链表中取得指定索引的号码。

参数：
    pIV [in]：指向IVector接口对象的指针。
    wIdx [in]：索引号。

返回值：

备注:
       
==============================================================================*/
CMultiSendItemInfo * WmsApp_GetAddItem(IVector *pIV, uint16 wIdx)
{
    CMultiSendItemInfo *pItem = NULL;
    int32    dwCount;

    if (pIV == NULL)
    {
        return NULL;
    }
    
    dwCount = IVector_Size(pIV);
    if (wIdx < dwCount)
    {
        pItem = (CMultiSendItemInfo *)IVector_ElementAt(pIV, wIdx);
    }
    
    return pItem;
}

/*==============================================================================
函数:
    CWmsApp_MessageService

说明:
    提供给外部Applet使用部分短信服务的助手函数。

参数:
    p [in]:指向IWmsApp接口t对象的指针。
    eStype [in]: 使用的服务
    toAddrStr [in]:(以null结尾的宽字符串)。消息发往目的地。

返回值:
    SUCCESS:Applet启动成功。
    EFAILED:Applet启动失败。

备注:
    该函数供BREW调用，用户不得调用。
        
==============================================================================*/
static int CWmsApp_MessageService(IWmsApp *p, 
        WMSServiceType eStype, 
        AECHAR *toAddrStr, 
        AECHAR *pwstrText)
{
    char  *args;
    WmsApp *pMe = (WmsApp*)p;
    int    nRet;

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP)
    {
        // WMS applet is already running
        return EFAILED;
    }
#if defined(FEATURE_UIM_TOOLKIT)
    if (STARTMETHOD_REFRESH == eStype)
    {
        pMe->m_refresh_in_progress = TRUE;
        
        // 发布更新卡上短信 Cache 的命令
        (void)IWMS_RefreshRUIMSMS(pMe->m_pwms,
                                  pMe->m_clientId,
                                  &pMe->m_callback,
                                  (void *)pMe);
        
        return SUCCESS;
    }
#endif
    // 发送特殊短信
    if (STARTMETHOD_SENDSPECMESSAGE == eStype)
    {
        int   nRet;
        wms_client_message_s_type   *pMsg = NULL;

		/*
        if ((pwstrText == NULL) || 
            (pMe->m_bCdmaWmsReady == FALSE) || 
            pMe->m_refresh_in_progress)
        {
            return EFAILED;
        }
        */

		//Add By zzg 2010_07_20
		if (NULL == pwstrText)
		{
			pMsg = CWmsApp_GetspecmsgEx();
			
	        if (NULL == pMsg)
	        {
	            return EFAILED;
	        }
			
#ifndef WIN32
			DBGPRINTF("***zzg CWmsApp_GetspecmsgEx wms_msg_send***");
	        if (wms_msg_send(pMe->m_clientId, NULL, NULL, WMS_SEND_MODE_CLIENT_MESSAGE, pMsg)==WMS_OK_S)
	        {
	        	DBGPRINTF("***zzg CWmsApp_GetspecmsgEx wms_msg_send SUCCESS***");
	            nRet = SUCCESS;
	        }
	        else
	        {
	        	DBGPRINTF("***zzg CWmsApp_GetspecmsgEx wms_msg_send EFAILED***");
	            nRet = EFAILED;
	        }
#else
			nRet = SUCCESS;
#endif
		}
		else if ((pMe->m_bCdmaWmsReady != FALSE) && (pMe->m_refresh_in_progress))
		{
			pMsg = CWmsApp_Getspecmsg(pwstrText);
			
	        if (NULL == pMsg)
	        {
	            return EFAILED;
	        }
			
#ifndef WIN32
	        if (wms_msg_send(pMe->m_clientId, NULL, NULL, WMS_SEND_MODE_CLIENT_MESSAGE, pMsg)==WMS_OK_S)
	        {
	            nRet = SUCCESS;
	        }
	        else
	        {
	            nRet = EFAILED;
	        }
#else
			nRet = SUCCESS;
#endif
		}
		//Add End

		/*
        pMsg = CWmsApp_Getspecmsg(pwstrText);
        if (NULL == pMsg)
        {
            return EFAILED;
        }
        
        // 这里没用 IWMS_MsgSend 接口发送消息是因为该接口第三个参数是不能为 NULL
        // 的。这里仅借短信发送特殊消息，不希望其干扰短信应用的正常秩序。直接调用
        // wms API 发送。
#ifndef WIN32
        if (wms_msg_send(pMe->m_clientId, NULL, NULL, WMS_SEND_MODE_CLIENT_MESSAGE, pMsg)==WMS_OK_S)
        {
            nRet = SUCCESS;
        }
        else
        {
            nRet = EFAILED;
        }
#else
		nRet = SUCCESS;
#endif
		*/
        FREEIF(pMsg);
        return nRet;
    }
    
    args = CWmsApp_BuildStartArgs(eStype);

    if (args == NULL)
    {
        return EFAILED;
    }
    
    WMSMessageStruct_Reset(&pMe->m_msSend);
    
    if ((NULL != toAddrStr) &&
        (WSTRLEN(toAddrStr)>0))
    {
        (void)WSTRCPY(pMe->m_msSend.m_szNum, toAddrStr);
    }
    
    if ((NULL != pwstrText) &&
        (WSTRLEN(pwstrText)>0))
    {
        pMe->m_msSend.m_szMessage = WSTRDUP(pwstrText);
    }
    
    nRet = ISHELL_StartApplet(pMe->m_pShell, AEECLSID_WMSAPP);
        
    if (NULL != pMe->m_pszArgs)
    {
        sys_free(pMe->m_pszArgs);
    }
    pMe->m_pszArgs = args;
    
    return nRet;
}

/*==============================================================================
函数:
    CWmsApp_BuildStartArgs

说明:
    函数用于建立启动参数字符串。

参数:
    eStype [in]: 使用的服务。

返回值:
    char *

备注:
    函数返回字符指针若非空(NULL)，调用函数必须释放返回的地址内存。
        
==============================================================================*/
static char * CWmsApp_BuildStartArgs (WMSServiceType eStype)
{
    char *str = NULL;

    switch (eStype)
    {
        //Power on start
        case STARTMETHOD_POWERONSTART: 
            str = (char *)sys_malloc(2);
            if(str)
            {
                *str = STARTARGPREFIX_POWERONSTART;
            }
            break;
        
        case STARTMETHOD_WMSNORMAL:
            break;

        // Send message with known address
        case STARTMETHOD_SENDTEXTMESSAGE:
        // write a new message called from idle
        case STARTMETHOD_WRITENEWMESSAGE: 
            str = (char *)sys_malloc(2);
            if(str)
            {
                *str = STARTARGPREFIX_WRITENEWMESSAGE;
            }
            break;

        //Show sms received list called from idle
        case STARTMETHOD_SHOWINBOXLIST: 
            str = (char *)sys_malloc(2);
            if(str)
            {
                *str = STARTARGPREFIX_SHOWINBOXLIST;
            }
            break;

        // View voice mail
        case STARTMETHOD_VIEWVMAIL:
            str = (char *)sys_malloc(2);
            if(str)
            {
                *str = STARTARGPREFIX_VIEWVMAIL;
            }
            break;
            
        default:
            break;
    }

    return str;
}

/*==============================================================================
函数:
    WmsApp_ArgsStartInit

说明:
    函数用于初始化带参数启动 WMS Applet 时的相关数据。

参数:
    pMe [in]:指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pszArgs [in]: 启动参数
       
返回值:
    无。

备注:
       
==============================================================================*/
static void WmsApp_ArgsStartInit(WmsApp *pMe, const char *pszArgs)
{
    int nLen;
    boolean  bsmslock = FALSE;
    
    if (NULL == pMe)
    {
        return;
    }
    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_SMS_LOCK_CHECK,
                           &bsmslock,
                           sizeof(bsmslock));
    
    if (pszArgs != NULL)
    {   //  外部调用接口, 要执行具体的操作视参数而定
        CMultiSendItemInfo *pItem = NULL;
        
        // 在输入界面选返回将退出短信
        pMe->m_STSwitchToEditMsg = WMSST_EXIT;
        
        // 先清空群发地址链表
        WmsApp_FreeMultiSendList(pMe->m_pSendList);
        
        nLen = STRLEN(pszArgs);
        
        if ((nLen == 1) && *pszArgs == STARTARGPREFIX_POWERONSTART)
        {
            pMe->m_currState = WMSST_EXIT;
            return;
        }
        else if ((pMe->m_bCdmaWmsReady == FALSE) || pMe->m_refresh_in_progress)
        {
            pMe->m_eDlgReturn = DLGRET_CREATE;
            pMe->m_currState = WMSST_INIT;
            return;
        }
        
        if (WSTRLEN(pMe->m_msSend.m_szNum)>0)
        {
            // 将传入号码保存入链表
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            if (pItem == NULL)
            {
                return;
            }
            (void)WSTRCPY(pItem->m_szTo, pMe->m_msSend.m_szNum);
            pMe->m_CurAddID = MSG_CMD_BASE;
            if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
            {
                sys_free(pItem);
                return;
            }
            
            WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
        }
        switch (*pszArgs)
        {
            case STARTARGPREFIX_SENDTEXTMESSAGE:
            case STARTARGPREFIX_WRITENEWMESSAGE:
                if (bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    pMe->m_stchkpwdbk = WMSST_WRITEMSG;
                }
                else if (pMe->m_bNeedContinueSend)
                {
                    pMe->m_currState = WMSST_CONTINUESEND_QUERY;
                    pMe->m_stcontinuesendbk = WMSST_WRITEMSG;
                }
                else
                {
                    pMe->m_currState = WMSST_WRITEMSG;
                }
                pMe->m_dwInsertPos = 0;
                break;
            
            case STARTARGPREFIX_SHOWINBOXLIST:
                if (bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    pMe->m_stchkpwdbk = WMSST_INBOXES;
                }
                else if (pMe->m_bNeedContinueSend)
                {
                    pMe->m_currState = WMSST_CONTINUESEND_QUERY;
                    pMe->m_eMBoxType = WMS_MB_INBOX;
                    pMe->m_stcontinuesendbk = WMSST_INBOXES;
                }
                else
                {    
                    pMe->m_currState = WMSST_INBOXES;
                }
                break;
                
            case STARTARGPREFIX_VIEWVMAIL:
                if (bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    pMe->m_stchkpwdbk = WMSST_VOICEMAIL;
                }
                else if (pMe->m_bNeedContinueSend)
                {
                    pMe->m_currState = WMSST_CONTINUESEND_QUERY;
                    pMe->m_stcontinuesendbk = WMSST_VOICEMAIL;
                }
                else
                {
                    pMe->m_currState = WMSST_VOICEMAIL;
                }
                break;
                
            case STARTARGPREFIX_RESERVEDMSGSTART:
                // 不再继续发送
                pMe->m_bNeedContinueSend = FALSE;
                pMe->m_ContinueSendType = NONE_CONTINUE;
                
                if (bsmslock)
                {
                    pMe->m_currState = WMSST_CHKPWD;
                    pMe->m_stchkpwdbk = WMSST_RESERVEDMSGALERT;
                }
                else
                {
                    pMe->m_currState = WMSST_RESERVEDMSGALERT;
                }
                break;
                
            default:
                break;
        }
    }
}


/*==============================================================================
函数: 
    WmsApp_CfgCb
       
说明: 
    本函数为所有 CFG 相关事件的回调函数。
       
参数: 
    event [In]: CFG 相关事件 。
    pInfo [In]: wms_cfg_event_info_s_type 结构指针，存有事件相关信息。
    
       
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void WmsApp_CfgCb(wms_cfg_event_e_type event, wms_cfg_event_info_s_type *pInfo)
{
    wms_cfg_event_info_s_type   *pInfobuf=NULL;
    IShell                      *pShell = AEE_GetShell();
    uint8                       btRet; 
    AEEEvent                    evt=0;
    
    if (pShell == NULL)
    {
        MSG_ERROR("WMSAPP: pShell = NULL",0,0,0);
        return;
    }
    
    pInfobuf = MALLOC(sizeof(wms_cfg_event_info_s_type));
    if (pInfobuf == NULL)
    {
        MSG_ERROR("WMSAPP: pInfobuf = NULL",0,0,0);
        return;
    }
    
    (void)MEMCPY(pInfobuf, pInfo, sizeof(wms_cfg_event_info_s_type));

    MSG_ERROR("WmsApp_CfgCb: %d", event, 0, 0);
    switch (event)
    {
//        case WMS_CFG_EVENT_GW_READY:
//            evt = EVT_WMS_CFG_GW_READY;
//            break;
            
        case WMS_CFG_EVENT_CDMA_READY:
            evt = EVT_WMS_CFG_CDMA_READY;
            break;
            
        case WMS_CFG_EVENT_REFRESH_DONE:
            evt = EVT_WMS_CFG_REFRESH_DONE;
            break;
            
        case WMS_CFG_EVENT_ROUTES:
            evt = EVT_WMS_CFG_ROUTES;
            break;
            
        case WMS_CFG_EVENT_MEMORY_STATUS:
            evt = EVT_WMS_CFG_MEMORY_STATUS;
            break;
            
        case WMS_CFG_EVENT_MEMORY_FULL:
            evt = EVT_WMS_CFG_MEMORY_FULL;
            break;
            
        case WMS_CFG_EVENT_MESSAGE_LIST:
            evt = EVT_WMS_CFG_MESSAGE_LIST;
            break;
            
//        case WMS_CFG_EVENT_GW_DOMAIN_PREF:
//            evt = EVT_WMS_CFG_GW_DOMAIN_PREF;
//            break;
            
//        case WMS_CFG_EVENT_CELL_CHANGE:
//            evt = EVT_WMS_CFG_CELL_CHANGE;
//            break;
            
//        case WMS_CFG_EVENT_PRIMARY_CLIENT_SET:
//            evt = EVT_WMS_CFG_PRIMARY_CLIENT_SET;
//            break;
      
//        case WMS_CFG_EVENT_MEMORY_STATUS_SET:
//        case WMS_CFG_EVENT_LINK_CONTROL:
//            WMSAPPU_FREE(pInfobuf)
//            break;
            
        default:
            MSG_ERROR("Invalid cfg event %d", event, 0,0);
            WMSAPPU_FREE(pInfobuf)
            break;
    }
    
    if (evt != 0)
    {
        btRet = ISHELL_PostEventEx(pShell,
                                   EVTFLG_ASYNC, 
                                   AEECLSID_WMSAPP, 
                                   evt,
                                   0, 
                                   (uint32) pInfobuf);
                        
        if (btRet == FALSE)
        {
            MSG_HIGH("Event  %x could not be posted!", evt,0,0);
        }
    }
} // WmsApp_CfgCb

/*==============================================================================
函数: 
    WmsApp_MsgCb
       
说明: 
    本函数为所有 MSG 相关事件的回调函数。
       
参数: 
    event [In]: CFG 相关事件 。
    pInfo [In]: wms_msg_event_info_s_type 结构指针，存有事件相关信息。
    
       
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void WmsApp_MsgCb(wms_msg_event_e_type       event,
                  wms_msg_event_info_s_type  *pInfo,
                  boolean                    *shared)
{
    wms_msg_event_info_s_type   *pInfobuf=NULL;
    IShell                      *pShell = AEE_GetShell();
    uint8                       btRet; 
    AEEEvent                    evt=0;
    
    if (pShell == NULL)
    {
        MSG_ERROR("WMSAPP: pShell = NULL",0,0,0);
        return;
    }
    
    pInfobuf = MALLOC(sizeof(wms_msg_event_info_s_type));
    if (pInfobuf == NULL)
    {
        MSG_ERROR("WMSAPP: msg_event_buf = NULL",0,0,0);
        return;
    }
    
    (void)MEMCPY(pInfobuf, pInfo, sizeof(wms_msg_event_info_s_type));
    MSG_ERROR("WmsApp_MsgCb: %d", event, 0, 0);
    switch (event)
    {
        case WMS_MSG_EVENT_SEND:
            if ((pInfobuf->submit_report_info.client_id == WMS_CLIENT_TYPE_WMS_APP) &&
                (pInfobuf->submit_report_info.report_status != WMS_RPT_OK))
            {
                evt = EVT_WMS_MSG_SEND;
            }
            break;
            
//        case WMS_MSG_EVENT_ACK:
//            evt = EVT_WMS_MSG_ACK;
//            break;
            
        case WMS_MSG_EVENT_READ:
            evt = EVT_WMS_MSG_READ;
            break;
            
        case WMS_MSG_EVENT_WRITE:
            evt = EVT_WMS_MSG_WRITE;
            break;
            
        case WMS_MSG_EVENT_DELETE:
            evt = EVT_WMS_MSG_DELETE;
            break;
            
        case WMS_CMD_MSG_DELETE_BOX:
            evt = EVT_WMS_MSG_DELETE_BOX;
            break;
            
//        case WMS_CMD_MSG_COPY:
//            evt = EVT_WMS_MSG_COPY;
//            break;
            
        case WMS_MSG_EVENT_MODIFY_TAG:
            evt = EVT_WMS_MSG_MODIFY_TAG;
            break;
            
        case WMS_MSG_EVENT_READ_TEMPLATE:
            evt = EVT_WMS_MSG_READ_TEMPLATE;
            break;
            
        case WMS_MSG_EVENT_WRITE_TEMPLATE:
            evt = EVT_WMS_MSG_WRITE_TEMPLATE;
            break;
            
        case WMS_MSG_EVENT_DELETE_TEMPLATE:
            evt = EVT_WMS_MSG_DELETE_TEMPLATE;
            break;
        
        case WMS_MSG_EVENT_RECEIVED_MESSAGE:
            evt = EVT_WMS_MSG_RECEIVED_MESSAGE;
            break;
            
        case WMS_MSG_EVENT_SUBMIT_REPORT:
            pInfobuf->submit_report_info.alpha_id.data = (uint8 *)MALLOC(WMS_ALPHA_ID_MAX);
            pInfobuf->submit_report_info.alpha_id.len = pInfo->submit_report_info.alpha_id.len;
            
            if ((pInfo->submit_report_info.alpha_id.data != NULL) &&
                (pInfo->submit_report_info.alpha_id.len > 0))
            {
                (void)MEMCPY(pInfobuf->submit_report_info.alpha_id.data,
                            pInfo->submit_report_info.alpha_id.data,
                            pInfo->submit_report_info.alpha_id.len);
            }
            
            evt = EVT_WMS_MSG_SUBMIT_REPORT;
            break;
            
        case WMS_MSG_EVENT_STATUS_REPORT:
            evt = EVT_WMS_MSG_STATUS_REPORT;
            break;
            
//        case WMS_MSG_EVENT_DELETE_ALL:
//            evt = EVT_WMS_MSG_DELETE_ALL;
//            break;
//        
//        case WMS_MSG_EVENT_DELETE_TEMPLATE_ALL:
//            evt = EVT_WMS_MSG_DELETE_TEMPLATE_ALL;
//            break;
            
//        case WMS_MSG_EVENT_READ_STS_REPORT:
//            evt = EVT_WMS_MSG_READ_STATUS_REPORT;
//            break;
            
//        case WMS_MSG_EVENT_DELETE_STS_REPORT:
//            evt = EVT_WMS_MSG_DELETE_STATUS_REPORT;
//            break;
            
//        case WMS_MSG_EVENT_ACK_REPORT:
//            WMSAPPU_FREE(pInfobuf)
//            break;
            
//        case WMS_MSG_EVENT_DUPLICATE_CB_PAGE:
//            evt = EVT_WMS_MSG_DUPLICATE_CB_PAGE;
//            break;
            
        default:
            MSG_ERROR("Invalid msg event %d", event, 0,0);
            break;
    } // switch
    
    if (evt != 0)
    {
        btRet = ISHELL_PostEventEx(pShell,
                                   EVTFLG_ASYNC, 
                                   AEECLSID_WMSAPP, 
                                   evt,
                                   0, 
                                   (uint32) pInfobuf);
                        
        if (btRet == FALSE)
        {
            MSG_HIGH("Event  %x could not be posted!", evt,0,0);
            WMSAPPU_FREE(pInfobuf)
        }
    }
    else
    {
        WMSAPPU_FREE(pInfobuf)
    }
} // WmsApp_MsgCb() 

#ifdef FEATURE_BROADCAST_SMS
/*==============================================================================
函数: 
    WmsApp_BcCb
       
说明: 
    本函数为所有 WMS  Broadcast 相关事件的回调函数。
       
参数: 
    event [In]: WMS  Broadcast 相关事件 。
    pInfo [In]: wms_bc_event_info_s_type 结构指针，存有事件相关信息。
    
       
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void WmsApp_BcCb(wms_bc_event_e_type  event, wms_bc_event_info_s_type  *pInfo)
{
    wms_bc_event_info_s_type    *pInfobuf = NULL;
    IShell                      *pShell = AEE_GetShell();
    uint8                       btRet; 
    AEEEvent                    evt = EVT_WMS_BC_EVENT;
    
    if (pShell == NULL)
    {
        MSG_ERROR("WMSAPP: pShell = NULL",0,0,0);
        return;
    }
    
    pInfobuf = MALLOC(sizeof(wms_bc_event_info_s_type));
    if (pInfobuf == NULL)
    {
        MSG_ERROR("WMSAPP: bc_event_buf = NULL",0,0,0);
        return;
    }
    
    (void)MEMCPY(pInfobuf, pInfo, sizeof(wms_bc_event_info_s_type));
    
    MSG_ERROR("WmsApp_BcCb: %d", event, 0, 0);

    switch (event)
    {
        case WMS_BC_EVENT_CONFIG:
        case WMS_BC_EVENT_PREF:
        case WMS_BC_EVENT_TABLE:
        case WMS_BC_EVENT_SRV_IDS:
        case WMS_BC_EVENT_SRV_INFO:
        case WMS_BC_EVENT_SRV_ADDED:
        case WMS_BC_EVENT_SRV_DELETED:
        case WMS_BC_EVENT_SRV_UPDATED:
            
        // lower layer (CM & CP) failure
        case WMS_BC_EVENT_ENABLE_FAILURE:  
        
        // lower layer (CM & CP) failure
        case WMS_BC_EVENT_DISABLE_FAILURE:
             
        case WMS_BC_EVENT_ALL_SRV_DELETED:
            break;
            
        default:
            evt = 0;
            MSG_HIGH("BC event not recognized!!!",0,0,0);
            WMSAPPU_FREE(pInfobuf)
            break;
    }
    
    if (evt != 0)
    {
        btRet = ISHELL_PostEventEx(pShell,
                                   EVTFLG_ASYNC, 
                                   AEECLSID_WMSAPP, 
                                   evt,
                                   (uint16)event, 
                                   (uint32)pInfobuf);
                        
        if (btRet == FALSE)
        {
            MSG_HIGH("Event  %x (%x) could not be posted!", evt,event,0);
        }
    }
}
#endif /* FEATURE_BROADCAST_SMS */

#ifdef  FEATURE_CDSMS
/*==============================================================================
函数: 
    WmsApp_DcCb
       
说明: 
    本函数为所有 DC 相关事件的回调函数。
       
参数: 
    event [In]: DC 相关事件 。
    info_ptr [In]: wms_dc_event_info_s_type 结构指针，存有事件相关信息。
    
返回值:
    none
       
备注:
    Posts the Event to itself

==============================================================================*/
void WmsApp_DcCb(wms_dc_event_e_type  event,wms_dc_event_info_s_type  *info_ptr)
{
    wms_dc_event_info_s_type    *pInfobuf = NULL;
    IShell                      *pShell = AEE_GetShell();
    uint8                       btRet; 
    AEEEvent                    evt=0;
    
    if (pShell == NULL)
    {
        MSG_ERROR("WMSAPP: pShell = NULL",0,0,0);
        return;
    }
    
    pInfobuf = MALLOC(sizeof(wms_dc_event_info_s_type));
    if(pInfobuf == NULL)
    {
        MSG_ERROR("WMSAPP: dc event info buf = NULL", 0, 0, 0);
        return;
    }
    
    (void) MEMCPY(pInfobuf, info_ptr, sizeof(wms_dc_event_info_s_type));
    MSG_ERROR("WmsApp_DcCb: %d", event, 0, 0);
    switch(event)
    {
//        case WMS_DC_EVENT_CONNECTED:
//            if ((pInfobuf->so != WMS_DC_SO_6) &&
//                (pInfobuf->so != WMS_DC_SO_14))
//            {
//                MSG_MED("NOT a WMS call", 0, 0, 0);
//            }
//            else
//            {
//                evt = EVT_WMS_DC_CONNECTED;
//            }
//            WMSAPPU_FREE(pInfobuf)
//            break;
            
//        case WMS_DC_EVENT_ABORTED:
//            evt = EVT_WMS_DC_ABORTED;
//            WMSAPPU_FREE(pInfobuf)
//            break;
            
//        case WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT:
//            evt = EVT_WMS_DC_ENABLE_AUTO_DISCONNECT;
//            WMSAPPU_FREE(pInfobuf)
//            break;
            
//        case WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT:
//            evt = EVT_WMS_DC_DISABLE_AUTO_DISCONNECT;
//            WMSAPPU_FREE(pInfobuf)
//            break;

        case WMS_DC_EVENT_DISCONNECTED:
            evt = EVT_WMS_DC_DISCONNECTED;
            break;
            
        case WMS_DC_EVENT_INCOMING:
        case WMS_DC_EVENT_CONNECTING:
        default:
            WMSAPPU_FREE(pInfobuf)
            break;
    }
    
    if (evt != 0)
    {
        btRet = ISHELL_PostEventEx(pShell,
                        EVTFLG_ASYNC, 
                        AEECLSID_WMSAPP, 
                        evt,
                        0, 
                        0);
                        
        if(btRet == FALSE)
        {
            MSG_HIGH("Event  %x could not be posted!", evt,0,0);
        }
    }
} /* WmsApp_DcCb */
#endif

/*==============================================================================
函数: 
    WmsApp_Init
       
说明: 
    在初始化过程中，Wms Applet 发送一些命令给 WMS 以更新消息列表、内存状态等。
       
参数: 
    pMe [In]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    
返回值:
    none
       
备注:
    WMS Applet 将收到对应命令的响应事件。

==============================================================================*/
static void WmsApp_Init(WmsApp *pMe)
{
    int nRet;
    
    (void)IWMS_SetPrimaryClient(pMe->m_pwms, 
                        WMS_CLIENT_TYPE_WMS_APP, 
                        &pMe->m_callback, 
                        (void*)pMe, 
                        TRUE, FALSE);
    
#ifdef FEATURE_CDSMS
    if (pMe->m_mode == WMS_MESSAGE_MODE_CDMA)
    {
        (void)IWMS_CfgGetRoutes(pMe->m_pwms,
                               pMe->m_clientId,
                               &pMe->m_callback,
                               (void*)pMe);
        
#ifdef FEATURE_CDSMS_RUIM
        // 获取 RUIM 卡上消息列表
        /*
        (void)IWMS_CfgGetMessageList(pMe->m_pwms,
                                     pMe->m_clientId,
                                     &pMe->m_callback,
                                     (void*)pMe,
                                     WMS_MEMORY_STORE_RUIM,
                                     WMS_TAG_NONE);
                                     
        // 获取 RUIM 卡上模板列表
        (void)IWMS_CfgGetMessageList(pMe->m_pwms,
                                     pMe->m_clientId,
                                     &pMe->m_callback,(void*)pMe,
                                     WMS_MEMORY_STORE_RUIM,
                                     WMS_TAG_MO_TEMPLATE);
        */                             
        
        // 获取 RUIM 卡存储状态
        (void)IWMS_CfgGetMemoryStatus(pMe->m_pwms,
                                      pMe->m_clientId,
                                      &pMe->m_callback,
                                      (void*)pMe,
                                      WMS_MEMORY_STORE_RUIM,
                                      WMS_TAG_NONE);
#endif // FEATURE_CDSMS_RUIM

        /*
        // 获取 NV 消息列表
        (void)IWMS_CfgGetMessageList(pMe->m_pwms,
                                     pMe->m_clientId,
                                     &pMe->m_callback,
                                     (void*)pMe,
                                     WMS_MEMORY_STORE_NV_CDMA,
                                     WMS_TAG_NONE);
        */
        
        // 获取 NV 存储状态
        (void)IWMS_CfgGetMemoryStatus(pMe->m_pwms,
                                      pMe->m_clientId,
                                      &pMe->m_callback,
                                      (void*)pMe,
                                      WMS_MEMORY_STORE_NV_CDMA,
                                      WMS_TAG_NONE);

#ifdef FEATURE_BROADCAST_SMS
        // 初始化广播短信配置、参考和业务表
#ifdef FEATURE_CDSMS_BROADCAST
        nRet = IWMS_BcMmGetConfig(pMe->m_pwms,
                                  pMe->m_clientId,
                                  &pMe->m_callback,
                                  NULL,
                                  WMS_MESSAGE_MODE_CDMA);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_mm_get_config(CDMA)", 0, 0, 0);
        }
        
        nRet = IWMS_BcMmGetPref(pMe->m_pwms,
                                pMe->m_clientId,
                                &pMe->m_callback,
                                NULL,
                                WMS_MESSAGE_MODE_CDMA);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_mm_get_pref(CDMA)", 0, 0, 0);
        }
        
        nRet = IWMS_BcMmGetTable(pMe->m_pwms,
                                 pMe->m_clientId,
                                 &pMe->m_callback,
                                 NULL,
                                 WMS_MESSAGE_MODE_CDMA);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_mm_get_table(CDMA)", 0, 0, 0);
        }
#else // FEATURE_CDSMS_BROADCAST
        
        nRet = IWMS_BcGetConfig(pMe->m_pwms,
                                pMe->m_clientId,
                                &pMe->m_callback,
                                NULL);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_get_config", 0, 0, 0);
        }
        
        nRet = IWMS_BcGetPref(pMe->m_pwms,
                              pMe->m_clientId,
                              &pMe->m_callback,
                              NULL);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_get_pref", 0, 0, 0);
        }
        
        nRet = IWMS_BcGetTable(pMe->m_pwms,
                               pMe->m_clientId,
                               &pMe->m_callback,
                               NULL);
        if (nRet != SUCCESS)
        {
            MSG_ERROR("wms_bc_get_table", 0, 0, 0);
        }
#endif // FEATURE_CDSMS_BROADCAST
#endif // FEATURE_BROADCAST_SMS

        nRet = IWMS_DcEnableAutoDisconnect(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback, 
                                           NULL,
                                           WMSAPP_AUTO_DISCONNECT_TIME);
        /* Set up DC auto disconnect */
        if (nRet != SUCCESS)
        {
            MSG_ERROR("dc enable auto disconnect",0,0,0);
        }
    }
    
    (void)IWMS_MsgSetRetryPeriod(pMe->m_pwms, pMe->m_clientId, WMS_MAX_RETRY_PERIOD);
    
    pMe->m_bCdmaWmsReady = TRUE;

    pMe->m_pImage = NULL;
#ifdef FEATURE_AUTOREPLACE
    {
        boolean autoreplace = FALSE;
        
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_WMS_AUTOREPLACE,
                               &autoreplace,
                               sizeof(autoreplace));
        
        (void)IWMS_SetAutoreplace(pMe->m_pwms, 
                                  pMe->m_clientId, 
                                  &pMe->m_callback,
                                  (void*)pMe,
                                  autoreplace);
    }
#endif /* FEATURE_AUTOREPLACE */    
    
    // 设置预约短信定时器
    WmsApp_ReservedMsgSetTimer(pMe);
#endif // FEATURE_CDSMS
}


/*==============================================================================
函数: 
    WmsApp_wmscommandreport
       
说明: 
    WMS 用此函数向命令发出者汇报命令处理结果。
       
参数: 
    cmd [in]: wms 命令类型。
    user_data[in]: 发布命令时传入的用户数据指针。
    cmd_err [in]: 命令状态或错误类型。
    
返回值:
    none
       
备注:

==============================================================================*/
void WmsApp_wmscommandreport(wms_cmd_id_e_type   cmd,
                             void                *user_data,
                             wms_cmd_err_e_type  cmd_err)
{
    WmsApp * pMe = (WmsApp *) (user_data);
  
    if (pMe != NULL)
    {
        wmsapp_cmd_status_type *cmd_status = MALLOC(sizeof(wmsapp_cmd_status_type));
        
        if (cmd_status == NULL)
        {
            return;
        }
        cmd_status->cmd = cmd;
        cmd_status->cmd_err = cmd_err;
        
        (void)ISHELL_PostEventEx(pMe->m_pShell, 
                                 EVTFLG_ASYNC,
                                 AEECLSID_WMSAPP,
                                 EVT_WMS_CMD_STATUS, 
                                 0, 
                                 (uint32)cmd_status);
    }
}

/*==============================================================================
函数:
    WmsApp_UpdateMemoryStatus

说明:
    函数更新当前短信支持的存储器的状态。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
static void WmsApp_UpdateMemoryStatus(WmsApp *pMe, wms_memory_status_s_type *ptr)
{
    switch (ptr->mem_store)
    {
        case WMS_MEMORY_STORE_RUIM:
            if (ptr->tag == WMS_TAG_NONE)
            {
                (void)MEMCPY(&(pMe->m_memruimStatuse),
                             ptr, sizeof(wms_memory_status_s_type));
            }
            break;
            
        case WMS_MEMORY_STORE_NV_CDMA:
            break;
        
        default:
            MSG_ERROR("Not support this mem_store :%d",ptr->mem_store,0,0);
            break;
    }
}

/*==============================================================================
函数:
    WmsApp_FreeSaveNumList

说明:
    释放链表中的全部数据。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeSaveNumList(WmsApp *pMe)
{
    int32     i, dwCount;
    AECHAR    *pItem = NULL;

    if (pMe == NULL)
    {
        return;
    }
    
    if (pMe->m_pSaveNumList == NULL)
    {
        return;
    }
    
    dwCount = IVector_Size(pMe->m_pSaveNumList);

    for (i=dwCount-1; i >= 0; i--)
    {
        // 从链表中删除
        pItem = (AECHAR *)IVector_RemoveElementAt(pMe->m_pSaveNumList, i);
        if (NULL != pItem)
        {
            // 释放动态分配的空间
            FREE(pItem);
            pItem = NULL;
        }
    }
}

/*==============================================================================
函数:
    WmsApp_FreeMultiSendList

说明:
    释放链表中的全部数据。

参数:
    pIV [in]: 指向IVector接口对象的指针。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeMultiSendList(IVector *pIV)
{
    int32       i, dwCount;
    CMultiSendItemInfo *pItem = NULL;

    if (pIV == NULL)
    {
        return;
    }
    
    dwCount = IVector_Size(pIV);

    for (i=dwCount-1; i >= 0; i--)
    {
        pItem = (CMultiSendItemInfo *)IVector_RemoveElementAt(pIV, i);
        if (NULL != pItem)
        {
            // 释放动态分配的空间
            sys_free(pItem);
            pItem = NULL;
        }
    }
}

/*==============================================================================
函数:
    WmsApp_FreeMsgNodeMs

说明:
    函数释放 WmsApp 结构中 m_CurMsgNodesMS 成员保存的数据。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeMsgNodeMs(WmsApp *pMe)
{
    int i;
    
    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
    {
        if (pMe->m_CurMsgNodesMS[i] != NULL)
        {
            FREE(pMe->m_CurMsgNodesMS[i]);
            pMe->m_CurMsgNodesMS[i] = NULL;
        }
    }
}

/*==============================================================================
函数:
    WmsApp_CombinateMsg

说明:
    函数将 WmsApp 结构中 m_CurMsgNodesMS 成员保存的消息数据合并到 m_msCur 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_CombinateMsg(WmsApp *pMe)
{
    WMSMessageStruct *pTep;
    int i, nlen=0, nMaxIdx=1;
    boolean bFirst = TRUE;
    int         nBranches=0;
    int         nCurBranchNum=0;
#ifdef FEATURE_SMS_UDH
    uint8       total_sm;   // 消息数据包总数
    uint8       seq_num;    // 消息序号
#endif    
    AECHAR *pMsgText = NULL;
    AECHAR *pNotRecieved = NULL;
    int32  nSize;
    
    // 结构数据清零
    WMSMessageStruct_Reset(&pMe->m_msCur);
    
    // 先查找任一非空节点
    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
    {
        pTep = pMe->m_CurMsgNodesMS[i];
        if (pTep != NULL)
        {
            break;
        }
    }
    if (NULL == pTep)
    {// 任一非空节点没找到
        return;
    }
    
#ifdef FEATURE_SMS_UDH
    total_sm = pTep->total_sm;
    seq_num = pTep->seq_num;
    if (total_sm > 1)
    {
        // 计算长短信分支总数(每个分支最大含 LONGSMS_MAX_PACKAGES 个数据包)
        nBranches = total_sm / (LONGSMS_MAX_PACKAGES);
        if ((total_sm % (LONGSMS_MAX_PACKAGES)) != 0)
        {
            nBranches++;
        }
        
        // 确定当前处理的分支号(0,...,nBranches-1)
        nCurBranchNum = (seq_num - 1) / (LONGSMS_MAX_PACKAGES);
        if (nBranches>1)
        {
            nlen += 20;
            
            if (nCurBranchNum < (nBranches-1))
            {
                nMaxIdx = LONGSMS_MAX_PACKAGES;
            }
            else
            {
                nMaxIdx = total_sm - nCurBranchNum*LONGSMS_MAX_PACKAGES;
            }
        }
        else
        {
            nMaxIdx = total_sm;
        }
    }
#endif    
    
    // 计算消息体总长度
    for (i=0; i<nMaxIdx; i++)
    {
        if (pMe->m_CurMsgNodesMS[i] != NULL)
        {
            if (NULL != (pMe->m_CurMsgNodesMS[i])->m_szMessage)
            {
                nlen += WSTRLEN((pMe->m_CurMsgNodesMS[i])->m_szMessage);
            }
        }
        else
        {
            nlen += STRLEN(DATA_NOT_RECEIVED);
        }
    }
    
    nSize = (STRLEN(DATA_NOT_RECEIVED)+1)*sizeof(AECHAR);
    pNotRecieved = (AECHAR *)MALLOC(nSize);
    if (NULL == pNotRecieved)
    {
        return;
    }
    (void)STRTOWSTR(DATA_NOT_RECEIVED, pNotRecieved, nSize);
    
    nSize = (nlen+1)*sizeof(AECHAR);
    pMsgText = (AECHAR *)MALLOC(nSize);
    if (NULL == pMsgText)
    {
        FREE(pNotRecieved);
        return;
    }
    
    if (nBranches>1)
    {
        AECHAR  wszFmt[20] = {0};
        
        (void)STRTOWSTR("(%d/%d)", wszFmt, sizeof(wszFmt));
        WSPRINTF(pMsgText, nSize, wszFmt, (nCurBranchNum+1), nBranches);
    }
    
    // 开始合并消息
    for (i=0; i<nMaxIdx; i++)
    {
        if (pMe->m_CurMsgNodesMS[i] != NULL)
        {
            if (bFirst)
            {
                bFirst = FALSE;
                MEMCPY(&pMe->m_msCur, pMe->m_CurMsgNodesMS[i], sizeof(WMSMessageStruct));
                pMe->m_msCur.m_szMessage = pMsgText;
            }
            if (NULL != (pMe->m_CurMsgNodesMS[i])->m_szMessage)
            {
                (void)WSTRCAT(pMsgText, (pMe->m_CurMsgNodesMS[i])->m_szMessage);
            }
        }
        else
        {
            (void)WSTRCAT(pMsgText, pNotRecieved);
        }
    }
    
    FREE(pNotRecieved);
}

/*==============================================================================
函数:
    WmsApp_CopyCurMsgForSend

说明:
    函数将当前处理的消息数据复制到 m_msSend 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_CopyCurMsgForSend(WmsApp *pMe)
{
    int i, nlen=0;
    AECHAR *pMsgText = NULL;
    int32 nSize = 0;
    
    // 结构数据清零
    WMSMessageStruct_Reset(&pMe->m_msSend);

    // 计算消息体总长度
    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
    {
        if ((pMe->m_CurMsgNodesMS[i] != NULL) &&
            (NULL != (pMe->m_CurMsgNodesMS[i])->m_szMessage))
        {
            nlen += WSTRLEN((pMe->m_CurMsgNodesMS[i])->m_szMessage);
        }
    }
    
    nSize = (nlen+1)*sizeof(AECHAR);
    pMsgText = (AECHAR *)MALLOC(nSize);
    if (NULL == pMsgText)
    {
        return;
    }
    
    // 开始拷贝消息
    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
    {
        if ((pMe->m_CurMsgNodesMS[i] != NULL) &&
            (NULL != (pMe->m_CurMsgNodesMS[i])->m_szMessage))
        {
            (void)WSTRCAT(pMsgText, (pMe->m_CurMsgNodesMS[i])->m_szMessage);
        }
    }
    
    pMe->m_msSend.m_szMessage = pMsgText;
}

/*==============================================================================
函数:
    WmsApp_FreeUserDataMOList

说明:
    函数释放 WmsApp 结构中 m_pUserDataMOList 成员保存的数据。

参数:
    pIV [in]: 指向IVector接口对象的指针。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeUserDataMOList(IVector *pIV)
{
    int32       i, dwCount;
    wms_cdma_user_data_s_type *pItem = NULL;

    if (pIV == NULL)
    {
        return;
    }
    
    dwCount = IVector_Size(pIV);

    for (i=dwCount-1; i >= 0; i--)
    {
        // 从链表中删除
        pItem = (wms_cdma_user_data_s_type *)IVector_RemoveElementAt(pIV, i);
        if (NULL != pItem)
        {
            // 释放动态分配的空间
            FREE(pItem);
            pItem = NULL;
        }
    }
}

/*==============================================================================
函数:
    WmsApp_GetUserDataMOList

说明:
    函数用于获取群发链表。

参数:
    none
       
返回值:
    IVector *

备注:
       
==============================================================================*/
static IVector * WmsApp_GetUserDataMOList(void)
{
    // 收件箱消息基本信息列表
    static IVector  *gUserDataMOList = NULL;
    int             nRet;
    
    IShell * pShell = AEE_GetShell();
    
    // 先判断链表接口是否创建，若未创建再创建
    if (NULL != gUserDataMOList)
    {
        return gUserDataMOList;
    }
    
    if (NULL == pShell)
    {
        return NULL;
    }
    
    nRet = ISHELL_CreateInstance(pShell,AEECLSID_VECTOR,(void**)&gUserDataMOList);
    if (nRet != SUCCESS )
    {
        ERR("Failed to Create gMultiSendList !",0,0,0);
        gUserDataMOList = NULL;
    }
    
    return gUserDataMOList;
} // WmsApp_GetUserDataMOList

/*==============================================================================
函数:
    WmsApp_PrepareUserDataMOList

说明:
    函数将当前发送的消息数据 m_msSend 转换到 m_pUserDataMOList 中。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
    本应用发出长短信串接方式固定使用 concat_8
==============================================================================*/
void WmsApp_PrepareUserDataMOList(WmsApp *pMe)
{
    int i;
    int nItems = 0;     // 将消息划分后得到的总项数
    int nMaxItemChars = 0;     // 每项最大字符数
    int nLen = 0;
    int nChars=0;
    int nEnd;
    int nMaxChars_UNICODE = MAX_UNICODMSG_PAYLOAD;
    int nMaxChars_ASCII = MAX_ASCIIMSG_PAYLOAD;
    int32 nSize;
    wms_user_data_encoding_e_type      encoding;
    wms_cdma_user_data_s_type          *pUserdata = NULL;
    
    // 释放消息用户数据链表
    WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
    
    if (NULL != pMe->m_msSend.m_szMessage)
    {
        nLen = WSTRLEN(pMe->m_msSend.m_szMessage);
    }
    
    if (nLen == 0)
    {// 空消息
        nSize = sizeof(wms_cdma_user_data_s_type);
        
        pUserdata = (wms_cdma_user_data_s_type *)MALLOC(nSize);
        if (NULL != pUserdata)
        {
            MEMSET(pUserdata, 0, nSize);
            pUserdata->encoding = WMS_ENCODING_ASCII;
            (void) IVector_EnsureCapacity(pMe->m_pUserDataMOList, 1);
            
            if (SUCCESS != IVector_AddElement(pMe->m_pUserDataMOList, pUserdata))
            {
                FREE(pUserdata);
                ERR("Add item Failed",0,0,0);
            }
        }
        
        return;
    }
    // 确定消息编码方式及需划分的条数
    if (WMSUtil_HaveNoneASCIIChar(pMe->m_msSend.m_szMessage, NULL))
    {
        encoding = WMS_ENCODING_UNICODE;
    }
    else
    {
#if defined (FEATURE_CARRIER_ANGOLA_MOVICEL)
        byte  mo_encoding=0;
        
        (void) ICONFIG_GetItem(pMe->m_pConfig, 
                               CFGI_SMS_MO_ENCODING,
                               &mo_encoding,
                               sizeof(mo_encoding));
        if (mo_encoding == OEMNV_SMS_MO_ENCODING_7BIT )
        {
            encoding = WMS_ENCODING_ASCII;
        }
        else
        {
            encoding = WMS_ENCODING_LATIN;
        }
#elif defined(FEATURE_CARRIER_ISRAEL_PELEPHONE)        
        encoding = WMS_ENCODING_LATIN_HEBREW;
#else //FEATURE_CARRIER_ANGOLA_MOVICEL
        if (nLen<=MAX_OCTETMSG_PAYLOAD)
        {
#if defined(FEATURE_SMS_GSM7BIT)
#if defined (FEATURE_CARRIER_VENEZUELA_MOVILNET)
            encoding = WMS_ENCODING_ASCII;
#else
            encoding = WMS_ENCODING_LATIN;
#endif
#else
            encoding = WMS_ENCODING_ASCII;
#endif               
        }
        else
        {
            encoding = WMS_ENCODING_ASCII;
        }
#endif //FEATURE_CARRIER_ANGOLA_MOVICEL
    }

    // 确定消息编码方式及需划分的条数
    if (WMS_ENCODING_UNICODE == encoding)
    {        
        if (nLen<=nMaxChars_UNICODE)
        {
            nMaxItemChars = nMaxChars_UNICODE;
        }
        else
        {
#ifdef FEATURE_SMS_UDH
            nMaxItemChars = (nMaxChars_UNICODE-3);
#else
            nMaxItemChars = nMaxChars_UNICODE;
#endif            
        }
    }
    else if (WMS_ENCODING_ASCII == encoding)
    {        
        if (nLen<=nMaxChars_ASCII)
        {
            nMaxItemChars = nMaxChars_ASCII;
        }
        else
        {
#ifdef FEATURE_SMS_UDH
            nMaxItemChars = (nMaxChars_ASCII-7);
#else
            nMaxItemChars = nMaxChars_ASCII;
#endif            
        }
    }
    else
    {        
#ifdef FEATURE_SMS_UDH
        if (nLen <= MAX_OCTETMSG_PAYLOAD)
        {
            nMaxItemChars = MAX_OCTETMSG_PAYLOAD;
        }
        else
        {
            nMaxItemChars = MAX_OCTETMSG_PAYLOAD-6;
        }
#else
        nMaxItemChars = MAX_OCTETMSG_PAYLOAD;
#endif            
    }
    
    nItems = nLen / nMaxItemChars;
    if (nLen % nMaxItemChars != 0)
    {
        nItems++;
    }
    
    (void) IVector_EnsureCapacity(pMe->m_pUserDataMOList, nItems);
    
    // 开始创建消息用户数据列表
    for (i=0; i<nItems; i++)
    {
        nSize = sizeof(wms_cdma_user_data_s_type);
        
        pUserdata = (wms_cdma_user_data_s_type *)MALLOC(nSize);
        if (NULL == pUserdata)
        {
            ERR("Malloc Failed!",0,0,0);
            break;
        }
        
        MEMSET(pUserdata, 0, nSize);
        pUserdata->encoding = encoding;
#ifdef FEATURE_SMS_UDH
        if (nItems>1)
        {
            pUserdata->num_headers = 1;
            pUserdata->headers[0].header_id = WMS_UDH_CONCAT_8;
            
            // 注意: msg_ref 在稍后设置
            //pUserdata->headers[0].u.concat_8.msg_ref = ?;
            pUserdata->headers[0].u.concat_8.total_sm = nItems;
            pUserdata->headers[0].u.concat_8.seq_num = i+1;
        }
#endif 

        if (i==nItems-1)
        {
            nChars = nLen - i*nMaxItemChars;
        }
        else
        {
            nChars = nMaxItemChars;
        }
        nEnd = i*nMaxItemChars + nChars;
        
        // 开始编码数据       
        if (encoding == WMS_ENCODING_ASCII)
        {
            char  *szText = NULL;
#if !defined(FEATURE_SMS_GSM7BIT)
            AECHAR wChar = pMe->m_msSend.m_szMessage[nEnd];
#endif            
            
            nSize = (nChars+1)*sizeof(char);
            szText = (char *)MALLOC(nSize);
            if (NULL == szText)
            {
                FREE(pUserdata);
                ERR("Malloc Failed!",0,0,0);
                
                return;
            }
                
#if defined(FEATURE_SMS_GSM7BIT)
            {
                int     j;
                word    c_temp;
                char    *strTep = szText;
                
                for (j=i*nMaxItemChars; j<nEnd; j++, strTep++)
                {
                    c_temp = Ucs2ToGsm[pMe->m_msSend.m_szMessage[j]];
                    
                    if (c_temp == NON_GSM)
                    {
                        c_temp = 0x20;//' '
                    }
                    *strTep = (char)c_temp;
                }
            }
#else            
            pMe->m_msSend.m_szMessage[nEnd] = 0;
            (void)WSTRTOSTR(&pMe->m_msSend.m_szMessage[i*nMaxItemChars], szText, nSize);
            pMe->m_msSend.m_szMessage[nEnd] = wChar;
#endif
            
            pUserdata->data_len = nChars;
            pUserdata->number_of_digits = wms_ts_pack_ascii(szText,
                                                        pUserdata->data,
                                                        &pUserdata->data_len,
                                                        &pUserdata->padding_bits);
            FREE(szText);
            if (SUCCESS != IVector_AddElement(pMe->m_pUserDataMOList, pUserdata))
            {
                FREE(pUserdata);
                ERR("Add item Failed",0,0,0);
                return;
            }
        }
        else if (encoding == WMS_ENCODING_UNICODE)
        {
            uint16  *pTep = (uint16 *)pUserdata->data;
            int j;
            
            pUserdata->number_of_digits = nChars;
            pUserdata->data_len = nChars*sizeof(AECHAR);
            pUserdata->padding_bits = 0;
            
            for (j=i*nMaxItemChars; j<nEnd; j++,pTep++)
            {
                *pTep = HTONS(pMe->m_msSend.m_szMessage[j]);
            }
            
            if (SUCCESS != IVector_AddElement(pMe->m_pUserDataMOList, pUserdata))
            {
                FREE(pUserdata);
                ERR("Add item Failed",0,0,0);
                return;
            }
        }
#ifdef FEATURE_LANG_HEBREW
        else if (encoding == WMS_ENCODING_LATIN_HEBREW)
        {
            byte    *pTep = (byte *)pUserdata->data;
            int     j;
            AECHAR  wChar;
            
            pUserdata->number_of_digits = nChars;
            pUserdata->data_len = nChars;
            pUserdata->padding_bits = 0;
            
            for (j=i*nMaxItemChars; j<nEnd; j++,pTep++)
            {
                if (pMe->m_msSend.m_szMessage[j] == 0x203E)
                {
                    *pTep = 0xAF;
                }
                else if (pMe->m_msSend.m_szMessage[j] == 0x00D7)
                {
                    *pTep = 0xAA;
                }
                else
                {
                    wChar = Ucs2ToHebrew[pMe->m_msSend.m_szMessage[j] & 0x00FF];
                    
                    *pTep = (wChar == NON_HEB ? 0x20 : (byte)wChar);
                }
            }
            
            if (SUCCESS != IVector_AddElement(pMe->m_pUserDataMOList, pUserdata))
            {
                FREE(pUserdata);
                ERR("Add item Failed",0,0,0);
                return;
            }
        }
#endif        
        else
        {// encoding == WMS_ENCODING_OCTET
            byte  *pTep = (byte *)pUserdata->data;
            int j;
            
            pUserdata->number_of_digits = nChars;
            pUserdata->data_len = nChars;
            pUserdata->padding_bits = 0;
            
            for (j=i*nMaxItemChars; j<nEnd; j++,pTep++)
            {
                *pTep = (byte)pMe->m_msSend.m_szMessage[j];
            }
            
            if (SUCCESS != IVector_AddElement(pMe->m_pUserDataMOList, pUserdata))
            {
                FREE(pUserdata);
                ERR("Add item Failed",0,0,0);
                return;
            }
        }
    }
}

/*==============================================================================
函数:
    WmsApp_GetClientMsgMO

说明:
    函数根据 wms_cdma_user_data_s_type 结构消息创建 wms_client_message_s_type 结
    构 MO 消息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    bSend [in]: 消息是否需要发送。
    

返回值:
    none

备注:
    返回数据 buffer 系动态分配，由调用者负责释放。

==============================================================================*/
wms_client_message_s_type *WmsApp_GetClientMsgMO(WmsApp *pMe, boolean bSend)
{
    int32 nSize;
    int32 dwCount;
    wms_client_ts_data_s_type *pCltTsData = NULL;
    wms_client_message_s_type *pCltMsg = NULL;
    wms_cdma_user_data_s_type *pUserdata = NULL;
#ifdef FEATURE_SMS_UDH
    boolean udh_present = FALSE;
#endif
    char  strNum[MAX_PH_DIGITS+1];
    
    if ((IVector_Size(pMe->m_pSendList) == 0) && bSend)
    {// 消息已发送完毕
        return NULL;
    }
    dwCount = IVector_Size(pMe->m_pUserDataMOList);
#ifdef FEATURE_SMS_UDH
    if ((dwCount>1) && (pMe->m_idxUserdata == 0))
    {// 设置长短信的消息参考号
        int  i;
        uint8 msg_ref;
        
        msg_ref = WmsApp_GetMsgRef(pMe);
        for (i=0; i<dwCount; i++)
        {
            pUserdata = (wms_cdma_user_data_s_type *)IVector_ElementAt(pMe->m_pUserDataMOList, i);
            
            if (NULL != pUserdata)
            {
                pUserdata->headers[0].u.concat_8.msg_ref = msg_ref;
            }
        }
    }
    
    if (dwCount>1)
    {
        udh_present = TRUE;
    }
#endif 
   
    if (pMe->m_idxUserdata >= dwCount)
    {
        return NULL;
    }
    
    pUserdata = (wms_cdma_user_data_s_type *)IVector_ElementAt(pMe->m_pUserDataMOList, pMe->m_idxUserdata);
    if (NULL == pUserdata)
    {
        return NULL;
    }
    pMe->m_idxUserdata++;
    
    nSize = sizeof(wms_client_ts_data_s_type);
    pCltTsData = (wms_client_ts_data_s_type *)MALLOC(nSize);
    if (NULL == pCltTsData)
    {
        ERR("sys_malloc Failed!",0,0,0);
        return NULL;
    }
    MEMSET(pCltTsData, 0, nSize);
    
    nSize = sizeof(wms_client_message_s_type);
    pCltMsg = (wms_client_message_s_type *)MALLOC(nSize);
    if (NULL == pCltMsg)
    {
        FREE(pCltTsData);
        ERR("sys_malloc Failed!",0,0,0);
        return NULL;
    }
    
    // -------- 填充 wms_client_ts_data_s_type 结构 --------
    pCltTsData->format = WMS_FORMAT_CDMA;
    pCltTsData->u.cdma.mask = 0;

    // 消息 ID 部分
    pCltTsData->u.cdma.message_id.id_number = WmsApp_GetMsgIDNum(pMe);
    pCltTsData->u.cdma.message_id.type = WMS_BD_TYPE_SUBMIT;
#ifdef FEATURE_SMS_UDH
    pCltTsData->u.cdma.message_id.udh_present = udh_present;
#endif
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_MSG_ID;

    // 消息用户数据部分
    nSize = sizeof(wms_cdma_user_data_s_type);
    MEMCPY(&pCltTsData->u.cdma.user_data, pUserdata, nSize);
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
    
    // 时间戳
    {
        uint32 sec = GETTIMESECONDS();
        
        if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
            (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
        {
            sec = pMe->m_rsvDateTime.dwSecs;
        }
        ConvertMStoMcTime(sec, &pCltTsData->u.cdma.mc_time);
    }
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_MC_TIME;
    
    // 绝对有效时间: TODO
    
    // 相对有效时间:
    {
        byte   btValidity = OEMNV_SMS_VALIDITYPERIOD_MAX;
        
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_WMS_VALIDITYPERIOD,
                               &btValidity,
                               sizeof(btValidity));
                               
        wms_ts_decode_relative_time(btValidity, &(pCltTsData->u.cdma.validity_relative));
        //pCltTsData->u.cdma.mask |= WMS_MASK_BD_VALID_REL;
    }
    
    
    // 绝对延迟时间: TODO
    
    
    // 相对延迟时间: TODO
    
    
    // 优先级: 
    pCltTsData->u.cdma.priority = pMe->m_msSend.m_epriority;
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_PRIORITY;
    
    // 隐私级别: TODO
    
    
    // 回复选项:
    pCltTsData->u.cdma.reply_option.user_ack_requested = FALSE;
#ifndef FEATURE_CARRIER_TAIWAN_APBW
    pCltTsData->u.cdma.reply_option.delivery_ack_requested = pMe->m_msSend.m_bDeliveryReport;
#else
    pCltTsData->u.cdma.reply_option.delivery_ack_requested = FALSE;
#endif    
    pCltTsData->u.cdma.reply_option.read_ack_requested = FALSE;
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_REPLY_OPTION;
    
    pCltTsData->u.cdma.num_messages = 1;
    
#ifndef FEATURE_CARRIER_TAIWAN_APBW
    // Callback Number
    if (WSTRLEN(pMe->m_msSend.m_szCallBkNum)>0)
    {
        MEMSET(strNum, 0, sizeof(strNum));
        (void)WSTRTOSTR(pMe->m_msSend.m_szCallBkNum, strNum, sizeof(strNum));
        if (strNum[0] == '+')
        {
            pCltTsData->u.cdma.callback.number_of_digits = IWMS_TsAsciiToDtmf(pMe->m_pwms, &strNum[1], pCltTsData->u.cdma.callback.digits);
        }
        else
        {
            pCltTsData->u.cdma.callback.number_of_digits = IWMS_TsAsciiToDtmf(pMe->m_pwms, strNum, pCltTsData->u.cdma.callback.digits);
        }
        pCltTsData->u.cdma.callback.digit_mode = WMS_DIGIT_MODE_4_BIT;
        pCltTsData->u.cdma.callback.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
        pCltTsData->u.cdma.callback.number_type = WMS_NUMBER_UNKNOWN;
        pCltTsData->u.cdma.callback.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
        pCltTsData->u.cdma.mask |= WMS_MASK_BD_CALLBACK;
    }
#endif    
    
    // -------- 填充 wms_client_message_s_type 结构 --------
    pCltMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
        (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
    {
        pCltMsg->msg_hdr.tag = WMS_TAG_RESERVE;
    }
    else
    {
        pCltMsg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
    }
    pCltMsg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
    pCltMsg->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;
    
    pCltMsg->u.cdma_message.is_mo = TRUE;
    pCltMsg->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
    
#ifdef FEATURE_SMS_UDH
    if (udh_present)
    {
        pCltMsg->u.cdma_message.concat_8.msg_ref = pUserdata->headers[0].u.concat_8.msg_ref;
        pCltMsg->u.cdma_message.concat_8.total_sm = pUserdata->headers[0].u.concat_8.total_sm;
        pCltMsg->u.cdma_message.concat_8.seq_num = pUserdata->headers[0].u.concat_8.seq_num;
    }
#endif    
    
    // 接收地址
    MEMSET(strNum, 0, sizeof(strNum));
    (void)WSTRTOSTR(pMe->m_msSend.m_szNum, strNum, sizeof(strNum));
    if (strNum[0] == '+')
    {
        pCltMsg->u.cdma_message.address.number_of_digits = IWMS_TsAsciiToDtmf(pMe->m_pwms, &strNum[1], pCltMsg->u.cdma_message.address.digits);
    }
    else
    {
        pCltMsg->u.cdma_message.address.number_of_digits = IWMS_TsAsciiToDtmf(pMe->m_pwms, strNum, pCltMsg->u.cdma_message.address.digits);
    }
    pCltMsg->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
    pCltMsg->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
    pCltMsg->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
    pCltMsg->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    
    pCltMsg->u.cdma_message.is_tl_ack_requested = TRUE;
    pCltMsg->u.cdma_message.is_service_present = FALSE;

    (void)IWMS_TsEncode(pMe->m_pwms, pCltTsData, &pCltMsg->u.cdma_message.raw_ts);
    
    // 消息编码结束
    
    FREE(pCltTsData);
    
    return pCltMsg;
}

/*==============================================================================
函数:
    WmsApp_ProcessStatus

说明:
    函数处理来自 wms 的报告信息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pRptInfo [in]: wms_submit_report_info_s_type 结构指针。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ProcessStatus(WmsApp *pMe, wms_submit_report_info_s_type *pRptInfo)
{
    pMe->m_SendStatus = pRptInfo->report_status;
    pMe->m_SendtlStatus = pRptInfo->cause_info.tl_status;
    
    switch (pRptInfo->report_status)
    {
        // Success: submit_report_ack info is available for GSM/WCDMA
        case WMS_RPT_OK:
        // Lower layer errors;
        // For GSM/WCDMA: cause_value = LL Cause, not available to clients.
        case WMS_RPT_LL_ERROR:
        
        case WMS_RPT_OUT_OF_RESOURCES:
        case WMS_RPT_SMR_NO_RESOURCES:

        case WMS_RPT_NETWORK_NOT_READY:

        case WMS_RPT_PHONE_NOT_READY:

        case WMS_RPT_NO_ACK:

        case WMS_RPT_CDMA_TL_ERROR:
            break;

        case WMS_RPT_ACCESS_TOO_LARGE:
            // 从接入信道发送失败，改走业务信道
//            if (WMSAPPMN_DcConnect(pMe, pMe->m_AutoDcSendSO))
//            {
//                return TRUE;
//            }
        /* Fall Through */
            return;

        case WMS_RPT_DC_TOO_LARGE:

        case WMS_RPT_NOT_ALLOWED_IN_AMPS:
            
        case WMS_RPT_CANNOT_SEND_BROADCAST:
            
        case WMS_RPT_RP_ERROR:
            
        case WMS_RPT_CP_ERROR:
            
        case WMS_RPT_SMR_TR1M_TIMEOUT:
            
        case WMS_RPT_SMR_TR2M_TIMEOUT:
            
        // Any other error
        default:
            break;
    }
    
    ERR("pRptInfo->report_status = %d", pRptInfo->report_status, 0, 0);
    
    if (pRptInfo->report_status != WMS_RPT_OK && 
        pMe->m_nDisconnectedInSendingRetryTimes==0 &&
        pMe->m_bDCDisconnectedInSending)
    {
        int nRet;
        
        pMe->m_bDCDisconnectedInSending = FALSE;
        // 此情况下将消息重发一次
        if ((NULL != pMe->m_pCurSendCltMsg) &&
            (pMe->m_idxCurSend < pMe->m_nSendItems))
        {
            pMe->m_nDisconnectedInSendingRetryTimes = 1;
            nRet = IWMS_MsgSend(pMe->m_pwms, 
                                pMe->m_clientId, 
                                &pMe->m_callback,
                                (void*)pMe,
                                WMS_SEND_MODE_CLIENT_MESSAGE,
                                pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
                                
            if (nRet == SUCCESS)
            {
                pMe->m_bSending = TRUE;
                return;
            }
        }
    }
    
    if ((pRptInfo->report_status == WMS_RPT_OK) &&
        (pMe->m_eCreateWMSType != SEND_MSG_RESERVE))
    {// 修改消息标识
        if ((NULL != pMe->m_pCurSendCltMsg) &&
            (pMe->m_idxCurSend < pMe->m_nSendItems))
        {
            wms_client_message_s_type *pItem = pMe->m_pCurSendCltMsg[pMe->m_idxCurSend];
             
            if ((NULL != pItem) &&
                (pItem->msg_hdr.index != WMS_DUMMY_MESSAGE_INDEX))
            {
                (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                        pMe->m_clientId,
                                        &pMe->m_callback,
                                        (void *)pMe,
                                        pItem->msg_hdr.mem_store,
                                        pItem->msg_hdr.index,
                                        WMS_TAG_MO_SENT);
            }
        }
    }
    
    pMe->m_nDisconnectedInSendingRetryTimes = 0;
    
    if ((pMe->m_wActiveDlgID == IDD_SENDING) &&
        (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP))
    {
        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                 EVTFLG_ASYNC, 
                                 AEECLSID_WMSAPP, 
                                 EVT_UPDATE,
                                 0, 
                                 0);
    }
    else
    {
        if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
        {// 作预约失败处理
            WmsApp_ReservedMsgSetTimer(pMe);
        }
    }
}

/*==============================================================================
函数:
    WmsApp_MemberReset

说明:
    函数对 WmsApp 结构部分成员变量进行重置。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_MemberReset(WmsApp *pMe)
{
    if (NULL == pMe)
    {
        return;
    }
    
    // 取消发送短信的定时器
    (void)ISHELL_CancelTimer(pMe->m_pShell, WmsApp_MultSendMsgTimer, pMe);
    (void)ISHELL_CancelTimer(pMe->m_pShell, WmsApp_ReSendMsgTimer, pMe);
    
    if (NULL != pMe->m_pCurCltPhrase)
    {
        FREE(pMe->m_pCurCltPhrase);
        pMe->m_pCurCltPhrase = NULL;
    }
    
    WMSMessageStruct_Reset(&pMe->m_msCur);
    
    MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
    WmsApp_FreeMsgNodeMs(pMe);

    pMe->m_prevState = 0;
    pMe->m_bDoNotOverwriteDlgResult = FALSE;
    pMe->m_eMBoxType = WMS_MB_NONE;
    pMe->m_eMakeListMode = MAKEMSGLIST_INIT;
}

/*==============================================================================
函数：
    WmsApp_UpdateAnnunciators

说明：
    基于当前新来短信(包含语音邮件)数，更新提示图标。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    即使Applet不激活也会更新。
        
==============================================================================*/
void WmsApp_UpdateAnnunciators(WmsApp * pMe)
{
    uint16  nNews = 0, nVmNews = 0, nVmMsgs = 0;
    uint16  nMsgs=0;
    uint16  nOnUIMs=0;
    wms_routing_s_type  rt;
    
    // 0-full 1-VM 2-SMS
    boolean smsiconstatus[3] = {0};
    
    if ((!pMe->m_bCdmaWmsReady) || (pMe->m_refresh_in_progress))
    {// 初始化还没完成
        return;
    }
    
    rt.route = WMS_ROUTE_STORE_AND_NOTIFY;
    rt.mem_store = WMS_MEMORY_STORE_NV_CDMA;
    
    wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nVmNews, NULL, &nVmMsgs);
    wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNews, &nOnUIMs, &nMsgs);
#ifdef FEATURE_LED_CONTROL
    if ((nVmNews>0) || (nNews>0))
    {
        IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);    //cancel the previous LED control
        IBACKLIGHT_SigLedEnable(pMe->m_pBacklight, SIG_LED_SMS);
        IBACKLIGHT_Enable(pMe->m_pBacklight);
    }
    else
    {
        IBACKLIGHT_SigLedDisable(pMe->m_pBacklight);
    }
#endif /*FEATURE_LED_CONTROL */    
    
    // 检查语音通知短信
#if defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
    if (nVmMsgs)
#else
    if (nVmNews)
#endif        
    {
        smsiconstatus[1] = TRUE;
    }
    
    // 检查收件箱
    if (nNews)
    {
        smsiconstatus[2] = TRUE;
        
    }
    
    // 存储空间的相关检查
#ifdef FEATURE_CDSMS_RUIM
    if (IsRunAsUIMVersion())
    {// 有卡版本
        uint16  nInMsgs=0;
        
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                               CFGI_WMS_MEMSTORE,
                               &rt.mem_store,
                               sizeof(rt.mem_store));

        nInMsgs = nMsgs - nOnUIMs;
        if (rt.mem_store == WMS_MEMORY_STORE_RUIM)
        {// 用户优先使用RUIM
            if (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots)
            {// UIM 存储空间已满
                if (nInMsgs >= IN_MAX)
                {// 存储空间已满
                    rt.route = WMS_ROUTE_TRANSFER_ONLY;
                    smsiconstatus[0] = TRUE;
                }
                else
                {// 话机收件箱还有存储空间
                    rt.route = WMS_ROUTE_STORE_AND_NOTIFY;
                    rt.mem_store = WMS_MEMORY_STORE_NV_CDMA;
                }
            }
            else
            {// RUIM 上尚有空间
                rt.route = WMS_ROUTE_STORE_AND_NOTIFY;
            }
        }
        else
        {// 用户优先使用手机内存
            if (nInMsgs >= IN_MAX)
            {// 话机收件箱已满
                if (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots)
                {// 存储空间已满
                    rt.route = WMS_ROUTE_TRANSFER_ONLY;
                    smsiconstatus[0] = TRUE;
                }
                else
                {// RUIM 上尚有空间
                    rt.route = WMS_ROUTE_STORE_AND_NOTIFY;
                    rt.mem_store = WMS_MEMORY_STORE_RUIM;
                }
            }
            else
            {// 话机收件箱还有存储空间
                rt.route = WMS_ROUTE_STORE_AND_NOTIFY;
            }
        }
    }
    else
#endif   
    {// 无卡版本
        if (nMsgs >= IN_MAX)
        {// 存储空间已满
            rt.route = WMS_ROUTE_TRANSFER_ONLY;
            smsiconstatus[0] = TRUE;
        }
    }
    
    if ((pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store != rt.mem_store) ||
        (rt.route != pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].route))
    {
        pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].route = rt.route;
        pMe->m_routes.pp_routes[WMS_MESSAGE_CLASS_CDMA].mem_store = rt.mem_store;
        
        ERR("rt.mem_store=%d", rt.mem_store, 0, 0);
        if (rt.route != WMS_ROUTE_TRANSFER_ONLY)
        {
            (void)IWMS_CfgSetRoutes(pMe->m_pwms,
                                   pMe->m_clientId,
                                   &pMe->m_callback,
                                   (void*)pMe,
                                   &pMe->m_routes);
        }
                               
        (void)IWMS_SetMemoryFull(pMe->m_pwms, 
                                 pMe->m_clientId, 
                                 &pMe->m_callback,
                                 (void*)pMe,
                                 (rt.route == WMS_ROUTE_TRANSFER_ONLY ? TRUE : FALSE));
    }
    
    if (smsiconstatus[0])
    {
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_SMSMEMORYFULL*/, ANNUN_STATE_SMS_MAILFULL_ON/*ANNUN_STATE_ON*/ | ANNUN_STATE_BLINK);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_VMAIL*/, ANNUN_STATE_SMS_VMAIL_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS, ANNUN_STATE_SMS_SMAIL_OFF/*ANNUN_STATE_OFF*/);
    }
    else if (smsiconstatus[1])
    {
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_VMAIL*/, ANNUN_STATE_SMS_VMAIL_ON/*ANNUN_STATE_ON*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_SMSMEMORYFULL*/, ANNUN_STATE_SMS_MAILFULL_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS, ANNUN_STATE_SMS_SMAIL_OFF/*ANNUN_STATE_OFF*/);
    }
    else if (smsiconstatus[2])
    {
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS, ANNUN_STATE_SMS_SMAIL_ON/*ANNUN_STATE_ON*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_SMSMEMORYFULL*/, ANNUN_STATE_SMS_MAILFULL_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_VMAIL*/, ANNUN_STATE_SMS_VMAIL_OFF/*ANNUN_STATE_OFF*/);
    }
    else
    {
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_SMSMEMORYFULL*/, ANNUN_STATE_SMS_MAILFULL_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS/*ANNUN_FIELD_VMAIL*/, ANNUN_STATE_SMS_VMAIL_OFF/*ANNUN_STATE_OFF*/);
        IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_SMS, ANNUN_STATE_SMS_SMAIL_OFF/*ANNUN_STATE_OFF*/);
    }
}

/*==============================================================================
函数：
    WmsApp_GetmemAlertID

说明：
    函数获取当前存储空间告警字符串在资源文件中的 ID。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eBox[in]: wms_box_e_type

返回值：
    告警字符串在资源文件中的 ID。

备注：
    若返回值为0，表示不需要告警
        
==============================================================================*/
uint16 WmsApp_GetmemAlertID(WmsApp * pMe, wms_box_e_type eBox)
{
    uint16  nMsgs=0;
    uint16  nOnUIMs=0;
    uint16  nMsgID = 0;
    
    // 获取消息数
    wms_cacheinfolist_getcounts(eBox, NULL, &nOnUIMs, &nMsgs);
    
    switch (eBox)
    {
        case WMS_MB_VOICEMAIL:
            if (nMsgs == 0)
            {
                nMsgID = IDS_EMPTY;
            }
            break;
            
        case WMS_MB_INBOX:   
            if (nMsgs == 0)
            {
                nMsgID = IDS_EMPTY;
            }
#ifdef FEATURE_CARRIER_THAILAND_HUTCH 
#ifdef FEATURE_CDSMS_RUIM
            else if (IsRunAsUIMVersion())
            {
                uint16  nInMsgs=0;
                nInMsgs = nMsgs - nOnUIMs;
                if ((nInMsgs >= IN_MAX) && (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots))
                {
                    nMsgID = IDS_INBOXSFULL;
                }  
            }
            else
#endif //FEATURE_CDSMS_RUIM                
            {
                if (nMsgs >= IN_MAX)
                {
                    nMsgID = IDS_INBOXSFULL;
                }              
            }
#else //FEATURE_CARRIER_THAILAND_HUTCH            
#ifdef FEATURE_CDSMS_RUIM
            else if (IsRunAsUIMVersion())
            {
                uint16  nInMsgs=0;
                wms_memory_store_e_type   mem_store = WMS_MEMORY_STORE_RUIM;

                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_MEMSTORE,
                                       &mem_store,
                                       sizeof(mem_store));
                nInMsgs = nMsgs - nOnUIMs;
                
                if (mem_store == WMS_MEMORY_STORE_RUIM)
                {
                    if (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots)
                    {
                        if (nInMsgs >= IN_MAX)
                        {
                            nMsgID = IDS_INBOXSFULL;
                        }
                        else
                        {
                            nMsgID = IDS_RUIMFULL;
                        }
                    }
                    else if (pMe->m_memruimStatuse.used_tag_slots + 5 >= pMe->m_memruimStatuse.max_slots)
                    {
                        nMsgID = IDS_RUIMWILLFULL;
                    }
                    else if (nInMsgs >= IN_MAX)
                    {
                        nMsgID = IDS_INBOXSFULL;
                    }
                    else if (nInMsgs >= IN_WATERMARK)
                    {
                        nMsgID = IDS_INBOXWILLFULL;
                    }
                }
                else
                {
                    if (nInMsgs >= IN_MAX)
                    {
                        if (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots)
                        {
                            nMsgID = IDS_INBOXSFULL;
                        }
                        else
                        {
                            nMsgID = IDS_INBOXSFULL;
                        }
                    }
                    else if (nInMsgs >= IN_WATERMARK)
                    {
                        nMsgID = IDS_INBOXWILLFULL;
                    }
                    else if (pMe->m_memruimStatuse.used_tag_slots >= pMe->m_memruimStatuse.max_slots)
                    {
                        nMsgID = IDS_RUIMFULL;
                    }
                    else if (pMe->m_memruimStatuse.used_tag_slots + 5 >= pMe->m_memruimStatuse.max_slots)
                    {
                        nMsgID = IDS_RUIMWILLFULL;
                    }
                }
            }
            else
#endif   
            {
                if (nMsgs >= IN_MAX)
                {
                    nMsgID = IDS_INBOXSFULL;
                }
                else if (nMsgs >= IN_WATERMARK)
                {
                    nMsgID = IDS_INBOXWILLFULL;
                }
            }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
            break;
            
        case WMS_MB_OUTBOX:   
            if (nMsgs == 0)
            {
                nMsgID = IDS_EMPTY;
            }
            else if (nMsgs-nOnUIMs >= OUT_MAX)
            {
                nMsgID = IDS_OUTBOXFULL;
            }
            else if (nMsgs-nOnUIMs >= OUT_WATERMARK)
            {
                nMsgID = IDS_OUTBOXWILLFULL;
            }
            break;
            
        case WMS_MB_DRAFT:   
            if (nMsgs == 0)
            {
                nMsgID = IDS_EMPTY;
            }
            else if (nMsgs >= DRAFT_MAX)
            {
                nMsgID = IDS_DRAFTFULL;
            }
            break;
    
        default:
            break;
    }
    
    return  nMsgID;
}

/*==============================================================================
函数：
    WmsApp_PlaySMSAlert

说明：
    函数用来播放短信提示。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    bsmsin[in]: TRUE - 来信  FALSE - 去信

返回值：
    none

备注：
        
==============================================================================*/
void WmsApp_PlaySMSAlert(WmsApp * pMe, boolean bsmsin)
{
    byte    btActiveProfile;
    ringID  SmsRingerID[PROFILENUMBER]={0};

    IBACKLIGHT_Enable(pMe->m_pBacklight); 

#ifdef FEATURE_VIDEOPLAYER
#ifndef WIN32
     if(GetDeviceState(DEVICE_TYPE_MP4) == DEVICE_MP4_STATE_ON)
     {
         ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE);
         return;
     }
#endif
#endif
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                    CFGI_PROFILE_CUR_NUMBER,
                    &btActiveProfile,
                    sizeof(btActiveProfile));

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                    CFGI_PROFILE_SMS_RINGER_ID, 
                    (void*)SmsRingerID, 
                    sizeof(SmsRingerID));
    
    if (btActiveProfile<PROFILENUMBER)
    {
         IALERT_StopMp3Alert(pMe->m_pAlert);
        if(SmsRingerID[btActiveProfile].ringType == OEMNV_MP3_RINGER)
        {

            // 注册按键notify事件，当有按键时关闭MP3短信提示音
            (void)ISHELL_RegisterNotify(pMe->m_pShell, 
                                        AEECLSID_WMSAPP,
                                        AEECLSID_SHELL,
                                        ( ( (uint32)(NOTIFIER_VAL_ANY) ) << 16 ) | NMASK_SHELL_KEY);
                
            if ((IALERT_StartMp3Alert(pMe->m_pAlert, SmsRingerID[btActiveProfile].szMusicname,ALERT_SMS_SND) != SUCCESS))
            {
                (void) IALERT_StartSMSAlert(pMe->m_pAlert, SmsRingerID[btActiveProfile].midID);
            }            
        }
        else
        {
            (void) IALERT_StartSMSAlert(pMe->m_pAlert, SmsRingerID[btActiveProfile].midID);
        }
    }    
}

/*==============================================================================
函数:
    WmsApp_FreeSendClentMsgList

说明:
    释放发送客户消息表中的全部数据项。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_FreeSendClentMsgList(WmsApp * pMe)
{
    int    i;
    wms_client_message_s_type *pItem = NULL;

    if (pMe->m_pCurSendCltMsg == NULL)
    {
        pMe->m_idxCurSend = 0;
        pMe->m_nSendItems = 0;
        return;
    }
    
    for (i=0; i<pMe->m_nSendItems; i++)
    {
        pItem = pMe->m_pCurSendCltMsg[i];
        if (NULL != pItem)
        {
            // 释放动态分配的空间
            FREE(pItem);
        }
    }
    
    pMe->m_idxCurSend = 0;
    pMe->m_nSendItems = 0;
    
    FREE(pMe->m_pCurSendCltMsg);
    pMe->m_pCurSendCltMsg = NULL;
}

/*==============================================================================
函数:
    WmsApp_BuildSendClentMsgList

说明:
    建立发送客户消息列表。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
void WmsApp_BuildSendClentMsgList(WmsApp * pMe)
{
    int    i;
    int32  nItems = 0;
    int32  nSize;
    CMultiSendItemInfo *pItem = NULL;
    wms_client_message_s_type *pClientMsg = NULL;

    WmsApp_FreeSendClentMsgList(pMe);
    
    if (0 == IVector_Size(pMe->m_pSendList))
    {// 接收地址列表为空
        return;
    }
    
    nItems = IVector_Size(pMe->m_pUserDataMOList);
    if (0 == nItems)
    {
        return;
    }
    
    pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, 0);
    
    if (NULL == pItem)
    {
        goto BuildList_ERR;
    }

    // 拷贝当前号码
    pMe->m_msSend.m_szNum[0] = 0;
    (void)WSTRCPY(pMe->m_msSend.m_szNum, pItem->m_szTo);
    
    nSize = nItems*sizeof(wms_client_message_s_type *);
    pMe->m_pCurSendCltMsg = (wms_client_message_s_type **)MALLOC(nSize);
    if (NULL == pMe->m_pCurSendCltMsg)
    {
        goto BuildList_ERR;
    }
    
    pMe->m_idxUserdata = 0;
    for (i=0; i<nItems; i++)
    {
        pClientMsg = WmsApp_GetClientMsgMO(pMe, TRUE);
        if (NULL == pClientMsg)
        {
            goto BuildList_ERR;
        }
        
        pMe->m_pCurSendCltMsg[i] = pClientMsg;
    }
    
    pMe->m_idxCurSend = 0;
    pMe->m_nSendItems = nItems;
    return;
                    
BuildList_ERR:
    WmsApp_FreeSendClentMsgList(pMe);
}

/*==============================================================================
函数:
    WmsApp_GetMsgICONID

说明:
    获取短信使用的图标。

参数:
    pNode [in]: 指向 wms_cache_info_node 结构的指针。

返回值:
    uint16

备注:
       
==============================================================================*/
static uint16 WmsApp_GetMsgICONID(wms_cache_info_node * pNode)
{
    uint16 wID = 0;
    
    if (NULL == pNode)
    {
        return wID;
    }
    
    if (pNode->mem_store == WMS_MEMORY_STORE_RUIM)
    {
        if (pNode->msg_type == WMS_BD_TYPE_DELIVERY_ACK)
        {
            if (pNode->msg_tag == WMS_TAG_MT_READ)
            {
                wID = IDB_READ_UIM_R;
            }
            else
            {
                wID = IDB_UREAD_UIM_R;
            }
            
            return wID;
        }
        
        switch (pNode->msg_tag)
        {
            case WMS_TAG_MT_READ:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_READ_UIM_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_READ_UIM_EMERGENCY;
                }
                else
                {
                    wID = IDB_READ_UIM_NORMAL;
                }
                break;
                
            case WMS_TAG_MT_NOT_READ:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_UREAD_UIM_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_UREAD_UIM_EMERGENCY;
                }
                else
                {
                    wID = IDB_UREAD_UIM_NORMAL;
                }
                break;
                
            case WMS_TAG_MO_SENT:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_SENT_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_SENT_EMERGENCY;
                }
                else
                {
                    wID = IDB_SENT_NORMAL;
                }
                break;
                
            case WMS_TAG_RESERVE:
            case WMS_TAG_MO_NOT_SENT:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_UNSENT_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_UNSENT_EMERGENCY;
                }
                else
                {
                    wID = IDB_UNSENT_NORMAL;
                }
                break;
                
            case WMS_TAG_MO_TEMPLATE:
                wID = IDB_TEMPLATE;
                break;
                
            case WMS_TAG_MO_DRAFT:
                wID = IDB_DRAFT;
                break;
                
            default:
                break; 
            
        }
    }
    else
    {
        if (pNode->msg_type == WMS_BD_TYPE_DELIVERY_ACK)
        {
            if (pNode->msg_tag == WMS_TAG_MT_READ)
            {
                wID = IDB_READ_PH_R;
            }
            else
            {
                wID = IDB_UREAD_PH_R;
            }
            
            return wID;
        }
        
        switch (pNode->msg_tag)
        {
            case WMS_TAG_MT_READ:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_READ_PH_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_READ_PH_EMERGENCY;
                }
                else
                {
                    wID = IDB_READ_PH_NORMAL;
                }
                break;
                
            case WMS_TAG_MT_NOT_READ:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_UREAD_PH_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_UREAD_PH_EMERGENCY;
                }
                else
                {
                    wID = IDB_UREAD_PH_NORMAL;
                }
                break;
                
            case WMS_TAG_RESERVE:
            case WMS_TAG_MO_SENT:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_SENT_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_SENT_EMERGENCY;
                }
                else
                {
                    wID = IDB_SENT_NORMAL;
                }
                break;
                
            case WMS_TAG_RSVFAILED:
            case WMS_TAG_MO_NOT_SENT:
                if (pNode->priority == WMS_PRIORITY_URGENT)
                {
                    wID = IDB_UNSENT_URGENT;
                }
                else if (pNode->priority == WMS_PRIORITY_EMERGENCY)
                {
                    wID = IDB_UNSENT_EMERGENCY;
                }
                else
                {
                    wID = IDB_UNSENT_NORMAL;
                }
                break;
                
            case WMS_TAG_MO_TEMPLATE:
                wID = IDB_TEMPLATE;
                break;
                
            case WMS_TAG_MO_DRAFT:
                wID = IDB_DRAFT;
                break;
                
            default:
                break; 
            
        }
    }
    
    return wID;
}

/*==============================================================================
函数：
    WmsApp_CurmessageIsFullSendout

说明：
    函数用来判断当前发送的消息是否发送完毕。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    TRUE - 完毕
    FALSE - 尚未完毕

备注：
        
==============================================================================*/
boolean WmsApp_CurmessageIsFullSendout(WmsApp * pMe)
{
    boolean bRet = TRUE;
    
    if (pMe->m_eCreateWMSType == SEND_MSG_RESEND)
    {
        pMe->m_idxCur++;
        // 确定当前发送消息节点是否发送完毕
        for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
        {
            if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
            {
                break;
            }
        }
        if (pMe->m_idxCur<LONGSMS_MAX_PACKAGES)
        {
            bRet = FALSE;
        }
    }
    else if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
    {
        if (pMe->m_SendStatus == WMS_RPT_OK)
        {
            // 保存消息至发件箱
            (void)IWMS_MsgWrite(pMe->m_pwms, 
                                pMe->m_clientId, 
                                &pMe->m_callback,
                                (void*)pMe,
                                WMS_WRITE_MODE_INSERT,
                                pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
            
            // 从预约短信中删除此节点
            if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
            {
                // 发布删除消息命令
                (void)IWMS_MsgDelete(pMe->m_pwms,
                                     pMe->m_clientId,
                                     &pMe->m_callback,
                                     (void *)pMe,
                                     (pMe->m_CurMsgNodes[pMe->m_idxCur])->mem_store,
                                     (pMe->m_CurMsgNodes[pMe->m_idxCur])->index);
            }
        }
        else
        {// 发送失败作预约失败处理
            (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                    pMe->m_clientId,
                                    &pMe->m_callback,
                                    (void *)pMe,
                                    (pMe->m_CurMsgNodes[pMe->m_idxCur])->mem_store,
                                    (pMe->m_CurMsgNodes[pMe->m_idxCur])->index,
                                    WMS_TAG_RSVFAILED);
            {
                uint16 nMsgs,nNews;
                int i;
                int nRet=EFAILED;
                wms_cache_info_node  *pFailnode = NULL;
                wms_cache_info_node  *ptepnode  = NULL;

                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, &nMsgs);
                
                if(nMsgs - nNews > MAX_RSVFAILD)
                {
                    wms_cacheinfolist_enumbegin(WMS_MB_RSVFAILED);
                    pFailnode = wms_cacheinfolist_enumnext(WMS_MB_RSVFAILED);
                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                    {
                        ptepnode = pFailnode->pItems[i];
                        if (ptepnode != NULL)
                        {
                            
                            // 发布删除消息命令
                            nRet = ENOMEMORY;
                            do
                            {
                                nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                   pMe->m_clientId,
                                                   &pMe->m_callback,
                                                   (void *)pMe,
                                                   pFailnode->mem_store,
                                                   pFailnode->index);
                            } while(nRet != SUCCESS);
                            ptepnode = NULL;
                        }     
                    }
                }

            }
        }
        pMe->m_idxCur++;
        // 确定当前发送消息节点是否发送完毕
        for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
        {
            if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
            {
                break;
            }
        }
        if (pMe->m_idxCur<LONGSMS_MAX_PACKAGES)
        {
            bRet = FALSE;
        }
    }
    else
    {
        pMe->m_idxCurSend++;
        if (pMe->m_idxCurSend<pMe->m_nSendItems)
        {
            // 发送完才提示
            bRet = FALSE;
        }
    }
    
    return bRet;
}

/*==============================================================================
函数：
    WmsApp_ReservedMsgStatusUpdate

说明：
    函数用来检查预约短信列表, 对于过期的短信需修改其标记。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    none

备注：
        
==============================================================================*/
static void WmsApp_ReservedMsgStatusUpdate(WmsApp * pMe)
{
#ifdef FEATURE_SMS_UDH
    int i;
    wms_cache_info_node  *ptepnode = NULL;
#endif    
    uint32  dwSecs;
    wms_cache_info_node  *pnode = NULL;
    
    dwSecs = GETTIMESECONDS();
    
    wms_cacheinfolist_enumbegin(WMS_MB_RESERVE);
    pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
    while ((NULL != pnode) && (pnode->dwTime < dwSecs))
    {
#ifdef FEATURE_SMS_UDH
        if (pnode->pItems != NULL)
        {
            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
            {
                ptepnode = pnode->pItems[i];
                
                if (NULL != ptepnode)
                {
                    (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                            pMe->m_clientId,
                                            &pMe->m_callback,
                                            (void *)pMe,
                                            ptepnode->mem_store,
                                            ptepnode->index,
                                            WMS_TAG_RSVFAILED);
                }
            }
        }
        else
#endif            
        {
            (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                    pMe->m_clientId,
                                    &pMe->m_callback,
                                    (void *)pMe,
                                    pnode->mem_store,
                                    pnode->index,
                                    WMS_TAG_RSVFAILED);
        }

        {
            uint16 nMsgs,nNews;
            int i;
            int nRet=EFAILED;
            wms_cache_info_node  *pFailnode = NULL;

            wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, &nMsgs);
            
            if(nMsgs - nNews > MAX_RSVFAILD)
            {
                wms_cacheinfolist_enumbegin(WMS_MB_RSVFAILED);
                pFailnode = wms_cacheinfolist_enumnext(WMS_MB_RSVFAILED);
                for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                {
                    ptepnode = pFailnode->pItems[i];
                    if (ptepnode != NULL)
                    {
                        
                        // 发布删除消息命令
                        nRet = ENOMEMORY;
                        do
                        {
                            nRet = IWMS_MsgDelete(pMe->m_pwms,
                                               pMe->m_clientId,
                                               &pMe->m_callback,
                                               (void *)pMe,
                                               pFailnode->mem_store,
                                               pFailnode->index);
                        } while(nRet != SUCCESS);
                        ptepnode = NULL;
                    }     
                }
            }

        }

        pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
    }
}

/*==============================================================================
函数：
    WmsApp_ReservedMsgTimer

说明：
    预约短信定时器回调函数。

参数：
    pUser [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    none

备注：
        
==============================================================================*/
static void WmsApp_ReservedMsgTimer(void * pUser)
{
    WmsApp * pMe = (WmsApp *)pUser;
    
    if (NULL == pMe)
    {
        return;
    }
    
    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                            EVTFLG_ASYNC,
                            AEECLSID_WMSAPP,
                            EVT_RESERVEDMSGALERT,
                            0, 
                            0);
}

/*==============================================================================
函数：
    WmsApp_ReservedMsgSetTimer

说明：
    函数用来检查预约短信列表, 对于过期的短信需修改其标记, 若有有效预约短信，就设
    预约短信定时器。

参数：
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值：
    none

备注：
        
==============================================================================*/
void WmsApp_ReservedMsgSetTimer(WmsApp * pMe)
{
    uint32  dwSecs;
    wms_cache_info_node  *pnode = NULL;
    
    AEE_CancelTimer(WmsApp_ReservedMsgTimer, pMe);
    dwSecs = GETTIMESECONDS();
    
    // 检查预约短信列表, 对于过期的短信需修改其标记
    WmsApp_ReservedMsgStatusUpdate(pMe);
    
    wms_cacheinfolist_enumbegin(WMS_MB_RESERVE);
    pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
    if (NULL != pnode)
    {
        AEE_SetSysTimer((pnode->dwTime - dwSecs)*1000, WmsApp_ReservedMsgTimer, pMe);
    }
}

/*==============================================================================
函数：
    WmsApp_IsNeedContinueSendTask

说明：
    函数用来判断是否需要继续未尽短信发送任务。

参数：
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    TRUE - 有未尽短信发送任务，需继续
    FALSE - 无未尽短信发送任务

备注：
    是否有未尽短信发送任务，判断条件依次为:
    1、调用本函数时当前状态必需处于 WMSST_SENDING ;
    2、发送到特定号码的短信数据包是否发送完毕;
    3、群发是否发送完毕;
        
==============================================================================*/
boolean WmsApp_IsNeedContinueSendTask(WmsApp * pMe)
{
    boolean bRet = FALSE;
    int i, nAddrs;
    
    if ((NULL == pMe) || (pMe->m_currState != WMSST_SENDING))
    {
        return bRet;
    }
    
    nAddrs = IVector_Size(pMe->m_pSendList);
    if (nAddrs < 1)
    {
        return bRet;
    }
    
    if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
    {
        return bRet;
    }
    
    if (pMe->m_eCreateWMSType == SEND_MSG_RESEND)
    {// 重发
        i = pMe->m_idxCur+1;
        
        // 确定当前发送消息节点是否发送完毕
        for (; (i<LONGSMS_MAX_PACKAGES) && (pMe->m_CurMsgNodes[i] != NULL); i++)
        {
            ;
        }
        if (i<LONGSMS_MAX_PACKAGES)
        {
            bRet = TRUE;
            pMe->m_ContinueSendType = CONTINUE_CURPERSON;
        }
    }
    else
    {
        if ((pMe->m_idxCurSend+1)<pMe->m_nSendItems)
        {
            bRet = TRUE;
            pMe->m_ContinueSendType = CONTINUE_CURPERSON;
        }
        else if (nAddrs > 1)
        {
            bRet = TRUE;
            pMe->m_ContinueSendType = CONTINUE_NEXTPERSON;
        }
    }
    
    if (!bRet)
    {
        pMe->m_ContinueSendType = NONE_CONTINUE;
    }
    
    return bRet;
}

/*==============================================================================
函数:
    CWmsApp_GetLeftSendListNodeNum

说明:
    提供给外部Applet用来获取sendList最多还能添加号码的个数

参数:
    p [in]:指向IWmsApp接口t对象的指针。
    leftNum [out]: 剩余的地址个数

返回值:
    SUCCESS:Applet启动成功。
    EFAILED:Applet启动失败。

备注:
        
==============================================================================*/
static int CWmsApp_GetLeftSendListNodeNum(IWmsApp *p, int *leftNum)
{
    WmsApp *pMe = (WmsApp*)p;

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP)
    {
        // WMS applet is already running
        return EFAILED;
    }
    
    *leftNum = MAXNUM_MULTISEND - IVector_Size(pMe->m_pSendList);
    
    return SUCCESS;
}

/*==============================================================================
函数:
    CWmsApp_GetLeftSendListNodeNum

说明:
    提供给外部Applet用来获取sendList最多还能添加号码的个数

参数:
    p [in]:指向IWmsApp接口t对象的指针。
    leftNum [out]: 剩余的地址个数

返回值:
    SUCCESS:Applet启动成功。
    EFAILED:Applet启动失败。

备注:
        
==============================================================================*/
static int CWmsApp_DeleteAllNvCdmaSms(IWmsApp *p)
{
    WmsApp *pMe = (WmsApp*)p;
    int    nRet;

    if (ISHELL_ActiveApplet(pMe->m_pShell) == AEECLSID_WMSAPP)
    {
        // WMS applet is already running
        return EFAILED;
    }

    //  删除收件箱存在手机上的信息
    nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                pMe->m_clientId, 
                &pMe->m_callback,
                (void*)pMe,
                WMS_INBOX_DEL_PHONE);    

    // 删除发件箱的信息
    if(nRet == SUCCESS)
    {
        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                    pMe->m_clientId, 
                    &pMe->m_callback,
                    (void*)pMe,
                    WMS_OUTBOX_DEL_ALL);        
    }

    // 删除草稿箱的信息
    if(nRet == SUCCESS)
    {
        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                    pMe->m_clientId, 
                    &pMe->m_callback,
                    (void*)pMe,
                    WMS_DRAFT_DEL_ALL);        
    }

    // 删除预约信息
    if(nRet == SUCCESS)
    {
        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                    pMe->m_clientId, 
                    &pMe->m_callback,
                    (void*)pMe,
                    WMS_RSVANDRSVFAILED_DEL_ALL);        
    }
    
    return nRet;
}

