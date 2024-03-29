// SPDX-License-Identifier: GPL-2.0

/*

 * Copyright (c) 2019 MediaTek Inc.

 */

/*
** Id:
*/

/*! \file   "roaming_fsm.c"
    \brief  This file defines the FSM for Roaming MODULE.

    This file defines the FSM for Roaming MODULE.
*/

/*
** Log: roaming_fsm.c
 *
 * 11 24 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * Adjust code for DBG and CONFIG_XLOG.
 *
 * 11 11 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * modify the xlog related code.
 *
 * 11 02 2011 wh.su
 * [WCXRP00001078] [MT6620 Wi-Fi][Driver] Adding the mediatek log improment support : XLOG
 * adding the code for XLOG.
 *
 * 08 31 2011 tsaiyuan.hsu
 * [WCXRP00000931] [MT5931 Wi-Fi][DRV/FW] add swcr to disable roaming from driver
 * remove obsolete code.
 *
 * 08 15 2011 tsaiyuan.hsu
 * [WCXRP00000931] [MT5931 Wi-Fi][DRV/FW] add swcr to disable roaming from driver
 * add swcr in driver reg, 0x9fxx0000, to disable roaming .
 *
 * 03 16 2011 tsaiyuan.hsu
 * [WCXRP00000517] [MT6620 Wi-Fi][Driver][FW] Fine Tune Performance of Roaming
 * remove obsolete definition and unused variables.
 *
 * 02 26 2011 tsaiyuan.hsu
 * [WCXRP00000391] [MT6620 Wi-Fi][FW] Add Roaming Support
 * not send disassoc or deauth to leaving AP so as to improve performace of roaming.
 *
 * 01 27 2011 tsaiyuan.hsu
 * [WCXRP00000392] [MT6620 Wi-Fi][Driver] Add Roaming Support
 * add roaming fsm
 * 1. not support 11r, only use strength of signal to determine roaming.
 * 2. not enable CFG_SUPPORT_ROAMING until completion of full test.
 * 3. in 6620, adopt work-around to avoid sign extension problem of cck of hw
 * 4. assume that change of link quality in smooth way.
 *
 * 01 27 2011 tsaiyuan.hsu
 * [WCXRP00000392] [MT6620 Wi-Fi][Driver] Add Roaming Support
 * add roaming fsm
 * 1. not support 11r, only use strength of signal to determine roaming.
 * 2. not enable CFG_SUPPORT_ROAMING until completion of full test.
 * 3. in 6620, adopt work-around to avoid sign extension problem of cck of hw
 * 4. assume that change of link quality in smooth way.
 *
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"

#if CFG_SUPPORT_ROAMING
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

#if DBG
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugRoamingState[ROAMING_STATE_NUM] = {
	(PUINT_8) DISP_STRING("ROAMING_STATE_IDLE"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DECISION"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DISCOVERY"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_ROAM")
};

/*lint -restore */
#endif /* DBG */

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
/*
#define ROAMING_ENABLE_CHECK(_roam) \
{ \
	if (!(_roam->fgIsEnableRoaming)) \
		return; \
}
*/
/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* @brief Initialize the value in ROAMING_FSM_INFO_T for ROAMING FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmInit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	P_CONNECTION_SETTINGS_T prConnSettings;
	P_CMD_ROAMING_CTRL_T prRoamCtrl = &prAdapter->rWifiVar.rRoamingInfo.rRoamCtrl;

	DBGLOG(ROAMING, LOUD, "->roamingFsmInit(): Current Time = %u\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);
	prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

	/* 4 <1> Initiate FSM */
	prRoamingFsmInfo->fgIsEnableRoaming = prConnSettings->fgIsEnableRoaming;
	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;
	prRoamingFsmInfo->rRoamingDiscoveryUpdateTime = 0;

	/* default value of roaming ctrl in firmware. */
	prRoamCtrl->fgEnable = TRUE;
	prRoamingFsmInfo->fgDrvRoamingAllow = TRUE;
	prRoamCtrl->ucRcpiAdjustStep = 8;
	prRoamCtrl->u2RcpiLowThr = 90;
	prRoamCtrl->ucRoamingRetryCount = 2;
	prRoamCtrl->ucRoamingStableTimeout = 10;
}				/* end of roamingFsmInit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Uninitialize the value in AIS_FSM_INFO_T for AIS FSM operation
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmUninit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	DBGLOG(ROAMING, LOUD, "->roamingFsmUninit(): Current Time = %u\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;

}				/* end of roamingFsmUninit() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Send commands to firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmSendCmd(IN P_ADAPTER_T prAdapter, IN P_ROAMING_PARAM_T prParam)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	WLAN_STATUS rStatus;

	DBGLOG(ROAMING, LOUD, "->roamingFsmSendCmd(): Current Time = %u\n", kalGetTimeTick());

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	rStatus = wlanSendSetQueryCmd(prAdapter,	/* prAdapter */
				      CMD_ID_ROAMING_TRANSIT,	/* ucCID */
				      TRUE,	/* fgSetQuery */
				      FALSE,	/* fgNeedResp */
				      FALSE,	/* fgIsOid */
				      NULL,	/* pfCmdDoneHandler */
				      NULL,	/* pfCmdTimeoutHandler */
				      sizeof(ROAMING_PARAM_T),	/* u4SetQueryInfoLen */
				      (PUINT_8) prParam,	/* pucInfoBuffer */
				      NULL,	/* pvSetQueryBuffer */
				      0	/* u4SetQueryBufferLen */
	    );

	ASSERT(rStatus == WLAN_STATUS_PENDING);

}				/* end of roamingFsmSendCmd() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Update the recent time when ScanDone occurred
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmScanResultsUpdate(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	DBGLOG(ROAMING, LOUD, "->roamingFsmScanResultsUpdate(): Current Time = %u", kalGetTimeTick());

	GET_CURRENT_SYSTIME(&prRoamingFsmInfo->rRoamingDiscoveryUpdateTime);

}				/* end of roamingFsmScanResultsUpdate() */
#if CFG_SUPPORT_ROAMING_SKIP_ONE_AP
/*----------------------------------------------------------------------------*/
/*
* @brief Check if need to do scan for roaming
*
* @param [out] fgIsNeedScan Set to TRUE if need to scan since
*	there is roaming candidate in current scan result or skip roaming times > limit times
* @return
*/
/*----------------------------------------------------------------------------*/
static BOOLEAN roamingFsmIsNeedScan(IN P_ADAPTER_T prAdapter)
{
	P_SCAN_INFO_T prScanInfo;
	P_LINK_T prRoamBSSDescList;
	P_ROAM_BSS_DESC_T prRoamBssDesc;
	P_BSS_INFO_T prAisBssInfo;
	P_BSS_DESC_T prBssDesc = NULL;
	P_BSS_DESC_T prTargetBssDesc = NULL;
	/*CMD_SW_DBG_CTRL_T rCmdSwCtrl;*/
	CMD_ROAMING_SKIP_ONE_AP_T rCmdRoamingSkipOneAP;
	BOOLEAN fgIsNeedScan, fgIsRoamingSSID;

	fgIsNeedScan = FALSE;
	fgIsRoamingSSID = FALSE; /*Whether there's roaming candidate in RoamBssDescList*/

	kalMemZero(&rCmdRoamingSkipOneAP, sizeof(CMD_ROAMING_SKIP_ONE_AP_T));

	prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prRoamBSSDescList = &prScanInfo->rRoamBSSDescList;
	/* <1> Count same BSS Desc from current SCAN result list. */
	LINK_FOR_EACH_ENTRY(prRoamBssDesc, prRoamBSSDescList, rLinkEntry, ROAM_BSS_DESC_T) {
		if (EQUAL_SSID(prRoamBssDesc->aucSSID,
				       prRoamBssDesc->ucSSIDLen,
				       prAisBssInfo->aucSSID, prAisBssInfo->ucSSIDLen)) {
			fgIsRoamingSSID = TRUE;
			fgIsNeedScan = TRUE;
			DBGLOG(ROAMING, INFO, "roamingFsmSteps: IsRoamingSSID:%d\n", fgIsRoamingSSID);
			break;
		}
	}

	/* <2> Start skip roaming scan mechanism if there is no candidate in current SCAN result list */
	if (!fgIsRoamingSSID) {
		prTargetBssDesc = prAdapter->rWifiVar.rAisFsmInfo.prTargetBssDesc;
		if (prTargetBssDesc) {
			DBGLOG(ROAMING, INFO, "AIS BSSID %pM Target BSSID %pM RCPI:%d\n",
				prAisBssInfo->aucBSSID,
				prTargetBssDesc->aucBSSID,
				prTargetBssDesc->ucRCPI);
		}

		prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
		if (prBssDesc) {
			DBGLOG(ROAMING, INFO,
				"roamingFsmSteps: BSSID %pM RCPI:%d RoamSkipTimes:%d\n",
				prBssDesc->aucBSSID,prBssDesc->ucRCPI, prAisBssInfo->ucRoamSkipTimes);
			/*rCmdSwCtrl.u4Id = 0xa0280000;*/
			/*rCmdSwCtrl.u4Data = 0x1;*/
			rCmdRoamingSkipOneAP.fgIsRoamingSkipOneAP = 1;

			if (prBssDesc->ucRCPI > ROAMING_ONE_AP_GOOD_AREA_RCPI) {	/* Set parameters related to Good Area */
				prAisBssInfo->ucRoamSkipTimes = ROAMING_ONE_AP_SKIP_TIMES_IN_GOOD;
				prAisBssInfo->fgGoodRcpiArea = TRUE;
				prAisBssInfo->fgPoorRcpiArea = FALSE;
			} else {
				if (prAisBssInfo->fgGoodRcpiArea) {
					prAisBssInfo->ucRoamSkipTimes--;
				} else if (prBssDesc->ucRCPI > ROAMING_ONE_AP_POOR_AREA_RCPI) {	/* Set parameters related to Poor Area */
					if (!prAisBssInfo->fgPoorRcpiArea) {
						prAisBssInfo->ucRoamSkipTimes = ROAMING_ONE_AP_SKIP_TIMES_IN_POOR;
						prAisBssInfo->fgPoorRcpiArea = TRUE;
						prAisBssInfo->fgGoodRcpiArea = FALSE;
					} else {
						prAisBssInfo->ucRoamSkipTimes--;
					}
				} else {
					prAisBssInfo->fgGoodRcpiArea = FALSE;
					prAisBssInfo->ucRoamSkipTimes--;
				}
			}

			if (prAisBssInfo->ucRoamSkipTimes == 0) {
				prAisBssInfo->ucRoamSkipTimes = ROAMING_ONE_AP_SKIP_TIMES_INITIAL;
				prAisBssInfo->fgPoorRcpiArea = FALSE;
				prAisBssInfo->fgGoodRcpiArea = FALSE;
				DBGLOG(ROAMING, INFO, "roamingFsmSteps: Need Scan\n");
				fgIsNeedScan = TRUE;
			} else
				wlanSendSetQueryCmd(prAdapter,
					    CMD_ID_SET_ROAMING_SKIP,
					    TRUE,
					    FALSE,
					    FALSE, NULL, NULL, sizeof(CMD_ROAMING_SKIP_ONE_AP_T),
					    (PUINT_8)&rCmdRoamingSkipOneAP, NULL, 0);
		} else {
			DBGLOG(ROAMING, WARN, "Target BssDesc in AisFsmInfo is NULL\n");
			fgIsNeedScan = TRUE;
		}
	}

	return fgIsNeedScan;
}
#endif
/*----------------------------------------------------------------------------*/
/*!
* @brief The Core FSM engine of ROAMING for AIS Infra.
*
* @param [IN P_ADAPTER_T]          prAdapter
*        [IN ENUM_ROAMING_STATE_T] eNextState Enum value of next AIS STATE
*
* @return (none)
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_ROAMING_STATE_T eNextState)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T ePreviousState;
	BOOLEAN fgIsTransition = (BOOLEAN) FALSE;
	BOOLEAN fgIsNeedScan = FALSE;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	do {

		/* Do entering Next State */
