// SPDX-License-Identifier: GPL-2.0

/*

 * Copyright (c) 2019 MediaTek Inc.

 */

/*
** Id: @(#) gl_cfg80211.c@@
*/

/*! \file   gl_cfg80211.c
    \brief  Main routines for supporintg MT6620 cfg80211 control interface

    This file contains the support routines of Linux driver for MediaTek Inc. 802.11
    Wireless LAN Adapters.
*/

/*
** Log: gl_cfg80211.c
**
** 09 05 2013 cp.wu
** correct length to pass to wlanoidSetBssid()
**
** 09 04 2013 cp.wu
** fix typo
**
** 09 03 2013 cp.wu
** add path for reassociation
**
** 11 23 2012 yuche.tsai
** [ALPS00398671] [Acer-Tablet] Remove Wi-Fi Direct completely
** Fix bug of WiFi may reboot under user load, when WiFi Direct is removed..
**
** 09 12 2012 wcpadmin
** [ALPS00276400] Remove MTK copyright and legal header on GPL/LGPL related packages
** .
**
** 08 30 2012 chinglan.wang
** [ALPS00349664] [6577JB][WIFI] Phone can not connect to AP secured with AES via WPS in 802.11n Only
** .
 *
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "gl_os.h"
#include "debug.h"
#include "wlan_lib.h"
#include "gl_wext.h"
#include "precomp.h"
#include <linux/can/netlink.h>
#include <net/netlink.h>
#include <net/cfg80211.h>
#include "gl_cfg80211.h"
#include "gl_vendor.h"
#include "gl_p2p_ioctl.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* These values must sync from Wifi HAL
 * /hardware/libhardware_legacy/include/hardware_legacy/wifi_hal.h
 */
/* Basic infrastructure mode */
#define WIFI_FEATURE_INFRA              (0x0001)
	/* Support for 5 GHz Band */
#define WIFI_FEATURE_INFRA_5G           (0x0002)
	/* Support for GAS/ANQP */
#define WIFI_FEATURE_HOTSPOT            (0x0004)
	/* Wifi-Direct */
#define WIFI_FEATURE_P2P                (0x0008)
	/* Soft AP */
#define WIFI_FEATURE_SOFT_AP            (0x0010)
	/* Google-Scan APIs */
#define WIFI_FEATURE_GSCAN              (0x0020)
	/* Neighbor Awareness Networking */
#define WIFI_FEATURE_NAN                (0x0040)
	/* Device-to-device RTT */
#define WIFI_FEATURE_D2D_RTT            (0x0080)
	/* Device-to-AP RTT */
#define WIFI_FEATURE_D2AP_RTT           (0x0100)
	/* Batched Scan (legacy) */
#define WIFI_FEATURE_BATCH_SCAN         (0x0200)
	/* Preferred network offload */
#define WIFI_FEATURE_PNO                (0x0400)
	/* Support for two STAs */
#define WIFI_FEATURE_ADDITIONAL_STA     (0x0800)
	/* Tunnel directed link setup */
#define WIFI_FEATURE_TDLS               (0x1000)
	/* Support for TDLS off channel */
#define WIFI_FEATURE_TDLS_OFFCHANNEL    (0x2000)
	/* Enhanced power reporting */
#define WIFI_FEATURE_EPR                (0x4000)
	/* Support for AP STA Concurrency */
#define WIFI_FEATURE_AP_STA             (0x8000)
	/* Link layer stats collection */
#define WIFI_FEATURE_LINK_LAYER_STATS   (0x10000)
	/* WiFi Logger */
#define WIFI_FEATURE_LOGGER             (0x20000)
	/* WiFi PNO enhanced */
#define WIFI_FEATURE_HAL_EPNO           (0x40000)
	/* RSSI Monitor */
#define WIFI_FEATURE_RSSI_MONITOR       (0x80000)
	/* WiFi mkeep_alive */
#define WIFI_FEATURE_MKEEP_ALIVE        (0x100000)
	/* ND offload configure */
#define WIFI_FEATURE_CONFIG_NDO         (0x200000)
	/* Capture Tx transmit power levels */
#define WIFI_FEATURE_TX_TRANSMIT_POWER  (0x400000)
	/* Enable/Disable firmware roaming */
#define WIFI_FEATURE_CONTROL_ROAMING    (0x800000)
	/* Support Probe IE white listing */
#define WIFI_FEATURE_IE_WHITELIST       (0x1000000)
	/* Support MAC & Probe Sequence Number randomization */
#define WIFI_FEATURE_SCAN_RAND          (0x2000000)
	/* Support Tx Power Limit setting */
#define WIFI_FEATURE_SET_TX_POWER_LIMIT (0x4000000)
	/* Support Using Body/Head Proximity for SAR */
#define WIFI_FEATURE_USE_BODY_HEAD_SAR  (0x8000000)

/* note: WIFI_FEATURE_GSCAN be enabled just for ACTS test item: scanner */
#define WIFI_HAL_FEATURE_SET ((WIFI_FEATURE_P2P) |\
			      (WIFI_FEATURE_SOFT_AP) |\
			      (WIFI_FEATURE_PNO) |\
			      (WIFI_FEATURE_TDLS) |\
			      (WIFI_FEATURE_RSSI_MONITOR) |\
			      (WIFI_FEATURE_CONTROL_ROAMING))

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

static struct nla_policy nla_parse_wifi_policy[WIFI_ATTRIBUTE_ROAMING_WHITELIST_SSID + 1] = {
	[WIFI_ATTRIBUTE_BAND] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_NUM_CHANNELS] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_CHANNEL_LIST] = {.type = NLA_UNSPEC},

	[WIFI_ATTRIBUTE_NUM_FEATURE_SET] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_FEATURE_SET] = {.type = NLA_UNSPEC},
	[WIFI_ATTRIBUTE_PNO_RANDOM_MAC_OUI] = {.type = NLA_UNSPEC},
	[WIFI_ATTRIBUTE_NODFS_VALUE] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_COUNTRY_CODE] = {.type = NLA_STRING},

	[WIFI_ATTRIBUTE_MAX_RSSI] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_MIN_RSSI] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_RSSI_MONITOR_START] = {.type = NLA_U32},

	[WIFI_ATTRIBUTE_ROAMING_CAPABILITIES] = {.type = NLA_UNSPEC},
	[WIFI_ATTRIBUTE_ROAMING_BLACKLIST_NUM] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_ROAMING_BLACKLIST_BSSID] = {.type = NLA_UNSPEC},
	[WIFI_ATTRIBUTE_ROAMING_WHITELIST_NUM] = {.type = NLA_U32},
	[WIFI_ATTRIBUTE_ROAMING_WHITELIST_SSID] = {.type = NLA_UNSPEC},
};

static struct nla_policy nla_parse_offloading_policy[MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC + 1] = {
	[MKEEP_ALIVE_ATTRIBUTE_ID] = {.type = NLA_U8},
	[MKEEP_ALIVE_ATTRIBUTE_IP_PKT] = {.type = NLA_UNSPEC},
	[MKEEP_ALIVE_ATTRIBUTE_IP_PKT_LEN] = {.type = NLA_U16},
	[MKEEP_ALIVE_ATTRIBUTE_SRC_MAC_ADDR] = {.type = NLA_UNSPEC},
	[MKEEP_ALIVE_ATTRIBUTE_DST_MAC_ADDR] = {.type = NLA_UNSPEC},
	[MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC] = {.type = NLA_U32},
};

/* workaround for some ANR CRs. if suppliant is blocked longer than 10s, wifi hal will tell wifiMonitor
to teminate. for the case which can block supplicant 10s is to del key more than 5 times. the root cause
is that there is no resource in TC4, so del key command was not able to set, and then oid
timeout was happed. if we found the root cause why fw couldn't release TC resouce, we will remove this
workaround */
static UINT_8 gucKeyIndex = 255;

P_SW_RFB_T g_arGscnResultsTempBuffer[MAX_BUFFERED_GSCN_RESULTS];
UINT_8 g_GscanResultsTempBufferIndex;
UINT_8 g_arGscanResultsIndicateNumber[MAX_BUFFERED_GSCN_RESULTS] = { 0, 0, 0, 0, 0 };

UINT_8 g_GetResultsBufferedCnt;
UINT_8 g_GetResultsCmdCnt;

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
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
 * @brief This routine is responsible for change STA type between
 *        1. Infrastructure Client (Non-AP STA)
 *        2. Ad-Hoc IBSS
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int
mtk_cfg80211_change_iface(struct wiphy *wiphy,
			  struct net_device *ndev, enum nl80211_iftype type, u32 *flags, struct vif_params *params)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	ENUM_PARAM_OP_MODE_T eOpMode;
	UINT_32 u4BufLen;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	if (type == NL80211_IFTYPE_STATION)
		eOpMode = NET_TYPE_INFRA;
	else if (type == NL80211_IFTYPE_ADHOC)
		eOpMode = NET_TYPE_IBSS;
	else
		return -EINVAL;

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetInfrastructureMode,
			   &eOpMode, sizeof(eOpMode), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(REQ, WARN, "set infrastructure mode error:%x\n", rStatus);

	/* reset wpa info */
	prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;
	prGlueInfo->rWpaInfo.u4KeyMgmt = 0;
	prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
#if CFG_SUPPORT_802_11W
	prGlueInfo->rWpaInfo.u4Mfp = IW_AUTH_MFP_DISABLED;
#endif

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for adding key
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int
mtk_cfg80211_add_key(struct wiphy *wiphy,
		     struct net_device *ndev,
		     u8 key_index, bool pairwise, const u8 *mac_addr, struct key_params *params)
{
	PARAM_KEY_T rKey;
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Rslt = -EINVAL;
	UINT_32 u4BufLen = 0;
	UINT_8 tmp1[8];
	UINT_8 tmp2[8];

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	kalMemZero(&rKey, sizeof(PARAM_KEY_T));

	rKey.u4KeyIndex = key_index;

	if (mac_addr) {
		COPY_MAC_ADDR(rKey.arBSSID, mac_addr);
		if ((rKey.arBSSID[0] == 0x00) && (rKey.arBSSID[1] == 0x00) && (rKey.arBSSID[2] == 0x00) &&
		    (rKey.arBSSID[3] == 0x00) && (rKey.arBSSID[4] == 0x00) && (rKey.arBSSID[5] == 0x00)) {
			rKey.arBSSID[0] = 0xff;
			rKey.arBSSID[1] = 0xff;
			rKey.arBSSID[2] = 0xff;
			rKey.arBSSID[3] = 0xff;
			rKey.arBSSID[4] = 0xff;
			rKey.arBSSID[5] = 0xff;
		}
		if (rKey.arBSSID[0] != 0xFF) {
			rKey.u4KeyIndex |= BIT(31);
			if ((rKey.arBSSID[0] != 0x00) || (rKey.arBSSID[1] != 0x00) || (rKey.arBSSID[2] != 0x00) ||
			    (rKey.arBSSID[3] != 0x00) || (rKey.arBSSID[4] != 0x00) || (rKey.arBSSID[5] != 0x00))
				rKey.u4KeyIndex |= BIT(30);
		}
	} else {
		rKey.arBSSID[0] = 0xff;
		rKey.arBSSID[1] = 0xff;
		rKey.arBSSID[2] = 0xff;
		rKey.arBSSID[3] = 0xff;
		rKey.arBSSID[4] = 0xff;
		rKey.arBSSID[5] = 0xff;
		/* rKey.u4KeyIndex |= BIT(31);//Enable BIT 31 will make tx use bc key id,should use pairwise key id 0 */
	}

	if (params->key) {
		/* rKey.aucKeyMaterial[0] = kalMemAlloc(params->key_len, VIR_MEM_TYPE); */
		kalMemCopy(rKey.aucKeyMaterial, params->key, params->key_len);
		if (params->key_len == 32) {
			kalMemCopy(tmp1, &params->key[16], 8);
			kalMemCopy(tmp2, &params->key[24], 8);
			kalMemCopy(&rKey.aucKeyMaterial[16], tmp2, 8);
			kalMemCopy(&rKey.aucKeyMaterial[24], tmp1, 8);
		}
	}

	rKey.u4KeyLength = params->key_len;
	rKey.u4Length = ((ULONG)&(((P_P2P_PARAM_KEY_T) 0)->aucKeyMaterial)) + rKey.u4KeyLength;

	rStatus = kalIoctl(prGlueInfo, wlanoidSetAddKey, &rKey, rKey.u4Length, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus == WLAN_STATUS_SUCCESS)
		i4Rslt = 0;

	return i4Rslt;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for getting key for specified STA
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int
mtk_cfg80211_get_key(struct wiphy *wiphy,
		     struct net_device *ndev,
		     u8 key_index,
		     bool pairwise,
		     const u8 *mac_addr, void *cookie, void (*callback) (void *cookie, struct key_params *))
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	/* not implemented */

	return -EINVAL;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for removing key for specified STA
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev, u8 key_index, bool pairwise, const u8 *mac_addr)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	PARAM_REMOVE_KEY_T rRemoveKey;
	UINT_32 u4BufLen = 0;
	INT_32 i4Rslt = -EINVAL;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	kalMemZero(&rRemoveKey, sizeof(PARAM_REMOVE_KEY_T));
	if (mac_addr)
		COPY_MAC_ADDR(rRemoveKey.arBSSID, mac_addr);
	else if (key_index <= gucKeyIndex) {	/* new operation, reset gucKeyIndex */
		gucKeyIndex = 255;
	} else {			/* bypass the next remove key operation */
		gucKeyIndex = key_index;
		return -EBUSY;
	}
	rRemoveKey.u4KeyIndex = key_index;
	rRemoveKey.u4Length = sizeof(PARAM_REMOVE_KEY_T);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetRemoveKey, &rRemoveKey, rRemoveKey.u4Length, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "remove key error:%x\n", rStatus);
		if (WLAN_STATUS_FAILURE == rStatus && mac_addr) {
			i4Rslt = -EBUSY;
			gucKeyIndex = key_index;
		}
	} else {
		gucKeyIndex = 255;
		i4Rslt = 0;
	}

	return i4Rslt;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for setting default key on an interface
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int
mtk_cfg80211_set_default_key(struct wiphy *wiphy, struct net_device *ndev, u8 key_index, bool unicast, bool multicast)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	/* not implemented */

	return WLAN_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for setting set_default_mgmt_ke on an interface
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_set_default_mgmt_key(struct wiphy *wiphy, struct net_device *netdev, u8 key_index)
{
	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for getting station information such as RSSI
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_get_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac, struct station_info *sinfo)
{
#define LINKSPEED_MAX_RANGE_11BGN 3000
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	PARAM_MAC_ADDRESS arBssid;
	UINT_32 u4BufLen;
	UINT_32 u4Rate = 0;
	UINT_32 u8diffTxBad, u8diffRetry;
	INT_32 i4Rssi = 0;
	PARAM_802_11_STATISTICS_STRUCT_T rStatistics;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	kalMemZero(arBssid, MAC_ADDR_LEN);
	wlanQueryInformation(prGlueInfo->prAdapter, wlanoidQueryBssid, &arBssid[0], sizeof(arBssid), &u4BufLen);

	/* 1. check BSSID */
	if (UNEQUAL_MAC_ADDR(arBssid, mac)) {
		/* wrong MAC address */
		DBGLOG(REQ, WARN, "incorrect BSSID: [ %pM ] currently connected BSSID[ %pM ]\n",
				   mac, arBssid);
		return -ENOENT;
	}

	/* 2. fill TX rate */
	if (prGlueInfo->eParamMediaStateIndicated != PARAM_MEDIA_STATE_CONNECTED) {
		/* not connected */
		DBGLOG(REQ, WARN, "not yet connected\n");
	} else {
		rStatus = kalIoctl(prGlueInfo,
				wlanoidQueryLinkSpeed, &u4Rate, sizeof(u4Rate), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_BITRATE);
#else
		sinfo->filled |= STATION_INFO_TX_BITRATE;
#endif

		if ((rStatus != WLAN_STATUS_SUCCESS) || (u4Rate == 0)) {
			/* DBGLOG(REQ, WARN, "unable to retrieve link speed\n")); */
			DBGLOG(REQ, WARN, "last link speed\n");
			sinfo->txrate.legacy = prGlueInfo->u4LinkSpeedCache;
		} else {
			/* sinfo->filled |= STATION_INFO_TX_BITRATE; */
			sinfo->txrate.legacy = u4Rate / 1000;	/* convert from 100bps to 100kbps */
			prGlueInfo->u4LinkSpeedCache = u4Rate / 1000;
		}
	}

	/* 3. fill RSSI */
	if (prGlueInfo->eParamMediaStateIndicated != PARAM_MEDIA_STATE_CONNECTED) {
		/* not connected */
		DBGLOG(REQ, WARN, "not yet connected\n");
	} else {
		rStatus = kalIoctl(prGlueInfo,
				   wlanoidQueryRssi, &i4Rssi, sizeof(i4Rssi), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
#else
		sinfo->filled |= STATION_INFO_SIGNAL;
#endif

		if (rStatus != WLAN_STATUS_SUCCESS || (i4Rssi == PARAM_WHQL_RSSI_MIN_DBM)
		|| (i4Rssi == PARAM_WHQL_RSSI_MAX_DBM)) {
			/* DBGLOG(REQ, WARN, "unable to retrieve link speed\n"); */
			DBGLOG(REQ, WARN, "last rssi\n");
			sinfo->signal = prGlueInfo->i4RssiCache;
		} else {
			/* in the cfg80211 layer, the signal is a signed char variable. */
			sinfo->signal = i4Rssi;	/* dBm */
			prGlueInfo->i4RssiCache = i4Rssi;
		}
		sinfo->rx_packets = prGlueInfo->rNetDevStats.rx_packets;

		/* 4. Fill Tx OK and Tx Bad */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_PACKETS);
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_FAILED);
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_BYTES);
		sinfo->filled |= BIT(NL80211_STA_INFO_RX_PACKETS);
		sinfo->filled |= BIT(NL80211_STA_INFO_RX_BYTES);
#else
		sinfo->filled |= STATION_INFO_TX_PACKETS;
		sinfo->filled |= STATION_INFO_TX_FAILED;
		sinfo->filled |= STATION_INFO_TX_BYTES;
		sinfo->filled |= STATION_INFO_RX_PACKETS;
		sinfo->filled |= STATION_INFO_RX_BYTES;
#endif
		{
			WLAN_STATUS rStatus;

			kalMemZero(&rStatistics, sizeof(rStatistics));
			/* Get Tx OK/Fail cnt from AIS statistic counter */
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidQueryStatisticsPL,
					   &rStatistics, sizeof(rStatistics), TRUE, TRUE, TRUE, FALSE, &u4BufLen);

			if (rStatus != WLAN_STATUS_SUCCESS) {
				DBGLOG(REQ, WARN, "unable to retrieive statistic\n");
			} else {
				INT_32 i4RssiThreshold = -85;	/* set rssi threshold -85dBm */
				UINT_32 u4LinkspeedThreshold = 55;	/* set link speed threshold 5.5Mbps */
				BOOLEAN fgWeighted = 0;

				/* calculate difference */
				u8diffTxBad = rStatistics.rFailedCount.QuadPart - prGlueInfo->u8Statistic[0];
				u8diffRetry = rStatistics.rRetryCount.QuadPart - prGlueInfo->u8Statistic[1];
				/* restore counters */
				prGlueInfo->u8Statistic[0] = rStatistics.rFailedCount.QuadPart;
				prGlueInfo->u8Statistic[1] = rStatistics.rRetryCount.QuadPart;

				/* check threshold is valid */
				if (prGlueInfo->fgPoorlinkValid) {
					if (prGlueInfo->i4RssiThreshold)
						i4RssiThreshold = prGlueInfo->i4RssiThreshold;
					if (prGlueInfo->u4LinkspeedThreshold)
						u4LinkspeedThreshold = prGlueInfo->u4LinkspeedThreshold;
				}
				/* add weighted to fail counter */
				if (sinfo->txrate.legacy < u4LinkspeedThreshold || sinfo->signal < i4RssiThreshold) {
					prGlueInfo->u8TotalFailCnt += (u8diffTxBad * 16 + u8diffRetry);
					fgWeighted = 1;
				} else {
					prGlueInfo->u8TotalFailCnt += u8diffTxBad;
				}
				/* report counters */
				prGlueInfo->rNetDevStats.tx_packets = rStatistics.rTransmittedFragmentCount.QuadPart;
				prGlueInfo->rNetDevStats.tx_errors = prGlueInfo->u8TotalFailCnt;

				sinfo->tx_packets = prGlueInfo->rNetDevStats.tx_packets;
				sinfo->tx_failed = prGlueInfo->rNetDevStats.tx_errors;
				/* Good Fail Bad Difference retry difference Linkspeed Rate Weighted */
				DBGLOG(REQ, TRACE,
					"Poorlink State TxOK(%d) TxFail(%d) Bad(%d) Retry(%d)",
					sinfo->tx_packets,
					sinfo->tx_failed,
					(int)u8diffTxBad,
					(int)u8diffRetry);
				DBGLOG(REQ, TRACE,
					"Rate(%d) Signal(%d) Weight(%d) QuadPart(%d)\n",
					sinfo->txrate.legacy,
					sinfo->signal,
					(int)fgWeighted,
					(int)rStatistics.rMultipleRetryCount.QuadPart);
			}
		}

	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for adding a station information
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_change_station(struct wiphy *wiphy, struct net_device *ndev,
				const u8 *mac, struct station_parameters *params)
{
#if (CFG_SUPPORT_TDLS == 1)
	/*
	   EX: In supplicant,
	   (Supplicant) wpa_tdls_process_tpk_m3() ->
	   (Supplicant) wpa_tdls_enable_link() ->
	   (Supplicant) wpa_sm_tdls_peer_addset() ->
	   (Supplicant) ..tdls_peer_addset() ->
	   (Supplicant) wpa_supplicant_tdls_peer_addset() ->
	   (Supplicant) wpa_drv_sta_add() ->
	   (Supplicant) ..sta_add() ->
	   (Supplicant) wpa_driver_nl80211_sta_add() ->
	   (NL80211) nl80211_set_station() ->
	   (Driver) mtk_cfg80211_change_station()

	   if nl80211_set_station fails, supplicant will tear down the link.
	 */
	P_GLUE_INFO_T prGlueInfo;
	TDLS_CMD_PEER_UPDATE_T rCmdUpdate;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen, u4Temp;

	/* sanity check */
	if ((wiphy == NULL) || (mac == NULL) || (params == NULL))
		return -EINVAL;

	DBGLOG(TDLS, INFO, "%s: 0x%p 0x%x\n", __func__, params->supported_rates, params->sta_flags_set);

	if (!(params->sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER)))
		return -EOPNOTSUPP;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (prGlueInfo == NULL)
		return -EINVAL;

	/* TODO: check if we are station mode, not AP mode */

	/* init */
	kalMemZero(&rCmdUpdate, sizeof(rCmdUpdate));
	kalMemCopy(rCmdUpdate.aucPeerMac, mac, 6);

	if (params->supported_rates != NULL) {
		u4Temp = params->supported_rates_len;
		if (u4Temp > TDLS_CMD_PEER_UPDATE_SUP_RATE_MAX) {
			u4Temp = TDLS_CMD_PEER_UPDATE_SUP_RATE_MAX;
			DBGLOG(TDLS, ERROR, "%s sup rate too long: %d\n", __func__, params->supported_rates_len);
		}
		kalMemCopy(rCmdUpdate.aucSupRate, params->supported_rates, u4Temp);
		rCmdUpdate.u2SupRateLen = u4Temp;
	}

	/*
	   In supplicant, only recognize WLAN_EID_QOS 46, not 0xDD WMM
	   So force to support UAPSD here.
	 */
	rCmdUpdate.UapsdBitmap = 0x0F;	/*params->uapsd_queues; */
	rCmdUpdate.UapsdMaxSp = 0;	/*params->max_sp; */

	DBGLOG(TDLS, INFO, "%s: UapsdBitmap=0x%x UapsdMaxSp=%d\n",
			    __func__, rCmdUpdate.UapsdBitmap, rCmdUpdate.UapsdMaxSp);

	rCmdUpdate.u2Capability = params->capability;

	if (params->ext_capab != NULL) {
		u4Temp = params->ext_capab_len;
		if (u4Temp > TDLS_CMD_PEER_UPDATE_EXT_CAP_MAXLEN) {
			u4Temp = TDLS_CMD_PEER_UPDATE_EXT_CAP_MAXLEN;
			DBGLOG(TDLS, ERROR, "%s ext_capab too long: %d\n", __func__, params->ext_capab_len);
		}
		kalMemCopy(rCmdUpdate.aucExtCap, params->ext_capab, u4Temp);
		rCmdUpdate.u2ExtCapLen = u4Temp;
	}

	if (params->ht_capa != NULL) {
		DBGLOG(TDLS, INFO, "%s: peer is 11n device\n", __func__);

		rCmdUpdate.rHtCap.u2CapInfo = params->ht_capa->cap_info;
		rCmdUpdate.rHtCap.ucAmpduParamsInfo = params->ht_capa->ampdu_params_info;
		rCmdUpdate.rHtCap.u2ExtHtCapInfo = params->ht_capa->extended_ht_cap_info;
		rCmdUpdate.rHtCap.u4TxBfCapInfo = params->ht_capa->tx_BF_cap_info;
		rCmdUpdate.rHtCap.ucAntennaSelInfo = params->ht_capa->antenna_selection_info;
		kalMemCopy(rCmdUpdate.rHtCap.rMCS.arRxMask,
			   params->ht_capa->mcs.rx_mask, sizeof(rCmdUpdate.rHtCap.rMCS.arRxMask));
		rCmdUpdate.rHtCap.rMCS.u2RxHighest = params->ht_capa->mcs.rx_highest;
		rCmdUpdate.rHtCap.rMCS.ucTxParams = params->ht_capa->mcs.tx_params;
		rCmdUpdate.fgIsSupHt = TRUE;
	}

	/* update a TDLS peer record */
	rStatus = kalIoctl(prGlueInfo,
			   TdlsexPeerUpdate,
			   &rCmdUpdate, sizeof(TDLS_CMD_PEER_UPDATE_T), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(TDLS, ERROR, "%s update error:%x\n", __func__, rStatus);
		return -EINVAL;
	}
