// SPDX-License-Identifier: GPL-2.0

/*

 * Copyright (c) 2019 MediaTek Inc.

 */

/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/platform.c#1
*/

/*! \file   "platform.c"
    \brief  This file including the protocol layer privacy function.

    This file provided the macros and functions library support for the
    protocol layer security setting from wlan_oid.c and for parse.c and
    rsn.c and nic_privacy.c

*/

/*
** Log: platform.c
 *
 * 11 14 2011 cm.chang
 * NULL
 * Fix compiling warning
 *
 * 09 13 2011 jeffrey.chang
 * [WCXRP00000983] [MT6620][Wi-Fi Driver] invalid pointer casting causes kernel panic during p2p connection
 * fix the pointer casting
 *
 * 06 29 2011 george.huang
 * [WCXRP00000818] [MT6620 Wi-Fi][Driver] Remove unused code segment regarding CONFIG_IPV6
 * .
 *
 * 06 28 2011 george.huang
 * [WCXRP00000818] [MT6620 Wi-Fi][Driver] Remove unused code segment regarding CONFIG_IPV6
 * remove un-used code
 *
 * 05 11 2011 jeffrey.chang
 * NULL
 * fix build error
 *
 * 05 09 2011 jeffrey.chang
 * [WCXRP00000710] [MT6620 Wi-Fi] Support pattern filter update function on IP address change
 * support ARP filter through kernel notifier
 *
 * 04 08 2011 pat.lu
 * [WCXRP00000623] [MT6620 Wi-Fi][Driver] use ARCH define to distinguish PC Linux driver
 * Use CONFIG_X86 instead of PC_LINUX_DRIVER_USE option to have proper compile setting for PC Linux driver
 *
 * 03 22 2011 pat.lu
 * [WCXRP00000592] [MT6620 Wi-Fi][Driver] Support PC Linux Environment Driver Build
 * Add a compiler option "PC_LINUX_DRIVER_USE" for building driver in PC Linux environment.
 *
 * 03 21 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * improve portability for awareness of early version of linux kernel and wireless extension.
 *
 * 03 18 2011 jeffrey.chang
 * [WCXRP00000512] [MT6620 Wi-Fi][Driver] modify the net device relative functions to support the H/W multiple queue
 * remove early suspend functions
 *
 * 03 03 2011 jeffrey.chang
 * NULL
 * add the ARP filter callback
 *
 * 02 15 2011 jeffrey.chang
 * NULL
 * to support early suspend in android
 *
 * 02 01 2011 cp.wu
 * [WCXRP00000413] [MT6620 Wi-Fi][Driver] Merge 1103 changes on NVRAM file path change to DaVinci main trunk and V1.1
 * branch
 * upon Jason Zhang(NVRAM owner)'s change, ALPS has modified its NVRAM storage from /nvram/... to /data/nvram/...
 *
 * 11 01 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check[WCXRP00000150] [MT6620 Wi-Fi][Driver]
 * Add implementation for querying current TX rate from firmware auto rate module
 * 1) Query link speed (TX rate) from firmware directly with buffering mechanism to reduce overhead
 * 2) Remove CNM CH-RECOVER event handling
 * 3) cfg read/write API renamed with kal prefix for unified naming rules.
 *
 * 10 18 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check[WCXRP00000086] [MT6620 Wi-Fi][Driver]
 * The mac address is all zero at android
 * complete implementation of Android NVRAM access
 *
 * 10 05 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check
 * 1) add NVRAM access API
 * 2) fake scanning result when NVRAM doesn't exist and/or version mismatch. (off by compiler option)
 * 3) add OID implementation for NVRAM read/write service
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
#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/fs.h>

#include <linux/uaccess.h>

#include "gl_os.h"

#ifndef CONFIG_X86
#if defined(CONFIG_HAS_EARLY_SUSPEND)
#include <linux/earlysuspend.h>
#endif
#endif

#include <linux/platform_device.h>
#include <linux/device.h>
#ifdef CONFIG_IP_WOW
#include <linux/skbuff.h>
#include <net/wow.h>
#endif


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define WIFI_NVRAM_FILE_NAME   "/data/nvram/APCFG/APRDEB/WIFI"
#define WIFI_NVRAM_CUSTOM_NAME "/data/nvram/APCFG/APRDEB/WIFI_CUSTOM"

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
#if CONFIG_PM
#define SPM_WAKEUP_EVENT_READY 0
#define DEV_NAME "wlan_ad_die"

static atomic_t fgSuspendFlag = ATOMIC_INIT(0);
static atomic_t fgIndicateWoW = ATOMIC_INIT(0);
static int wlan_probe(struct platform_device *pdev);
static int wlan_remove(struct platform_device *pdev);
static int wlan_suspend(struct platform_device *pdev, pm_message_t state);
static int wlan_resume(struct platform_device *pdev);
static void wlan_release(struct device *dev);
static void wlan_shutdown(struct platform_device *pdev);

static struct platform_device mtk_wlan_dev = {
	.name = DEV_NAME,
	.id = -1,
	.dev = {
		.release = wlan_release,
	}
};

static struct platform_driver mtk_wlan_drv = {
	.probe = wlan_probe,
	.remove = wlan_remove,
#ifdef CONFIG_PM
	.shutdown = wlan_shutdown,
	.suspend = wlan_suspend,
	.resume = wlan_resume,
#endif
	.driver = {
		.name = DEV_NAME,
		.owner = THIS_MODULE,
	}
};
#endif

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

static int netdev_event(struct notifier_block *nb, unsigned long notification, void *ptr)
{
	UINT_8 ip[4] = { 0 };
	UINT_32 u4NumIPv4 = 0;
#ifdef CONFIG_IPV6
	UINT_8 ip6[16] = { 0 };	/* FIX ME: avoid to allocate large memory in stack */
	UINT_32 u4NumIPv6 = 0;
	P_PARAM_NETWORK_ADDRESS_IPV6 prParamIpv6Addr;
