/*  SPDX-License-Identifier: GPL-2.0 */  

/*

 * Copyright (c) 2019 MediaTek Inc.

 */

/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/include/gl_cfg80211.h#1
*/

/*! \file   gl_cfg80211.h
    \brief  This file is for Portable Driver linux cfg80211 support.
*/

/*
** Log: gl_cfg80211.h
**
** 09 03 2013 cp.wu
** add path for reassociation
**
** 09 12 2012 wcpadmin
** [ALPS00276400] Remove MTK copyright and legal header on GPL/LGPL related packages
** .
**
** 08 30 2012 chinglan.wang
** [ALPS00349664] [6577JB][WIFI] Phone can not connect to AP secured with AES via WPS in 802.11n Only
** .
 *
*/

#ifndef _GL_CFG80211_H
#define _GL_CFG80211_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <net/cfg80211.h>

#include "gl_os.h"
extern void wlanHandleSystemResume(void);
extern void wlanHandleSystemSuspend(void);
extern void p2pHandleSystemResume(void);
extern void p2pHandleSystemSuspend(void);

#if CFG_SUPPORT_WAPI
extern UINT_8 keyStructBuf[1024];	/* add/remove key shared buffer */
#else
extern UINT_8 keyStructBuf[100];	/* add/remove key shared buffer */
#endif

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
extern struct semaphore g_halt_sem;
extern int g_u4HaltFlag;
#endif

extern struct delayed_work sched_workq;

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#if CONFIG_NL80211_TESTMODE
#define NL80211_DRIVER_TESTMODE_VERSION 2
#endif

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

#if CONFIG_NL80211_TESTMODE

typedef struct _NL80211_DRIVER_GET_STA_STATISTICS_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_32 u4Version;
	UINT_32 u4Flag;
	UINT_8 aucMacAddr[MAC_ADDR_LEN];
} NL80211_DRIVER_GET_STA_STATISTICS_PARAMS, *P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS;

#if CFG_SUPPORT_WAKEUP_STATISTICS
typedef struct _NL80211_QUERY_WAKEUP_STATISTICS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	struct _WAKEUP_STATISTIC *prWakeupCount;
} NL80211_QUERY_WAKEUP_STATISTICS, *P_NL80211_QUERY_WAKEUP_STATISTICS;
#endif

typedef struct _NL80211_DRIVER_POORLINK_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	INT_8 cRssi;		/* cRssi=0 means it is a invalid value. */
	UINT_8 ucLinkSpeed;	/* ucLinkSpeed=0 means it is a invalid value */
	UINT_16 u2Reserved;
} NL80211_DRIVER_POORLINK_PARAMS, *P_NL80211_DRIVER_POORLINK_PARAMS;

typedef enum _ENUM_TESTMODE_STA_STATISTICS_ATTR {
	NL80211_TESTMODE_STA_STATISTICS_INVALID = 0,
	NL80211_TESTMODE_STA_STATISTICS_VERSION,
	NL80211_TESTMODE_STA_STATISTICS_MAC,
	NL80211_TESTMODE_STA_STATISTICS_LINK_SCORE,
	NL80211_TESTMODE_STA_STATISTICS_FLAG,

	NL80211_TESTMODE_STA_STATISTICS_PER,
	NL80211_TESTMODE_STA_STATISTICS_RSSI,
	NL80211_TESTMODE_STA_STATISTICS_PHY_MODE,
	NL80211_TESTMODE_STA_STATISTICS_TX_RATE,

	NL80211_TESTMODE_STA_STATISTICS_TOTAL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_THRESHOLD_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_PROCESS_TIME,
	NL80211_TESTMODE_STA_STATISTICS_TX_PKTS,
	NL80211_TESTMODE_STA_STATISTICS_TX_BYTES,
	NL80211_TESTMODE_STA_STATISTICS_RX_PKTS,
	NL80211_TESTMODE_STA_STATISTICS_RX_BYTES,

	NL80211_TESTMODE_STA_STATISTICS_FAIL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_TIMEOUT_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_AIR_TIME,

	NL80211_TESTMODE_STA_STATISTICS_TC_EMPTY_CNT_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_TC_AVG_QUE_LEN_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_CUR_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_RESERVED_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_NUM
} ENUM_TESTMODE_STA_STATISTICS_ATTR;