#endif /* CFG_SUPPORT_TDLS */

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for adding a station information
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_add_station(struct wiphy *wiphy, struct net_device *ndev,
				const u8 *mac, struct station_parameters *params)
{
#if (CFG_SUPPORT_TDLS == 1)
	/* from supplicant -- wpa_supplicant_tdls_peer_addset() */
	P_GLUE_INFO_T prGlueInfo;
	TDLS_CMD_PEER_ADD_T rCmdCreate;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;

	if ((wiphy == NULL) || (mac == NULL) || (params == NULL))
		return -EINVAL;

	/*
	   wpa_sm_tdls_peer_addset(sm, peer->addr, 1, 0, 0, NULL, 0, NULL, NULL, 0,
	   NULL, 0);

	   wpa_sm_tdls_peer_addset(struct wpa_sm *sm, const u8 *addr, int add,
	   u16 aid, u16 capability, const u8 *supp_rates,
	   size_t supp_rates_len,
	   const struct ieee80211_ht_capabilities *ht_capab,
	   const struct ieee80211_vht_capabilities *vht_capab,
	   u8 qosinfo, const u8 *ext_capab, size_t ext_capab_len)

	   Only MAC address of the peer is valid.
	 */

	DBGLOG(TDLS, INFO, "%s: 0x%p %d\n", __func__, params->supported_rates, params->supported_rates_len);

	/* sanity check */
	if (!(params->sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER)))
		return -EOPNOTSUPP;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (prGlueInfo == NULL)
		return -EINVAL;

	/* TODO: check if we are station mode, not AP mode */

	/* init */
	kalMemZero(&rCmdCreate, sizeof(rCmdCreate));
	kalMemCopy(rCmdCreate.aucPeerMac, mac, 6);

#if 0
	rCmdCreate.eNetTypeIndex = NETWORK_TYPE_AIS_INDEX;

	rCmdCreate.u2CapInfo = params->capability;

	DBGLOG(TDLS, INFO, "<tdls_cmd> %s: capability = 0x%x\n", __func__, rCmdCreate.u2CapInfo);

	if ((params->supported_rates != NULL) && (params->supported_rates_len != 0)) {
		UINT32 u4Idx;

		DBGLOG(TDLS, INFO, "<tdls_cmd> %s: sup rate = 0x", __func__);

		rIeSup.ucId = ELEM_ID_SUP_RATES;
		rIeSup.ucLength = params->supported_rates_len;
		for (u4Idx = 0; u4Idx < rIeSup.ucLength; u4Idx++) {
			rIeSup.aucSupportedRates[u4Idx] = params->supported_rates[u4Idx];
			DBGLOG(TDLS, INFO, "%x ", rIeSup.aucSupportedRates[u4Idx]);
		}
		DBGLOG(TDLS, INFO, "\n");

		rateGetRateSetFromIEs(&rIeSup,
				      NULL,
				      &rCmdCreate.u2OperationalRateSet,
				      &rCmdCreate.u2BSSBasicRateSet, &rCmdCreate.fgIsUnknownBssBasicRate);
	}

	/* phy type */
#endif

	/* create a TDLS peer record */
	rStatus = kalIoctl(prGlueInfo,
			   TdlsexPeerAdd,
			   &rCmdCreate, sizeof(TDLS_CMD_PEER_ADD_T), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(TDLS, ERROR, "%s create error:%x\n", __func__, rStatus);
		return -EINVAL;
	}
#endif /* CFG_SUPPORT_TDLS */

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for deleting a station information
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 *
 * @other
 *		must implement if you have add_station().
 */
/*----------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
int mtk_cfg80211_del_station(struct wiphy *wiphy, struct net_device *ndev, struct station_del_parameters *params)
#else
int mtk_cfg80211_del_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac)
#endif
{
	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to do a scan
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	int i, j = 0;
	struct PARAM_SCAN_REQUEST_ADV_T rScanRequest;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "mtk_cfg80211_scan\n");
	kalMemZero(&rScanRequest, sizeof(PARAM_SCAN_REQUEST_EXT_T));

	/* check if there is any pending scan not yet finished */
	if (prGlueInfo->prScanRequest != NULL) {
		DBGLOG(REQ, ERROR, "prGlueInfo->prScanRequest != NULL\n");
		return -EBUSY;
	}

	if (request->n_ssids == 0) {
		rScanRequest.u4SsidNum = 0;
	} else if (request->n_ssids <= SCN_SSID_MAX_NUM) {
		rScanRequest.u4SsidNum = request->n_ssids;

		for (i = 0; i < request->n_ssids; i++) {
			if (request->ssids[i].ssid_len == 0) {
				continue;
			} else {
				COPY_SSID(rScanRequest.rSsid[j].aucSsid,
				rScanRequest.rSsid[j].u4SsidLen,
				request->ssids[i].ssid,
				request->ssids[i].ssid_len);
				j++;
			}
		}
	} else {
		DBGLOG(REQ, ERROR, "request->n_ssids:%d\n", request->n_ssids);
		return -EINVAL;
	}

	if (request->ie_len > 0) {
		rScanRequest.u4IELength = request->ie_len;
		rScanRequest.pucIE = (PUINT_8) (request->ie);
	} else {
		rScanRequest.u4IELength = 0;
	}

	prGlueInfo->prScanRequest = request;
	kalMemZero(prGlueInfo->rScanRequestChannel, sizeof(prGlueInfo->rScanRequestChannel));

	if(request->n_channels > MAXIMUM_OPERATION_CHANNEL_LIST) {
		DBGLOG(REQ, ERROR, "scan channel exceed max support number %d\n", request->n_channels);
		return -EINVAL;
	} else if (request->n_channels > 0) {
		for (i = 0; i < request->n_channels; i++) {
			if (request->channels[i] == NULL) {
				prGlueInfo->prScanRequest = NULL;
				kalMemZero(prGlueInfo->rScanRequestChannel, sizeof(prGlueInfo->rScanRequestChannel));
				DBGLOG(REQ, ERROR, "scan channel error\n");
				return -EINVAL;
			}
			kalMemCopy(&(prGlueInfo->rScanRequestChannel[i]),
						request->channels[i],
						sizeof(struct ieee80211_channel));
		}
	}

	rStatus = kalIoctl(prGlueInfo,
		wlanoidSetBssidListScanAdv,
		&rScanRequest,
		sizeof(struct PARAM_SCAN_REQUEST_ADV_T),
		FALSE, FALSE, FALSE, FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		prGlueInfo->prScanRequest = NULL;
		kalMemZero(prGlueInfo->rScanRequestChannel, sizeof(prGlueInfo->rScanRequestChannel));
		DBGLOG(REQ, ERROR, "scan error:%x\n", rStatus);
		return -EINVAL;
	}

	return 0;
}

static UINT_8 wepBuf[48];

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to connect to
 *        the ESS with the specified parameters
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_connect(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_connect_params *sme)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	ENUM_PARAM_ENCRYPTION_STATUS_T eEncStatus;
	ENUM_PARAM_AUTH_MODE_T eAuthMode;
	UINT_32 cipher;
	UINT_32 u4AkmSuite = 0;
	PARAM_CONNECT_T rNewSsid;
	BOOLEAN fgCarryWPSIE = FALSE;
	ENUM_PARAM_OP_MODE_T eOpMode;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "[wlan] mtk_cfg80211_connect %p %zu\n", sme->ie, sme->ie_len);

	if (prGlueInfo->prAdapter->rWifiVar.rConnSettings.eOPMode > NET_TYPE_AUTO_SWITCH)
		eOpMode = NET_TYPE_AUTO_SWITCH;
	else
		eOpMode = prGlueInfo->prAdapter->rWifiVar.rConnSettings.eOPMode;

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetInfrastructureMode,
			   &eOpMode, sizeof(eOpMode), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "wlanoidSetInfrastructureMode fail 0x%x\n", rStatus);
		return -EFAULT;
	}

	/* after set operation mode, key table are cleared */

	/* reset wpa info */
	prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;
	prGlueInfo->rWpaInfo.u4KeyMgmt = 0;
	prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
#if CFG_SUPPORT_802_11W
	prGlueInfo->rWpaInfo.u4Mfp = IW_AUTH_MFP_DISABLED;
#endif

	if (sme->crypto.wpa_versions & NL80211_WPA_VERSION_1)
		prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_WPA;
	else if (sme->crypto.wpa_versions & NL80211_WPA_VERSION_2)
		prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_WPA2;
	else
		prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;

	switch (sme->auth_type) {
	case NL80211_AUTHTYPE_OPEN_SYSTEM:
		prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
		break;
	case NL80211_AUTHTYPE_SHARED_KEY:
		prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_SHARED_KEY;
		break;
#if CFG_SUPPORT_802_11R
	case NL80211_AUTHTYPE_FT:
		prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_FT;
		break;
#endif
	default:
		prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM | IW_AUTH_ALG_SHARED_KEY;
		break;
	}

	if (sme->crypto.n_ciphers_pairwise) {
		prGlueInfo->prAdapter->rWifiVar.rConnSettings.rRsnInfo.au4PairwiseKeyCipherSuite[0] =
		    sme->crypto.ciphers_pairwise[0];
		switch (sme->crypto.ciphers_pairwise[0]) {
		case WLAN_CIPHER_SUITE_WEP40:
			prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_WEP40;
			break;
		case WLAN_CIPHER_SUITE_WEP104:
			prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_WEP104;
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_TKIP;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_CCMP;
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_CCMP;
			break;
		default:
			DBGLOG(REQ, WARN, "invalid cipher pairwise (%d)\n", sme->crypto.ciphers_pairwise[0]);
			return -EINVAL;
		}
	}

	if (sme->crypto.cipher_group) {
		prGlueInfo->prAdapter->rWifiVar.rConnSettings.rRsnInfo.u4GroupKeyCipherSuite = sme->crypto.cipher_group;
		switch (sme->crypto.cipher_group) {
		case WLAN_CIPHER_SUITE_WEP40:
			prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_WEP40;
			break;
		case WLAN_CIPHER_SUITE_WEP104:
			prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_WEP104;
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_TKIP;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_CCMP;
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_CCMP;
			break;
		default:
			DBGLOG(REQ, WARN, "invalid cipher group (%d)\n", sme->crypto.cipher_group);
			return -EINVAL;
		}
	}

	if (sme->crypto.n_akm_suites) {
		prGlueInfo->prAdapter->rWifiVar.rConnSettings.rRsnInfo.au4AuthKeyMgtSuite[0] =
		    sme->crypto.akm_suites[0];
		if (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_WPA) {
			switch (sme->crypto.akm_suites[0]) {
			case WLAN_AKM_SUITE_8021X:
				eAuthMode = AUTH_MODE_WPA;
				u4AkmSuite = WPA_AKM_SUITE_802_1X;
				break;
			case WLAN_AKM_SUITE_PSK:
				eAuthMode = AUTH_MODE_WPA_PSK;
				u4AkmSuite = WPA_AKM_SUITE_PSK;
				break;
			default:
				DBGLOG(REQ, WARN, "invalid auth mode (%d)\n", sme->crypto.akm_suites[0]);
				return -EINVAL;
			}
		} else if (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_WPA2) {
			switch (sme->crypto.akm_suites[0]) {
			case WLAN_AKM_SUITE_8021X:
				eAuthMode = AUTH_MODE_WPA2;
				u4AkmSuite = WPA_AKM_SUITE_802_1X;
				break;
			case WLAN_AKM_SUITE_PSK:
				eAuthMode = AUTH_MODE_WPA2_PSK;
				u4AkmSuite = WPA_AKM_SUITE_PSK;
				break;
#if CFG_SUPPORT_802_11R
			case WLAN_AKM_SUITE_FT_8021X:
				eAuthMode = AUTH_MODE_WPA2_FT;
				u4AkmSuite = RSN_AKM_SUITE_FT_802_1X;
				break;
			case WLAN_AKM_SUITE_FT_PSK:
				eAuthMode = AUTH_MODE_WPA2_FT_PSK;
				u4AkmSuite = RSN_AKM_SUITE_FT_PSK;
				break;
#endif

			default:
				DBGLOG(REQ, WARN, "invalid auth mode (%d)\n", sme->crypto.akm_suites[0]);
				return -EINVAL;
			}
		}
	}

	if (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_DISABLED) {
#if CFG_SUPPORT_802_11R
		switch (prGlueInfo->rWpaInfo.u4AuthAlg) {
		case IW_AUTH_ALG_OPEN_SYSTEM:
			eAuthMode = AUTH_MODE_OPEN;
			break;
		case IW_AUTH_ALG_FT:
			eAuthMode = AUTH_MODE_NON_RSN_FT;
			break;
		default:
			eAuthMode = AUTH_MODE_AUTO_SWITCH;
			break;
		}
#else
		eAuthMode = (prGlueInfo->rWpaInfo.u4AuthAlg == IW_AUTH_ALG_OPEN_SYSTEM) ?
			AUTH_MODE_OPEN : AUTH_MODE_AUTO_SWITCH;
#endif
	}

	prGlueInfo->rWpaInfo.fgPrivacyInvoke = sme->privacy;

	prGlueInfo->fgWpsActive = FALSE;
#if CFG_SUPPORT_HOTSPOT_2_0
	prGlueInfo->fgConnectHS20AP = FALSE;