#if DBG
		DBGLOG(ROAMING, STATE, "TRANSITION: [%s] -> [%s]\n",
					apucDebugRoamingState[prRoamingFsmInfo->eCurrentState],
					apucDebugRoamingState[eNextState]);
#else
		DBGLOG(ROAMING, STATE, "[%d] TRANSITION: [%d] -> [%d]\n",
					DBG_ROAMING_IDX, prRoamingFsmInfo->eCurrentState, eNextState);
#endif
		/* NOTE(Kevin): This is the only place to change the eCurrentState(except initial) */
		ePreviousState = prRoamingFsmInfo->eCurrentState;
		prRoamingFsmInfo->eCurrentState = eNextState;

		fgIsTransition = (BOOLEAN) FALSE;

		/* Do tasks of the State that we just entered */
		switch (prRoamingFsmInfo->eCurrentState) {
			/* NOTE(Kevin): we don't have to rearrange the sequence of following
			 * switch case. Instead I would like to use a common lookup table of array
			 * of function pointer to speed up state search.
			 */
		case ROAMING_STATE_IDLE:
		case ROAMING_STATE_DECISION:
			break;

		case ROAMING_STATE_DISCOVERY:
			{
				OS_SYSTIME rCurrentTime;
#if CFG_SUPPORT_ROAMING_SKIP_ONE_AP
				fgIsNeedScan = roamingFsmIsNeedScan(prAdapter);
#else
				fgIsNeedScan = TRUE;
#endif
				if ((prAdapter->rWifiVar.rAisFsmInfo.ucNeighborApChnlTotalCnt != 0) && prRoamingFsmInfo->eRoaming_type != ROAMING_11V)
					prRoamingFsmInfo->eRoaming_type = ROAMING_11K;

				GET_CURRENT_SYSTIME(&rCurrentTime);
				if (CHECK_FOR_TIMEOUT(rCurrentTime, prRoamingFsmInfo->rRoamingDiscoveryUpdateTime,
						      SEC_TO_SYSTIME(ROAMING_DISCOVERY_TIMEOUT_SEC)) && fgIsNeedScan) {
					DBGLOG(ROAMING, LOUD, "roamingFsmSteps: DiscoveryUpdateTime Timeout");
					aisFsmRunEventRoamingDiscovery(prAdapter, TRUE);
				} else {
					DBGLOG(ROAMING, LOUD, "roamingFsmSteps: DiscoveryUpdateTime Updated");
#if CFG_SUPPORT_ROAMING_ENC
					if (prAdapter->fgIsRoamingEncEnabled == TRUE)
						aisFsmRunEventRoamingDiscovery(prAdapter, TRUE);
					else
#endif /* CFG_SUPPORT_ROAMING_ENC */
						aisFsmRunEventRoamingDiscovery(prAdapter, FALSE);
				}
			}
			break;

		case ROAMING_STATE_ROAM:
			break;

		default:
			ASSERT(0);	/* Make sure we have handle all STATEs */
		}
	} while (fgIsTransition);

	return;

}				/* end of roamingFsmSteps() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state after join completion
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventStart(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	P_BSS_INFO_T prAisBssInfo;
	ROAMING_PARAM_T rParam;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	if (prAisBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE)
		return;

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING START: Current Time = %u\n", kalGetTimeTick());

	/* IDLE, DECISION, ROAM -> DECISION */
	/* Errors as DISCOVERY -> DECISION */
	/* DECISION state here is possible after framework roaming
	 * (CMD_AUTO_ROAM) and new AP connected. Need to request firmware to
	 * restart roaming state machine.
	 */
	if (!(prRoamingFsmInfo->eCurrentState == ROAMING_STATE_IDLE ||
		prRoamingFsmInfo->eCurrentState == ROAMING_STATE_DECISION ||
		prRoamingFsmInfo->eCurrentState == ROAMING_STATE_ROAM))
		return;

	eNextState = ROAMING_STATE_DECISION;
	rParam.u2Event = ROAMING_EVENT_START;
	roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) & rParam);

	/* Step to next state */
	roamingFsmSteps(prAdapter, eNextState);

}				/* end of roamingFsmRunEventStart() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Discovery state when deciding to find a candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventDiscovery(IN P_ADAPTER_T prAdapter, IN P_ROAMING_PARAM_T prParam)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	prRoamingFsmInfo->u4RoamingStartTime = (OS_SYSTIME) kalGetTimeTick();
	prRoamingFsmInfo->eRoaming_type = ROAMING_LEGACY;
	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING DISCOVERY: Current Time = %u Reason = %u\n",
		kalGetTimeTick(), prParam->u2Reason);

	/* DECISION -> DISCOVERY */
	/* Errors as IDLE, DISCOVERY, ROAM -> DISCOVERY */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DECISION)
		return;