#endif
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *prDev = ifa->ifa_dev->dev;
	UINT_32 i;
	P_PARAM_NETWORK_ADDRESS_IP prParamIpAddr;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (prDev == NULL) {
		DBGLOG(REQ, ERROR, "netdev_event: device is empty.\n");
		return NOTIFY_DONE;
	}
	DBGLOG(REQ, INFO, "netdev_event, addr=%x, notification=%lx, dev_name=%s\n",
			ifa->ifa_address, notification, prDev->name);
	if (!fgIsUnderSuspend)
		return NOTIFY_DONE;
	if (strncmp(prDev->name, "wlan", 4) != 0) {
		DBGLOG(REQ, WARN, "netdev_event: only accept wlan's command\n");
		return NOTIFY_DONE;
	}
#if 0				/* CFG_SUPPORT_HOTSPOT_2_0 */
	{
		/* printk(KERN_INFO "[netdev_event] IPV4_DAD is unlock now!!\n"); */
		prGlueInfo->fgIsDad = FALSE;
	}
#endif

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

	if (prGlueInfo == NULL) {
		DBGLOG(REQ, ERROR, "netdev_event: prGlueInfo is empty.\n");
		return NOTIFY_DONE;
	}
	ASSERT(prGlueInfo);

	/* <3> get the IPv4 address */
	if (!prDev || !(prDev->ip_ptr) ||
	    !((struct in_device *)(prDev->ip_ptr))->ifa_list ||
	    !(&(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local))) {
		DBGLOG(REQ, INFO, "ip is not available.\n");
		return NOTIFY_DONE;
	}

	kalMemCopy(ip, &(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local), sizeof(ip));
	DBGLOG(REQ, INFO, "ip is %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

	/* todo: traverse between list to find whole sets of IPv4 addresses */
	if (!((ip[0] == 0) && (ip[1] == 0) && (ip[2] == 0) && (ip[3] == 0)))
		u4NumIPv4++;
#ifdef CONFIG_IPV6
		/* <5> get the IPv6 address*/
		if (!prDev || !(prDev->ip6_ptr) ||\
			!((struct inet6_dev *)(prDev->ip6_ptr))->addr_list.next) {
			DBGLOG(REQ, WARN, "ipv6 is not avaliable.\n");
			u4NumIPv6 = 0;
		} else {
			struct inet6_ifaddr *ifa;
			struct list_head *addr_list;
			addr_list = &(((struct inet6_dev *)(prDev->ip6_ptr))->addr_list);
			ifa = list_entry(addr_list->next, typeof (*ifa), if_list);
			/* <6> copy the IPv6 address */
			kalMemCopy(ip6, &(ifa->addr.s6_addr[0]), sizeof (ip6));
			DBGLOG(REQ, INFO, "ipv6 is %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\n",
					ip6[0], ip6[1], ip6[2], ip6[3],
					ip6[4], ip6[5], ip6[6], ip6[7],
					ip6[8], ip6[9], ip6[10], ip6[11],
					ip6[12], ip6[13], ip6[14], ip6[15]
					);
			/* todo: traverse between list to find whole sets of IPv6 addresses */
			for (i = 0; i < sizeof (ip6); i++) {
				if (ip6[i] != 0) {
					u4NumIPv6 = 1;
					break;
				}
			}
		}
#endif

	/* here we can compare the dev with other network's netdev to */
	/* set the proper arp filter */
	/*  */
	/* IMPORTANT: please make sure if the context can sleep, if the context can't sleep */
	/* we should schedule a kernel thread to do this for us */

	/* <7> set up the ARP filter */
	{
		WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
		UINT_32 u4SetInfoLen = 0;
		UINT_8 aucBuf[64] = {0};
		UINT_32 u4Len = OFFSET_OF(PARAM_NETWORK_ADDRESS_LIST, arAddress);
		P_PARAM_NETWORK_ADDRESS_LIST prParamNetAddrList = (P_PARAM_NETWORK_ADDRESS_LIST) aucBuf;
		P_PARAM_NETWORK_ADDRESS prParamNetAddr = prParamNetAddrList->arAddress;

#ifdef CONFIG_IPV6
		prParamNetAddrList->u4AddressCount = u4NumIPv4 + u4NumIPv6;
		DBGLOG(REQ, INFO, "u4NumIPv4:%d, u4NumIPv6:%d", u4NumIPv4, u4NumIPv6);
#else
		prParamNetAddrList->u4AddressCount = u4NumIPv4;
		DBGLOG(REQ, INFO, "u4NumIPv4:%d", u4NumIPv4);
#endif
		prParamNetAddrList->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
		for (i = 0; i < u4NumIPv4; i++) {
			prParamNetAddr->u2AddressLength = sizeof(PARAM_NETWORK_ADDRESS_IP);	/* 4;; */
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
			prParamIpAddr = (P_PARAM_NETWORK_ADDRESS_IP) prParamNetAddr->aucAddress;
			kalMemCopy(&prParamIpAddr->in_addr, ip, sizeof(ip));
			prParamNetAddr = (P_PARAM_NETWORK_ADDRESS)((ULONG)prParamNetAddr + prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress));
			u4Len += prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress);
		}