#endif

	if (sme->ie && sme->ie_len > 0) {
		WLAN_STATUS rStatus;
		UINT_32 u4BufLen;
		PUINT_8 prDesiredIE = NULL;
		PUINT_8 pucIEStart = (PUINT_8)sme->ie;

#if CFG_SUPPORT_WAPI
		if (wextSrchDesiredWAPIIE(pucIEStart, sme->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetWapiAssocInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

			if (rStatus != WLAN_STATUS_SUCCESS)
				DBGLOG(SEC, WARN, "[wapi] set wapi assoc info error:%x\n", rStatus);
		}
#endif

		DBGLOG(REQ, TRACE, "[wlan] wlanoidSetWapiAssocInfo: .fgWapiMode = %d\n",
				   prGlueInfo->prAdapter->rWifiVar.rConnSettings.fgWapiMode);

#if CFG_SUPPORT_WPS2
		if (wextSrchDesiredWPSIE(pucIEStart, sme->ie_len, 0xDD, (PUINT_8 *) &prDesiredIE)) {
			prGlueInfo->fgWpsActive = TRUE;
			fgCarryWPSIE = TRUE;

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetWSCAssocInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, FALSE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS)
				DBGLOG(SEC, WARN, "WSC] set WSC assoc info error:%x\n", rStatus);
		}
#endif

#if CFG_SUPPORT_HOTSPOT_2_0
		if (wextSrchDesiredHS20IE(pucIEStart, sme->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetHS20Info,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set HS20 assoc info error:%lx\n", rStatus); */
			}
		}
		if (wextSrchDesiredInterworkingIE(pucIEStart, sme->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetInterworkingInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set Interworking assoc info error:%lx\n", rStatus); */
			}
		}
		if (wextSrchDesiredRoamingConsortiumIE(pucIEStart, sme->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetRoamingConsortiumIEInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set RoamingConsortium assoc info error:%lx\n", rStatus); */
			}
		}
#endif
	}

	/* clear WSC Assoc IE buffer in case WPS IE is not detected */
	if (fgCarryWPSIE == FALSE) {
		kalMemZero(&prGlueInfo->aucWSCAssocInfoIE, 200);
		prGlueInfo->u2WSCAssocInfoIELen = 0;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetAuthMode, &eAuthMode, sizeof(eAuthMode), FALSE, FALSE, FALSE, FALSE, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(REQ, WARN, "set auth mode error:%x\n", rStatus);

	cipher = prGlueInfo->rWpaInfo.u4CipherGroup | prGlueInfo->rWpaInfo.u4CipherPairwise;

	if (prGlueInfo->rWpaInfo.fgPrivacyInvoke) {
		if (cipher & IW_AUTH_CIPHER_CCMP) {
			eEncStatus = ENUM_ENCRYPTION3_ENABLED;
		} else if (cipher & IW_AUTH_CIPHER_TKIP) {
			eEncStatus = ENUM_ENCRYPTION2_ENABLED;
		} else if (cipher & (IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40)) {
			eEncStatus = ENUM_ENCRYPTION1_ENABLED;
		} else if (cipher & IW_AUTH_CIPHER_NONE) {
			if (prGlueInfo->rWpaInfo.fgPrivacyInvoke)
				eEncStatus = ENUM_ENCRYPTION1_ENABLED;
			else
				eEncStatus = ENUM_ENCRYPTION_DISABLED;
		} else {
			eEncStatus = ENUM_ENCRYPTION_DISABLED;
		}
	} else {
		eEncStatus = ENUM_ENCRYPTION_DISABLED;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetEncryptionStatus,
			   &eEncStatus, sizeof(eEncStatus), FALSE, FALSE, FALSE, FALSE, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(REQ, WARN, "set encryption mode error:%x\n", rStatus);

	if (sme->key_len != 0 && prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_DISABLED) {
		P_PARAM_WEP_T prWepKey = (P_PARAM_WEP_T) wepBuf;

		prWepKey->u4Length = 12 + sme->key_len;
		prWepKey->u4KeyLength = (UINT_32) sme->key_len;
		prWepKey->u4KeyIndex = (UINT_32) sme->key_idx;
		prWepKey->u4KeyIndex |= BIT(31);
		if (prWepKey->u4KeyLength > 32) {
			DBGLOG(REQ, ERROR, "Too long key length (%u)\n", prWepKey->u4KeyLength);
			return -EINVAL;
		}
		kalMemCopy(prWepKey->aucKeyMaterial, sme->key, prWepKey->u4KeyLength);

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetAddWep,
				   prWepKey, prWepKey->u4Length, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(REQ, ERROR, "wlanoidSetAddWep fail 0x%x\n", rStatus);
			return -EFAULT;
		}
	}

	if (sme->channel) {
		rNewSsid.u4CenterFreq = sme->channel->center_freq;
		rNewSsid.ucSpecificChnl = 1;
		mtk_p2p_cfg80211func_channel_format_switch(sme->channel,
							NL80211_CHAN_NO_HT,
							&rNewSsid.rChannelInfo,
							&rNewSsid.eChnlSco);
	} else {
		rNewSsid.u4CenterFreq = 0;
		rNewSsid.ucSpecificChnl = 0;
	}
	rNewSsid.pucBssid = (UINT_8 *)sme->bssid_hint;
	rNewSsid.pucSsid = (UINT_8 *)sme->ssid;
	rNewSsid.u4SsidLen = sme->ssid_len;
	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetConnect,
			   (PVOID)(&rNewSsid), sizeof(PARAM_CONNECT_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "set SSID:%x\n", rStatus);
		return -EINVAL;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to disconnect from
 *        currently connected ESS
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *ndev, u16 reason_code)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo, wlanoidSetDisassociate, NULL, 0, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "disassociate error:%x\n", rStatus);
		return -EFAULT;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to join an IBSS group
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_join_ibss(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_ibss_params *params)
{
	PARAM_SSID_T rNewSsid;
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4ChnlFreq;	/* Store channel or frequency information */
	UINT_32 u4BufLen = 0;
	WLAN_STATUS rStatus;
	struct ieee80211_channel *channel = NULL;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	/* set channel */
	if (params->chandef.chan)
		channel = params->chandef.chan;
	if (channel) {
		u4ChnlFreq = nicChannelNum2Freq(channel->hw_value);
		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetFrequency,
				   &u4ChnlFreq, sizeof(u4ChnlFreq), FALSE, FALSE, FALSE, FALSE, &u4BufLen);
		if (rStatus != WLAN_STATUS_SUCCESS)
			return -EFAULT;
	}

	/* set SSID */
	kalMemCopy(rNewSsid.aucSsid, params->ssid, params->ssid_len);
	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetSsid,
			   (PVOID)(&rNewSsid), sizeof(PARAM_SSID_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "set SSID:%x\n", rStatus);
		return -EFAULT;
	}

	return 0;

	return -EINVAL;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to leave from IBSS group
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_leave_ibss(struct wiphy *wiphy, struct net_device *ndev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo, wlanoidSetDisassociate, NULL, 0, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "disassociate error:%x\n", rStatus);
		return -EFAULT;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to configure
 *        WLAN power managemenet
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_set_power_mgmt(struct wiphy *wiphy, struct net_device *ndev, bool enabled, int timeout)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	PARAM_POWER_MODE ePowerMode;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	if (enabled) {
		if (timeout == -1)
			ePowerMode = Param_PowerModeFast_PSP;
		else
			ePowerMode = Param_PowerModeMAX_PSP;
	} else {
		ePowerMode = Param_PowerModeCAM;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSet802dot11PowerSaveProfile,
			   &ePowerMode, sizeof(ePowerMode), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "set_power_mgmt error:%x\n", rStatus);
		return -EFAULT;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to cache
 *        a PMKID for a BSSID
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_set_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	P_PARAM_PMKID_T prPmkid;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	prPmkid = (P_PARAM_PMKID_T) kalMemAlloc(8 + sizeof(PARAM_BSSID_INFO_T), VIR_MEM_TYPE);
	if (!prPmkid) {
		DBGLOG(REQ, ERROR, "Can not alloc memory for IW_PMKSA_ADD\n");
		return -ENOMEM;
	}

	prPmkid->u4Length = 8 + sizeof(PARAM_BSSID_INFO_T);
	prPmkid->u4BSSIDInfoCount = 1;
	kalMemCopy(prPmkid->arBSSIDInfo->arBSSID, pmksa->bssid, 6);
	kalMemCopy(prPmkid->arBSSIDInfo->arPMKID, pmksa->pmkid, IW_PMKID_LEN);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetPmkid, prPmkid, sizeof(PARAM_PMKID_T), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(REQ, WARN, "add pmkid error:%x\n", rStatus);
	kalMemFree(prPmkid, VIR_MEM_TYPE, 8 + sizeof(PARAM_BSSID_INFO_T));

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to remove
 *        a cached PMKID for a BSSID
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_del_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa)
{

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to flush
 *        all cached PMKID
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_flush_pmksa(struct wiphy *wiphy, struct net_device *ndev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	P_PARAM_PMKID_T prPmkid;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	prPmkid = (P_PARAM_PMKID_T) kalMemAlloc(8, VIR_MEM_TYPE);
	if (!prPmkid) {
		DBGLOG(REQ, ERROR, "Can not alloc memory for IW_PMKSA_FLUSH\n");
		return -ENOMEM;
	}

	prPmkid->u4Length = 8;
	prPmkid->u4BSSIDInfoCount = 0;

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetPmkid, prPmkid, sizeof(PARAM_PMKID_T), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(REQ, WARN, "flush pmkid error:%x\n", rStatus);
	kalMemFree(prPmkid, VIR_MEM_TYPE, 8);

	return 0;
}

void mtk_cfg80211_mgmt_frame_register(IN struct wiphy *wiphy,
				      IN struct wireless_dev *wdev,
				      IN u16 frame_type, IN bool reg)
{
#if 0
	P_MSG_P2P_MGMT_FRAME_REGISTER_T prMgmtFrameRegister = (P_MSG_P2P_MGMT_FRAME_REGISTER_T) NULL;
#endif
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) NULL;

	do {

		DBGLOG(REQ, LOUD, "mtk_cfg80211_mgmt_frame_register\n");

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

		switch (frame_type) {
		case MAC_FRAME_PROBE_REQ:
			if (reg) {
				prGlueInfo->u4OsMgmtFrameFilter |= PARAM_PACKET_FILTER_PROBE_REQ;
				DBGLOG(REQ, LOUD, "Open packet filer probe request\n");
			} else {
				prGlueInfo->u4OsMgmtFrameFilter &= ~PARAM_PACKET_FILTER_PROBE_REQ;
				DBGLOG(REQ, LOUD, "Close packet filer probe request\n");
			}
			break;
		case MAC_FRAME_ACTION:
			if (reg) {
				prGlueInfo->u4OsMgmtFrameFilter |= PARAM_PACKET_FILTER_ACTION_FRAME;
				DBGLOG(REQ, LOUD, "Open packet filer action frame.\n");
			} else {
				prGlueInfo->u4OsMgmtFrameFilter &= ~PARAM_PACKET_FILTER_ACTION_FRAME;
				DBGLOG(REQ, LOUD, "Close packet filer action frame.\n");
			}
			break;
		default:
			DBGLOG(REQ, TRACE, "Ask frog to add code for mgmt:%x\n", frame_type);
			break;
		}

		if (prGlueInfo->prAdapter != NULL) {
			/* prGlueInfo->ulFlag |= GLUE_FLAG_FRAME_FILTER_AIS; */
			set_bit(GLUE_FLAG_FRAME_FILTER_AIS_BIT, &prGlueInfo->ulFlag);

			/* wake up main thread */
			wake_up_interruptible(&prGlueInfo->waitq);

			if (in_interrupt())
				DBGLOG(REQ, TRACE, "It is in interrupt level\n");
		}
#if 0

		prMgmtFrameRegister = (P_MSG_P2P_MGMT_FRAME_REGISTER_T) cnmMemAlloc(prGlueInfo->prAdapter,
										    RAM_TYPE_MSG,
										    sizeof
										    (MSG_P2P_MGMT_FRAME_REGISTER_T));

		if (prMgmtFrameRegister == NULL) {
			ASSERT(FALSE);
			break;
		}

		prMgmtFrameRegister->rMsgHdr.eMsgId = MID_MNY_P2P_MGMT_FRAME_REGISTER;

		prMgmtFrameRegister->u2FrameType = frame_type;
		prMgmtFrameRegister->fgIsRegister = reg;

		mboxSendMsg(prGlueInfo->prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prMgmtFrameRegister, MSG_SEND_METHOD_BUF);

#endif

	} while (FALSE);

}				/* mtk_cfg80211_mgmt_frame_register */

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to stay on a
 *        specified channel
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_remain_on_channel(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   struct ieee80211_channel *chan,
				   unsigned int duration, u64 *cookie)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 i4Rslt = -EINVAL;
	P_MSG_REMAIN_ON_CHANNEL_T prMsgChnlReq = (P_MSG_REMAIN_ON_CHANNEL_T) NULL;

	do {
		if ((wiphy == NULL) || (wdev == NULL) || (chan == NULL) || (cookie == NULL))
			break;

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
		ASSERT(prGlueInfo);

#if 1
		DBGLOG(REQ, TRACE, "--> %s()\n", __func__);
#endif

		*cookie = prGlueInfo->u8Cookie++;

		prMsgChnlReq = cnmMemAlloc(prGlueInfo->prAdapter, RAM_TYPE_MSG, sizeof(MSG_REMAIN_ON_CHANNEL_T));

		if (prMsgChnlReq == NULL) {
			ASSERT(FALSE);
			i4Rslt = -ENOMEM;
			break;
		}

		prMsgChnlReq->rMsgHdr.eMsgId = MID_MNY_AIS_REMAIN_ON_CHANNEL;
		prMsgChnlReq->u8Cookie = *cookie;
		prMsgChnlReq->u4DurationMs = duration;

		prMsgChnlReq->ucChannelNum = nicFreq2ChannelNum(chan->center_freq * 1000);
		prMsgChnlReq->eSco = 0;

		switch (chan->band) {
		case NL80211_BAND_2GHZ:
			prMsgChnlReq->eBand = BAND_2G4;
			break;
		case NL80211_BAND_5GHZ:
			prMsgChnlReq->eBand = BAND_5G;
			break;
		default:
			prMsgChnlReq->eBand = BAND_2G4;
			break;
		}

		mboxSendMsg(prGlueInfo->prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prMsgChnlReq, MSG_SEND_METHOD_BUF);

		i4Rslt = 0;
	} while (FALSE);

	return i4Rslt;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to cancel staying
 *        on a specified channel
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_cancel_remain_on_channel(struct wiphy *wiphy,
					  struct wireless_dev *wdev,
					  u64 cookie)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 i4Rslt = -EINVAL;
	P_MSG_CANCEL_REMAIN_ON_CHANNEL_T prMsgChnlAbort = (P_MSG_CANCEL_REMAIN_ON_CHANNEL_T) NULL;

	do {
		if ((wiphy == NULL) || (wdev == NULL))
			break;

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
		ASSERT(prGlueInfo);

#if 1
		DBGLOG(REQ, TRACE, "--> %s()\n", __func__);
#endif

		prMsgChnlAbort =
		    cnmMemAlloc(prGlueInfo->prAdapter, RAM_TYPE_MSG, sizeof(MSG_CANCEL_REMAIN_ON_CHANNEL_T));

		if (prMsgChnlAbort == NULL) {
			ASSERT(FALSE);
			i4Rslt = -ENOMEM;
			break;
		}

		prMsgChnlAbort->rMsgHdr.eMsgId = MID_MNY_AIS_CANCEL_REMAIN_ON_CHANNEL;
		prMsgChnlAbort->u8Cookie = cookie;

		mboxSendMsg(prGlueInfo->prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prMsgChnlAbort, MSG_SEND_METHOD_BUF);

		i4Rslt = 0;
	} while (FALSE);

	return i4Rslt;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to send a management frame
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int
mtk_cfg80211_mgmt_tx(struct wiphy *wiphy,
		     struct wireless_dev *wdev,
		     struct cfg80211_mgmt_tx_params *params,
		     u64 *cookie)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 i4Rslt = -EINVAL;
	P_MSG_MGMT_TX_REQUEST_T prMsgTxReq = (P_MSG_MGMT_TX_REQUEST_T) NULL;
	P_MSDU_INFO_T prMgmtFrame = (P_MSDU_INFO_T) NULL;
	PUINT_8 pucFrameBuf = (PUINT_8) NULL;

	do {
#if 1
		DBGLOG(REQ, TRACE, "--> %s()\n", __func__);
#endif

		if ((wiphy == NULL) || (wdev == NULL) || (params == 0) || (cookie == NULL))
			break;

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
		ASSERT(prGlueInfo);

		*cookie = prGlueInfo->u8Cookie++;

		/* Channel & Channel Type & Wait time are ignored. */
		prMsgTxReq = cnmMemAlloc(prGlueInfo->prAdapter, RAM_TYPE_MSG, sizeof(MSG_MGMT_TX_REQUEST_T));

		if (prMsgTxReq == NULL) {
			ASSERT(FALSE);
			i4Rslt = -ENOMEM;
			break;
		}

		prMsgTxReq->fgNoneCckRate = FALSE;
		prMsgTxReq->fgIsWaitRsp = TRUE;

		prMgmtFrame = cnmMgtPktAlloc(prGlueInfo->prAdapter, (UINT_32) (params->len + MAC_TX_RESERVED_FIELD));
		prMsgTxReq->prMgmtMsduInfo = prMgmtFrame;
		if (prMsgTxReq->prMgmtMsduInfo == NULL) {
			ASSERT(FALSE);
			i4Rslt = -ENOMEM;
			break;
		}

		prMsgTxReq->u8Cookie = *cookie;
		prMsgTxReq->rMsgHdr.eMsgId = MID_MNY_AIS_MGMT_TX;

		pucFrameBuf = (PUINT_8) ((ULONG) prMgmtFrame->prPacket + MAC_TX_RESERVED_FIELD);

		kalMemCopy(pucFrameBuf, params->buf, params->len);

		prMgmtFrame->u2FrameLength = params->len;

		mboxSendMsg(prGlueInfo->prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prMsgTxReq, MSG_SEND_METHOD_BUF);

		i4Rslt = 0;
	} while (FALSE);

	if ((i4Rslt != 0) && (prMsgTxReq != NULL)) {
		if (prMsgTxReq->prMgmtMsduInfo != NULL)
			cnmMgtPktFree(prGlueInfo->prAdapter, prMsgTxReq->prMgmtMsduInfo);

		cnmMemFree(prGlueInfo->prAdapter, prMsgTxReq);
	}

	return i4Rslt;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to cancel the wait time
 *        from transmitting a management frame on another channel
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_mgmt_tx_cancel_wait(struct wiphy *wiphy,
				     struct wireless_dev *wdev,
				     u64 cookie)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	/* not implemented */

	return -EINVAL;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for handling sched_scan start/stop request
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/

int
mtk_cfg80211_sched_scan_start(IN struct wiphy *wiphy,
			      IN struct net_device *ndev, IN struct cfg80211_sched_scan_request *request)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 i, u4BufLen;
	P_PARAM_SCHED_SCAN_REQUEST prSchedScanRequest;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	ASSERT(prGlueInfo);

	/* check if there is any pending scan/sched_scan not yet finished */
	if (prGlueInfo->prScanRequest != NULL || prGlueInfo->prSchedScanRequest != NULL) {
		DBGLOG(SCN, ERROR, "(prGlueInfo->prScanRequest != NULL || prGlueInfo->prSchedScanRequest != NULL)\n");
		return -EBUSY;
	} else if (request == NULL || request->n_match_sets > CFG_SCAN_SSID_MATCH_MAX_NUM) {
		DBGLOG(SCN, ERROR, "(request == NULL || request->n_match_sets > CFG_SCAN_SSID_MATCH_MAX_NUM)\n");
		/* invalid scheduled scan request */
		return -EINVAL;
	} else if (/* !request->n_ssids || */!request->n_match_sets) {
		/* invalid scheduled scan request */
		return -EINVAL;
	}

	prSchedScanRequest = (P_PARAM_SCHED_SCAN_REQUEST) kalMemAlloc(sizeof(PARAM_SCHED_SCAN_REQUEST), VIR_MEM_TYPE);
	if (prSchedScanRequest == NULL) {
		DBGLOG(SCN, ERROR, "(prSchedScanRequest == NULL) kalMemAlloc fail\n");
		return -ENOMEM;
	}

	kalMemZero(prSchedScanRequest, sizeof(PARAM_SCHED_SCAN_REQUEST));

	prSchedScanRequest->u4SsidNum = request->n_match_sets;
	for (i = 0; i < request->n_match_sets; i++) {
		if (request->match_sets == NULL || &(request->match_sets[i]) == NULL) {
			prSchedScanRequest->arSsid[i].u4SsidLen = 0;
		} else {
			COPY_SSID(prSchedScanRequest->arSsid[i].aucSsid,
				  prSchedScanRequest->arSsid[i].u4SsidLen,
				  request->match_sets[i].ssid.ssid, request->match_sets[i].ssid.ssid_len);
		}
	}

	prSchedScanRequest->u4IELength = request->ie_len;
	if (request->ie_len > 0)
		prSchedScanRequest->pucIE = (PUINT_8) (request->ie);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	prSchedScanRequest->u2ScanInterval = (UINT_16) (request->scan_plans[0].interval);
#else
	prSchedScanRequest->u2ScanInterval = (UINT_16) (request->interval);
#endif

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetStartSchedScan,
			   prSchedScanRequest, sizeof(PARAM_SCHED_SCAN_REQUEST), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	kalMemFree(prSchedScanRequest, VIR_MEM_TYPE, sizeof(PARAM_SCHED_SCAN_REQUEST));

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(SCN, ERROR, "scheduled scan error:%x\n", rStatus);
		return -EINVAL;
	}

	prGlueInfo->prSchedScanRequest = request;

	return 0;
}

int mtk_cfg80211_sched_scan_stop(IN struct wiphy *wiphy, IN struct net_device *ndev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	/* check if there is any pending scan/sched_scan not yet finished */
	if (prGlueInfo->prSchedScanRequest == NULL) {
		DBGLOG(SCN, ERROR, "prGlueInfo->prSchedScanRequest == NULL\n");
		return -EBUSY;
	}

	rStatus = kalIoctl(prGlueInfo, wlanoidSetStopSchedScan, NULL, 0, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(SCN, ERROR, "scheduled scan error, rStatus: %d\n", rStatus);
		return -EINVAL;
	}

	/* 1. reset first for newly incoming request */
	/* GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV); */
	if (prGlueInfo->prSchedScanRequest != NULL)
		prGlueInfo->prSchedScanRequest = NULL;
	/* GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV); */

	DBGLOG(SCN, TRACE, "start work queue to send event\n");
	schedule_delayed_work(&sched_workq, 0);
	DBGLOG(SCN, TRACE, "tx_thread return from kalSchedScanStoppped\n");

	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for handling association request
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int mtk_cfg80211_assoc(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_assoc_request *req)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	PARAM_MAC_ADDRESS arBssid;
#if CFG_SUPPORT_HOTSPOT_2_0
	PUINT_8 prDesiredIE = NULL;
#endif
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	kalMemZero(arBssid, MAC_ADDR_LEN);
	wlanQueryInformation(prGlueInfo->prAdapter, wlanoidQueryBssid, &arBssid[0], sizeof(arBssid), &u4BufLen);

	/* 1. check BSSID */
	if (UNEQUAL_MAC_ADDR(arBssid, req->bss->bssid)) {
		/* wrong MAC address */
		DBGLOG(REQ, WARN, "incorrect BSSID: [ %pM ] currently connected BSSID[ %pM ]\n",
				   req->bss->bssid, arBssid);
		return -ENOENT;
	}

	if (req->ie && req->ie_len > 0) {
#if CFG_SUPPORT_HOTSPOT_2_0
		if (wextSrchDesiredHS20IE((PUINT_8) req->ie, req->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetHS20Info,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set HS20 assoc info error:%lx\n", rStatus); */
			}
		}

		if (wextSrchDesiredInterworkingIE((PUINT_8) req->ie, req->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetInterworkingInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set Interworking assoc info error:%lx\n", rStatus); */
			}
		}

		if (wextSrchDesiredRoamingConsortiumIE((PUINT_8) req->ie, req->ie_len, (PUINT_8 *) &prDesiredIE)) {
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetRoamingConsortiumIEInfo,
					   prDesiredIE, IE_SIZE(prDesiredIE), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			if (rStatus != WLAN_STATUS_SUCCESS) {
				/* Do nothing */
				/* printk(KERN_INFO "[HS20] set RoamingConsortium assoc info error:%lx\n", rStatus); */
			}
		}
#endif
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetBssid,
			   (PVOID) req->bss->bssid, MAC_ADDR_LEN, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, "set BSSID:%x\n", rStatus);
		return -EINVAL;
	}

	return 0;
}

#if CONFIG_NL80211_TESTMODE
/*

static inline test_nla_put(struct sk_buff *skb, int attrtype, int attrlen, const void *data)
{

}

#define NLA_PUT_TYPE(skb, type, attrtype, value) \
do { \
	type __tmp = value; \
	test_nla_put(skb, attrtype, sizeof(type), &__tmp); \
} while (0)

#define NLA_PUT_U8(skb, attrtype, value) \
	 NLA_PUT_TYPE(skb, u8, attrtype, value)

#define NLA_PUT_U16(skb, attrtype, value) \
	 NLA_PUT_TYPE(skb, u16, attrtype, value)

#define NLA_PUT_U32(skb, attrtype, value) \
	 NLA_PUT_TYPE(skb, u32, attrtype, value)

#define NLA_PUT_U64(skb, attrtype, value) \
	 NLA_PUT_TYPE(skb, u64, attrtype, value)
*/
#if CFG_SUPPORT_WAPI
int mtk_cfg80211_testmode_set_key_ext(IN struct wiphy *wiphy, IN void *data, IN int len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_NL80211_DRIVER_SET_KEY_EXTS prParams = (P_NL80211_DRIVER_SET_KEY_EXTS) NULL;
	struct iw_encode_exts *prIWEncExt = (struct iw_encode_exts *)NULL;
	WLAN_STATUS rstatus = WLAN_STATUS_SUCCESS;
	int fgIsValid = 0;
	UINT_32 u4BufLen = 0;

	P_PARAM_WPI_KEY_T prWpiKey = (P_PARAM_WPI_KEY_T) keyStructBuf;

	memset(keyStructBuf, 0, sizeof(keyStructBuf));

	ASSERT(wiphy);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	if (data && len) {
		prParams = (P_NL80211_DRIVER_SET_KEY_EXTS) data;
	} else {
		DBGLOG(REQ, ERROR, "mtk_cfg80211_testmode_set_key_ext, data is NULL\n");
		return -EINVAL;
	}

	if (prParams)
		prIWEncExt = (struct iw_encode_exts *)&prParams->ext;

	if (prIWEncExt->alg == IW_ENCODE_ALG_SMS4) {
		/* KeyID */
		prWpiKey->ucKeyID = prParams->key_index;
		prWpiKey->ucKeyID--;
		if (prWpiKey->ucKeyID > 1) {
			/* key id is out of range */
			/* printk(KERN_INFO "[wapi] add key error: key_id invalid %d\n", prWpiKey->ucKeyID); */
			return -EINVAL;
		}

		if (prIWEncExt->key_len != 32) {
			/* key length not valid */
			/* printk(KERN_INFO "[wapi] add key error: key_len invalid %d\n", prIWEncExt->key_len); */
			return -EINVAL;
		}
		/* printk(KERN_INFO "[wapi] %d ext_flags %d\n", prEnc->flags, prIWEncExt->ext_flags); */

		if (prIWEncExt->ext_flags & IW_ENCODE_EXT_GROUP_KEY) {
			prWpiKey->eKeyType = ENUM_WPI_GROUP_KEY;
			prWpiKey->eDirection = ENUM_WPI_RX;
		} else if (prIWEncExt->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			prWpiKey->eKeyType = ENUM_WPI_PAIRWISE_KEY;
			prWpiKey->eDirection = ENUM_WPI_RX_TX;
		}
/* #if CFG_SUPPORT_WAPI */
		/* handle_sec_msg_final(prIWEncExt->key, 32, prIWEncExt->key, NULL); */
/* #endif */
		/* PN */
		memcpy(prWpiKey->aucPN, prIWEncExt->tx_seq, IW_ENCODE_SEQ_MAX_SIZE);
		memcpy(prWpiKey->aucPN + IW_ENCODE_SEQ_MAX_SIZE, prIWEncExt->rx_seq, IW_ENCODE_SEQ_MAX_SIZE);


		/* BSSID */
		memcpy(prWpiKey->aucAddrIndex, prIWEncExt->addr, 6);

		memcpy(prWpiKey->aucWPIEK, prIWEncExt->key, 16);
		prWpiKey->u4LenWPIEK = 16;

		memcpy(prWpiKey->aucWPICK, &prIWEncExt->key[16], 16);
		prWpiKey->u4LenWPICK = 16;

		rstatus = kalIoctl(prGlueInfo,
				   wlanoidSetWapiKey,
				   prWpiKey, sizeof(PARAM_WPI_KEY_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

		if (rstatus != WLAN_STATUS_SUCCESS) {
			/* printk(KERN_INFO "[wapi] add key error:%lx\n", rStatus); */
			fgIsValid = -EFAULT;
		}

	}
	return fgIsValid;
}
#endif

int
mtk_cfg80211_testmode_get_sta_statistics(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	UINT_32 u4BufLen;
	UINT_32 u4LinkScore;
	UINT_32 u4TotalError;
	UINT_32 u4TxExceedThresholdCount;
	UINT_32 u4TxTotalCount;

	P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS prParams = NULL;
	PARAM_GET_STA_STA_STATISTICS rQueryStaStatistics;
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	if (data && len) {
		prParams = (P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS) data;
	} else {
		DBGLOG(QM, ERROR, "mtk_cfg80211_testmode_get_sta_statistics, data is NULL\n");
		return -EINVAL;
	}

	if (!prParams->aucMacAddr) {
		DBGLOG(QM, TRACE, "%s MAC Address is NULL\n", __func__);
		return -EINVAL;
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(PARAM_GET_STA_STA_STATISTICS) + 1);

	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:%x\n", __func__, rStatus);
		return -ENOMEM;
	}


	kalMemZero(&rQueryStaStatistics, sizeof(rQueryStaStatistics));
	COPY_MAC_ADDR(rQueryStaStatistics.aucMacAddr, prParams->aucMacAddr);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryStaStatistics,
			   &rQueryStaStatistics, sizeof(rQueryStaStatistics), TRUE, FALSE, TRUE, TRUE, &u4BufLen);

	/* Calcute Link Score */
	u4TxExceedThresholdCount = rQueryStaStatistics.u4TxExceedThresholdCount;
	u4TxTotalCount = rQueryStaStatistics.u4TxTotalCount;
	u4TotalError = rQueryStaStatistics.u4TxFailCount + rQueryStaStatistics.u4TxLifeTimeoutCount;

	/* u4LinkScore 10~100 , ExceedThreshold ratio 0~90 only */
	/* u4LinkScore 0~9    , Drop packet ratio 0~9 and all packets exceed threshold */
	if (u4TxTotalCount) {
		if (u4TxExceedThresholdCount <= u4TxTotalCount)
			u4LinkScore = (90 - ((u4TxExceedThresholdCount * 90) / u4TxTotalCount));
		else
			u4LinkScore = 0;
	} else {
		u4LinkScore = 90;
	}

	u4LinkScore += 10;

	if (u4LinkScore == 10) {

		if (u4TotalError <= u4TxTotalCount)
			u4LinkScore = (10 - ((u4TotalError * 10) / u4TxTotalCount));
		else
			u4LinkScore = 0;

	}

	if (u4LinkScore > 100)
		u4LinkScore = 100;

	rQueryStaStatistics.u4NetDevRxPkts = prGlueInfo->rNetDevStats.rx_packets;
	rQueryStaStatistics.u4NetDevRxBytes = prGlueInfo->rNetDevStats.rx_bytes;
	rQueryStaStatistics.u4NetDevTxPkts = prGlueInfo->rNetDevStats.tx_packets;
	rQueryStaStatistics.u4NetDevTxBytes = prGlueInfo->rNetDevStats.tx_bytes;

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_INVALID, 0);*/
	{
		unsigned char __tmp = 0;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_INVALID, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_VERSION, NL80211_DRIVER_TESTMODE_VERSION);*/
	{
		unsigned char __tmp = NL80211_DRIVER_TESTMODE_VERSION;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_VERSION, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT(skb, NL80211_TESTMODE_STA_STATISTICS_MAC, MAC_ADDR_LEN, prParams->aucMacAddr);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_MAC, MAC_ADDR_LEN, &prParams->aucMacAddr) < 0))
		goto nla_put_failure;

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_FLAG, rQueryStaStatistics.u4Flag);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4Flag;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_FLAG,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}


	/* FW part STA link status */
	/*NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_PER, rQueryStaStatistics.ucPer);*/
	{
		unsigned char __tmp = rQueryStaStatistics.ucPer;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_PER, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_RSSI, rQueryStaStatistics.ucRcpi);*/
	{
		unsigned char __tmp = rQueryStaStatistics.ucRcpi;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_RSSI, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_PHY_MODE, rQueryStaStatistics.u4PhyMode);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4PhyMode;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_PHY_MODE,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U16(skb, NL80211_TESTMODE_STA_STATISTICS_TX_RATE, rQueryStaStatistics.u2LinkSpeed);*/
	{
		unsigned short __tmp = rQueryStaStatistics.u2LinkSpeed;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TX_RATE,
			sizeof(unsigned short), &__tmp) < 0))
			goto nla_put_failure;
	}
	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_FAIL_CNT, rQueryStaStatistics.u4TxFailCount);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxFailCount;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_FAIL_CNT,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_TIMEOUT_CNT, rQueryStaStatistics.u4TxLifeTimeoutCount);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxLifeTimeoutCount;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TIMEOUT_CNT,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_AVG_AIR_TIME, rQueryStaStatistics.u4TxAverageAirTime);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxAverageAirTime;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_AVG_AIR_TIME,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/* Driver part link status */
	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_TOTAL_CNT, rQueryStaStatistics.u4TxTotalCount);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxTotalCount;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TOTAL_CNT,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_THRESHOLD_CNT,
		rQueryStaStatistics.u4TxExceedThresholdCount);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxExceedThresholdCount;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_THRESHOLD_CNT,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}
	/*NLA_PUT_U32(skb, NL80211_TESTMODE_STA_STATISTICS_AVG_PROCESS_TIME,
		rQueryStaStatistics.u4TxAverageProcessTime);*/
	{
		unsigned int __tmp = rQueryStaStatistics.u4TxAverageProcessTime;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_AVG_PROCESS_TIME,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	if (nla_put_u32(skb, NL80211_TESTMODE_STA_STATISTICS_TX_PKTS,
		       rQueryStaStatistics.u4NetDevTxPkts) ||
	    nla_put_u32(skb, NL80211_TESTMODE_STA_STATISTICS_TX_BYTES,
		       rQueryStaStatistics.u4NetDevTxBytes) ||
	    nla_put_u32(skb, NL80211_TESTMODE_STA_STATISTICS_RX_PKTS,
			rQueryStaStatistics.u4NetDevRxPkts) ||
	    nla_put_u32(skb, NL80211_TESTMODE_STA_STATISTICS_RX_BYTES,
			rQueryStaStatistics.u4NetDevRxBytes))
		goto nla_put_failure;

	/* Network counter */
	/*NLA_PUT(skb,
		NL80211_TESTMODE_STA_STATISTICS_TC_EMPTY_CNT_ARRAY,
		sizeof(rQueryStaStatistics.au4TcResourceEmptyCount), rQueryStaStatistics.au4TcResourceEmptyCount);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TC_EMPTY_CNT_ARRAY,
		sizeof(rQueryStaStatistics.au4TcResourceEmptyCount), &rQueryStaStatistics.au4TcResourceEmptyCount) < 0))
		goto nla_put_failure;


	/* Sta queue length */
	/*NLA_PUT(skb,
		NL80211_TESTMODE_STA_STATISTICS_TC_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcQueLen), rQueryStaStatistics.au4TcQueLen);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TC_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcQueLen), &rQueryStaStatistics.au4TcQueLen) < 0))
		goto nla_put_failure;


	/* Global QM counter */
	/*NLA_PUT(skb,
		NL80211_TESTMODE_STA_STATISTICS_TC_AVG_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcAverageQueLen), rQueryStaStatistics.au4TcAverageQueLen);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TC_AVG_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcAverageQueLen), &rQueryStaStatistics.au4TcAverageQueLen) < 0))
		goto nla_put_failure;

	/*NLA_PUT(skb,
		NL80211_TESTMODE_STA_STATISTICS_TC_CUR_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcCurrentQueLen), rQueryStaStatistics.au4TcCurrentQueLen);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_TC_CUR_QUE_LEN_ARRAY,
		sizeof(rQueryStaStatistics.au4TcCurrentQueLen), &rQueryStaStatistics.au4TcCurrentQueLen) < 0))
		goto nla_put_failure;


	/* Reserved field */
	/*NLA_PUT(skb,
		NL80211_TESTMODE_STA_STATISTICS_RESERVED_ARRAY,
		sizeof(rQueryStaStatistics.au4Reserved), rQueryStaStatistics.au4Reserved);*/
	if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_RESERVED_ARRAY,
		sizeof(rQueryStaStatistics.au4Reserved), &rQueryStaStatistics.au4Reserved) < 0))
		goto nla_put_failure;


	i4Status = cfg80211_testmode_reply(skb);