#if CFG_SUPPORT_ROAMING_ENC
	prRoamingFsmInfo->RoamingEntryTimeoutSkipCount = 0;
#endif

	eNextState = ROAMING_STATE_DISCOVERY;
	/* DECISION -> DISCOVERY */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		P_BSS_INFO_T prAisBssInfo;
		P_BSS_DESC_T prBssDesc;

		/* sync. rcpi with firmware */
		prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
		prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
		if (prBssDesc) {
			prBssDesc->ucRCPI = (UINT_8) (prParam->u2Data & 0xff);
			prRoamingFsmInfo->oldApRssi = RCPI_TO_dBm(prBssDesc->ucRCPI);
			DBGLOG(ROAMING, STATE, "EVENT-ROAMING DISCOVERY: Current Time = %u, RCPI = %d RSSI = %d\n",
				kalGetTimeTick(), prBssDesc->ucRCPI, prRoamingFsmInfo->oldApRssi);
		}

		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventDiscovery() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Roam state after Scan Done
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventRoam(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	ROAMING_PARAM_T rParam;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* Check Roaming Conditions */
	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING ROAM: Current Time = %u\n", kalGetTimeTick());

	/* IDLE, ROAM -> DECISION */
	/* Errors as IDLE, DECISION, ROAM -> ROAM */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DISCOVERY)
		return;

	eNextState = ROAMING_STATE_ROAM;
	/* DISCOVERY -> ROAM */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rParam.u2Event = ROAMING_EVENT_ROAM;
		roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) & rParam);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventRoam() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Decision state as being failed to find out any candidate
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventFail(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	ROAMING_PARAM_T rParam;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/* add for fos7*/
	kalIndicateRoamingMetrics(prAdapter->prGlueInfo);
	/* Check Roaming Conditions */
	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING FAIL: reason %x Current Time = %u\n", u4Param, kalGetTimeTick());

	/* IDLE, ROAM -> DECISION */
	/* Errors as IDLE, DECISION, DISCOVERY -> DECISION */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_ROAM)
		return;

	eNextState = ROAMING_STATE_DECISION;
	/* ROAM -> DECISION */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rParam.u2Event = ROAMING_EVENT_FAIL;
		rParam.u2Data = (UINT_16) (u4Param & 0xffff);
		roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) & rParam);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventFail() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Transit to Idle state as beging aborted by other moduels, AIS