#ifdef CONFIG_IPV6
		if (u4NumIPv6 > 0) {
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
			prParamNetAddr->u2AddressLength = sizeof (PARAM_NETWORK_ADDRESS_IPV6);
			prParamIpv6Addr = (P_PARAM_NETWORK_ADDRESS_IPV6)prParamNetAddr->aucAddress;
			kalMemCopy(prParamIpv6Addr->addr, ip6, sizeof (ip6));
			prParamNetAddr = (P_PARAM_NETWORK_ADDRESS)((ULONG)prParamNetAddr + prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress));
			u4Len += prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress);
		}
#endif
		ASSERT(u4Len <= sizeof(aucBuf));

		DBGLOG(REQ, INFO, "kalIoctl (0x%p, 0x%p)\n", prGlueInfo, prParamNetAddrList);

		rStatus = kalIoctl(prGlueInfo,
				wlanoidSetNetworkAddress,
				(PVOID) prParamNetAddrList, u4Len, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS)
			DBGLOG(REQ, ERROR, "set HW pattern filter fail 0x%x\n", rStatus);

#ifdef CONFIG_IPV6
		DBGLOG(REQ, LOUD, "kalIoctl (0x%p, 0x%p)\n", prGlueInfo, prParamNetAddrList);
		rStatus = kalIoctl(prGlueInfo,
				wlanoidSetIPv6NetworkAddress,
				(PVOID)prParamNetAddrList,
				u4Len,
				FALSE,
				FALSE,
				TRUE,
				FALSE,
				&u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(REQ, WARN, "set IPv6 HW pattern filter fail 0x%x\n", rStatus);
		}
#endif
	}

	return NOTIFY_DONE;

}