/*RX FILTER */
struct NL80211_DRIVER_RX_FILTER_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	enum PARAM_RX_FILTER_OPCODE_T eOpcode;
	UINT_8 ucPatternIndex;
	UINT_8 aucPattern[64];
	UINT_8 aucPatternBitMask[64];
	UINT_32 u4Offset;
	UINT_32 u4Length;
	BOOLEAN fsIsWhiteList;
	BOOLEAN fgIsEqual;
};

enum ENUM_TESTMODE_RX_FILTER_ATTR {
	NL80211_TESTMODE_RX_FILTER_INVALID = 0,
	NL80211_TESTMODE_RX_FILTER_RETURN,
	NL80211_TESTMODE_RX_FILTER_NUM
};

/*TX GENERATOR */
struct NL80211_DRIVER_TX_GENERATOR_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	enum PARAM_RX_FILTER_OPCODE_T eOpcode;
	UINT_8 ucPatternIndex;
	UINT_8 aucEthernetFrame[ETHER_MAX_PKT_SIZE];
	UINT_16 u2EthernetLength;
	UINT_32 u4Period;
};

enum ENUM_TESTMODE_TX_GENERATOR_ATTR {
	NL80211_TESTMODE_TX_GENERATOR_INVALID = 0,
	NL80211_TESTMODE_TX_GENERATOR_RETURN,
	NL80211_TESTMODE_TX_GENERATOR_NUM
};

/*TCP GENERATOR */
struct NL80211_DRIVER_TCP_GENERATOR_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	enum PARAM_RX_FILTER_OPCODE_T eOpcode;
	UINT_8 ucPatternIndex;
	UINT_32 u4Period;
	UINT_32 u4Timeout;
	UINT_8 aucDestMac[6];
	UINT_8 aucDestIp[4];
	UINT_8 aucSrcIp[4];
	UINT_32 ipId;
	UINT_32 u4DestPort;
	UINT_32 u4SrcPort;
	UINT_32 u4Seq;
};

enum ENUM_TESTMODE_TCP_GENERATOR_ATTR {
	NL80211_TESTMODE_TCP_GENERATOR_INVALID = 0,
	NL80211_TESTMODE_TCP_GENERATOR_RETURN,
	NL80211_TESTMODE_TCP_GENERATOR_NUM
};

/*TCP TIMEOUT*/
struct EVENT_HEARTBEAT_INFO_T {
	UINT_8 ucTimeoutSessionID;
	UINT_8 aucReserved[3];
};

enum ENUM_TESTMODE_TCP_HEARTBEAT_ATTR {
	NL80211_TESTMODE_TCP_HEARTBEAT_INVALID = 0,
	NL80211_TESTMODE_TCP_HEARTBEAT_TIMEOUT,
	NL80211_TESTMODE_TCP_HEARTBEAT_NUM
};

typedef struct _NL80211_DRIVER_SET_NFC_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_32 NFC_Enable;

} NL80211_DRIVER_SET_NFC_PARAMS, *P_NL80211_DRIVER_SET_NFC_PARAMS;
typedef struct _NL80211_DRIVER_GET_SCANDONE_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_32 u4ScanDone;

} NL80211_DRIVER_GET_SCANDONE_PARAMS, *P_NL80211_DRIVER_GET_SCANDONE_PARAMS;

typedef enum _ENUM_TESTMODE_LINK_DETECTION_ATTR {
	NL80211_TESTMODE_LINK_INVALID = 0,
	NL80211_TESTMODE_LINK_TX_FAIL_CNT,
	NL80211_TESTMODE_LINK_TX_RETRY_CNT,
	NL80211_TESTMODE_LINK_TX_MULTI_RETRY_CNT,
	NL80211_TESTMODE_LINK_ACK_FAIL_CNT,
	NL80211_TESTMODE_LINK_FCS_ERR_CNT,

	NL80211_TESTMODE_LINK_DETECT_NUM,
} ENUM_TESTMODE_LINK_DETECTION_ATTR;

enum ENUM_TESTMODE_COUNTRY_GET_ATTR {
	NL80211_TESTMODE_COUNTRY_GET_INVALID = 0,
	NL80211_TESTMODE_COUNTRY_GET_COUNTRY0,
	NL80211_TESTMODE_COUNTRY_GET_COUNTRY1,