*
* @param [IN P_ADAPTER_T] prAdapter
*
* @return none
*/
/*----------------------------------------------------------------------------*/
VOID roamingFsmRunEventAbort(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	ROAMING_PARAM_T rParam;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	if (!(prRoamingFsmInfo->fgIsEnableRoaming))
		return;

	DBGLOG(ROAMING, EVENT, "EVENT-ROAMING ABORT: Current Time = %u\n", kalGetTimeTick());

	eNextState = ROAMING_STATE_IDLE;
	/* IDLE, DECISION, DISCOVERY, ROAM -> IDLE */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rParam.u2Event = ROAMING_EVENT_ABORT;
		roamingFsmSendCmd(prAdapter, (P_ROAMING_PARAM_T) & rParam);

		/* Step to next state */
		roamingFsmSteps(prAdapter, eNextState);
	}

}				/* end of roamingFsmRunEventAbort() */

/*----------------------------------------------------------------------------*/
/*!
* @brief Process events from firmware
*
* @param [IN P_ADAPTER_T]       prAdapter
*        [IN P_ROAMING_PARAM_T] prParam
*
* @return none
*/
/*----------------------------------------------------------------------------*/
WLAN_STATUS roamingFsmProcessEvent(IN P_ADAPTER_T prAdapter, IN P_ROAMING_PARAM_T prParam, IN UINT_32 u4EventBufLen)
{
	DBGLOG(ROAMING, LOUD, "ROAMING Process Events: Current Time = %u\n", kalGetTimeTick());

	if (u4EventBufLen < sizeof(ROAMING_PARAM_T))
	{
		DBGLOG(ROAMING, WARN,
			"Invalid event length: %d < %d \n",
			u4EventBufLen,
			sizeof(ROAMING_PARAM_T));
		return WLAN_STATUS_FAILURE;
	}
	if (ROAMING_EVENT_DISCOVERY == prParam->u2Event)
		roamingFsmRunEventDiscovery(prAdapter, prParam);

	return WLAN_STATUS_SUCCESS;
}

#endif