nla_put_failure:
	return i4Status;
}

int
mtk_cfg80211_testmode_get_link_detection(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo)
{

	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	UINT_32 u4BufLen;

	PARAM_802_11_STATISTICS_STRUCT_T rStatistics;
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(PARAM_GET_STA_STA_STATISTICS) + 1);

	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:%x\n", __func__, rStatus);
		return -ENOMEM;
	}

	kalMemZero(&rStatistics, sizeof(rStatistics));

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryStatistics,
			   &rStatistics, sizeof(rStatistics), TRUE, TRUE, TRUE, FALSE, &u4BufLen);

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_INVALID, 0);*/
	{
		unsigned char __tmp = 0;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_STA_STATISTICS_INVALID, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U64(skb, NL80211_TESTMODE_LINK_TX_FAIL_CNT, rStatistics.rFailedCount.QuadPart);*/
	{
		u64 __tmp = rStatistics.rFailedCount.QuadPart;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_LINK_TX_FAIL_CNT,
			sizeof(u64), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U64(skb, NL80211_TESTMODE_LINK_TX_RETRY_CNT, rStatistics.rRetryCount.QuadPart);*/
	{
		u64 __tmp = rStatistics.rFailedCount.QuadPart;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_LINK_TX_RETRY_CNT,
			sizeof(u64), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U64(skb, NL80211_TESTMODE_LINK_TX_MULTI_RETRY_CNT, rStatistics.rMultipleRetryCount.QuadPart);*/
	{
		u64 __tmp = rStatistics.rMultipleRetryCount.QuadPart;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_LINK_TX_MULTI_RETRY_CNT,
			sizeof(u64), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U64(skb, NL80211_TESTMODE_LINK_ACK_FAIL_CNT, rStatistics.rACKFailureCount.QuadPart);*/
	{
		u64 __tmp = rStatistics.rACKFailureCount.QuadPart;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_LINK_ACK_FAIL_CNT,
			sizeof(u64), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U64(skb, NL80211_TESTMODE_LINK_FCS_ERR_CNT, rStatistics.rFCSErrorCount.QuadPart);*/
	{
		u64 __tmp = rStatistics.rFCSErrorCount.QuadPart;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_LINK_FCS_ERR_CNT,
			sizeof(u64), &__tmp) < 0))
			goto nla_put_failure;
	}


	i4Status = cfg80211_testmode_reply(skb);

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_testmode_sw_cmd(IN struct wiphy *wiphy, IN void *data, IN int len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_NL80211_DRIVER_SW_CMD_PARAMS prParams = (P_NL80211_DRIVER_SW_CMD_PARAMS) NULL;
	WLAN_STATUS rstatus = WLAN_STATUS_SUCCESS;
	int fgIsValid = 0;
	UINT_32 u4SetInfoLen = 0;

	ASSERT(wiphy);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	if (data && len)
		prParams = (P_NL80211_DRIVER_SW_CMD_PARAMS) data;

	if (prParams) {
		if (prParams->set == 1) {
			rstatus = kalIoctl(prGlueInfo,
					   (PFN_OID_HANDLER_FUNC) wlanoidSetSwCtrlWrite,
					   &prParams->adr, (UINT_32) 8, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);
		}
	}

	if (WLAN_STATUS_SUCCESS != rstatus)
		fgIsValid = -EFAULT;

	return fgIsValid;
}

#if CFG_SUPPORT_HOTSPOT_2_0
int mtk_cfg80211_testmode_hs20_cmd(IN struct wiphy *wiphy, IN void *data, IN int len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct wpa_driver_hs20_data_s *prParams = NULL;
	WLAN_STATUS rstatus = WLAN_STATUS_SUCCESS;
	int fgIsValid = 0;
	UINT_32 u4SetInfoLen = 0;

	ASSERT(wiphy);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	DBGLOG(REQ, TRACE, "--> %s()\n", __func__);

	if (data && len)
		prParams = (struct wpa_driver_hs20_data_s *)data;

	DBGLOG(REQ, TRACE, "[%s] Cmd Type (%d)\n", __func__, prParams->CmdType);

	if (prParams) {
		int i;

		switch (prParams->CmdType) {
		case HS20_CMD_ID_SET_BSSID_POOL:
			DBGLOG(REQ, TRACE, "fgBssidPoolIsEnable=%d, ucNumBssidPool=%d\n",
				prParams->hs20_set_bssid_pool.fgBssidPoolIsEnable,
				prParams->hs20_set_bssid_pool.ucNumBssidPool);
			for (i = 0; i < prParams->hs20_set_bssid_pool.ucNumBssidPool; i++) {
				DBGLOG(REQ, TRACE, "[%d][ %pM ]\n", i,
					(prParams->hs20_set_bssid_pool.arBssidPool[i]));
			}
			rstatus = kalIoctl(prGlueInfo,
					(PFN_OID_HANDLER_FUNC) wlanoidSetHS20BssidPool,
					&prParams->hs20_set_bssid_pool,
					sizeof(struct param_hs20_set_bssid_pool),
					FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);
			break;
		default:
			DBGLOG(REQ, TRACE, "[%s] Unknown Cmd Type (%d)\n", __func__, prParams->CmdType);
			rstatus = WLAN_STATUS_FAILURE;

		}

	}

	if (WLAN_STATUS_SUCCESS != rstatus)
		fgIsValid = -EFAULT;

	return fgIsValid;
}

#endif
int
mtk_cfg80211_testmode_set_poorlink_param(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo)
{
	int fgIsValid = 0;
	P_NL80211_DRIVER_POORLINK_PARAMS prParams = NULL;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	if (data && len) {
		prParams = (P_NL80211_DRIVER_POORLINK_PARAMS) data;
	} else {
		DBGLOG(REQ, ERROR, "mtk_cfg80211_testmode_set_poorlink_param, data is NULL\n");
		return -EINVAL;
	}
	if (prParams->ucLinkSpeed)
		prGlueInfo->u4LinkspeedThreshold = prParams->ucLinkSpeed * 10;
	if (prParams->cRssi)
		prGlueInfo->i4RssiThreshold = prParams->cRssi;
	if (!prGlueInfo->fgPoorlinkValid)
		prGlueInfo->fgPoorlinkValid = 1;
#if 0
	DBGLOG(REQ, TRACE, "poorlink set param valid(%d)rssi(%d)linkspeed(%d)\n",
	       prGlueInfo->fgPoorlinkValid, prGlueInfo->i4RssiThreshold, prGlueInfo->u4LinkspeedThreshold);
#endif

	return fgIsValid;

}


int mtk_cfg80211_testmode_rx_filter(IN struct wiphy *wiphy, IN void *data, IN int len,
											IN P_GLUE_INFO_T prGlueInfo)
{
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)*/
#define NLA_PUT(skb, attrtype, attrlen, data) \
		do { \
			if (unlikely(nla_put(skb, attrtype, attrlen, data) < 0)) \
				pr_info("NLA PUT Error!!!!\n"); \
		} while (0)

#define NLA_PUT_TYPE(skb, type, attrtype, value) \
		do { \
			type __tmp = value; \
			NLA_PUT(skb, attrtype, sizeof(type), &__tmp); \
		} while (0)

#define NLA_PUT_U8(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u8, attrtype, value)

#define NLA_PUT_U16(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u16, attrtype, value)

#define NLA_PUT_U32(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u32, attrtype, value)

/*#endif*/
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	struct NL80211_DRIVER_RX_FILTER_PARAMS *prParams = (struct NL80211_DRIVER_RX_FILTER_PARAMS *) NULL;
	static struct PARAM_DRIVER_FW_RX_FILTER rQueryRxFilter;
	UINT_8 success = 0;
	struct sk_buff *skb;
	UINT_32 u4BufLen;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

#if 1
	pr_info("--> %s()\n", __func__);
#endif

	if (data && len)
		prParams = (struct NL80211_DRIVER_RX_FILTER_PARAMS *) data;

	if (prParams) { /*print wpa_cli cmd parameter*/
		pr_info("rx_filter cmd=%d idx=%u IsWhiteList=%d IsEqual=%d Offset=%u Len=%u\r\n",
			prParams->eOpcode, prParams->ucPatternIndex, prParams->fsIsWhiteList, prParams->fgIsEqual,
			prParams->u4Offset, prParams->u4Length);
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(success) + 1);
	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:\n", __func__);
		i4Status = -ENOMEM;
		goto nla_put_failure;
	}

	kalMemZero(&rQueryRxFilter, sizeof(rQueryRxFilter));

	rQueryRxFilter.eOpcode = prParams->eOpcode;
	rQueryRxFilter.ucPatternIndex = prParams->ucPatternIndex;
	kalMemCopy(rQueryRxFilter.aucPattern, prParams->aucPattern, 64);
	kalMemCopy(rQueryRxFilter.aucPatternBitMask, prParams->aucPatternBitMask, 64);
	rQueryRxFilter.u4Offset = prParams->u4Offset;
	rQueryRxFilter.u4Length = prParams->u4Length;
	rQueryRxFilter.fsIsWhiteList = prParams->fsIsWhiteList;
	rQueryRxFilter.fgIsEqual = prParams->fgIsEqual;

	rStatus = kalIoctl(prGlueInfo,
				wlanoidSetRxFilter,
				&rQueryRxFilter,
				sizeof(rQueryRxFilter), FALSE, TRUE, TRUE, FALSE, &u4BufLen);
	if (rStatus == WLAN_STATUS_SUCCESS)
		success = 1;

	NLA_PUT_U8(skb, NL80211_TESTMODE_RX_FILTER_INVALID, 0);
	NLA_PUT_U8(skb, NL80211_TESTMODE_RX_FILTER_RETURN, success);

	i4Status = cfg80211_testmode_reply(skb);

	if (WLAN_STATUS_SUCCESS != rStatus)
		i4Status = -EFAULT;

	if (i4Status != 0) /*in case supplicant hang up*/
		pr_info("rx_filter failed, rStatus=%x, i4Status=%d", rStatus, i4Status);

nla_put_failure:
	return 0;
}

int mtk_cfg80211_testmode_tx_heartbeat(IN struct wiphy *wiphy, IN void *data, IN int len,
											IN P_GLUE_INFO_T prGlueInfo)
{
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)*/
#define NLA_PUT(skb, attrtype, attrlen, data) \
		do { \
			if (unlikely(nla_put(skb, attrtype, attrlen, data) < 0)) \
				pr_info("NLA PUT Error!!!!\n"); \
		 } while (0)

#define NLA_PUT_TYPE(skb, type, attrtype, value) \
		do { \
			type __tmp = value; \
			NLA_PUT(skb, attrtype, sizeof(type), &__tmp); \
		} while (0)

#define NLA_PUT_U8(skb, attrtype, value) \
		 NLA_PUT_TYPE(skb, u8, attrtype, value)

#define NLA_PUT_U16(skb, attrtype, value) \
		 NLA_PUT_TYPE(skb, u16, attrtype, value)

#define NLA_PUT_U32(skb, attrtype, value) \
		 NLA_PUT_TYPE(skb, u32, attrtype, value)

/*#endif*/
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	struct NL80211_DRIVER_TX_GENERATOR_PARAMS *prParams = (struct NL80211_DRIVER_TX_GENERATOR_PARAMS *) NULL;
	static struct PARAM_DRIVER_FW_TX_GENERATOR rQueryTxGenerator;
	UINT_8 success = 0;
	struct sk_buff *skb;
	UINT_32 u4BufLen;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

#if 1
	pr_info("--> %s()\n", __func__);
#endif

	if (data && len)
		prParams = (struct NL80211_DRIVER_TX_GENERATOR_PARAMS *) data;

	if (prParams) { /*print wpa_cli cmd parameter*/
		pr_info("tx_heartbeat cmd=%d id=%u u2EthernetLength=%u u4Period=%u\r\n",
			prParams->eOpcode, prParams->ucPatternIndex, prParams->u2EthernetLength, prParams->u4Period);
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(success) + 1);
	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:\n", __func__);
		i4Status = -ENOMEM;
		goto nla_put_failure;
	}

	kalMemZero(&rQueryTxGenerator, sizeof(rQueryTxGenerator));

	rQueryTxGenerator.eOpcode = prParams->eOpcode;
	rQueryTxGenerator.ucPatternIndex = prParams->ucPatternIndex;
	kalMemCopy(rQueryTxGenerator.aucEthernetFrame, prParams->aucEthernetFrame, ETHER_MAX_PKT_SIZE);
	rQueryTxGenerator.u2EthernetLength = prParams->u2EthernetLength;
	rQueryTxGenerator.u4Period = prParams->u4Period;

	rStatus = kalIoctl(prGlueInfo,
				wlanoidSetTxHeartbeat,
				&rQueryTxGenerator,
				sizeof(rQueryTxGenerator), FALSE, TRUE, TRUE, FALSE, &u4BufLen);
	if (rStatus == WLAN_STATUS_SUCCESS)
		success = 1;

	NLA_PUT_U8(skb, NL80211_TESTMODE_TX_GENERATOR_INVALID, 0);
	NLA_PUT_U8(skb, NL80211_TESTMODE_TX_GENERATOR_RETURN, success);

	i4Status = cfg80211_testmode_reply(skb);

	if (WLAN_STATUS_SUCCESS != rStatus)
		i4Status = -EFAULT;

	if (i4Status != 0) /*not return i4Status in case supplicant hang up*/
		pr_info("tx_heartbeat failed, rStatus=%x, i4Status=%d", rStatus, i4Status);

nla_put_failure:
	return 0;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief Handle EVENT_ID_HEARTBEAT_INFO event packet by indicating to OS with
*        proper information
*
* @param pvGlueInfo           Pointer of GLUE Data Structure
* @param prHeartbeatInfo    Pointer of EVENT_ID_HEARTBEAT_INFO Packet
*
* @return none
*/
/*----------------------------------------------------------------------------*/
int kalHandleTcpHeartbeatTimeout(IN P_GLUE_INFO_T prGlueInfo, IN struct EVENT_HEARTBEAT_INFO_T *prHeartbeatInfo)
{
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)*/
#define NLA_PUT(skb, attrtype, attrlen, data) \
			do { \
				if (unlikely(nla_put(skb, attrtype, attrlen, data) < 0)) \
					pr_info("NLA PUT Error!!!!\n"); \
			} while (0)

#define NLA_PUT_TYPE(skb, type, attrtype, value) \
			do { \
				type __tmp = value; \
				NLA_PUT(skb, attrtype, sizeof(type), &__tmp); \
			} while (0)

#define NLA_PUT_U8(skb, attrtype, value) \
			NLA_PUT_TYPE(skb, u8, attrtype, value)

#define NLA_PUT_U16(skb, attrtype, value) \
			NLA_PUT_TYPE(skb, u16, attrtype, value)

#define NLA_PUT_U32(skb, attrtype, value) \
			NLA_PUT_TYPE(skb, u32, attrtype, value)

/*#endif*/
	INT_32 i4Status = -EINVAL;
	struct wiphy *wiphy;
	struct EVENT_HEARTBEAT_INFO_T rHeartbeatInfo;
	struct sk_buff *skb;

	ASSERT(prGlueInfo);
	ASSERT(prHeartbeatInfo);
	wiphy = priv_to_wiphy(prGlueInfo);

	skb = cfg80211_testmode_alloc_event_skb(wiphy, sizeof(rHeartbeatInfo) + 1, GFP_KERNEL);
	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:\n", __func__);
		i4Status = -ENOMEM;
		goto nla_put_failure;
	}

	kalMemCopy(&rHeartbeatInfo, prHeartbeatInfo, sizeof(rHeartbeatInfo));

	NLA_PUT_U8(skb, NL80211_TESTMODE_TCP_HEARTBEAT_INVALID, 0);
	NLA_PUT(skb,
		NL80211_TESTMODE_TCP_HEARTBEAT_TIMEOUT,
		sizeof(rHeartbeatInfo), &rHeartbeatInfo);

	cfg80211_testmode_event(skb, GFP_KERNEL);

nla_put_failure:
	return 0;
}

/* test tcp heartbeat timeout processing
VOID mtk_cfg80211_testmode_tcp_timeout(IN P_ADAPTER_T prAdapter, UINT_32 u4Param)
{
	EVENT_HEARTBEAT_INFO_T rHeartbeat;

	kalMemZero(&rHeartbeat, sizeof(rHeartbeat));
	rHeartbeat.ucTimeoutSessionID = 15;

	pr_info("tcp_heartbeat timeout ucTimeoutSessionID=%u, current time=%u.\n",
		rHeartbeat.ucTimeoutSessionID, kalGetTimeTick());

	kalHandleTcpHeartbeatTimeout(prAdapter->prGlueInfo, &rHeartbeat);
}
*/

int mtk_cfg80211_testmode_tcp_heartbeat(IN struct wiphy *wiphy, IN void *data, IN int len,
											IN P_GLUE_INFO_T prGlueInfo)
{
/*#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)*/
#define NLA_PUT(skb, attrtype, attrlen, data) \
		do { \
			if (unlikely(nla_put(skb, attrtype, attrlen, data) < 0)) \
				pr_info("NLA PUT Error!!!!\n"); \
		} while (0)

#define NLA_PUT_TYPE(skb, type, attrtype, value) \
		do { \
			type __tmp = value; \
			NLA_PUT(skb, attrtype, sizeof(type), &__tmp); \
		} while (0)

#define NLA_PUT_U8(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u8, attrtype, value)

#define NLA_PUT_U16(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u16, attrtype, value)

#define NLA_PUT_U32(skb, attrtype, value) \
		NLA_PUT_TYPE(skb, u32, attrtype, value)

/*#endif*/
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	struct NL80211_DRIVER_TCP_GENERATOR_PARAMS *prParams = (struct NL80211_DRIVER_TCP_GENERATOR_PARAMS *) NULL;
	static struct PARAM_DRIVER_FW_TCP_GENERATOR rQueryTcpGenerator;
	UINT_8 success = 0;
	struct sk_buff *skb;
	UINT_32 u4BufLen;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

#if 1
	pr_info("--> %s()\n", __func__);
#endif

	if (data && len)
		prParams = (struct NL80211_DRIVER_TCP_GENERATOR_PARAMS *) data;

	if (prParams) { /*print wpa_cli cmd parameter*/
		pr_info("tcp_heartbeat cmd=%d id=%u Period=%u Tout=%u ipId=%u Port=%u %u Seq=%u\n",
			prParams->eOpcode, prParams->ucPatternIndex, prParams->u4Period, prParams->u4Timeout,
			prParams->ipId, prParams->u4SrcPort, prParams->u4DestPort, prParams->u4Seq);

		pr_info("MAC=%02x:%02x:%02x:%02x:%02x:%02x srcIP=%d.%d.%d.%d dstIP=%d.%d.%d.%d\n",
			prParams->aucDestMac[0], prParams->aucDestMac[1], prParams->aucDestMac[2],
			prParams->aucDestMac[3], prParams->aucDestMac[4], prParams->aucDestMac[5],
			prParams->aucSrcIp[0], prParams->aucSrcIp[1], prParams->aucSrcIp[2], prParams->aucSrcIp[3],
			prParams->aucDestIp[0], prParams->aucDestIp[1], prParams->aucDestIp[2], prParams->aucDestIp[3]);
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(success) + 1);
	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:\n", __func__);
		i4Status = -ENOMEM;
		goto nla_put_failure;
	}

	kalMemZero(&rQueryTcpGenerator, sizeof(rQueryTcpGenerator));

	rQueryTcpGenerator.eOpcode = prParams->eOpcode;
	rQueryTcpGenerator.ucPatternIndex = prParams->ucPatternIndex;
	rQueryTcpGenerator.u4Period = prParams->u4Period;
	rQueryTcpGenerator.u4Timeout = prParams->u4Timeout;
	kalMemCopy(rQueryTcpGenerator.aucDestMac, prParams->aucDestMac, 6);
	kalMemCopy(rQueryTcpGenerator.aucDestIp, prParams->aucDestIp, 4);
	kalMemCopy(rQueryTcpGenerator.aucSrcIp, prParams->aucSrcIp, 4);
	rQueryTcpGenerator.ipId = prParams->ipId;
	rQueryTcpGenerator.u4DestPort = prParams->u4DestPort;
	rQueryTcpGenerator.u4SrcPort = prParams->u4SrcPort;
	rQueryTcpGenerator.u4Seq = prParams->u4Seq;

	rStatus = kalIoctl(prGlueInfo,
				wlanoidSetTcpHeartbeat,
				&rQueryTcpGenerator,
				sizeof(rQueryTcpGenerator), FALSE, TRUE, TRUE, FALSE, &u4BufLen);
	if (rStatus == WLAN_STATUS_SUCCESS)
		success = 1;

	NLA_PUT_U8(skb, NL80211_TESTMODE_TCP_GENERATOR_INVALID, 0);
	NLA_PUT_U8(skb, NL80211_TESTMODE_TCP_GENERATOR_RETURN, success);

	i4Status = cfg80211_testmode_reply(skb);

	if (WLAN_STATUS_SUCCESS != rStatus)
		i4Status = -EFAULT;