	NL80211_TESTMODE_COUNTRY_GET_NUM
};

#if CFG_AUTO_CHANNEL_SEL_SUPPORT

typedef struct _NL80211_DRIVER_GET_LTE_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_32 u4Version;
	UINT_32 u4Flag;

} NL80211_DRIVER_GET_LTE_PARAMS, *P_NL80211_DRIVER_GET_LTE_PARAMS;

/*typedef enum _ENUM_TESTMODE_AVAILABLE_CHAN_ATTR{
	NL80211_TESTMODE_AVAILABLE_CHAN_INVALID = 0,
	NL80211_TESTMODE_AVAILABLE_CHAN_2G_BASE_1,
	NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_34,
	NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_149,
	NL80211_TESTMODE_AVAILABLE_CHAN_5G_BASE_184,

	NL80211_TESTMODE_AVAILABLE_CHAN_NUM,
}ENUM_TESTMODE_AVAILABLE_CHAN_ATTR;*/

#endif
#if CFG_SUPPORT_RSSI_STATISTICS
enum nl80211_testmode_rssi_statistics_attr{
    NL80211_TESTMODE_RSSI_STATISTICS_INVALID = 0,
    NL80211_TESTMODE_AUTH_RCPI_STATISTICS,
    NL80211_TESTMODE_AUTH_RETRANMISSION_STATISTICS,
    NL80211_TESTMODE_ASS_RCPI_STATISTICS,
    NL80211_TESTMODE_ASS_RETRANMISSION_STATISTICS,
    NL80211_TESTMODE_EAPOL_RCPI_STATISTICS,
    NL80211_TESTMODE_EAPOL_RETRANMISSION_STATISTICS,
    NL80211_TESTMODE_RX_COUNT_STATISTICS,
    NL80211_TESTMODE_TX_COUNT_STATISTICS,
    NL80211_TESTMODE_AIS_CONNECTION_STATISTICS,
    NL80211_TESTMODE_RSSI_STATISTICS_NUM
};

struct NL80211_DRIVER_GET_RSSI_STATISTICS_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	uint32_t u4Version;
	uint32_t u4Flag;
};
#endif
#endif
/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/* cfg80211 hooks */
int
mtk_cfg80211_change_iface(struct wiphy *wiphy,
			  struct net_device *ndev, enum nl80211_iftype type, u32 *flags, struct vif_params *params);

int
mtk_cfg80211_add_key(struct wiphy *wiphy,
		     struct net_device *ndev,
		     u8 key_index, bool pairwise, const u8 *mac_addr, struct key_params *params);

int
mtk_cfg80211_get_key(struct wiphy *wiphy,
		     struct net_device *ndev,
		     u8 key_index,
		     bool pairwise,
		     const u8 *mac_addr, void *cookie, void (*callback) (void *cookie, struct key_params *)
);

int
mtk_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev, u8 key_index, bool pairwise, const u8 *mac_addr);

int
mtk_cfg80211_set_default_key(struct wiphy *wiphy, struct net_device *ndev, u8 key_index, bool unicast, bool multicast);

int mtk_cfg80211_set_default_mgmt_key(struct wiphy *wiphy, struct net_device *netdev, u8 key_index);

int mtk_cfg80211_get_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac, struct station_info *sinfo);

int mtk_cfg80211_add_station(struct wiphy *wiphy, struct net_device *ndev,
				const u8 *mac, struct station_parameters *params);

int mtk_cfg80211_change_station(struct wiphy *wiphy, struct net_device *ndev,
				const u8 *mac, struct station_parameters *params);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
int mtk_cfg80211_del_station(struct wiphy *wiphy, struct net_device *ndev, struct station_del_parameters *params);
#else
int mtk_cfg80211_del_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac);
#endif

int mtk_cfg80211_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request);

int mtk_cfg80211_connect(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_connect_params *sme);

int mtk_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *ndev, u16 reason_code);

int mtk_cfg80211_join_ibss(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_ibss_params *params);

int mtk_cfg80211_leave_ibss(struct wiphy *wiphy, struct net_device *ndev);

int mtk_cfg80211_set_power_mgmt(struct wiphy *wiphy, struct net_device *ndev, bool enabled, int timeout);