/* #if CFG_SUPPORT_HOTSPOT_2_0 */
#if 0
static int net6dev_event(struct notifier_block *nb, unsigned long notification, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *prDev = ifa->idev->dev;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (prDev == NULL) {
		DBGLOG(REQ, INFO, "net6dev_event: device is empty.\n");
		return NOTIFY_DONE;
	}

	if ((strncmp(prDev->name, "p2p", 3) != 0) && (strncmp(prDev->name, "wlan", 4) != 0)) {
		DBGLOG(REQ, INFO, "net6dev_event: xxx\n");
		return NOTIFY_DONE;
	}

	if (strncmp(prDev->name, "p2p", 3) == 0) {
		/* because we store the address of prGlueInfo in p2p's private date of net device */
		/* *((P_GLUE_INFO_T *) netdev_priv(prGlueInfo->prP2PInfo->prDevHandler)) = prGlueInfo; */
		prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	} else {		/* wlan0 */
		prGlueInfo = (P_GLUE_INFO_T) netdev_priv(prDev);
	}

	if (prGlueInfo == NULL) {
		DBGLOG(REQ, INFO, "netdev_event: prGlueInfo is empty.\n");
		return NOTIFY_DONE;
	}
	/* printk(KERN_INFO "[net6dev_event] IPV6_DAD is unlock now!!\n"); */
	prGlueInfo->fgIs6Dad = FALSE;

	return NOTIFY_DONE;
}
#endif

static struct notifier_block inetaddr_notifier = {
	.notifier_call = netdev_event,
};

#if 0				/* CFG_SUPPORT_HOTSPOT_2_0 */
static struct notifier_block inet6addr_notifier = {
	.notifier_call = net6dev_event,
};
#endif

void wlanRegisterNotifier(void)
{
	register_inetaddr_notifier(&inetaddr_notifier);

#if CFG_SUPPORT_HOTSPOT_2_0
	/* register_inet6addr_notifier(&inet6addr_notifier); */
#endif
}

/* EXPORT_SYMBOL(wlanRegisterNotifier); */

void wlanUnregisterNotifier(void)
{
	unregister_inetaddr_notifier(&inetaddr_notifier);

#if CFG_SUPPORT_HOTSPOT_2_0
	/* unregister_inetaddr_notifier(&inet6addr_notifier); */
#endif
}

#ifdef CONFIG_PM
/*-----------platform bus related operation APIs----------------*/
static int wlan_probe(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);
	DBGLOG(INIT, INFO, "wlan platform driver probe\n");
	return 0;
}

static int wlan_remove(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, NULL);
	DBGLOG(INIT, INFO, "wlan platform driver remove\n");
	return 0;
}

static void wlan_shutdown(struct platform_device *pdev)
{

	return;
}

static int wlan_suspend(struct platform_device *pdev, pm_message_t state)
{
	glWlanSetSuspendFlag();
	return 0;
}

static int wlan_resume(struct platform_device *pdev)
{
	return 0;
}

static void wlan_release(struct device *dev)
{
}

int glRegisterPlatformDev(void)
{
	int retval;
	/* Register platform device */
	retval = platform_device_register(&mtk_wlan_dev);
	if (retval) {
		DBGLOG(INIT, ERROR, "wlan platform device register failed, ret(%d)\n", retval);
		return retval;
	}

	/* Register platform driver */
	retval = platform_driver_register(&mtk_wlan_drv);
	if (retval) {
		DBGLOG(INIT, ERROR, "wlan platform driver register failed, ret(%d)\n", retval);
	}

	return retval;
}

int glUnregisterPlatformDev(void)
{
	platform_device_unregister(&mtk_wlan_dev);
	platform_driver_unregister(&mtk_wlan_drv);
	return 0;
}

int glWlanSetSuspendFlag(void)
{
	return atomic_set(&fgSuspendFlag, 1);
}

int glWlanGetSuspendFlag(void)
{
#if SPM_WAKEUP_EVENT_READY
	if (atomic_read(&fgSuspendFlag) != 0) {
		int irq_num = 0;
		wakeup_event_t wake_event;

		wake_event = spm_read_wakeup_event_and_irq(&irq_num);
		if (wake_event != 1)
			atomic_set(&fgSuspendFlag, 0);
	}
#endif
	return atomic_read(&fgSuspendFlag);
}

int glWlanSetIndicateWoWFlag(void)
{
	return atomic_set(&fgIndicateWoW, 1);
}

int glWlanClearSuspendFlag(void)
{
	return atomic_set(&fgSuspendFlag, 0);
}

int glIndicateWoWPacket(void *data)
{
#if (defined(CONFIG_IP_WOW) && defined(CONFIG_PM_SLEEP))
	if (0 != atomic_read(&fgIndicateWoW)) {
		/*check wakeup event*/
		atomic_set(&fgIndicateWoW, 0);
		DBGLOG(RX, INFO, ("tagging wow skb..\n"));
		tag_wow_skb(data);
	}
#endif
	return 0;
}