/*test tcp heartbeat timeout processing
	cnmTimerInitTimer(prGlueInfo->prAdapter, &prGlueInfo->prAdapter->rTcpTimeoutTimer,
		(PFN_MGMT_TIMEOUT_FUNC)mtk_cfg80211_testmode_tcp_timeout, (UINT_32) NULL);

	cnmTimerStartTimer(prGlueInfo->prAdapter,
						&prGlueInfo->prAdapter->rTcpTimeoutTimer,
						SEC_TO_MSEC(10));
	pr_info("tcp_heartbeat timeout current time=%u\n", kalGetTimeTick());
*/
	/*cnmTimerStopTimer(prGlueInfo->prAdapter, &tcp_timer);*/

	if (i4Status != 0) /*in case supplicant hang up*/
		pr_info("tcp_heartbeat failed, rStatus=%x, i4Status=%d", rStatus, i4Status);

nla_put_failure:
	return 0;
}

int mtk_cfg80211_testmode_country_get(IN struct wiphy *wiphy, IN void *data, IN int len,
										IN P_GLUE_INFO_T prGlueInfo)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL;
	UINT_32 u4BufLen = 0;
	UINT_8 aucCountry[2];

	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo, wlanoidGetCountryCode,
		&aucCountry[0], 2, TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(INIT, INFO, "failed get country code");
		return -EINVAL;
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(PARAM_GET_STA_STA_STATISTICS) + 1);

	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:%x\n", __func__, rStatus);
		return -ENOMEM;
	}

	{
		unsigned char __tmp = 0;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_COUNTRY_GET_INVALID, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	{
		u8 __tmp = aucCountry[1];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_COUNTRY_GET_COUNTRY0,
			sizeof(u8), &__tmp) < 0))
			goto nla_put_failure;
	}

	{
		u8 __tmp = aucCountry[0];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_COUNTRY_GET_COUNTRY1,
			sizeof(u8), &__tmp) < 0))
			goto nla_put_failure;
	}

	i4Status = cfg80211_testmode_reply(skb);

nla_put_failure:
	return i4Status;

}