int mtk_cfg80211_set_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa);

int mtk_cfg80211_del_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa);

int mtk_cfg80211_flush_pmksa(struct wiphy *wiphy, struct net_device *ndev);

int mtk_cfg80211_remain_on_channel(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   struct ieee80211_channel *chan,
				   unsigned int duration, u64 *cookie);

int mtk_cfg80211_cancel_remain_on_channel(struct wiphy *wiphy, struct wireless_dev *wdev, u64 cookie);

int
mtk_cfg80211_mgmt_tx(struct wiphy *wiphy,
		     struct wireless_dev *wdev,
		     struct cfg80211_mgmt_tx_params *params,
		     u64 *cookie);

void mtk_cfg80211_mgmt_frame_register(IN struct wiphy *wiphy,
				      IN struct wireless_dev *wdev,
				      IN u16 frame_type, IN bool reg);

int mtk_cfg80211_mgmt_tx_cancel_wait(struct wiphy *wiphy, struct wireless_dev *wdev, u64 cookie);

int mtk_cfg80211_assoc(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_assoc_request *req);

int
mtk_cfg80211_sched_scan_start(IN struct wiphy *wiphy,
			      IN struct net_device *ndev, IN struct cfg80211_sched_scan_request *request);

int mtk_cfg80211_sched_scan_stop(IN struct wiphy *wiphy, IN struct net_device *ndev);

#if CONFIG_NL80211_TESTMODE
#if CFG_AUTO_CHANNEL_SEL_SUPPORT
WLAN_STATUS
wlanoidQueryACSChannelList(IN P_ADAPTER_T prAdapter,
			   IN PVOID pvQueryBuffer, IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

int
mtk_cfg80211_testmode_get_lte_channel(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo);
#endif
int
mtk_cfg80211_testmode_get_sta_statistics(IN struct wiphy *wiphy,
					 IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo);

int
mtk_cfg80211_testmode_country_get(IN struct wiphy *wiphy,
					IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo);

int mtk_cfg80211_testmode_get_scan_done(IN struct wiphy *wiphy, IN void *data, IN int len, IN P_GLUE_INFO_T prGlueInfo);

int mtk_cfg80211_testmode_cmd(IN struct wiphy *wiphy, IN struct wireless_dev *wdev, IN void *data, IN int len);

int mtk_cfg80211_testmode_sw_cmd(IN struct wiphy *wiphy, IN void *data, IN int len);
int kalHandleTcpHeartbeatTimeout(IN P_GLUE_INFO_T prGlueInfo, IN struct EVENT_HEARTBEAT_INFO_T *prHeartbeatInfo);
#if CFG_SUPPORT_WAPI
int mtk_cfg80211_testmode_set_key_ext(IN struct wiphy *wiphy, IN void *data, IN int len);
#endif

#if CFG_SUPPORT_HOTSPOT_2_0
int mtk_cfg80211_testmode_hs20_cmd(IN struct wiphy *wiphy, IN void *data, IN int len);
#endif

#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_WIFI_DIRECT_CFG_80211
int mtk_p2p_cfg80211_testmode_sw_cmd(IN struct wiphy *wiphy, IN void *data, IN int len);
#endif

#else
#error "Please ENABLE kernel config (CONFIG_NL80211_TESTMODE) to support Wi-Fi Direct"
#endif

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
int mtk_cfg80211_suspend(struct wiphy *wiphy, struct cfg80211_wowlan *wow);
int mtk_cfg80211_resume(struct wiphy *wiphy);
#endif

INT_32 mtk_cfg80211_process_str_cmd(P_GLUE_INFO_T prGlueInfo, PUINT_8 cmd, INT_32 len);


#if CFG_SUPPORT_802_11R
	int mtk_cfg80211_update_ft_ies(struct wiphy *wiphy, struct net_device *dev,
				       struct cfg80211_update_ft_ies_params *ftie);
#endif

#if CFG_SUPPORT_RSSI_STATISTICS
int
mtk_cfg80211_testmode_get_rssi_statistics(IN struct wiphy
		*wiphy, IN void *data, IN int len,
		P_GLUE_INFO_T prGlueInfo);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif
	
#endif /* _GL_CFG80211_H */