#else
int glRegisterPlatformDev(void)
{
	return 0;
}

int glUnregisterPlatformDev(void)
{
	return 0;
}

int glWlanSetSuspendFlag(void)
{
	return 0;
}

int glWlanGetSuspendFlag(void)
{
	return 0;
}

int glWlanClearSuspendFlag(void)
{
	return 0;
}

int glIndicateWoWPacket(void *data)
{
	return 0;
}
#endif

/* EXPORT_SYMBOL(wlanUnregisterNotifier); */

/*----------------------------------------------------------------------------*/
/*!
* \brief Utility function for reading data from files on NVRAM-FS
*
* \param[in]
*           filename
*           len
*           offset
* \param[out]
*           buf
* \return
*           actual length of data being read
*/
/*----------------------------------------------------------------------------*/
static int nvram_read(char *filename, char *buf, ssize_t len, int offset)
{
#if CFG_SUPPORT_NVRAM
	struct file *fd;
	int retLen = -1;

	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);

	fd = filp_open(filename, O_RDONLY, 0644);

	if (IS_ERR(fd)) {
		DBGLOG(INIT, INFO, "[MT6620][nvram_read] : failed to open!!\n");
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->read == NULL)) {
			DBGLOG(INIT, INFO, "[MT6620][nvram_read] : file can not be read!!\n");
			break;
		}

		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO, "[MT6620][nvram_read] : failed to seek!!\n");
					break;
				}
			} else {
				fd->f_pos = offset;
			}
		}

		retLen = fd->f_op->read(fd, buf, len, &fd->f_pos);

	} while (FALSE);

	filp_close(fd, NULL);

	set_fs(old_fs);

	return retLen;

#else /* !CFG_SUPPORT_NVRAM */

	return -EIO;

#endif
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Utility function for writing data to files on NVRAM-FS
*
* \param[in]
*           filename
*           buf
*           len
*           offset
* \return
*           actual length of data being written
*/
/*----------------------------------------------------------------------------*/
static int nvram_write(char *filename, char *buf, ssize_t len, int offset)
{
#if CFG_SUPPORT_NVRAM
	struct file *fd;
	int retLen = -1;

	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);

	fd = filp_open(filename, O_WRONLY | O_CREAT, 0644);

	if (IS_ERR(fd)) {
		DBGLOG(INIT, INFO, "[MT6620][nvram_write] : failed to open!!\n");
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->write == NULL)) {
			DBGLOG(INIT, INFO, "[MT6620][nvram_write] : file can not be write!!\n");
			break;
		}
		/* End of if */
		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO, "[MT6620][nvram_write] : failed to seek!!\n");
					break;
				}
			} else {
				fd->f_pos = offset;
			}
		}

		retLen = fd->f_op->write(fd, buf, len, &fd->f_pos);

	} while (FALSE);

	filp_close(fd, NULL);

	set_fs(old_fs);

	return retLen;

#else /* !CFG_SUPPORT_NVRAMS */

	return -EIO;

#endif
}

/*----------------------------------------------------------------------------*/
/*!
* \brief API for reading data on NVRAM
*
* \param[in]
*           prGlueInfo
*           u4Offset
* \param[out]
*           pu2Data
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN kalCfgDataRead16(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Offset, OUT PUINT_16 pu2Data)
{
	if (pu2Data == NULL)
		return FALSE;

	if (nvram_read(WIFI_NVRAM_FILE_NAME,
		       (char *)pu2Data, sizeof(unsigned short), u4Offset) != sizeof(unsigned short)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

/*----------------------------------------------------------------------------*/
/*!
* \brief API for writing data on NVRAM
*
* \param[in]
*           prGlueInfo
*           u4Offset
*           u2Data
* \return
*           TRUE
*           FALSE
*/
/*----------------------------------------------------------------------------*/
BOOLEAN kalCfgDataWrite16(IN P_GLUE_INFO_T prGlueInfo, UINT_32 u4Offset, UINT_16 u2Data)
{
	if (nvram_write(WIFI_NVRAM_FILE_NAME,
			(char *)&u2Data, sizeof(unsigned short), u4Offset) != sizeof(unsigned short)) {
		return FALSE;
	} else {
		return TRUE;
	}
}