int mtk_cfg80211_testmode_cmd(IN struct wiphy *wiphy, IN struct wireless_dev *wdev, IN void *data, IN int len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_NL80211_DRIVER_TEST_MODE_PARAMS prParams = (P_NL80211_DRIVER_TEST_MODE_PARAMS) NULL;
	INT_32 i4Status = -EINVAL;
#if CFG_SUPPORT_HOTSPOT_2_0
	BOOLEAN fgIsValid = 0;
#endif

	ASSERT(wiphy);
	ASSERT(wdev);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	if (data && len) {
		prParams = (P_NL80211_DRIVER_TEST_MODE_PARAMS) data;
	} else {
		DBGLOG(REQ, ERROR, "mtk_cfg80211_testmode_cmd, data is NULL\n");
		return i4Status;
	}

	/* Clear the version byte */
	prParams->index = prParams->index & ~BITS(24, 31);

	if (prParams) {
		switch (prParams->index) {
		case TESTMODE_CMD_ID_SW_CMD:	/* SW cmd */
			i4Status = mtk_cfg80211_testmode_sw_cmd(wiphy, data, len);
			break;
		case TESTMODE_CMD_ID_WAPI:	/* WAPI */
#if CFG_SUPPORT_WAPI
			i4Status = mtk_cfg80211_testmode_set_key_ext(wiphy, data, len);
#endif
			break;
		case TESTMODE_CMD_ID_SUSPEND:
			{
				P_NL80211_DRIVER_SUSPEND_PARAMS prParams = (P_NL80211_DRIVER_SUSPEND_PARAMS) data;

				if (prParams->suspend == 1) {
					wlanHandleSystemSuspend();
					if (prGlueInfo->prAdapter->fgIsP2PRegistered)
						p2pHandleSystemSuspend();
					i4Status = 0;
				} else if (prParams->suspend == 0) {
					wlanHandleSystemResume();
					if (prGlueInfo->prAdapter->fgIsP2PRegistered)
						p2pHandleSystemResume();
					i4Status = 0;
				}
				break;
			}
		case TESTMODE_CMD_ID_STATISTICS:
			i4Status = mtk_cfg80211_testmode_get_sta_statistics(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_LINK_DETECT:
			i4Status = mtk_cfg80211_testmode_get_link_detection(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_POORLINK:
			i4Status = mtk_cfg80211_testmode_set_poorlink_param(wiphy, data, len, prGlueInfo);
			break;

#if CFG_SUPPORT_HOTSPOT_2_0
		case TESTMODE_CMD_ID_HS20:
			if (mtk_cfg80211_testmode_hs20_cmd(wiphy, data, len))
				fgIsValid = TRUE;
			break;
#endif
#if CFG_SUPPORT_WAKEUP_STATISTICS
		case TESTMODE_CMD_ID_WAKEUP_STATISTICS:
		{
			P_NL80211_QUERY_WAKEUP_STATISTICS prWakeupSta =
				(P_NL80211_QUERY_WAKEUP_STATISTICS)data;
			if (copy_to_user((PUINT_8)prWakeupSta->prWakeupCount,
				(PUINT_8)prGlueInfo->prAdapter->arWakeupStatistic,
				sizeof(prGlueInfo->prAdapter->arWakeupStatistic)))
				DBGLOG(INIT, ERROR, "copy wakeup statistics fail\n");

			i4Status = 0;
			break;
		}
#endif
		case TESTMODE_CMD_ID_RX_FILTER:
			i4Status = mtk_cfg80211_testmode_rx_filter(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_TX_HEARTBEAT:
			i4Status = mtk_cfg80211_testmode_tx_heartbeat(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_TCP_HEARTBEAT:
			i4Status = mtk_cfg80211_testmode_tcp_heartbeat(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_COUNTRY_GET:
			i4Status = mtk_cfg80211_testmode_country_get(wiphy, data, len, prGlueInfo);
			break;
		case TESTMODE_CMD_ID_STR_CMD:
			i4Status = mtk_cfg80211_process_str_cmd(prGlueInfo,
					(PUINT_8)(prParams+1), len - sizeof(*prParams));
			break;

#if CFG_SUPPORT_RSSI_STATISTICS
		case TESTMODE_RSSI_STATISTICS:
			i4Status = mtk_cfg80211_testmode_get_rssi_statistics(wiphy,
					data, len, prGlueInfo);
			break;
#endif
		default:
			i4Status = -EINVAL;
			break;
		}
	}

	return i4Status;
}

int mtk_cfg80211_testmode_get_scan_done(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo)
{
#define NL80211_TESTMODE_P2P_SCANDONE_INVALID 0
#define NL80211_TESTMODE_P2P_SCANDONE_STATUS 1
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	INT_32 i4Status = -EINVAL, READY_TO_BEAM = 0;

/* P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS prParams = NULL; */
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(UINT_32));
	READY_TO_BEAM =
	    (UINT_32) (prGlueInfo->prAdapter->rWifiVar.prP2pFsmInfo->rScanReqInfo.
		       fgIsGOInitialDone) &
	    (!prGlueInfo->prAdapter->rWifiVar.prP2pFsmInfo->rScanReqInfo.fgIsScanRequest);
	DBGLOG(QM, TRACE,
	       "NFC:GOInitialDone[%d] and P2PScanning[%d]\n",
		prGlueInfo->prAdapter->rWifiVar.prP2pFsmInfo->rScanReqInfo.fgIsGOInitialDone,
		prGlueInfo->prAdapter->rWifiVar.prP2pFsmInfo->rScanReqInfo.fgIsScanRequest);

	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:%x\n", __func__, rStatus);
		return -ENOMEM;
	}

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_P2P_SCANDONE_INVALID, 0);*/
	{
		unsigned char __tmp = 0;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_P2P_SCANDONE_INVALID, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}
	/*NLA_PUT_U32(skb, NL80211_TESTMODE_P2P_SCANDONE_STATUS, READY_TO_BEAM);*/
	{
		unsigned int __tmp = READY_TO_BEAM;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_P2P_SCANDONE_STATUS, sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}


	i4Status = cfg80211_testmode_reply(skb);

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_get_capabilities(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 i4Status = -EINVAL;
	PARAM_WIFI_GSCAN_CAPABILITIES_STRUCT_T rGscanCapabilities;
	struct sk_buff *skb;
	/* UINT_32 u4BufLen; */

	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

	ASSERT(wiphy);
	ASSERT(wdev);
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(rGscanCapabilities));
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed:%x\n", __func__, i4Status);
		return -ENOMEM;
	}

	kalMemZero(&rGscanCapabilities, sizeof(rGscanCapabilities));

	/*rStatus = kalIoctl(prGlueInfo,
	   wlanoidQueryStatistics,
	   &rGscanCapabilities,
	   sizeof(rGscanCapabilities),
	   TRUE,
	   TRUE,
	   TRUE,
	   FALSE,
	   &u4BufLen); */
	rGscanCapabilities.max_scan_cache_size = PSCAN_MAX_SCAN_CACHE_SIZE;
	rGscanCapabilities.max_scan_buckets = GSCAN_MAX_BUCKETS;
	rGscanCapabilities.max_ap_cache_per_scan = PSCAN_MAX_AP_CACHE_PER_SCAN;
	rGscanCapabilities.max_rssi_sample_size = 10;
	rGscanCapabilities.max_scan_reporting_threshold = GSCAN_MAX_REPORT_THRESHOLD;
	rGscanCapabilities.max_hotlist_aps = MAX_HOTLIST_APS;
	rGscanCapabilities.max_significant_wifi_change_aps = MAX_SIGNIFICANT_CHANGE_APS;
	rGscanCapabilities.max_bssid_history_entries = PSCAN_MAX_AP_CACHE_PER_SCAN * PSCAN_MAX_SCAN_CACHE_SIZE;

	/* NLA_PUT_U8(skb, NL80211_TESTMODE_STA_STATISTICS_INVALID, 0); */
	/* NLA_PUT_U32(skb, NL80211_ATTR_VENDOR_ID, GOOGLE_OUI); */
	/* NLA_PUT_U32(skb, NL80211_ATTR_VENDOR_SUBCMD, GSCAN_SUBCMD_GET_CAPABILITIES); */
	/*NLA_PUT(skb, NL80211_ATTR_VENDOR_CAPABILITIES, sizeof(rGscanCapabilities), &rGscanCapabilities);*/
	if (unlikely(nla_put(skb, NL80211_ATTR_VENDOR_CAPABILITIES,
		sizeof(rGscanCapabilities), &rGscanCapabilities) < 0))
		goto nla_put_failure;


	i4Status = cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_set_config(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4ArySize;
	/* CMD_GSCN_REQ_T rCmdGscnParam; */

	/* INT_32 i4Status = -EINVAL; */
	P_PARAM_WIFI_GSCAN_CMD_PARAMS prWifiScanCmd;
	struct nlattr *attr[GSCAN_ATTRIBUTE_REPORT_EVENTS + 1];
	struct nlattr *pbucket, *pchannel;
	UINT_32 len_basic, len_bucket, len_channel;
	int i, j, k;
	static struct nla_policy policy[GSCAN_ATTRIBUTE_REPORT_EVENTS + 1] = {
		[GSCAN_ATTRIBUTE_NUM_BUCKETS] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BASE_PERIOD] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BUCKETS_BAND] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BUCKET_ID] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BUCKET_PERIOD] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BUCKET_NUM_CHANNELS] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_BUCKET_CHANNELS] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_REPORT_EVENTS] = {.type = NLA_U32},
	};

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;

	prWifiScanCmd = (P_PARAM_WIFI_GSCAN_CMD_PARAMS) kalMemAlloc(sizeof(PARAM_WIFI_GSCAN_CMD_PARAMS), VIR_MEM_TYPE);
	if (!prWifiScanCmd) {
		DBGLOG(REQ, ERROR, "Can not alloc memory for PARAM_WIFI_GSCAN_CMD_PARAMS\n");
		return -ENOMEM;
	}

	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d \r\n", __func__, data_len);
	kalMemZero(prWifiScanCmd, sizeof(PARAM_WIFI_GSCAN_CMD_PARAMS));
	kalMemZero(attr, sizeof(struct nlattr *) * (GSCAN_ATTRIBUTE_REPORT_EVENTS + 1));

	nla_parse_nested(attr, GSCAN_ATTRIBUTE_REPORT_EVENTS, (struct nlattr *)(data - NLA_HDRLEN), policy);
	len_basic = 0;
	for (k = GSCAN_ATTRIBUTE_NUM_BUCKETS; k <= GSCAN_ATTRIBUTE_REPORT_EVENTS; k++) {
		if (attr[k]) {
			switch (k) {
			case GSCAN_ATTRIBUTE_BASE_PERIOD:
				prWifiScanCmd->base_period = nla_get_u32(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_NUM_BUCKETS:
				u4ArySize = nla_get_u32(attr[k]);
				prWifiScanCmd->num_buckets =
					(u4ArySize <= GSCAN_MAX_BUCKETS)
					? u4ArySize : GSCAN_MAX_BUCKETS;
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				DBGLOG(REQ, TRACE, "attr=0x%x, num_buckets=%d nla_len=%d, \r\n",
				       *(UINT_32 *) attr[k], prWifiScanCmd->num_buckets, attr[k]->nla_len);
				break;
			}
		}
	}
	pbucket = (struct nlattr *)((UINT_8 *) data + len_basic);
	DBGLOG(REQ, TRACE, "+++basic attribute size=%d pbucket=%p\r\n", len_basic, pbucket);

	for (i = 0; i < prWifiScanCmd->num_buckets; i++) {
		nla_parse_nested(attr, GSCAN_ATTRIBUTE_REPORT_EVENTS, (struct nlattr *)pbucket, policy);
		len_bucket = 0;
		for (k = GSCAN_ATTRIBUTE_NUM_BUCKETS; k <= GSCAN_ATTRIBUTE_REPORT_EVENTS; k++) {
			if (attr[k]) {
				switch (k) {
				case GSCAN_ATTRIBUTE_BUCKETS_BAND:
					prWifiScanCmd->buckets[i].band = nla_get_u32(attr[k]);
					len_bucket += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_BUCKET_ID:
					prWifiScanCmd->buckets[i].bucket = nla_get_u32(attr[k]);
					len_bucket += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_BUCKET_PERIOD:
					prWifiScanCmd->buckets[i].period = nla_get_u32(attr[k]);
					len_bucket += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_REPORT_EVENTS:
					prWifiScanCmd->buckets[i].report_events = nla_get_u32(attr[k]);
					len_bucket += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_BUCKET_NUM_CHANNELS:
					prWifiScanCmd->buckets[i].num_channels = nla_get_u32(attr[k]);
					len_bucket += NLA_ALIGN(attr[k]->nla_len);
					DBGLOG(REQ, TRACE, "bucket%d: attr=0x%x, num_channels=%d nla_len = %d, \r\n",
					       i, *(UINT_32 *) attr[k], nla_get_u32(attr[k]), attr[k]->nla_len);
					break;
				}
			}
		}
		pbucket = (struct nlattr *)((UINT_8 *) pbucket + NLA_HDRLEN);
		/* request.attr_start(i) as nested attribute */
		DBGLOG(REQ, TRACE, "+++pure bucket size=%d pbucket=%p \r\n", len_bucket, pbucket);
		pbucket = (struct nlattr *)((UINT_8 *) pbucket + len_bucket);
		/* pure bucket payload, not include channels */

		/*don't need to use nla_parse_nested to parse channels */
		/* the header of channel in bucket i */
		pchannel = (struct nlattr *)((UINT_8 *) pbucket + NLA_HDRLEN);
		for (j = 0; j < prWifiScanCmd->buckets[i].num_channels; j++) {
			prWifiScanCmd->buckets[i].channels[j].channel = nla_get_u32(pchannel);
			len_channel = NLA_ALIGN(pchannel->nla_len);
			DBGLOG(REQ, TRACE,
				"attr=0x%x, channel=%d, \r\n", *(UINT_32 *) pchannel, nla_get_u32(pchannel));

			pchannel = (struct nlattr *)((UINT_8 *) pchannel + len_channel);
		}
		pbucket = pchannel;
	}

	DBGLOG(REQ, TRACE, "base_period=%d, num_buckets=%d, bucket0: %d %d %d %d",
		prWifiScanCmd->base_period, prWifiScanCmd->num_buckets,
		prWifiScanCmd->buckets[0].bucket, prWifiScanCmd->buckets[0].period,
		prWifiScanCmd->buckets[0].band, prWifiScanCmd->buckets[0].report_events);

	DBGLOG(REQ, TRACE, "num_channels=%d, channel0=%d, channel1=%d; num_channels=%d, channel0=%d, channel1=%d",
		prWifiScanCmd->buckets[0].num_channels,
		prWifiScanCmd->buckets[0].channels[0].channel, prWifiScanCmd->buckets[0].channels[1].channel,
		prWifiScanCmd->buckets[1].num_channels,
		prWifiScanCmd->buckets[1].channels[0].channel, prWifiScanCmd->buckets[1].channels[1].channel);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetGSCNAParam,
			   prWifiScanCmd, sizeof(PARAM_WIFI_GSCAN_CMD_PARAMS), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_set_scan_config(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	P_GLUE_INFO_T prGlueInfo = NULL;

	INT_32 i4Status = -EINVAL;
	PARAM_WIFI_GSCAN_CMD_PARAMS rWifiScanCmd;
	struct nlattr *attr[GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE + 1];
	/* UINT_32 num_scans = 0; */	/* another attribute */
	int k;
	static struct nla_policy policy[GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE + 1] = {
		[GSCAN_ATTRIBUTE_NUM_AP_PER_SCAN] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_REPORT_THRESHOLD] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE] = {.type = NLA_U32},
	};

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d \r\n", __func__, data_len);
	kalMemZero(&rWifiScanCmd, sizeof(rWifiScanCmd));
	kalMemZero(attr, sizeof(struct nlattr *) * (GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE + 1));

	nla_parse_nested(attr, GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE, (struct nlattr *)(data - NLA_HDRLEN), policy);
	for (k = GSCAN_ATTRIBUTE_NUM_AP_PER_SCAN; k <= GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE; k++) {
		if (attr[k]) {
			switch (k) {
			case GSCAN_ATTRIBUTE_NUM_AP_PER_SCAN:
				rWifiScanCmd.max_ap_per_scan = nla_get_u32(attr[k]);
				break;
			case GSCAN_ATTRIBUTE_REPORT_THRESHOLD:
				rWifiScanCmd.report_threshold = nla_get_u32(attr[k]);
				break;
			case GSCAN_ATTRIBUTE_NUM_SCANS_TO_CACHE:
				rWifiScanCmd.num_scans = nla_get_u32(attr[k]);
				break;
			}
		}
	}
	DBGLOG(REQ, TRACE, "attr=0x%x, attr2=0x%x ", *(UINT_32 *) attr[GSCAN_ATTRIBUTE_NUM_AP_PER_SCAN],
	       *(UINT_32 *) attr[GSCAN_ATTRIBUTE_REPORT_THRESHOLD]);

	DBGLOG(REQ, TRACE, "max_ap_per_scan=%d, report_threshold=%d num_scans=%d \r\n",
	       rWifiScanCmd.max_ap_per_scan, rWifiScanCmd.report_threshold, rWifiScanCmd.num_scans);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetGSCNAConfig,
			   &rWifiScanCmd, sizeof(PARAM_WIFI_GSCAN_CMD_PARAMS), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_set_significant_change(struct wiphy *wiphy, struct wireless_dev *wdev,
					       const void *data, int data_len)
{
	INT_32 i4Status = -EINVAL;
	PARAM_WIFI_SIGNIFICANT_CHANGE rWifiChangeCmd;
	UINT_8 flush = 0;
	struct nlattr *attr[GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1];
	struct nlattr *paplist;
	int i, k;
	UINT_32 len_basic, len_aplist;
	static struct nla_policy policy[GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1] = {
		[GSCAN_ATTRIBUTE_BSSID] = {.type = NLA_UNSPEC},
		[GSCAN_ATTRIBUTE_RSSI_LOW] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_RSSI_HIGH] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_RSSI_SAMPLE_SIZE] = {.type = NLA_U16},
		[GSCAN_ATTRIBUTE_LOST_AP_SAMPLE_SIZE] = {.type = NLA_U16},
		[GSCAN_ATTRIBUTE_MIN_BREACHING] = {.type = NLA_U16},
		[GSCAN_ATTRIBUTE_NUM_AP] = {.type = NLA_U16},
		[GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH] = {.type = NLA_U8},
	};

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d \r\n", __func__, data_len);
	for (i = 0; i < 6; i++)
		DBGLOG(REQ, LOUD, "0x%x 0x%x 0x%x 0x%x \r\n",
			*((UINT_32 *) data + i * 4), *((UINT_32 *) data + i * 4 + 1),
			*((UINT_32 *) data + i * 4 + 2), *((UINT_32 *) data + i * 4 + 3));
	kalMemZero(&rWifiChangeCmd, sizeof(rWifiChangeCmd));
	kalMemZero(attr, sizeof(struct nlattr *) * (GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1));

	nla_parse_nested(attr, GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH, (struct nlattr *)(data - NLA_HDRLEN), policy);
	len_basic = 0;
	for (k = GSCAN_ATTRIBUTE_RSSI_SAMPLE_SIZE; k <= GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH; k++) {
		if (attr[k]) {
			switch (k) {
			case GSCAN_ATTRIBUTE_RSSI_SAMPLE_SIZE:
				rWifiChangeCmd.rssi_sample_size = nla_get_u16(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_LOST_AP_SAMPLE_SIZE:
				rWifiChangeCmd.lost_ap_sample_size = nla_get_u16(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_MIN_BREACHING:
				rWifiChangeCmd.min_breaching = nla_get_u16(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_NUM_AP:
				rWifiChangeCmd.num_ap = nla_get_u16(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				DBGLOG(REQ, TRACE, "attr=0x%x, num_ap=%d nla_len=%d, \r\n",
				       *(UINT_32 *) attr[k], rWifiChangeCmd.num_ap, attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH:
				flush = nla_get_u8(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			}
		}
	}
	paplist = (struct nlattr *)((UINT_8 *) data + len_basic);
	DBGLOG(REQ, TRACE, "+++basic attribute size=%d flush=%d\r\n", len_basic, flush);

	if (paplist->nla_type == GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_BSSIDS)
		paplist = (struct nlattr *)((UINT_8 *) paplist + NLA_HDRLEN);

	for (i = 0; i < rWifiChangeCmd.num_ap; i++) {
		nla_parse_nested(attr, GSCAN_ATTRIBUTE_RSSI_HIGH, (struct nlattr *)paplist, policy);
		paplist = (struct nlattr *)((UINT_8 *) paplist + NLA_HDRLEN);
		/* request.attr_start(i) as nested attribute */
		len_aplist = 0;
		for (k = GSCAN_ATTRIBUTE_BSSID; k <= GSCAN_ATTRIBUTE_RSSI_HIGH; k++) {
			if (attr[k]) {
				switch (k) {
				case GSCAN_ATTRIBUTE_BSSID:
					kalMemCopy(rWifiChangeCmd.ap[i].bssid, nla_data(attr[k]), sizeof(mac_addr));
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_RSSI_LOW:
					rWifiChangeCmd.ap[i].low = nla_get_u32(attr[k]);
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_RSSI_HIGH:
					rWifiChangeCmd.ap[i].high = nla_get_u32(attr[k]);
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				}
			}
		}
		if (((i + 1) % 4 == 0) || (i == rWifiChangeCmd.num_ap - 1))
			DBGLOG(REQ, TRACE, "ap[%d], len_aplist=%d\n", i, len_aplist);
		else
			DBGLOG(REQ, TRACE, "ap[%d], len_aplist=%d \t", i, len_aplist);
		paplist = (struct nlattr *)((UINT_8 *) paplist + len_aplist);
	}

	DBGLOG(REQ, TRACE,
		"flush=%d, rssi_sample_size=%d lost_ap_sample_size=%d min_breaching=%d",
		flush, rWifiChangeCmd.rssi_sample_size, rWifiChangeCmd.lost_ap_sample_size,
		rWifiChangeCmd.min_breaching);
	DBGLOG(REQ, TRACE,
		"ap[0].channel=%d low=%d high=%d, ap[1].channel=%d low=%d high=%d",
		rWifiChangeCmd.ap[0].channel, rWifiChangeCmd.ap[0].low, rWifiChangeCmd.ap[0].high,
		rWifiChangeCmd.ap[1].channel, rWifiChangeCmd.ap[1].low, rWifiChangeCmd.ap[1].high);
	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_set_hotlist(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	/*WLAN_STATUS rStatus;*/
	P_GLUE_INFO_T prGlueInfo = NULL;
	CMD_SET_PSCAN_ADD_HOTLIST_BSSID rCmdPscnAddHotlist;

	INT_32 i4Status = -EINVAL;
	PARAM_WIFI_BSSID_HOTLIST rWifiHotlistCmd;
	UINT_8 flush = 0;
	struct nlattr *attr[GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1];
	struct nlattr *paplist;
	int i, k;
	UINT_32 len_basic, len_aplist;
	static struct nla_policy policy[GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1] = {
		[GSCAN_ATTRIBUTE_BSSID] = {.type = NLA_UNSPEC},
		[GSCAN_ATTRIBUTE_RSSI_LOW] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_RSSI_HIGH] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_LOST_AP_SAMPLE_SIZE] = {.type = NLA_U32},
		[GSCAN_ATTRIBUTE_NUM_AP] = {.type = NLA_U16},
		[GSCAN_ATTRIBUTE_HOTLIST_FLUSH] = {.type = NLA_U8},
	};

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d \r\n", __func__, data_len);
	for (i = 0; i < 5; i++)
		DBGLOG(REQ, LOUD, "0x%x 0x%x 0x%x 0x%x \r\n",
			*((UINT_32 *) data + i * 4), *((UINT_32 *) data + i * 4 + 1),
			*((UINT_32 *) data + i * 4 + 2), *((UINT_32 *) data + i * 4 + 3));
	kalMemZero(&rWifiHotlistCmd, sizeof(rWifiHotlistCmd));
	kalMemZero(attr, sizeof(struct nlattr *) * (GSCAN_ATTRIBUTE_SIGNIFICANT_CHANGE_FLUSH + 1));

	nla_parse_nested(attr, GSCAN_ATTRIBUTE_NUM_AP, (struct nlattr *)(data - NLA_HDRLEN), policy);
	len_basic = 0;
	for (k = GSCAN_ATTRIBUTE_HOTLIST_FLUSH; k <= GSCAN_ATTRIBUTE_NUM_AP; k++) {
		if (attr[k]) {
			switch (k) {
			case GSCAN_ATTRIBUTE_LOST_AP_SAMPLE_SIZE:
				rWifiHotlistCmd.lost_ap_sample_size = nla_get_u32(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_NUM_AP:
				rWifiHotlistCmd.num_ap = nla_get_u16(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				DBGLOG(REQ, TRACE, "attr=0x%x, num_ap=%d nla_len=%d, \r\n",
				       *(UINT_32 *) attr[k], rWifiHotlistCmd.num_ap, attr[k]->nla_len);
				break;
			case GSCAN_ATTRIBUTE_HOTLIST_FLUSH:
				flush = nla_get_u8(attr[k]);
				len_basic += NLA_ALIGN(attr[k]->nla_len);
				break;
			}
		}
	}
	paplist = (struct nlattr *)((UINT_8 *) data + len_basic);
	DBGLOG(REQ, TRACE, "+++basic attribute size=%d flush=%d\r\n", len_basic, flush);

	if (paplist->nla_type == GSCAN_ATTRIBUTE_HOTLIST_BSSIDS)
		paplist = (struct nlattr *)((UINT_8 *) paplist + NLA_HDRLEN);

	for (i = 0; i < rWifiHotlistCmd.num_ap; i++) {
		nla_parse_nested(attr, GSCAN_ATTRIBUTE_RSSI_HIGH, (struct nlattr *)paplist, policy);
		paplist = (struct nlattr *)((UINT_8 *) paplist + NLA_HDRLEN);
		/* request.attr_start(i) as nested attribute */
		len_aplist = 0;
		for (k = GSCAN_ATTRIBUTE_BSSID; k <= GSCAN_ATTRIBUTE_RSSI_HIGH; k++) {
			if (attr[k]) {
				switch (k) {
				case GSCAN_ATTRIBUTE_BSSID:
					kalMemCopy(rWifiHotlistCmd.ap[i].bssid, nla_data(attr[k]), sizeof(mac_addr));
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_RSSI_LOW:
					rWifiHotlistCmd.ap[i].low = nla_get_u32(attr[k]);
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				case GSCAN_ATTRIBUTE_RSSI_HIGH:
					rWifiHotlistCmd.ap[i].high = nla_get_u32(attr[k]);
					len_aplist += NLA_ALIGN(attr[k]->nla_len);
					break;
				}
			}
		}
		if (((i + 1) % 4 == 0) || (i == rWifiHotlistCmd.num_ap - 1))
			DBGLOG(REQ, TRACE, "ap[%d], len_aplist=%d\n", i, len_aplist);
		else
			DBGLOG(REQ, TRACE, "ap[%d], len_aplist=%d \t", i, len_aplist);
		paplist = (struct nlattr *)((UINT_8 *) paplist + len_aplist);
	}

	DBGLOG(REQ, TRACE,
	"flush=%d, lost_ap_sample_size=%d, Hotlist:ap[0].channel=%d low=%d high=%d, ap[1].channel=%d low=%d high=%d",
		flush, rWifiHotlistCmd.lost_ap_sample_size,
		rWifiHotlistCmd.ap[0].channel, rWifiHotlistCmd.ap[0].low, rWifiHotlistCmd.ap[0].high,
		rWifiHotlistCmd.ap[1].channel, rWifiHotlistCmd.ap[1].low, rWifiHotlistCmd.ap[1].high);

	memcpy(&(rCmdPscnAddHotlist.aucMacAddr), &(rWifiHotlistCmd.ap[0].bssid), 6 * sizeof(UINT_8));
	rCmdPscnAddHotlist.ucFlags = (UINT_8) TRUE;
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_enable_scan(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	P_GLUE_INFO_T prGlueInfo = NULL;
	PARAM_WIFI_GSCAN_ACTION_CMD_PARAMS rWifiScanActionCmd;

	INT_32 i4Status = -EINVAL;
	struct nlattr *attr;
	UINT_8 gGScanEn = 0;

	static UINT_8 k; /* only for test */

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d, data=0x%x 0x%x\r\n",
	       __func__, data_len, *((UINT_32 *) data), *((UINT_32 *) data + 1));

	attr = (struct nlattr *)data;
	if (attr->nla_type == GSCAN_ATTRIBUTE_ENABLE_FEATURE)
		gGScanEn = nla_get_u32(attr);
	DBGLOG(REQ, INFO, "gGScanEn=%d, \r\n", gGScanEn);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);
	if (gGScanEn == TRUE)
		rWifiScanActionCmd.ucPscanAct = ENABLE;
	else
		rWifiScanActionCmd.ucPscanAct = DISABLE;

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetGSCNAction,
			   &rWifiScanActionCmd,
			   sizeof(PARAM_WIFI_GSCAN_ACTION_CMD_PARAMS), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	/* mtk_cfg80211_vendor_get_scan_results(wiphy, wdev, data, data_len ); */

	return 0;

	/* only for test */
	if (k % 3 == 1) {
		mtk_cfg80211_vendor_event_significant_change_results(wiphy, wdev, NULL, 0);
		mtk_cfg80211_vendor_event_hotlist_ap_found(wiphy, wdev, NULL, 0);
		mtk_cfg80211_vendor_event_hotlist_ap_lost(wiphy, wdev, NULL, 0);
	}
	k++;

	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_enable_full_scan_results(struct wiphy *wiphy, struct wireless_dev *wdev,
						 const void *data, int data_len)
{
	INT_32 i4Status = -EINVAL;
	struct nlattr *attr;
	UINT_8 gFullScanResultsEn = 0;

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, INFO, "%s for vendor command: data_len=%d, data=0x%x 0x%x\r\n",
	       __func__, data_len, *((UINT_32 *) data), *((UINT_32 *) data + 1));

	attr = (struct nlattr *)data;
	if (attr->nla_type == GSCAN_ENABLE_FULL_SCAN_RESULTS)
		gFullScanResultsEn = nla_get_u32(attr);
	DBGLOG(REQ, INFO, "gFullScanResultsEn=%d, \r\n", gFullScanResultsEn);

	return 0;

	/* only for test */
	mtk_cfg80211_vendor_event_complete_scan(wiphy, wdev, WIFI_SCAN_COMPLETE);
	mtk_cfg80211_vendor_event_scan_results_available(wiphy, wdev, 4);
	if (gFullScanResultsEn == TRUE)
		mtk_cfg80211_vendor_event_full_scan_results(wiphy, wdev, NULL, 0);

	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_get_scan_results(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	/*WLAN_STATUS rStatus;*/
	UINT_32 u4BufLen;
	P_GLUE_INFO_T prGlueInfo = NULL;
	PARAM_WIFI_GSCAN_GET_RESULT_PARAMS rGSscnResultParm;

	INT_32 i4Status = -EINVAL;
	struct nlattr *attr;
	UINT_32 get_num = 0, real_num = 0;
	UINT_8 flush = 0;
	/*PARAM_WIFI_GSCAN_RESULT result[4], *pResult;
	struct sk_buff *skb;*/
	int i; /*int j;*/
	/*UINT_32 scan_id;*/

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	DBGLOG(REQ, TRACE, "%s for vendor command: data_len=%d \r\n", __func__, data_len);
	for (i = 0; i < 2; i++)
		DBGLOG(REQ, LOUD, "0x%x 0x%x 0x%x 0x%x \r\n", *((UINT_32 *) data + i * 4),
			*((UINT_32 *) data + i * 4 + 1), *((UINT_32 *) data + i * 4 + 2),
			*((UINT_32 *) data + i * 4 + 3));

	attr = (struct nlattr *)data;
	if (attr->nla_type == GSCAN_ATTRIBUTE_NUM_OF_RESULTS) {
		get_num = nla_get_u32(attr);
		attr = (struct nlattr *)((UINT_8 *) attr + attr->nla_len);
	}
	if (attr->nla_type == GSCAN_ATTRIBUTE_FLUSH_RESULTS) {
		flush = nla_get_u8(attr);
		attr = (struct nlattr *)((UINT_8 *) attr + attr->nla_len);
	}
	DBGLOG(REQ, TRACE, "number=%d, flush=%d \r\n", get_num, flush);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	real_num = (get_num < PSCAN_MAX_SCAN_CACHE_SIZE) ? get_num : PSCAN_MAX_SCAN_CACHE_SIZE;
	get_num = real_num;

#if 0	/* driver buffer FW results and reports by buffer workaround for FW mismatch with hal results numbers */
	g_GetResultsCmdCnt++;
	DBGLOG(REQ, INFO,
	       "(g_GetResultsCmdCnt [%d], g_GetResultsBufferedCnt [%d]\n", g_GetResultsCmdCnt,
		g_GetResultsBufferedCnt);

	BOOLEAN fgIsGetResultFromBuffer = FALSE;
	UINT_8 BufferedResultReportIndex = 0;

	if (g_GetResultsBufferedCnt > 0) {

		DBGLOG(REQ, INFO,
		       "(g_GetResultsBufferedCnt > 0), report buffered results instead of ask from FW\n");

		/* reply the results to wifi_hal  */
		for (i = 0; i < MAX_BUFFERED_GSCN_RESULTS; i++) {

			if (g_arGscanResultsIndicateNumber[i] > 0) {
				real_num = g_arGscanResultsIndicateNumber[i];
				get_num = real_num;
				g_arGscanResultsIndicateNumber[i] = 0;
				fgIsGetResultFromBuffer = TRUE;
				BufferedResultReportIndex = i;
				break;
			}
		}
		if (i == MAX_BUFFERED_GSCN_RESULTS)
			DBGLOG(REQ, TRACE, "all buffered results are invalid, unexpected case \r\n");
		DBGLOG(REQ, TRACE, "BufferedResultReportIndex[%d] i = %d real_num[%d] get_num[%d] \r\n",
			BufferedResultReportIndex, i, real_num, get_num);
	}
#endif

	rGSscnResultParm.get_num = get_num;
	rGSscnResultParm.flush = flush;
#if 0/* //driver buffer FW results and reports by buffer workaround for FW results mismatch with hal results number */
	if (fgIsGetResultFromBuffer) {
		nicRxProcessGSCNEvent(prGlueInfo->prAdapter, g_arGscnResultsTempBuffer[BufferedResultReportIndex]);
		g_GetResultsBufferedCnt--;
		g_GetResultsCmdCnt--;
		nicRxReturnRFB(prGlueInfo->prAdapter, g_arGscnResultsTempBuffer[BufferedResultReportIndex]);
	} else
#endif
	{
		kalIoctl(prGlueInfo,
			 wlanoidGetGSCNResult,
			 &rGSscnResultParm,
			 sizeof(PARAM_WIFI_GSCAN_GET_RESULT_PARAMS), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	}
	return 0;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_get_channel_list(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo;
	struct nlattr *attr;
	UINT_32 band = 0;
	UINT_8 ucNumOfChannel, i, j;
	RF_CHANNEL_INFO_T aucChannelList[64];
	UINT_32 num_channels;
	wifi_channel channels[64];
	struct sk_buff *skb;

	ASSERT(wiphy && wdev);
	if ((data == NULL) || !data_len)
		return -EINVAL;

	attr = (struct nlattr *)data;
	if (attr->nla_type == WIFI_ATTRIBUTE_BAND)
		band = nla_get_u32(attr);

	if (wdev->iftype == NL80211_IFTYPE_AP)
		prGlueInfo = *((P_GLUE_INFO_T *) wiphy_priv(wiphy));
	else
		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (!prGlueInfo)
		return -EFAULT;

	switch (band) {
	case 1: /* 2.4G band */
		rlmDomainGetChnlList(prGlueInfo->prAdapter, BAND_2G4, TRUE,
				64, &ucNumOfChannel, aucChannelList);
		break;
	case 2: /* 5G band without DFS channels */
		rlmDomainGetChnlList(prGlueInfo->prAdapter, BAND_5G, TRUE,
				64, &ucNumOfChannel, aucChannelList);
		break;
	case 4: /* 5G band DFS channels only */
		rlmDomainGetDfsChnls(prGlueInfo->prAdapter, 64, &ucNumOfChannel, aucChannelList);
		break;
	default:
		ucNumOfChannel = 0;
		break;
	}

	kalMemZero(channels, sizeof(channels));
	for (i = 0, j = 0; i < ucNumOfChannel; i++) {
		/* We need to report frequency list to HAL */
		channels[j] = nicChannelNum2Freq(aucChannelList[i].ucChannelNum) / 1000;
		if (channels[j] == 0)
			continue;
		else {
			DBGLOG(REQ, TRACE, "channels[%d] = %d\n", j, channels[j]);
			j++;
		}
	}
	num_channels = j;
	DBGLOG(REQ, INFO, "Get channel list for band: %d, num_channels=%d\n", band, num_channels);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(channels));
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(nla_put_u32(skb, WIFI_ATTRIBUTE_NUM_CHANNELS, num_channels) < 0))
		goto nla_put_failure;

	if (unlikely(nla_put(skb, WIFI_ATTRIBUTE_CHANNEL_LIST,
		(sizeof(wifi_channel) * num_channels), channels) < 0))
		goto nla_put_failure;

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;

}

int mtk_cfg80211_vendor_set_country_code(struct wiphy *wiphy, struct wireless_dev *wdev,
					 const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	struct nlattr *attr;
	UINT_8 country[2] = {0, 0};

	ASSERT(wiphy && wdev);
	if ((data == NULL) || (data_len == 0))
		return -EINVAL;

	attr = (struct nlattr *)data;
	if (attr->nla_type == WIFI_ATTRIBUTE_COUNTRY_CODE) {
		country[0] = *((PUINT_8)nla_data(attr));
		country[1] = *((PUINT_8)nla_data(attr) + 1);
	}

	DBGLOG(REQ, INFO, "Set country code: %c%c, iftype=%d\n", country[0], country[1], wdev->iftype);

	if (wdev->iftype == NL80211_IFTYPE_AP)
		prGlueInfo = *((P_GLUE_INFO_T *) wiphy_priv(wiphy));
	else
		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (!prGlueInfo)
		return -EFAULT;

	rStatus = kalIoctl(prGlueInfo, wlanoidSetCountryCode, country, 2, FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "Set country code error: %x\n", rStatus);
		return -EFAULT;
	}

	return 0;
}


int mtk_cfg80211_vendor_get_roaming_capabilities(struct wiphy *wiphy,
				 struct wireless_dev *wdev, const void *data, int data_len)
{
	UINT_32 maxNumOfList[2] = { MAX_FW_ROAMING_BLACKLIST_SIZE, MAX_FW_ROAMING_WHITELIST_SIZE };
	struct sk_buff *skb;

	if (wiphy == NULL || wdev == NULL)
		return -EFAULT;

	DBGLOG(REQ, INFO, "Get roaming capabilities: max black/whitelist=%d/%d", maxNumOfList[0], maxNumOfList[1]);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(maxNumOfList));
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(nla_put(skb, WIFI_ATTRIBUTE_ROAMING_CAPABILITIES,
						 sizeof(maxNumOfList), maxNumOfList) < 0))
		goto nla_put_failure;

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}

int mtk_cfg80211_vendor_config_roaming(struct wiphy *wiphy,
				 struct wireless_dev *wdev, const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4ResultLen = 0;


	DBGLOG(REQ, TRACE, "Receives roaming blacklist & whitelist with data_len=%d\n", data_len);
	if ((wiphy == NULL) || (wdev == NULL)) {
		DBGLOG(REQ, INFO, "wiphy or wdev is NULL\n");
		return -EINVAL;
	}

	if ((data == NULL) || (data_len == 0))
		return -EINVAL;

	if (wdev->iftype != NL80211_IFTYPE_STATION)
		return -EINVAL;

	if (wdev->iftype == NL80211_IFTYPE_AP)
		prGlueInfo = *((P_GLUE_INFO_T *) wiphy_priv(wiphy));
	else
		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	if (!prGlueInfo)
		return -EINVAL;

	if (prGlueInfo->u4FWRoamingEnable == 0) {
		DBGLOG(REQ, INFO, "FWRoaming is disabled (FWRoamingEnable=%d)\n", prGlueInfo->u4FWRoamingEnable);
		return WLAN_STATUS_SUCCESS;
	}

	kalIoctl(prGlueInfo, wlanoidConfigRoaming, (PVOID)data, data_len, FALSE,
		FALSE, FALSE, FALSE, &u4ResultLen);

	return WLAN_STATUS_SUCCESS;
}


int mtk_cfg80211_vendor_enable_roaming(struct wiphy *wiphy,
				 struct wireless_dev *wdev, const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct nlattr *attr;
	UINT_32 u4ResultLen = 0;

	ASSERT(wiphy);	/* change to if (wiphy == NULL) then return? */
	ASSERT(wdev);	/* change to if (wiphy == NULL) then return? */

	if (wdev->iftype == NL80211_IFTYPE_AP)
		prGlueInfo = *((P_GLUE_INFO_T *) wiphy_priv(wiphy));
	else
		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	if (!prGlueInfo)
		return -EFAULT;

	attr = (struct nlattr *)data;
	if (attr->nla_type == WIFI_ATTRIBUTE_ROAMING_STATE)
		prGlueInfo->u4FWRoamingEnable = nla_get_u32(attr);

	if (prGlueInfo->u4FWRoamingEnable == 0)
		kalIoctl(prGlueInfo, wlanoidEnableRoaming, NULL, 0,
			FALSE, FALSE, FALSE, FALSE, &u4ResultLen);

	DBGLOG(REQ, INFO, "FWK set FWRoamingEnable = %d\n", prGlueInfo->u4FWRoamingEnable);

	return WLAN_STATUS_SUCCESS;
}

int mtk_cfg80211_vendor_get_rtt_capabilities(struct wiphy *wiphy, struct wireless_dev *wdev,
			const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 i4Status = -EINVAL;
	PARAM_WIFI_RTT_CAPABILITIES rRttCapabilities;
	struct sk_buff *skb;

	DBGLOG(REQ, TRACE, "vendor command\r\n");

	ASSERT(wiphy);
	ASSERT(wdev);
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(rRttCapabilities));
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed:%x\n", __func__, i4Status);
		return -ENOMEM;
	}

	kalMemZero(&rRttCapabilities, sizeof(rRttCapabilities));

	/* RTT Capabilities return from driver not firmware */
	rRttCapabilities.rtt_one_sided_supported = 0;
	rRttCapabilities.rtt_ftm_supported = 0;
	rRttCapabilities.lci_support = 0;
	rRttCapabilities.lcr_support = 0;
	rRttCapabilities.preamble_support = 0;
	rRttCapabilities.bw_support = 0;

	if (unlikely(nla_put(skb, RTT_ATTRIBUTE_CAPABILITIES,
		sizeof(rRttCapabilities), &rRttCapabilities) < 0))
		goto nla_put_failure;

	i4Status = cfg80211_vendor_cmd_reply(skb);
	return i4Status;

nla_put_failure:
	kfree_skb(skb);
	return i4Status;
}


int mtk_cfg80211_vendor_set_rssi_monitoring(struct wiphy *wiphy, struct wireless_dev *wdev,
					    const void *data, int data_len)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	P_GLUE_INFO_T prGlueInfo = NULL;

	INT_32 i4Status = -EINVAL;
	PARAM_RSSI_MONITOR_T rRSSIMonitor;
	struct nlattr *attr[WIFI_ATTRIBUTE_RSSI_MONITOR_START + 1];
	UINT_32 i = 0;

	ASSERT(wiphy);
	ASSERT(wdev);

	DBGLOG(REQ, TRACE, "vendor command: data_len=%d\r\n", data_len);
	kalMemZero(&rRSSIMonitor, sizeof(PARAM_RSSI_MONITOR_T));
	if ((data == NULL) || !data_len)
		goto nla_put_failure;
	kalMemZero(attr, sizeof(struct nlattr *) * (WIFI_ATTRIBUTE_RSSI_MONITOR_START + 1));

	if (NLA_PARSE_NESTED(attr, WIFI_ATTRIBUTE_RSSI_MONITOR_START,
		(struct nlattr *)(data - NLA_HDRLEN), nla_parse_wifi_policy) < 0) {
		DBGLOG(REQ, ERROR, "%s nla_parse_nested failed\n", __func__);
		goto nla_put_failure;
	}

	for (i = WIFI_ATTRIBUTE_MAX_RSSI; i <= WIFI_ATTRIBUTE_RSSI_MONITOR_START; i++) {
		if (attr[i]) {
			switch (i) {
			case WIFI_ATTRIBUTE_MAX_RSSI:
				rRSSIMonitor.max_rssi_value = nla_get_u32(attr[i]);
				break;
			case WIFI_ATTRIBUTE_MIN_RSSI:
				rRSSIMonitor.min_rssi_value = nla_get_u32(attr[i]);
				break;
			case WIFI_ATTRIBUTE_RSSI_MONITOR_START:
				rRSSIMonitor.enable = nla_get_u32(attr[i]);
				break;
			}
		}
	}

	DBGLOG(REQ, INFO, "mMax_rssi=%d, mMin_rssi=%d enable=%d\r\n",
	       rRSSIMonitor.max_rssi_value, rRSSIMonitor.min_rssi_value, rRSSIMonitor.enable);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidRssiMonitor,
			   &rRSSIMonitor, sizeof(PARAM_RSSI_MONITOR_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	return rStatus;

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_packet_keep_alive_start(struct wiphy *wiphy, struct wireless_dev *wdev,
						const void *data, int data_len)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	P_GLUE_INFO_T prGlueInfo = NULL;

	INT_32 i4Status = -EINVAL;
	P_PARAM_PACKET_KEEPALIVE_T prPkt = NULL;
	struct nlattr *attr[MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC + 1];
	UINT_32 i = 0;
	unsigned short u2IpPktLen = 0;

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;

	DBGLOG(REQ, TRACE, "vendor command: data_len=%d\r\n", data_len);
	prPkt = (P_PARAM_PACKET_KEEPALIVE_T) kalMemAlloc(sizeof(PARAM_PACKET_KEEPALIVE_T), VIR_MEM_TYPE);
	if (!prPkt) {
		DBGLOG(REQ, ERROR, "Can not alloc memory for PARAM_PACKET_KEEPALIVE_T\n");
		return -ENOMEM;
	}
	kalMemZero(prPkt, sizeof(PARAM_PACKET_KEEPALIVE_T));
	kalMemZero(attr, sizeof(struct nlattr *) * (MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC + 1));

	prPkt->enable = TRUE; /*start packet keep alive*/
	if (NLA_PARSE_NESTED(attr, MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC,
		(struct nlattr *)(data - NLA_HDRLEN), nla_parse_offloading_policy) < 0) {
		DBGLOG(REQ, ERROR, "%s nla_parse_nested failed\n", __func__);
		goto nla_put_failure;
	}

	for (i = MKEEP_ALIVE_ATTRIBUTE_ID; i <= MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC; i++) {
		if (attr[i]) {
			switch (i) {
			case MKEEP_ALIVE_ATTRIBUTE_ID:
				prPkt->index = nla_get_u8(attr[i]);
				break;
			case MKEEP_ALIVE_ATTRIBUTE_IP_PKT_LEN:
				prPkt->u2IpPktLen = (nla_get_u16(attr[i]) > sizeof(prPkt->pIpPkt))
				? sizeof(prPkt->pIpPkt):nla_get_u16(attr[i]);
				break;
			case MKEEP_ALIVE_ATTRIBUTE_IP_PKT:
				u2IpPktLen = prPkt->u2IpPktLen <= MAX_IP_PKT_LEN
					? prPkt->u2IpPktLen : MAX_IP_PKT_LEN;
				kalMemCopy(prPkt->pIpPkt, nla_data(attr[i]), u2IpPktLen);
				break;
			case MKEEP_ALIVE_ATTRIBUTE_SRC_MAC_ADDR:
				kalMemCopy(prPkt->ucSrcMacAddr, nla_data(attr[i]), sizeof(mac_addr));
				break;
			case MKEEP_ALIVE_ATTRIBUTE_DST_MAC_ADDR:
				kalMemCopy(prPkt->ucDstMacAddr, nla_data(attr[i]), sizeof(mac_addr));
				break;
			case MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC:
				prPkt->u4PeriodMsec = nla_get_u32(attr[i]);
				break;
			}
		}
	}

	DBGLOG(REQ, INFO, "enable=%d, index=%d, u2IpPktLen=%d u4PeriodMsec=%d\n",
		prPkt->enable, prPkt->index, prPkt->u2IpPktLen, prPkt->u4PeriodMsec);
	DBGLOG(REQ, TRACE, "prPkt->pIpPkt=0x%02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x",
		prPkt->pIpPkt[0], prPkt->pIpPkt[1], prPkt->pIpPkt[2], prPkt->pIpPkt[3],
		prPkt->pIpPkt[4], prPkt->pIpPkt[5], prPkt->pIpPkt[6], prPkt->pIpPkt[7],
		prPkt->pIpPkt[8], prPkt->pIpPkt[9], prPkt->pIpPkt[10], prPkt->pIpPkt[11],
		prPkt->pIpPkt[12], prPkt->pIpPkt[13], prPkt->pIpPkt[14], prPkt->pIpPkt[15]);
	DBGLOG(REQ, TRACE, "prPkt->srcMAC=%02x:%02x:%02x:%02x:%02x:%02x, dstMAC=%02x:%02x:%02x:%02x:%02x:%02x\n",
		prPkt->ucSrcMacAddr[0], prPkt->ucSrcMacAddr[1], prPkt->ucSrcMacAddr[2], prPkt->ucSrcMacAddr[3],
		prPkt->ucSrcMacAddr[4], prPkt->ucSrcMacAddr[5],
		prPkt->ucDstMacAddr[0], prPkt->ucDstMacAddr[1], prPkt->ucDstMacAddr[2], prPkt->ucDstMacAddr[3],
		prPkt->ucDstMacAddr[4], prPkt->ucDstMacAddr[5]);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidPacketKeepAlive,
			   prPkt, sizeof(PARAM_PACKET_KEEPALIVE_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	kalMemFree(prPkt, VIR_MEM_TYPE, sizeof(PARAM_PACKET_KEEPALIVE_T));
	return rStatus;

nla_put_failure:
	if (prPkt != NULL)
		kalMemFree(prPkt, VIR_MEM_TYPE, sizeof(PARAM_PACKET_KEEPALIVE_T));
	return i4Status;
}


int mtk_cfg80211_vendor_packet_keep_alive_stop(struct wiphy *wiphy, struct wireless_dev *wdev,
					       const void *data, int data_len)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	P_GLUE_INFO_T prGlueInfo = NULL;

	INT_32 i4Status = -EINVAL;
	P_PARAM_PACKET_KEEPALIVE_T prPkt = NULL;
	struct nlattr *attr;

	ASSERT(wiphy);
	ASSERT(wdev);
	if ((data == NULL) || !data_len)
		goto nla_put_failure;

	DBGLOG(REQ, TRACE, "vendor command: data_len=%d\r\n", data_len);
	prPkt = (P_PARAM_PACKET_KEEPALIVE_T) kalMemAlloc(sizeof(PARAM_PACKET_KEEPALIVE_T), VIR_MEM_TYPE);
	if (!prPkt) {
		DBGLOG(REQ, ERROR, "Can not alloc memory for PARAM_PACKET_KEEPALIVE_T\n");
		return -ENOMEM;
	}
	kalMemZero(prPkt, sizeof(PARAM_PACKET_KEEPALIVE_T));

	prPkt->enable = FALSE;  /*stop packet keep alive*/
	attr = (struct nlattr *)data;
	if (attr->nla_type == MKEEP_ALIVE_ATTRIBUTE_ID)
		prPkt->index = nla_get_u8(attr);

	DBGLOG(REQ, INFO, "enable=%d, index=%d\r\n", prPkt->enable, prPkt->index);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidPacketKeepAlive,
			   prPkt, sizeof(PARAM_PACKET_KEEPALIVE_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	kalMemFree(prPkt, VIR_MEM_TYPE, sizeof(PARAM_PACKET_KEEPALIVE_T));
	return rStatus;

nla_put_failure:
	if (prPkt != NULL)
		kalMemFree(prPkt, VIR_MEM_TYPE, sizeof(PARAM_PACKET_KEEPALIVE_T));
	return i4Status;
}

int mtk_cfg80211_vendor_llstats_get_info(struct wiphy *wiphy, struct wireless_dev *wdev, const void *data, int data_len)
{
	INT_32 i4Status = -EINVAL;
	WIFI_RADIO_STAT *pRadioStat;
	struct sk_buff *skb;
	UINT_32 u4BufLen;

	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

	ASSERT(wiphy);
	ASSERT(wdev);

	u4BufLen = sizeof(WIFI_RADIO_STAT) + sizeof(WIFI_IFACE_STAT);
	pRadioStat = kalMemAlloc(u4BufLen, VIR_MEM_TYPE);
	if (!pRadioStat) {
		DBGLOG(REQ, ERROR, "%s kalMemAlloc pRadioStat failed\n", __func__);
		return -ENOMEM;
	}
	kalMemZero(pRadioStat, u4BufLen);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, u4BufLen);
	if (!skb) {
		DBGLOG(REQ, TRACE, "%s allocate skb failed:%x\n", __func__, i4Status);
		return -ENOMEM;
	}

	/*rStatus = kalIoctl(prGlueInfo,
	   wlanoidQueryStatistics,
	   &rRadioStat,
	   sizeof(rRadioStat),
	   TRUE,
	   TRUE,
	   TRUE,
	   FALSE,
	   &u4BufLen); */
	/* only for test */
	pRadioStat->radio = 10;
	pRadioStat->on_time = 11;
	pRadioStat->tx_time = 12;
	pRadioStat->num_channels = 4;

	/*NLA_PUT(skb, NL80211_ATTR_VENDOR_LLSTAT, u4BufLen, pRadioStat);*/
	if (unlikely(nla_put(skb, NL80211_ATTR_VENDOR_LLSTAT,
		u4BufLen, pRadioStat) < 0))
		goto nla_put_failure;


	i4Status = cfg80211_vendor_cmd_reply(skb);

	kalMemFree(pRadioStat, VIR_MEM_TYPE, u4BufLen);

nla_put_failure:
	return i4Status;
}

int mtk_cfg80211_vendor_event_complete_scan(struct wiphy *wiphy, struct wireless_dev *wdev, WIFI_SCAN_EVENT complete)
{
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(wdev);
	/* WIFI_SCAN_EVENT complete_scan; */

	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

	skb = cfg80211_vendor_event_alloc(wiphy, wdev,
			sizeof(complete),
			GSCAN_EVENT_COMPLETE_SCAN, GFP_KERNEL);

	/*skb = cfg80211_vendor_event_alloc(wiphy, sizeof(complete), GSCAN_EVENT_COMPLETE_SCAN, GFP_KERNEL);*/
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}
	/* complete_scan = WIFI_SCAN_COMPLETE; */
	/*NLA_PUT_U32(skb, GSCAN_EVENT_COMPLETE_SCAN, complete);*/
	{
		unsigned int __tmp = complete;

		if (unlikely(nla_put(skb, GSCAN_EVENT_COMPLETE_SCAN,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_event_scan_results_available(struct wiphy *wiphy, struct wireless_dev *wdev, UINT_32 num)
{
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(wdev);
	/* UINT_32 scan_result; */

	DBGLOG(REQ, INFO, "%s for vendor command %d  \r\n", __func__, num);

	skb = cfg80211_vendor_event_alloc(wiphy, wdev,
			sizeof(num),
			GSCAN_EVENT_SCAN_RESULTS_AVAILABLE, GFP_KERNEL);
	/*skb = cfg80211_vendor_event_alloc(wiphy, sizeof(num), GSCAN_EVENT_SCAN_RESULTS_AVAILABLE, GFP_KERNEL);*/
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}
	/* scan_result = 2; */
	/*NLA_PUT_U32(skb, GSCAN_EVENT_SCAN_RESULTS_AVAILABLE, num);*/
	{
		unsigned int __tmp = num;

		if (unlikely(nla_put(skb, GSCAN_EVENT_SCAN_RESULTS_AVAILABLE,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_event_full_scan_results(struct wiphy *wiphy, struct wireless_dev *wdev,
						P_PARAM_WIFI_GSCAN_RESULT pdata, UINT_32 data_len)
{
	struct sk_buff *skb;
	PARAM_WIFI_GSCAN_RESULT result;

	ASSERT(wiphy);
	ASSERT(wdev);
	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

	skb = cfg80211_vendor_event_alloc(wiphy, wdev,
			sizeof(result),
			GSCAN_EVENT_FULL_SCAN_RESULTS, GFP_KERNEL);
	/*skb = cfg80211_vendor_event_alloc(wiphy, sizeof(result), GSCAN_EVENT_FULL_SCAN_RESULTS, GFP_KERNEL);*/
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}

	kalMemZero(&result, sizeof(result));
	kalMemCopy(result.ssid, "Gscan_full_test", sizeof("Gscan_full_test"));
	result.channel = 2437;

	/* kalMemCopy(&result, pdata, sizeof(PARAM_WIFI_GSCAN_RESULT); */
	/*NLA_PUT(skb, GSCAN_EVENT_FULL_SCAN_RESULTS, sizeof(result), &result);*/
	if (unlikely(nla_put(skb, GSCAN_EVENT_FULL_SCAN_RESULTS,
		sizeof(result), &result) < 0))
		goto nla_put_failure;


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_event_significant_change_results(struct wiphy *wiphy, struct wireless_dev *wdev,
							 P_PARAM_WIFI_CHANGE_RESULT pdata, UINT_32 data_len)
{
	struct sk_buff *skb;
	PARAM_WIFI_CHANGE_RESULT result[2], *presult;

	ASSERT(wiphy);
	ASSERT(wdev);
	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, sizeof(PARAM_WIFI_CHANGE_RESULT),
					  GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS, GFP_KERNEL);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, sizeof(PARAM_WIFI_CHANGE_RESULT),
					  GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS, GFP_KERNEL);
#endif

	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}

	presult = result;
	kalMemZero(presult, (sizeof(PARAM_WIFI_CHANGE_RESULT) * 2));
	/* only for test */
	kalMemCopy(presult->bssid, "213123", sizeof(mac_addr));
	presult->channel = 2437;
	presult->rssi[0] = -45;
	presult->rssi[1] = -46;
	presult++;
	presult->channel = 2439;
	presult->rssi[0] = -47;
	presult->rssi[1] = -48;
	/*NLA_PUT(skb, GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS, (sizeof(PARAM_WIFI_CHANGE_RESULT) * 2), result);*/
	if (unlikely(nla_put(skb, GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS,
		(sizeof(PARAM_WIFI_CHANGE_RESULT) * 2), result) < 0))
		goto nla_put_failure;


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_event_hotlist_ap_found(struct wiphy *wiphy, struct wireless_dev *wdev,
					       P_PARAM_WIFI_GSCAN_RESULT pdata, UINT_32 data_len)
{
	struct sk_buff *skb;
	PARAM_WIFI_GSCAN_RESULT result[2], *presult;

	ASSERT(wiphy);
	ASSERT(wdev);
	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, sizeof(PARAM_WIFI_GSCAN_RESULT),
					  GSCAN_EVENT_HOTLIST_RESULTS_FOUND, GFP_KERNEL);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, sizeof(PARAM_WIFI_GSCAN_RESULT),
					  GSCAN_EVENT_HOTLIST_RESULTS_FOUND, GFP_KERNEL);
#endif

	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}

	presult = result;
	kalMemZero(presult, (sizeof(PARAM_WIFI_GSCAN_RESULT) * 2));
	/* only for test */
	kalMemCopy(presult->bssid, "123123", sizeof(mac_addr));
	presult->channel = 2441;
	presult->rssi = -45;
	presult++;
	presult->channel = 2443;
	presult->rssi = -47;
	/*NLA_PUT(skb, GSCAN_EVENT_HOTLIST_RESULTS_FOUND, (sizeof(PARAM_WIFI_GSCAN_RESULT) * 2), result);*/
	if (unlikely(nla_put(skb, GSCAN_EVENT_HOTLIST_RESULTS_FOUND,
		(sizeof(PARAM_WIFI_GSCAN_RESULT) * 2), result) < 0))
		goto nla_put_failure;


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}

int mtk_cfg80211_vendor_event_hotlist_ap_lost(struct wiphy *wiphy, struct wireless_dev *wdev,
					      P_PARAM_WIFI_GSCAN_RESULT pdata, UINT_32 data_len)
{
	struct sk_buff *skb;
	PARAM_WIFI_GSCAN_RESULT result[2], *presult;

	ASSERT(wiphy);
	ASSERT(wdev);
	DBGLOG(REQ, INFO, "%s for vendor command \r\n", __func__);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, sizeof(PARAM_WIFI_GSCAN_RESULT),
					  GSCAN_EVENT_HOTLIST_RESULTS_LOST, GFP_KERNEL);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, sizeof(PARAM_WIFI_GSCAN_RESULT),
					  GSCAN_EVENT_HOTLIST_RESULTS_LOST, GFP_KERNEL);
#endif

	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}

	presult = result;
	kalMemZero(presult, (sizeof(PARAM_WIFI_GSCAN_RESULT) * 2));
	/* only for test */
	kalMemCopy(presult->bssid, "321321", sizeof(mac_addr));
	presult->channel = 2445;
	presult->rssi = -46;
	presult++;
	presult->channel = 2447;
	presult->rssi = -48;
	/*NLA_PUT(skb, GSCAN_EVENT_HOTLIST_RESULTS_LOST, (sizeof(PARAM_WIFI_GSCAN_RESULT) * 2), result);*/
	if (unlikely(nla_put(skb, GSCAN_EVENT_HOTLIST_RESULTS_LOST,
		(sizeof(PARAM_WIFI_GSCAN_RESULT) * 2), result) < 0))
		goto nla_put_failure;


	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	return -1;
}


int mtk_cfg80211_vendor_event_rssi_beyond_range(struct wiphy *wiphy, struct wireless_dev *wdev, INT_32 rssi)
{
	struct sk_buff *skb;
	PARAM_RSSI_MONITOR_EVENT rRSSIEvt;
	P_BSS_INFO_T prAisBssInfo;
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(wiphy);
	ASSERT(wdev);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, TRACE, "vendor command rssi=%d\r\n", rssi);
	kalMemZero(&rRSSIEvt, sizeof(PARAM_RSSI_MONITOR_EVENT));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, sizeof(PARAM_RSSI_MONITOR_EVENT),
					  WIFI_EVENT_RSSI_MONITOR, GFP_KERNEL);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, sizeof(PARAM_RSSI_MONITOR_EVENT),
					  WIFI_EVENT_RSSI_MONITOR, GFP_KERNEL);
#endif
	if (!skb) {
		DBGLOG(REQ, ERROR, "%s allocate skb failed\n", __func__);
		return -ENOMEM;
	}

	prAisBssInfo = &(prGlueInfo->prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	kalMemCopy(rRSSIEvt.BSSID, prAisBssInfo->aucBSSID, sizeof(UINT_8) * MAC_ADDR_LEN);

	rRSSIEvt.version = 1; /* RSSI_MONITOR_EVT_VERSION = 1 */
	if (rssi > PARAM_WHQL_RSSI_MAX_DBM)
		rssi = PARAM_WHQL_RSSI_MAX_DBM;
	else if (rssi < -127)
		rssi = -127;
	rRSSIEvt.rssi = (INT_8)rssi;
	DBGLOG(REQ, INFO, "RSSI Event: version=%d, rssi=%d, BSSID=" MACSTR "\n",
		rRSSIEvt.version, rRSSIEvt.rssi, MAC2STR(rRSSIEvt.BSSID));

	/*NLA_PUT_U32(skb, GOOGLE_RSSI_MONITOR_EVENT, rssi);*/
	{
		/* unsigned int __tmp = rssi; */

		if (unlikely(nla_put(skb, WIFI_EVENT_RSSI_MONITOR,
			sizeof(PARAM_RSSI_MONITOR_EVENT), &rRSSIEvt) < 0))
			goto nla_put_failure;
	}

	cfg80211_vendor_event(skb, GFP_KERNEL);
	return 0;

nla_put_failure:
	kfree_skb(skb);
	return -ENOMEM;
}


int mtk_cfg80211_vendor_set_roaming_policy(struct wiphy *wiphy, struct wireless_dev *wdev,
					const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	struct nlattr *attr;
	UINT_32 setRoaming = 0;
	UINT_32 u4BufLen = 0;
	INT_32 i4Status = -EINVAL;

	ASSERT(wiphy);
	ASSERT(wdev);

	if ((data == NULL) || !data_len)
		goto nla_put_failure;

	attr = (struct nlattr *)data;
	setRoaming = nla_get_u32(attr);
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(REQ, INFO, "vendor command: data_len=%d, data=0x%x 0x%x, roaming policy=%d\r\n",
		data_len, *((UINT_32 *) data), *((UINT_32 *) data + 1), setRoaming);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetDrvRoamingPolicy,
			   &setRoaming, sizeof(UINT_32), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	return rStatus;

nla_put_failure:
	return i4Status;

}

int mtk_cfg80211_vendor_get_supported_feature_set(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int data_len)
{
	UINT_32 u4FeatureSet = WIFI_HAL_FEATURE_SET;
	P_GLUE_INFO_T prGlueInfo;
	P_REG_INFO_T prRegInfo;
	struct sk_buff *skb;

	ASSERT(wiphy);
	ASSERT(wdev);

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

	if (!prGlueInfo)
		return -EFAULT;
	prRegInfo = &(prGlueInfo->rRegInfo);
	if (!prRegInfo)
		return -EFAULT;

	if (prRegInfo->ucSupport5GBand)
		u4FeatureSet |= WIFI_FEATURE_INFRA_5G;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(u4FeatureSet));
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(
	    nla_put_nohdr(skb, sizeof(u4FeatureSet), &u4FeatureSet) < 0)) {
		DBGLOG(REQ, ERROR, "nla_put_nohdr failed\n");
		goto nla_put_failure;
	}

	DBGLOG(REQ, TRACE, "supported feature set=0x%x\n", u4FeatureSet);

	return cfg80211_vendor_cmd_reply(skb);

nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}

int mtk_cfg80211_vendor_get_version(struct wiphy *wiphy, struct wireless_dev *wdev,
				const void *data, int data_len)
{
	struct sk_buff *skb;
	struct nlattr *attrlist;
	char verInfoBuf[64];
	UINT_32 u4CopySize = 0;
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(wiphy);
	ASSERT(wdev);

	if ((data == NULL) || !data_len)
		return -ENOMEM;

	kalMemZero(verInfoBuf, 64);
	attrlist = (struct nlattr *)((UINT_8 *) data);
	if (attrlist->nla_type == LOGGER_ATTRIBUTE_DRIVER_VER) {
		char wifiDriverVersionStr[] = NIC_DRIVER_VERSION_STRING "-" DRIVER_BUILD_DATE;
		UINT_32 u4StrSize = strlen(wifiDriverVersionStr);

		u4CopySize = (u4StrSize >= 64) ? 63 : u4StrSize;
		strncpy(verInfoBuf, wifiDriverVersionStr, u4CopySize);
	} else if (attrlist->nla_type == LOGGER_ATTRIBUTE_FW_VER) {
		WIFI_VER_INFO_T *prVerInfo;

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);

		ASSERT(prGlueInfo);
		prVerInfo = &(prGlueInfo->prAdapter->rVerInfo);
		sprintf(verInfoBuf, "%x.%x.%x",
			(prVerInfo->u2FwOwnVersion >> 8),
			(prVerInfo->u2FwOwnVersion & 0xff),
			prVerInfo->u2FwPeerVersion);
		u4CopySize = strlen(verInfoBuf);
	}

	if (u4CopySize <= 0)
		return -EFAULT;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, u4CopySize);
	if (!skb) {
		DBGLOG(REQ, ERROR, "Allocate skb failed\n");
		return -ENOMEM;
	}

	if (unlikely(nla_put_nohdr(skb, u4CopySize, &verInfoBuf[0]) < 0))
		goto nla_put_failure;

	return cfg80211_vendor_cmd_reply(skb);
nla_put_failure:
	kfree_skb(skb);
	return -EFAULT;
}

int mtk_cfg80211_vendor_set_tx_power_scenario(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int data_len)
{
	/*Gen2 firmware do not support, just return*/
	return -EOPNOTSUPP;
}

int mtk_cfg80211_vendor_set_scan_mac_oui(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void *data, int data_len)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 rStatus = WLAN_STATUS_SUCCESS;
	struct nlattr *attr;
	UINT_32 i = 0;
	struct PARAM_BSS_MAC_OUI rParamMacOui;
	UINT_32 u4BufLen = 0;
	struct NETDEV_PRIVATE_GLUE_INFO *prNetDevPrivate = NULL;

	ASSERT(wiphy);
	ASSERT(wdev);

	if (data == NULL || data_len <= 0) {
		DBGLOG(REQ, ERROR, "data error(len=%d)\n", data_len);
		return -EINVAL;
	}
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (!prGlueInfo) {
		DBGLOG(REQ, ERROR, "Invalid glue info\n");
		return -EFAULT;
	}
	prNetDevPrivate =
		(struct NETDEV_PRIVATE_GLUE_INFO *) netdev_priv(wdev->netdev);
	if (!prNetDevPrivate) {
		DBGLOG(REQ, ERROR, "Invalid net device private\n");
		return -EFAULT;
	}
	rParamMacOui.ucBssIndex = prNetDevPrivate->ucBssIdx;

	attr = (struct nlattr *)data;
	kalMemZero(rParamMacOui.ucMacOui, MAC_OUI_LEN);
	if (nla_type(attr) != WIFI_ATTRIBUTE_PNO_RANDOM_MAC_OUI) {
		DBGLOG(REQ, ERROR, "Set MAC oui type error(%u)\n",
			nla_type(attr));
		return -EINVAL;
	}

	if (nla_len(attr) != MAC_OUI_LEN) {
		DBGLOG(REQ, ERROR, "Set MAC oui length error(%u), %u needed\n",
			nla_len(attr), MAC_OUI_LEN);
		return -EINVAL;
	}

	for (i = 0; i < MAC_OUI_LEN; i++)
		rParamMacOui.ucMacOui[i] = *((P_UINT_8)nla_data(attr) + i);

	DBGLOG(REQ, INFO, "Set MAC oui: %02x-%02x-%02x\n",
		rParamMacOui.ucMacOui[0], rParamMacOui.ucMacOui[1],
		rParamMacOui.ucMacOui[2]);

	rStatus = kalIoctl(prGlueInfo, wlanoidSetScanMacOui,
		&rParamMacOui, sizeof(rParamMacOui),
		FALSE, FALSE, FALSE, FALSE, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "Set MAC oui error: 0x%X\n", rStatus);
		return -EFAULT;
	}

	return 0;
}

#if CFG_AUTO_CHANNEL_SEL_SUPPORT
int
mtk_cfg80211_testmode_get_lte_channel(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo)
{
#define MAXMUN_2_4G_CHA_NUM 14
#define CHN_DIRTY_WEIGHT_UPPERBOUND 4

	BOOLEAN fgIsReady = FALSE, fgIsFistRecord = TRUE;
	BOOLEAN fgIsPureAP, fgIsLteSafeChn = FALSE;

	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_8 ucIdx = 0, ucMax_24G_Chn_List = 11, ucDefaultIdx = 0, ucArrayIdx = 0;
	UINT_16 u2APNumScore = 0, u2UpThreshold = 0, u2LowThreshold = 0, ucInnerIdx = 0;
	INT_32 i4Status = -EINVAL;
	UINT_32 u4BufLen, u4LteSafeChnBitMask_2_4G = 0;
	UINT32 AcsChnReport[4];
	/*RF_CHANNEL_INFO_T aucChannelList[MAXMUN_2_4G_CHA_NUM];*/

	struct sk_buff *skb;

	PARAM_GET_CHN_LOAD rQueryLTEChn;
	PARAM_PREFER_CHN_INFO rPreferChannels[2], ar2_4G_ChannelLoadingWeightScore[MAXMUN_2_4G_CHA_NUM];
	P_PARAM_CHN_LOAD_INFO prChnLoad;
	P_PARAM_GET_CHN_LOAD prGetChnLoad;

	P_DOMAIN_INFO_ENTRY prDomainInfo;

/*
	 P_PARAM_GET_CHN_LOAD prParams = NULL;
*/
	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	kalMemZero(rPreferChannels, sizeof(rPreferChannels));
	fgIsPureAP = prGlueInfo->prAdapter->rWifiVar.prP2pFsmInfo->fgIsApMode;
#if 0
	if (data && len)
		prParams = (P_NL80211_DRIVER_GET_LTE_PARAMS) data;
#endif
	skb = cfg80211_testmode_alloc_reply_skb(wiphy, sizeof(AcsChnReport) + sizeof(UINT8) + 1);
	if (!skb) {
		DBGLOG(QM, TRACE, "%s allocate skb failed:%x\n", __func__, rStatus);
		return -ENOMEM;
	}

	DBGLOG(P2P, INFO, "[Auto Channel]Get LTE Channels\n");

	kalMemZero(&rQueryLTEChn, sizeof(rQueryLTEChn));

	/* Query LTE Safe Channels */
	rQueryLTEChn.rLteSafeChnList.au4SafeChannelBitmask[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1] = 0xFFFFFFFF;

	rQueryLTEChn.rLteSafeChnList.au4SafeChannelBitmask[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34 - 1] = 0xFFFFFFFF;

	rQueryLTEChn.rLteSafeChnList.au4SafeChannelBitmask[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149 - 1] =
	    0xFFFFFFFF;

	rQueryLTEChn.rLteSafeChnList.au4SafeChannelBitmask[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184 - 1] =
	    0xFFFFFFFF;
	rStatus =
	    kalIoctl(prGlueInfo, wlanoidQueryACSChannelList, &rQueryLTEChn, sizeof(rQueryLTEChn), TRUE, FALSE, TRUE,
		     TRUE, &u4BufLen);
#if 0
	if (fgIsPureAP) {

		AcsChnRepot[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1] = 0x20;	/* Channel 6 */
	} else
#endif
	{
		fgIsReady = prGlueInfo->prAdapter->rWifiVar.rChnLoadInfo.fgDataReadyBit;
		rPreferChannels[0].u2APNum = 0xFFFF;
		rPreferChannels[1].u2APNum = 0xFFFF;

		/* 4 In LTE Mode, Hotspot pick up channels from ch4. */
		ucDefaultIdx = 0;
		/*
		   if (fgIsPureAP) {
		   ucDefaultIdx=3; //SKIP LTE Channels 1~3
		   }
		 */

		/* 4 Get the Maximun channel List in 2.4G Bands */

		prDomainInfo = rlmDomainGetDomainInfo(prGlueInfo->prAdapter);
		ASSERT(prDomainInfo);

		/* 4 ToDo: Enable Step 2 only if we could get Country Code from framework */
		/* 4 2. Get current domain channel list */

#if 0
		rlmDomainGetChnlList(prGlueInfo->prAdapter,
				     BAND_2G4, MAXMUN_2_4G_CHA_NUM, &ucMax_24G_Chn_List, aucChannelList);
#endif

		prGetChnLoad = (P_PARAM_GET_CHN_LOAD) &(prGlueInfo->prAdapter->rWifiVar.rChnLoadInfo);
		for (ucIdx = 0; ucIdx < ucMax_24G_Chn_List; ucIdx++) {
			DBGLOG(P2P, INFO,
			       "[Auto Channel] ch[%d]=%d\n", ucIdx,
				prGetChnLoad->rEachChnLoad[ucIdx + ucInnerIdx].u2APNum);
		}

		/*Calculate Each Channel Direty Score */
		for (ucIdx = ucDefaultIdx; ucIdx < ucMax_24G_Chn_List; ucIdx++) {

#if 1
			u2APNumScore = prGetChnLoad->rEachChnLoad[ucIdx].u2APNum * CHN_DIRTY_WEIGHT_UPPERBOUND;
			u2UpThreshold = u2LowThreshold = 3;

			if (ucIdx < 3) {
				u2UpThreshold = ucIdx;
				u2LowThreshold = 3;
			} else if (ucIdx >= (ucMax_24G_Chn_List - 3)) {
				u2UpThreshold = 3;
				u2LowThreshold = ucMax_24G_Chn_List - (ucIdx + 1);

			}

			/*Calculate Lower Channel Dirty Score */
			for (ucInnerIdx = 0; ucInnerIdx < u2LowThreshold; ucInnerIdx++) {
				ucArrayIdx = ucIdx + ucInnerIdx + 1;
				if (ucArrayIdx < MAX_AUTO_CHAL_NUM) {
					u2APNumScore +=
					    (prGetChnLoad->rEachChnLoad[ucArrayIdx].u2APNum *
					     (CHN_DIRTY_WEIGHT_UPPERBOUND - 1 - ucInnerIdx));
				}
			}

			/*Calculate Upper Channel Dirty Score */
			for (ucInnerIdx = 0; ucInnerIdx < u2UpThreshold; ucInnerIdx++) {
				ucArrayIdx = ucIdx - ucInnerIdx - 1;
				if (ucArrayIdx < MAX_AUTO_CHAL_NUM) {
					u2APNumScore +=
					    (prGetChnLoad->rEachChnLoad[ucArrayIdx].u2APNum *
					     (CHN_DIRTY_WEIGHT_UPPERBOUND - 1 - ucInnerIdx));
				}
			}

			ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum = u2APNumScore;

			DBGLOG(P2P, INFO, "[Auto Channel]chn=%d score=%d\n", ucIdx, u2APNumScore);
#else
			if (ucIdx == 0) {
				/* ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum =
				(prGetChnLoad->rEachChnLoad[ucIdx].u2APNum +
				prGetChnLoad->rEachChnLoad[ucIdx+1].u2APNum*0.75); */
				u2APNumScore = (prGetChnLoad->rEachChnLoad[ucIdx].u2APNum + ((UINT_16)
											     ((3 *
											       (prGetChnLoad->
												rEachChnLoad[ucIdx +
													     1].
												u2APNum +
												prGetChnLoad->
												rEachChnLoad[ucIdx +
													     2].
												u2APNum)) / 4)));

				ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum = u2APNumScore;
				DBGLOG(P2P, INFO,
				       "[Auto Channel]ucIdx=%d score=%d=%d+0.75*%d\n", ucIdx,
					ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx + 1].u2APNum));
			}
			if ((ucIdx > 0) && (ucIdx < (MAXMUN_2_4G_CHA_NUM - 1))) {
				u2APNumScore = (prGetChnLoad->rEachChnLoad[ucIdx].u2APNum + ((UINT_16)
											     ((3 *
											       (prGetChnLoad->
												rEachChnLoad[ucIdx +
													     1].
												u2APNum +
												prGetChnLoad->
												rEachChnLoad[ucIdx -
													     1].
												u2APNum)) / 4)));

				ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum = u2APNumScore;
				DBGLOG(P2P, INFO,
				       "[Auto Channel]ucIdx=%d score=%d=%d+0.75*%d+0.75*%d\n", ucIdx,
					ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx + 1].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx - 1].u2APNum));
			}

			if (ucIdx == (MAXMUN_2_4G_CHA_NUM - 1)) {
				u2APNumScore = (prGetChnLoad->rEachChnLoad[ucIdx].u2APNum +
						((UINT_16) ((3 * prGetChnLoad->rEachChnLoad[ucIdx - 1].u2APNum) / 4)));

				ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum = u2APNumScore;
				DBGLOG(P2P, INFO,
				       "[Auto Channel]ucIdx=%d score=%d=%d+0.75*%d\n", ucIdx,
					ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx].u2APNum,
					prGetChnLoad->rEachChnLoad[ucIdx - 1].u2APNum));
			}
#endif

		}

		u4LteSafeChnBitMask_2_4G =
		    rQueryLTEChn.rLteSafeChnList.au4SafeChannelBitmask[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1];

		/*Find out the best channel */
		for (ucIdx = ucDefaultIdx; ucIdx < ucMax_24G_Chn_List; ucIdx++) {
			/* 4 Skip LTE Unsafe Channel */
			fgIsLteSafeChn = ((u4LteSafeChnBitMask_2_4G & BIT(ucIdx + 1)) >> ucIdx);
			if (!fgIsLteSafeChn)
				continue;

			prChnLoad =
			    (P_PARAM_CHN_LOAD_INFO) &(prGlueInfo->prAdapter->rWifiVar.
						       rChnLoadInfo.rEachChnLoad[ucIdx]);
			if (rPreferChannels[0].u2APNum >= ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum) {
				rPreferChannels[1].ucChannel = rPreferChannels[0].ucChannel;
				rPreferChannels[1].u2APNum = rPreferChannels[0].u2APNum;

				rPreferChannels[0].ucChannel = ucIdx;
				rPreferChannels[0].u2APNum = ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum;
			} else {
				if (rPreferChannels[1].u2APNum >= ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum
				    || fgIsFistRecord == 1) {
					fgIsFistRecord = FALSE;
					rPreferChannels[1].ucChannel = ucIdx;
					rPreferChannels[1].u2APNum = ar2_4G_ChannelLoadingWeightScore[ucIdx].u2APNum;
				}
			}
		}
		/* AcsChnRepot[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1-1] =
		BITS((rQueryLTEChn.rLteSafeChnList.ucChannelLow-1),(rQueryLTEChn.rLteSafeChnList.ucChannelHigh-1)); */
		AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1] = fgIsReady ? BIT(31) : 0;
		AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1] |= BIT(rPreferChannels[0].ucChannel);
	}

	/* ToDo: Support 5G Channel Selection */
	AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34 - 1] = 0x11223344;
	AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149 - 1] = 0x55667788;
	AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184 - 1] = 0x99AABBCC;

	/*NLA_PUT_U8(skb, NL80211_TESTMODE_AVAILABLE_CHAN_INVALID, 0);*/
	{
		unsigned char __tmp = 0;

		if (unlikely(nla_put(skb, NL80211_TESTMODE_AVAILABLE_CHAN_INVALID, sizeof(unsigned char), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1,
		    AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1]);*/
	{
		unsigned int __tmp = AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1 - 1];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34,
		    AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34 - 1]);*/
	{
		unsigned int __tmp = AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34 - 1];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	/*NLA_PUT_U32(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149,
		    AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149 - 1]);*/
	{
		unsigned int __tmp = AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149 - 1];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}


	/*NLA_PUT_U32(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184,
		    AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184 - 1]);*/
	{
		unsigned int __tmp = AcsChnReport[NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184 - 1];

		if (unlikely(nla_put(skb, NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184,
			sizeof(unsigned int), &__tmp) < 0))
			goto nla_put_failure;
	}

	DBGLOG(P2P, INFO,
	       "[Auto Channel]Relpy AcsChanInfo[%x:%x:%x:%x]\n", AcsChnReport[0], AcsChnReport[1], AcsChnReport[2],
		AcsChnReport[3]);

	i4Status = cfg80211_testmode_reply(skb);

nla_put_failure:
	return i4Status;

}
#endif
#endif

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
/*----------------------------------------------------------------------------*/
/*!
 * @brief cfg80211 suspend callback, will be invoked in wiphy_suspend
 *
 * @param wiphy: pointer to wiphy
 *		wow: pointer to cfg80211_wowlan
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
int	mtk_cfg80211_suspend(struct wiphy *wiphy, struct cfg80211_wowlan *wow)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	down(&g_halt_sem);
	if (g_u4HaltFlag || !wiphy)
		goto end;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	set_bit(SUSPEND_FLAG_FOR_WAKEUP_REASON, &prGlueInfo->prAdapter->ulSuspendFlag);
	if (glIsDataStatEnabled()) {
		set_bit(SUSPEND_FLAG_FOR_APP_TRX_STAT, &prGlueInfo->prAdapter->ulSuspendFlag);
		glLogSuspendResumeTime(TRUE);
	}
end:
	up(&g_halt_sem);
	return 0;
}

static VOID wlanNotifyTRxOutputWork(PUINT_8 pucParams)
{
	UINT_32 u4InfoLen = 0;
	P_GLUE_INFO_T prGlueInfo = *(P_GLUE_INFO_T *)pucParams;

	DBGLOG(INIT, INFO, "Output App Tx Rx statistics\n");
	kalIoctl(prGlueInfo, wlanoidNotifyTRxStats, NULL, 0, FALSE, FALSE, FALSE, FALSE, &u4InfoLen);

}

int mtk_cfg80211_resume(struct wiphy *wiphy)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	down(&g_halt_sem);
	if (g_u4HaltFlag || !wiphy)
		goto end;

	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	if (glIsDataStatEnabled() &&
		test_and_clear_bit(SUSPEND_FLAG_FOR_APP_TRX_STAT, &prGlueInfo->prAdapter->ulSuspendFlag) &&
		kalWakeupFromSleep()) {
		static UINT_8 aucPsyWorkBuf[sizeof(struct DRV_COMMON_WORK_FUNC_T) + sizeof(prGlueInfo)];
		struct DRV_COMMON_WORK_FUNC_T *prPsyWork = (struct DRV_COMMON_WORK_FUNC_T *)&aucPsyWorkBuf[0];
		P_GLUE_INFO_T *pprGlueInfo = (P_GLUE_INFO_T *)prPsyWork->params;

		glLogSuspendResumeTime(FALSE);
		*pprGlueInfo = prGlueInfo;
		prPsyWork->work_func = wlanNotifyTRxOutputWork;
		kalScheduleCommonWork(&prGlueInfo->rDrvWork, prPsyWork);
	}
end:
	up(&g_halt_sem);
	return 0;
}
#endif

INT_32 mtk_cfg80211_process_str_cmd(P_GLUE_INFO_T prGlueInfo, PUINT_8 cmd, INT_32 len)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
#if (CFG_SUPPORT_802_11K || CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT)
	UINT_32 u4SetInfoLen = 0;
#endif

	DBGLOG(REQ, INFO, "--> %s()\n", __func__);

	if (kalStrniCmp(cmd, "NEIGHBOR-REQUEST", 16) == 0) {
		PUINT_8 pucSSID = NULL;
		UINT_32 u4SSIDLen = 0;

		if (len > 16 && (kalStrniCmp(cmd+16, " SSID=", 6) == 0)) {
			pucSSID = cmd + 22;
			u4SSIDLen = len - 22;
			DBGLOG(REQ, INFO, "cmd=%s, ssid len %u, ssid=%s\n", cmd, u4SSIDLen, pucSSID);
		}
#if CFG_SUPPORT_802_11K		
		rStatus = kalIoctl(prGlueInfo, wlanoidSendNeighborRequest,
				   (PVOID)pucSSID, u4SSIDLen, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);
#endif
	} else if (kalStrniCmp(cmd, "BSS-TRANSITION-QUERY", 20) == 0) {
		PUINT_8 pucReason = NULL;

		if (len > 20 && (kalStrniCmp(cmd+20, " reason=", 8) == 0))
			pucReason = cmd + 28;
#if CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT
		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSendBTMQuery,
				   (PVOID)pucReason, 1, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);
#endif
	} else
		return -EOPNOTSUPP;

	if (rStatus == WLAN_STATUS_SUCCESS)
		return 0;

	return -EINVAL;
}

#if CFG_SUPPORT_802_11R
int mtk_cfg80211_update_ft_ies(struct wiphy *wiphy, struct net_device *dev,
				 struct cfg80211_update_ft_ies_params *ftie)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4InfoBufLen = 0;
	WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;

	if (!wiphy)
		return -1;
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(wiphy);
	rStatus = kalIoctl(prGlueInfo, wlanoidUpdateFtIes, (PVOID)ftie, sizeof(*ftie), FALSE,
					FALSE, FALSE, FALSE, &u4InfoBufLen);
	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(OID, INFO, "update Ft IE failed\n");
	return 0;
}
#endif
#if CFG_SUPPORT_RSSI_STATISTICS
int
mtk_cfg80211_testmode_get_rssi_statistics(IN struct wiphy
		*wiphy, IN void *data, IN int len,
		P_GLUE_INFO_T prGlueInfo)
{
	uint32_t rStatus = WLAN_STATUS_SUCCESS;
	struct PARAM_GET_RSSI_STATISTICS rQueryRssiStatistics;

	struct NL80211_DRIVER_GET_RSSI_STATISTICS_PARAMS *prParams = NULL;
	struct sk_buff *skb;
	uint8_t ucData = 0;
	uint32_t u4BufLen = 0;
	uint32_t u4Data = 0;

	ASSERT(wiphy);
	ASSERT(prGlueInfo);

	DBGLOG(QM, TRACE, "mtk_cfg80211_testmode_get_rssi_statistics\n");

	if (!data || !len) {
		DBGLOG(QM, ERROR, "data or len is invalid len=%d\n", len);
		return -EINVAL;
	}

	prParams = (struct NL80211_DRIVER_GET_RSSI_STATISTICS_PARAMS *) data;
	if (prParams == NULL) {
		DBGLOG(QM, ERROR, "prParams is NULL, data=%p, len=%d\n",
		       data, len);
		return -EINVAL;
	}

	skb = cfg80211_testmode_alloc_reply_skb(wiphy,
				sizeof(struct PARAM_GET_RSSI_STATISTICS) + 1);
	if (!skb) {
		DBGLOG(QM, ERROR, "allocate skb failed:%x\n", rStatus);
		return -ENOMEM;
	}

	kalMemZero(&rQueryRssiStatistics,
		   sizeof(rQueryRssiStatistics));

	rStatus = kalIoctl(prGlueInfo, wlanoidQueryRssiStatistics,
				   &rQueryRssiStatistics,
				   sizeof(rQueryRssiStatistics),
				   TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(OID, INFO, "Query Rssi statistics failed\n");
		goto nla_put_failure;
	}

	DBGLOG(REQ, TRACE, "dump rssi testmode  [%d,%d],[%d,%d],[%d,%d],[%d,%d]\n",
				rQueryRssiStatistics.arRxRssiStatistics.ucAuthRcpi,
				rQueryRssiStatistics.arRxRssiStatistics.ucAuthRetransmission,
				rQueryRssiStatistics.arRxRssiStatistics.ucAssocRcpi,
				rQueryRssiStatistics.arRxRssiStatistics.ucAssocRetransmission,
				rQueryRssiStatistics.arRxRssiStatistics.ucM1Rcpi,
				rQueryRssiStatistics.arRxRssiStatistics.ucM1Retransmission,
				rQueryRssiStatistics.ucAisConnectionStatus,
				rQueryRssiStatistics.u4RxPktNum);


	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucAssocRcpi;
	if (unlikely(nla_put(skb,
		NL80211_TESTMODE_ASS_RCPI_STATISTICS, sizeof(u8),
		&ucData) < 0)) {
		DBGLOG(QM, ERROR, "put assoc rssi fail\n");
		goto nla_put_failure;
	}
	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucAssocRetransmission;
	if (unlikely(nla_put(skb,
		NL80211_TESTMODE_ASS_RETRANMISSION_STATISTICS, sizeof(u8),
		&ucData) < 0)) {
		DBGLOG(QM, ERROR, "put assoc retransmission flag fail\n");
		goto nla_put_failure;
	}

	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucAuthRcpi;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_AUTH_RCPI_STATISTICS, sizeof(u8),
		    &ucData) < 0)) {
			DBGLOG(QM, ERROR, "put auth rssi fail\n");
			goto nla_put_failure;
	}
	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucAuthRetransmission;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_AUTH_RETRANMISSION_STATISTICS, sizeof(u8),
		    &ucData) < 0)) {
			DBGLOG(QM, ERROR, "put auth retransmission flag fail\n");
			goto nla_put_failure;
	}

	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucM1Rcpi;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_EAPOL_RCPI_STATISTICS, sizeof(u8),
		    &ucData) < 0)) {
			DBGLOG(QM, ERROR, "put eapol rssi fail\n");
			goto nla_put_failure;
	}
	ucData = rQueryRssiStatistics.arRxRssiStatistics.ucM1Retransmission;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_EAPOL_RETRANMISSION_STATISTICS, sizeof(u8),
		    &ucData) < 0)) {
			DBGLOG(QM, ERROR, "put eapol retransmission flag fail\n");
			goto nla_put_failure;
	}

	ucData = rQueryRssiStatistics.ucAisConnectionStatus;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_AIS_CONNECTION_STATISTICS, sizeof(u8),
		    &ucData) < 0)) {
			DBGLOG(QM, ERROR, "put ais connection status fail\n");
			goto nla_put_failure;
	}

	u4Data = rQueryRssiStatistics.u4RxPktNum;
	if (unlikely(nla_put(skb,
		    NL80211_TESTMODE_RX_COUNT_STATISTICS, sizeof(u32),
		    &u4Data) < 0)) {
			DBGLOG(QM, ERROR, "put Rx count fail\n");
			goto nla_put_failure;
	}

	return cfg80211_testmode_reply(skb);
nla_put_failure:
	/* nal_put_skb_fail */
	kfree_skb(skb);

	return -EFAULT;
}

#endif

