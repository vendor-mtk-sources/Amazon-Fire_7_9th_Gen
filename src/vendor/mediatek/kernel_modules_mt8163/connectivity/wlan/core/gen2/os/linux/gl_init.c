// SPDX-License-Identifier: GPL-2.0

/*

 * Copyright (c) 2019 MediaTek Inc.

 */

/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/gl_init.c#7
*/

/*! \file   gl_init.c
    \brief  Main routines of Linux driver

    This file contains the main routines of Linux driver for MediaTek Inc. 802.11
    Wireless LAN Adapters.
*/

/*
** Log: gl_init.c
**
** 09 03 2013 cp.wu
** add path for reassociation
 *
 * 07 17 2012 yuche.tsai
 * NULL
 * Fix compile error.
 *
 * 07 17 2012 yuche.tsai
 * NULL
 * Fix compile error for JB.
 *
 * 07 17 2012 yuche.tsai
 * NULL
 * Let netdev bring up.
 *
 * 07 17 2012 yuche.tsai
 * NULL
 * Compile no error before trial run.
 *
 * 06 13 2012 yuche.tsai
 * NULL
 * Update maintrunk driver.
 * Add support for driver compose assoc request frame.
 *
 * 05 25 2012 yuche.tsai
 * NULL
 * Fix reset KE issue.
 *
 * 05 11 2012 cp.wu
 * [WCXRP00001237] [MT6620 Wi-Fi][Driver] Show MAC address and MAC address source for ACS's convenience
 * show MAC address & source while initiliazation
 *
 * 03 02 2012 terry.wu
 * NULL
 * EXPORT_SYMBOL(rsnParseCheckForWFAInfoElem);.
 *
 * 03 02 2012 terry.wu
 * NULL
 * Snc CFG80211 modification for ICS migration from branch 2.2.
 *
 * 03 02 2012 terry.wu
 * NULL
 * Sync CFG80211 modification from branch 2,2.
 *
 * 03 02 2012 terry.wu
 * NULL
 * Enable CFG80211 Support.
 *
 * 12 22 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * using global variable instead of stack for setting wlanoidSetNetworkAddress(), due to buffer may be released before
 * TX thread handling
 *
 * 11 18 2011 yuche.tsai
 * NULL
 * CONFIG P2P support RSSI query, default turned off.
 *
 * 11 14 2011 yuche.tsai
 * [WCXRP00001107] [Volunteer Patch][Driver] Large Network Type index assert in FW issue.
 * Fix large network type index assert in FW issue.
 *
 * 11 14 2011 cm.chang
 * NULL
 * Fix compiling warning
 *
 * 11 11 2011 yuche.tsai
 * NULL
 * Fix work thread cancel issue.
 *
 * 10 06 2011 eddie.chen
 * [WCXRP00001027] [MT6628 Wi-Fi][Firmware/Driver] Tx fragmentation
 * Add rlmDomainGetChnlList symbol.
 *
 * 09 22 2011 cm.chang
 * NULL
 * Safer writng stype to avoid unitialized regitry structure
 *
 * 09 21 2011 cm.chang
 * [WCXRP00000969] [MT6620 Wi-Fi][Driver][FW] Channel list for 5G band based on country code
 * Avoid possible structure alignment problem
 *
 * 09 20 2011 chinglan.wang
 * [WCXRP00000989] [WiFi Direct] [Driver] Add a new io control API to start the formation for the sigma test.
 * .
 *
 * 09 08 2011 cm.chang
 * [WCXRP00000969] [MT6620 Wi-Fi][Driver][FW] Channel list for 5G band based on country code
 * Use new fields ucChannelListMap and ucChannelListIndex in NVRAM
 *
 * 08 31 2011 cm.chang
 * [WCXRP00000969] [MT6620 Wi-Fi][Driver][FW] Channel list for 5G band based on country code
 * .
 *
 * 08 11 2011 cp.wu
 * [WCXRP00000830] [MT6620 Wi-Fi][Firmware] Use MDRDY counter to detect empty channel for shortening scan time
 * expose scnQuerySparseChannel() for P2P-FSM.
 *
 * 08 11 2011 cp.wu
 * [WCXRP00000830] [MT6620 Wi-Fi][Firmware] Use MDRDY counter to detect empty channel for shortening scan time
 * sparse channel detection:
 * driver: collect sparse channel information with scan-done event
 *
 * 08 02 2011 yuche.tsai
 * [WCXRP00000896] [Volunteer Patch][WiFi Direct][Driver] GO with multiple client, TX deauth to a disconnecting
 * device issue.
 * Fix GO send deauth frame issue.
 *
 * 07 07 2011 wh.su
 * [WCXRP00000839] [MT6620 Wi-Fi][Driver] Add the dumpMemory8 and dumpMemory32 EXPORT_SYMBOL
 * Add the dumpMemory8 symbol export for debug mode.
 *
 * 07 06 2011 terry.wu
 * [WCXRP00000735] [MT6620 Wi-Fi][BoW][FW/Driver] Protect BoW connection establishment
 * Improve BoW connection establishment speed.
 *
 * 07 05 2011 yuche.tsai
 * [WCXRP00000821] [Volunteer Patch][WiFi Direct][Driver] WiFi Direct Connection Speed Issue
 * Export one symbol for enhancement.
 *
 * 06 13 2011 eddie.chen
 * [WCXRP00000779] [MT6620 Wi-Fi][DRV]  Add tx rx statistics in linux and use netif_rx_ni
 * Add tx rx statistics and netif_rx_ni.
 *
 * 05 27 2011 cp.wu
 * [WCXRP00000749] [MT6620 Wi-Fi][Driver] Add band edge tx power control to Wi-Fi NVRAM
 * invoke CMD_ID_SET_EDGE_TXPWR_LIMIT when there is valid data exist in NVRAM content.
 *
 * 05 18 2011 cp.wu
 * [WCXRP00000734] [MT6620 Wi-Fi][Driver] Pass PHY_PARAM in NVRAM to firmware domain
 * pass PHY_PARAM in NVRAM from driver to firmware.
 *
 * 05 09 2011 jeffrey.chang
 * [WCXRP00000710] [MT6620 Wi-Fi] Support pattern filter update function on IP address change
 * support ARP filter through kernel notifier
 *
 * 05 03 2011 chinghwa.yu
 * [WCXRP00000065] Update BoW design and settings
 * Use kalMemAlloc to allocate event buffer for kalIndicateBOWEvent.
 *
 * 04 27 2011 george.huang
 * [WCXRP00000684] [MT6620 Wi-Fi][Driver] Support P2P setting ARP filter
 * Support P2P ARP filter setting on early suspend/ late resume
 *
 * 04 18 2011 terry.wu
 * [WCXRP00000660] [MT6620 Wi-Fi][Driver] Remove flag CFG_WIFI_DIRECT_MOVED
 * Remove flag CFG_WIFI_DIRECT_MOVED.
 *
 * 04 15 2011 chinghwa.yu
 * [WCXRP00000065] Update BoW design and settings
 * Add BOW short range mode.
 *
 * 04 14 2011 yuche.tsai
 * [WCXRP00000646] [Volunteer Patch][MT6620][FW/Driver] Sigma Test Modification for some test case.
 * Modify some driver connection flow or behavior to pass Sigma test more easier..
 *
 * 04 12 2011 cm.chang
 * [WCXRP00000634] [MT6620 Wi-Fi][Driver][FW] 2nd BSS will not support 40MHz bandwidth for concurrency
 * .
 *
 * 04 11 2011 george.huang
 * [WCXRP00000621] [MT6620 Wi-Fi][Driver] Support P2P supplicant to set power mode
 * export wlan functions to p2p
 *
 * 04 08 2011 pat.lu
 * [WCXRP00000623] [MT6620 Wi-Fi][Driver] use ARCH define to distinguish PC Linux driver
 * Use CONFIG_X86 instead of PC_LINUX_DRIVER_USE option to have proper compile setting for PC Linux driver
 *
 * 04 08 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * glBusFreeIrq() should use the same pvabc123 as glBusSetIrq() or request_irq()/free_irq() won't work as a pair.
 *
 * 04 08 2011 eddie.chen
 * [WCXRP00000617] [MT6620 Wi-Fi][DRV/FW] Fix for sigma
 * Fix for sigma
 *
 * 04 06 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * 1. do not check for pvData inside wlanNetCreate() due to it is NULL for eHPI  port
 * 2. update perm_addr as well for MAC address
 * 3. not calling check_mem_region() anymore for eHPI
 * 4. correct MSC_CS macro for 0-based notation
 *
 * 03 29 2011 cp.wu
 * [WCXRP00000598] [MT6620 Wi-Fi][Driver] Implementation of interface for communicating with user space process for
 * RESET_START and RESET_END events
 * fix typo.
 *
 * 03 29 2011 cp.wu
 * [WCXRP00000598] [MT6620 Wi-Fi][Driver] Implementation of interface for communicating with user space process for
 * RESET_START and RESET_END events
 * implement kernel-to-userspace communication via generic netlink socket for whole-chip resetting mechanism
 *
 * 03 23 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * apply multi-queue operation only for linux kernel > 2.6.26
 *
 * 03 22 2011 pat.lu
 * [WCXRP00000592] [MT6620 Wi-Fi][Driver] Support PC Linux Environment Driver Build
 * Add a compiler option "PC_LINUX_DRIVER_USE" for building driver in PC Linux environment.
 *
 * 03 21 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * portability for compatible with linux 2.6.12.
 *
 * 03 21 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * improve portability for awareness of early version of linux kernel and wireless extension.
 *
 * 03 21 2011 cp.wu
 * [WCXRP00000540] [MT5931][Driver] Add eHPI8/eHPI16 support to Linux Glue Layer
 * portability improvement
 *
 * 03 18 2011 jeffrey.chang
 * [WCXRP00000512] [MT6620 Wi-Fi][Driver] modify the net device relative functions to support the H/W multiple queue
 * remove early suspend functions
 *
 * 03 17 2011 cp.wu
 * [WCXRP00000562] [MT6620 Wi-Fi][Driver] I/O buffer pre-allocation to avoid physically continuous memory shortage
 * after system running for a long period
 * reverse order to prevent probing racing.
 *
 * 03 16 2011 cp.wu
 * [WCXRP00000562] [MT6620 Wi-Fi][Driver] I/O buffer pre-allocation to avoid physically continuous memory shortage
 * after system running for a long period
 * 1. pre-allocate physical continuous buffer while module is being loaded
 * 2. use pre-allocated physical continuous buffer for TX/RX DMA transfer
 *
 * The windows part remained the same as before, but added similar APIs to hide the difference.
 *
 * 03 15 2011 jeffrey.chang
 * [WCXRP00000558] [MT6620 Wi-Fi][MT6620 Wi-Fi][Driver] refine the queue selection algorithm for WMM
 * refine the queue_select function
 *
 * 03 10 2011 cp.wu
 * [WCXRP00000532] [MT6620 Wi-Fi][Driver] Migrate NVRAM configuration procedures from MT6620 E2 to MT6620 E3
 * deprecate configuration used by MT6620 E2
 *
 * 03 10 2011 terry.wu
 * [WCXRP00000505] [MT6620 Wi-Fi][Driver/FW] WiFi Direct Integration
 * Remove unnecessary assert and message.
 *
 * 03 08 2011 terry.wu
 * [WCXRP00000505] [MT6620 Wi-Fi][Driver/FW] WiFi Direct Integration
 * Export nicQmUpdateWmmParms.
 *
 * 03 03 2011 jeffrey.chang
 * [WCXRP00000512] [MT6620 Wi-Fi][Driver] modify the net device relative functions to support the H/W multiple queue
 * support concurrent network
 *
 * 03 03 2011 jeffrey.chang
 * [WCXRP00000512] [MT6620 Wi-Fi][Driver] modify the net device relative functions to support the H/W multiple queue
 * modify net device relative functions to support multiple H/W queues
 *
 * 02 24 2011 george.huang
 * [WCXRP00000495] [MT6620 Wi-Fi][FW] Support pattern filter for unwanted ARP frames
 * Support ARP filter during suspended
 *
 * 02 21 2011 cp.wu
 * [WCXRP00000482] [MT6620 Wi-Fi][Driver] Simplify logic for checking NVRAM existence in driver domain
 * simplify logic for checking NVRAM existence only once.
 *
 * 02 17 2011 terry.wu
 * [WCXRP00000459] [MT6620 Wi-Fi][Driver] Fix deference null pointer problem in wlanRemove
 * Fix deference a null pointer problem in wlanRemove.
 *
 * 02 16 2011 jeffrey.chang
 * NULL
 * fix compilig error
 *
 * 02 16 2011 jeffrey.chang
 * NULL
 * Add query ipv4 and ipv6 address during early suspend and late resume
 *
 * 02 15 2011 jeffrey.chang
 * NULL
 * to support early suspend in android
 *
 * 02 11 2011 yuche.tsai
 * [WCXRP00000431] [Volunteer Patch][MT6620][Driver] Add MLME support for deauthentication under AP(Hot-Spot) mode.
 * Add one more export symbol.
 *
 * 02 10 2011 yuche.tsai
 * [WCXRP00000431] [Volunteer Patch][MT6620][Driver] Add MLME support for deauthentication under AP(Hot-Spot) mode.
 * Add RX deauthentication & disassociation process under Hot-Spot mode.
 *
 * 02 09 2011 terry.wu
 * [WCXRP00000383] [MT6620 Wi-Fi][Driver] Separate WiFi and P2P driver into two modules
 * Halt p2p module init and exit until TxThread finished p2p register and unregister.
 *
 * 02 08 2011 george.huang
 * [WCXRP00000422] [MT6620 Wi-Fi][Driver] support query power mode OID handler
 * Support querying power mode OID.
 *
 * 02 08 2011 yuche.tsai
 * [WCXRP00000421] [Volunteer Patch][MT6620][Driver] Fix incorrect SSID length Issue
 * Export Deactivation Network.
 *
 * 02 01 2011 jeffrey.chang
 * [WCXRP00000414] KAL Timer is not unregistered when driver not loaded
 * Unregister the KAL timer during driver unloading
 *
 * 01 26 2011 cm.chang
 * [WCXRP00000395] [MT6620 Wi-Fi][Driver][FW] Search STA_REC with additional net type index argument
 * Allocate system RAM if fixed message or mgmt buffer is not available
 *
 * 01 19 2011 cp.wu
 * [WCXRP00000371] [MT6620 Wi-Fi][Driver] make linux glue layer portable for Android 2.3.1 with Linux 2.6.35.7
 * add compile option to check linux version 2.6.35 for different usage of system API to improve portability
 *
 * 01 12 2011 cp.wu
 * [WCXRP00000357] [MT6620 Wi-Fi][Driver][Bluetooth over Wi-Fi] add another net device interface for BT AMP
 * implementation of separate BT_OVER_WIFI data path.
 *
 * 01 10 2011 cp.wu
 * [WCXRP00000349] [MT6620 Wi-Fi][Driver] make kalIoctl() of linux port as a thread safe API to avoid potential issues
 * due to multiple access
 * use mutex to protect kalIoctl() for thread safe.
 *
 * 01 04 2011 cp.wu
 * [WCXRP00000338] [MT6620 Wi-Fi][Driver] Separate kalMemAlloc into kmalloc and vmalloc implementations to ease
 * physically continuous memory demands
 * separate kalMemAlloc() into virtually-continuous and physically-continuous type to ease slab system pressure
 *
 * 12 15 2010 cp.wu
 * [WCXRP00000265] [MT6620 Wi-Fi][Driver] Remove set_mac_address routine from legacy Wi-Fi Android driver
 * remove set MAC address. MAC address is always loaded from NVRAM instead.
 *
 * 12 10 2010 kevin.huang
 * [WCXRP00000128] [MT6620 Wi-Fi][Driver] Add proc support to Android Driver for debug and driver status check
 * Add Linux Proc Support
 *
 * 11 01 2010 yarco.yang
 * [WCXRP00000149] [MT6620 WI-Fi][Driver]Fine tune performance on MT6516 platform
 * Add GPIO debug function
 *
 * 11 01 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check[WCXRP00000150] [MT6620 Wi-Fi][Driver]
 * Add implementation for querying current TX rate from firmware auto rate module
 * 1) Query link speed (TX rate) from firmware directly with buffering mechanism to reduce overhead
 * 2) Remove CNM CH-RECOVER event handling
 * 3) cfg read/write API renamed with kal prefix for unified naming rules.
 *
 * 10 26 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check[WCXRP00000137] [MT6620 Wi-Fi] [FW]
 * Support NIC capability query command
 * 1) update NVRAM content template to ver 1.02
 * 2) add compile option for querying NIC capability (default: off)
 * 3) modify AIS 5GHz support to run-time option, which could be turned on by registry or NVRAM setting
 * 4) correct auto-rate compiler error under linux (treat warning as error)
 * 5) simplify usage of NVRAM and REG_INFO_T
 * 6) add version checking between driver and firmware
 *
 * 10 21 2010 chinghwa.yu
 * [WCXRP00000065] Update BoW design and settings
 * .
 *
 * 10 19 2010 jeffrey.chang
 * [WCXRP00000120] [MT6620 Wi-Fi][Driver] Refine linux kernel module to the license of MTK propietary and enable MTK
 * HIF by default
 * Refine linux kernel module to the license of MTK and enable MTK HIF
 *
 * 10 18 2010 jeffrey.chang
 * [WCXRP00000106] [MT6620 Wi-Fi][Driver] Enable setting multicast  callback in Android
 * .
 *
 * 10 18 2010 cp.wu
 * [WCXRP00000056] [MT6620 Wi-Fi][Driver] NVRAM implementation with Version Check[WCXRP00000086] [MT6620 Wi-Fi][Driver]
 * The mac address is all zero at android
 * complete implementation of Android NVRAM access
 *
 * 09 27 2010 chinghwa.yu
 * [WCXRP00000063] Update BCM CoEx design and settings[WCXRP00000065] Update BoW design and settings
 * Update BCM/BoW design and settings.
 *
 * 09 23 2010 cp.wu
 * [WCXRP00000051] [MT6620 Wi-Fi][Driver] WHQL test fail in MAC address changed item
 * use firmware reported mac address right after wlanAdapterStart() as permanent address
 *
 * 09 21 2010 kevin.huang
 * [WCXRP00000052] [MT6620 Wi-Fi][Driver] Eliminate Linux Compile Warning
 * Eliminate Linux Compile Warning
 *
 * 09 03 2010 kevin.huang
 * NULL
 * Refine #include sequence and solve recursive/nested #include issue
 *
 * 09 01 2010 wh.su
 * NULL
 * adding the wapi support for integration test.
 *
 * 08 18 2010 yarco.yang
 * NULL
 * 1. Fixed HW checksum offload function not work under Linux issue.
 * 2. Add debug message.
 *
 * 08 16 2010 yarco.yang
 * NULL
 * Support Linux x86
 *
 * 08 02 2010 jeffrey.chang
 * NULL
 * 1) modify tx service thread to avoid busy looping
 * 2) add spin lock declartion for linux build
 *
 * 07 29 2010 jeffrey.chang
 * NULL
 * fix memory leak for module unloading
 *
 * 07 28 2010 jeffrey.chang
 * NULL
 * 1) remove unused spinlocks
 * 2) enable encyption ioctls
 * 3) fix scan ioctl which may cause supplicant to hang
 *
 * 07 23 2010 jeffrey.chang
 *
 * bug fix: allocate regInfo when disabling firmware download
 *
 * 07 23 2010 jeffrey.chang
 *
 * use glue layer api to decrease or increase counter atomically
 *
 * 07 22 2010 jeffrey.chang
 *
 * add new spinlock
 *
 * 07 19 2010 jeffrey.chang
 *
 * modify cmd/data path for new design
 *
 * 07 08 2010 cp.wu
 *
 * [WPD00003833] [MT6620 and MT5931] Driver migration - move to new repository.
 *
 * 06 06 2010 kevin.huang
 * [WPD00003832][MT6620 5931] Create driver base
 * [MT6620 5931] Create driver base
 *
 * 05 26 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * 1) Modify set mac address code
 * 2) remove power management macro
 *
 * 05 10 2010 cp.wu
 * [WPD00003831][MT6620 Wi-Fi] Add framework for Wi-Fi Direct support
 * implement basic wi-fi direct framework
 *
 * 05 07 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * prevent supplicant accessing driver during resume
 *
 * 05 07 2010 cp.wu
 * [WPD00003831][MT6620 Wi-Fi] Add framework for Wi-Fi Direct support
 * add basic framework for implementating P2P driver hook.
 *
 * 04 27 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * 1) fix firmware download bug
 * 2) remove query statistics for acelerating firmware download
 *
 * 04 27 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * follow Linux's firmware framework, and remove unused kal API
 *
 * 04 21 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * add for private ioctl support
 *
 * 04 19 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * Query statistics from firmware
 *
 * 04 19 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * modify tcp/ip checksum offload flags
 *
 * 04 16 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * fix tcp/ip checksum offload bug
 *
 * 04 13 2010 cp.wu
 * [WPD00003823][MT6620 Wi-Fi] Add Bluetooth-over-Wi-Fi support
 * add framework for BT-over-Wi-Fi support.
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * 1) prPendingCmdInfo is replaced by queue for multiple handler
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *    capability
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * 2) command sequence number is now increased atomically
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * 3) private data could be hold and taken use for other purpose
 *
 * 04 09 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * fix spinlock usage
 *
 * 04 07 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * Set MAC address from firmware
 *
 * 04 07 2010 cp.wu
 * [WPD00001943]Create WiFi test driver framework on WinXP
 * rWlanInfo should be placed at adapter rather than glue due to most operations
 *  *  *  *  *  * are done in adapter layer.
 *
 * 04 07 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * (1)improve none-glue code portability
 *  * (2) disable set Multicast address during atomic context
 *
 * 04 06 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * adding debug module
 *
 * 03 31 2010 wh.su
 * [WPD00003816][MT6620 Wi-Fi] Adding the security support
 * modify the wapi related code for new driver's design.
 *
 * 03 30 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * emulate NDIS Pending OID facility
 *
 * 03 26 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * fix f/w download start and load address by using config.h
 *
 * 03 26 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * [WPD00003826] Initial import for Linux port
 * adding firmware download support
 *
 * 03 24 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * initial import for Linux port
**  \main\maintrunk.MT5921\52 2009-10-27 22:49:59 GMT mtk01090
**  Fix compile error for Linux EHPI driver
**  \main\maintrunk.MT5921\51 2009-10-20 17:38:22 GMT mtk01090
**  Refine driver unloading and clean up procedure. Block requests, stop main thread and clean up queued requests,
**  and then stop hw.
**  \main\maintrunk.MT5921\50 2009-10-08 10:33:11 GMT mtk01090
**  Avoid accessing private data of net_device directly. Replace with netdev_priv(). Add more checking for input
**  parameters and pointers.
**  \main\maintrunk.MT5921\49 2009-09-28 20:19:05 GMT mtk01090
**  Add private ioctl to carry OID structures. Restructure public/private ioctl interfaces to Linux kernel.
**  \main\maintrunk.MT5921\48 2009-09-03 13:58:46 GMT mtk01088
**  remove non-used code
**  \main\maintrunk.MT5921\47 2009-09-03 11:40:25 GMT mtk01088
**  adding the module parameter for wapi
**  \main\maintrunk.MT5921\46 2009-08-18 22:56:41 GMT mtk01090
**  Add Linux SDIO (with mmc core) support.
**  Add Linux 2.6.21, 2.6.25, 2.6.26.
**  Fix compile warning in Linux.
**  \main\maintrunk.MT5921\45 2009-07-06 20:53:00 GMT mtk01088
**  adding the code to check the wapi 1x frame
**  \main\maintrunk.MT5921\44 2009-06-23 23:18:55 GMT mtk01090
**  Add build option BUILD_USE_EEPROM and compile option CFG_SUPPORT_EXT_CONFIG for NVRAM support
**  \main\maintrunk.MT5921\43 2009-02-16 23:46:51 GMT mtk01461
**  Revise the order of increasing u4TxPendingFrameNum because of  CFG_TX_RET_TX_CTRL_EARLY
**  \main\maintrunk.MT5921\42 2009-01-22 13:11:59 GMT mtk01088
**  set the tid and 1x value at same packet reserved field
**  \main\maintrunk.MT5921\41 2008-10-20 22:43:53 GMT mtk01104
**  Fix wrong variable name "prDev" in wlanStop()
**  \main\maintrunk.MT5921\40 2008-10-16 15:37:10 GMT mtk01461
**  add handle WLAN_STATUS_SUCCESS in wlanHardStartXmit() for CFG_TX_RET_TX_CTRL_EARLY
**  \main\maintrunk.MT5921\39 2008-09-25 15:56:21 GMT mtk01461
**  Update driver for Code review
**  \main\maintrunk.MT5921\38 2008-09-05 17:25:07 GMT mtk01461
**  Update Driver for Code Review
**  \main\maintrunk.MT5921\37 2008-09-02 10:57:06 GMT mtk01461
**  Update driver for code review
**  \main\maintrunk.MT5921\36 2008-08-05 01:53:28 GMT mtk01461
**  Add support for linux statistics
**  \main\maintrunk.MT5921\35 2008-08-04 16:52:58 GMT mtk01461
**  Fix ASSERT if removing module in BG_SSID_SCAN state
**  \main\maintrunk.MT5921\34 2008-06-13 22:52:24 GMT mtk01461
**  Revise status code handling in wlanHardStartXmit() for WLAN_STATUS_SUCCESS
**  \main\maintrunk.MT5921\33 2008-05-30 18:56:53 GMT mtk01461
**  Not use wlanoidSetCurrentAddrForLinux()
**  \main\maintrunk.MT5921\32 2008-05-30 14:39:40 GMT mtk01461
**  Remove WMM Assoc Flag
**  \main\maintrunk.MT5921\31 2008-05-23 10:26:40 GMT mtk01084
**  modify wlanISR interface
**  \main\maintrunk.MT5921\30 2008-05-03 18:52:36 GMT mtk01461
**  Fix Unset Broadcast filter when setMulticast
**  \main\maintrunk.MT5921\29 2008-05-03 15:17:26 GMT mtk01461
**  Move Query Media Status to GLUE
**  \main\maintrunk.MT5921\28 2008-04-24 22:48:21 GMT mtk01461
**  Revise set multicast function by using windows oid style for LP own back
**  \main\maintrunk.MT5921\27 2008-04-24 12:00:08 GMT mtk01461
**  Fix multicast setting in Linux and add comment
**  \main\maintrunk.MT5921\26 2008-03-28 10:40:22 GMT mtk01461
**  Fix set mac address func in Linux
**  \main\maintrunk.MT5921\25 2008-03-26 15:37:26 GMT mtk01461
**  Add set MAC Address
**  \main\maintrunk.MT5921\24 2008-03-26 14:24:53 GMT mtk01461
**  For Linux, set net_device has feature with checksum offload by default
**  \main\maintrunk.MT5921\23 2008-03-11 14:50:52 GMT mtk01461
**  Fix typo
**  \main\maintrunk.MT5921\22 2008-02-29 15:35:20 GMT mtk01088
**  add 1x decide code for sw port control
**  \main\maintrunk.MT5921\21 2008-02-21 15:01:54 GMT mtk01461
**  Rearrange the set off place of GLUE spin lock in HardStartXmit
**  \main\maintrunk.MT5921\20 2008-02-12 23:26:50 GMT mtk01461
**  Add debug option - Packet Order for Linux and add debug level - Event
**  \main\maintrunk.MT5921\19 2007-12-11 00:11:12 GMT mtk01461
**  Fix SPIN_LOCK protection
**  \main\maintrunk.MT5921\18 2007-11-30 17:02:25 GMT mtk01425
**  1. Set Rx multicast packets mode before setting the address list
**  \main\maintrunk.MT5921\17 2007-11-26 19:44:24 GMT mtk01461
**  Add OS_TIMESTAMP to packet
**  \main\maintrunk.MT5921\16 2007-11-21 15:47:20 GMT mtk01088
**  fixed the unload module issue
**  \main\maintrunk.MT5921\15 2007-11-07 18:37:38 GMT mtk01461
**  Fix compile warnning
**  \main\maintrunk.MT5921\14 2007-11-02 01:03:19 GMT mtk01461
**  Unify TX Path for Normal and IBSS Power Save + IBSS neighbor learning
**  \main\maintrunk.MT5921\13 2007-10-30 10:42:33 GMT mtk01425
**  1. Refine for multicast list
**  \main\maintrunk.MT5921\12 2007-10-25 18:08:13 GMT mtk01461
**  Add VOIP SCAN Support  & Refine Roaming
** Revision 1.4  2007/07/05 07:25:33  MTK01461
** Add Linux initial code, modify doc, add 11BB, RF init code
**
** Revision 1.3  2007/06/27 02:18:50  MTK01461
** Update SCAN_FSM, Initial(Can Load Module), Proc(Can do Reg R/W), TX API
**
** Revision 1.2  2007/06/25 06:16:24  MTK01461
** Update illustrations, gl_init.c, gl_kal.c, gl_kal.h, gl_os.h and RX API
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
#include "gl_cfg80211.h"
#include "precomp.h"
#if CFG_SUPPORT_AGPS_ASSIST
#include "gl_kal.h"
#endif
#if defined(CONFIG_MTK_TC1_FEATURE)
#include <tc1_partition.h>
#endif
#include "gl_vendor.h"
#include <linux/of.h>
#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
#include <linux/power_supply.h>
#endif
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* #define MAX_IOREQ_NUM   10 */

BOOLEAN fgIsUnderSuspend;

struct semaphore g_halt_sem;
int g_u4HaltFlag = 1;

#if CFG_ENABLE_WIFI_DIRECT
spinlock_t g_p2p_lock;
int g_u4P2PEnding;
int g_u4P2POnOffing;
#endif

#define IDME_OF_MAC_ADDR        "/idme/mac_addr"
#define IDME_OF_WIFI_MFG        "/idme/wifi_mfg"
#define IDME_OF_BOARD_ID	"/idme/board_id"
#define BOARD_ID_abf123_STR "0031"
#define BOARD_ID_abh123_STR "0110"
#define BOARD_ID_abc123_STR "0110"
#define BOARD_ID_abd123_STR "0120"
#define BOARD_ID_abg123_STR "0032"
#define BOARD_ID_abc123_STR "0033"
#define BOARD_ID_MUSTANG_STR "0034"
char idme_board_id[16];

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Tasklet mechanism is like buttom-half in Linux. We just want to
 * send a signal to OS for interrupt defer processing. All resources
 * are NOT allowed reentry, so txPacket, ISR-DPC and ioctl must avoid preempty.
 */
typedef struct _WLANDEV_INFO_T {
	struct net_device *prDev;
} WLANDEV_INFO_T, *P_WLANDEV_INFO_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

MODULE_AUTHOR(NIC_AUTHOR);
MODULE_DESCRIPTION(NIC_DESC);
MODULE_SUPPORTED_DEVICE(NIC_NAME);
MODULE_LICENSE("GPL");

#define NIC_INF_NAME    "wlan%d"	/* interface name */
#if CFG_TC1_FEATURE
#define NIC_INF_NAME_IN_AP_MODE  "legacy%d"
#endif

/* support to change debug module info dynamically */
UINT_8 aucDebugModule[DBG_MODULE_NUM];
UINT_32 u4DebugModule;

/* 4 2007/06/26, mikewu, now we don't use this, we just fix the number of wlan device to 1 */
static WLANDEV_INFO_T arWlanDevInfo[CFG_MAX_WLAN_DEVICES] = { {0} };

static UINT_32 u4WlanDevNum;	/* How many NICs coexist now */

/**20150205 added work queue for sched_scan to avoid cfg80211 stop schedule scan dead loack**/
struct delayed_work sched_workq;

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#if CFG_ENABLE_WIFI_DIRECT
static SUB_MODULE_HANDLER rSubModHandler[SUB_MODULE_NUM] = { {NULL} };
#endif

#define CHAN2G(_channel, _freq, _flags)         \
{                                           \
	.band               = NL80211_BAND_2GHZ,  \
	.center_freq        = (_freq),              \
	.hw_value           = (_channel),           \
	.flags              = (_flags),             \
	.max_antenna_gain   = 0,                    \
	.max_power          = 30,                   \
}
static struct ieee80211_channel mtk_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#define CHAN5G(_channel, _flags)                    \
{                                               \
	.band               = NL80211_BAND_5GHZ,      \
	.center_freq        = 5000 + (5 * (_channel)),  \
	.hw_value           = (_channel),               \
	.flags              = (_flags),                 \
	.max_antenna_gain   = 0,                        \
	.max_power          = 30,                       \
}
static struct ieee80211_channel mtk_5ghz_channels[] = {
	CHAN5G(34, 0), CHAN5G(36, 0),
	CHAN5G(38, 0), CHAN5G(40, 0),
	CHAN5G(42, 0), CHAN5G(44, 0),
	CHAN5G(46, 0), CHAN5G(48, 0),
	CHAN5G(52, 0), CHAN5G(56, 0),
	CHAN5G(60, 0), CHAN5G(64, 0),
	CHAN5G(100, 0), CHAN5G(104, 0),
	CHAN5G(108, 0), CHAN5G(112, 0),
	CHAN5G(116, 0), CHAN5G(120, 0),
	CHAN5G(124, 0), CHAN5G(128, 0),
	CHAN5G(132, 0), CHAN5G(136, 0),
#ifdef CONFIG_WIFI_DFS_CHANNEL
	CHAN5G(140, 0), CHAN5G(144, 0),
	CHAN5G(149, 0),
#else
	CHAN5G(140, 0), CHAN5G(149, 0),
#endif
	CHAN5G(153, 0), CHAN5G(157, 0),
	CHAN5G(161, 0), CHAN5G(165, 0),
	CHAN5G(169, 0), CHAN5G(173, 0),
	CHAN5G(184, 0), CHAN5G(188, 0),
	CHAN5G(192, 0), CHAN5G(196, 0),
	CHAN5G(200, 0), CHAN5G(204, 0),
	CHAN5G(208, 0), CHAN5G(212, 0),
	CHAN5G(216, 0),
};

/* for cfg80211 - rate table */
static struct ieee80211_rate mtk_rates[] = {
	RATETAB_ENT(10, 0x1000, 0),
	RATETAB_ENT(20, 0x1001, 0),
	RATETAB_ENT(55, 0x1002, 0),
	RATETAB_ENT(110, 0x1003, 0),	/* 802.11b */
	RATETAB_ENT(60, 0x2000, 0),
	RATETAB_ENT(90, 0x2001, 0),
	RATETAB_ENT(120, 0x2002, 0),
	RATETAB_ENT(180, 0x2003, 0),
	RATETAB_ENT(240, 0x2004, 0),
	RATETAB_ENT(360, 0x2005, 0),
	RATETAB_ENT(480, 0x2006, 0),
	RATETAB_ENT(540, 0x2007, 0),	/* 802.11a/g */
};

#define mtk_a_rates         (mtk_rates + 4)
#define mtk_a_rates_size    (sizeof(mtk_rates) / sizeof(mtk_rates[0]) - 4)
#define mtk_g_rates         (mtk_rates + 0)
#define mtk_g_rates_size    (sizeof(mtk_rates) / sizeof(mtk_rates[0]) - 0)

#define MT6620_MCS_INFO                                 \
{                                                       \
	.rx_mask        = {0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0},\
	.rx_highest     = 0,                                \
	.tx_params      = IEEE80211_HT_MCS_TX_DEFINED,      \
}

#define MT6620_HT_CAP                                   \
{                                                       \
	.ht_supported   = true,                             \
	.cap            = IEEE80211_HT_CAP_SUP_WIDTH_20_40  \
		    | IEEE80211_HT_CAP_SM_PS            \
		    | IEEE80211_HT_CAP_GRN_FLD          \
		    | IEEE80211_HT_CAP_SGI_20           \
		    | IEEE80211_HT_CAP_SGI_40,          \
	.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K,       \
	.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_NONE,   \
	.mcs            = MT6620_MCS_INFO,                  \
}

/* public for both Legacy Wi-Fi / P2P access */
struct ieee80211_supported_band mtk_band_2ghz = {
	.band = NL80211_BAND_2GHZ,
	.channels = mtk_2ghz_channels,
	.n_channels = ARRAY_SIZE(mtk_2ghz_channels),
	.bitrates = mtk_g_rates,
	.n_bitrates = mtk_g_rates_size,
	.ht_cap = MT6620_HT_CAP,
};

/* public for both Legacy Wi-Fi / P2P access */
struct ieee80211_supported_band mtk_band_5ghz = {
	.band = NL80211_BAND_5GHZ,
	.channels = mtk_5ghz_channels,
	.n_channels = ARRAY_SIZE(mtk_5ghz_channels),
	.bitrates = mtk_a_rates,
	.n_bitrates = mtk_a_rates_size,
	.ht_cap = MT6620_HT_CAP,
};

static const UINT_32 mtk_cipher_suites[] = {
	/* keep WEP first, it may be removed below */
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,

	/* keep last -- depends on hw flags! */
	WLAN_CIPHER_SUITE_AES_CMAC
};

static struct cfg80211_ops mtk_wlan_ops = {
#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	.suspend = mtk_cfg80211_suspend,
	.resume = mtk_cfg80211_resume,
#endif
	.change_virtual_intf = mtk_cfg80211_change_iface,
	.add_key = mtk_cfg80211_add_key,
	.get_key = mtk_cfg80211_get_key,
	.del_key = mtk_cfg80211_del_key,
	.set_default_key = mtk_cfg80211_set_default_key,
	.set_default_mgmt_key = mtk_cfg80211_set_default_mgmt_key,
	.get_station = mtk_cfg80211_get_station,
	.change_station = mtk_cfg80211_change_station,
	.add_station = mtk_cfg80211_add_station,
	.del_station = mtk_cfg80211_del_station,
	.scan = mtk_cfg80211_scan,
	.connect = mtk_cfg80211_connect,
	.disconnect = mtk_cfg80211_disconnect,
	.join_ibss = mtk_cfg80211_join_ibss,
	.leave_ibss = mtk_cfg80211_leave_ibss,
	.set_power_mgmt = mtk_cfg80211_set_power_mgmt,
	.set_pmksa = mtk_cfg80211_set_pmksa,
	.del_pmksa = mtk_cfg80211_del_pmksa,
	.flush_pmksa = mtk_cfg80211_flush_pmksa,
	.assoc = mtk_cfg80211_assoc,

	/* Action Frame TX/RX */
	.remain_on_channel = mtk_cfg80211_remain_on_channel,
	.cancel_remain_on_channel = mtk_cfg80211_cancel_remain_on_channel,
	.mgmt_tx = mtk_cfg80211_mgmt_tx,
/* .mgmt_tx_cancel_wait        = mtk_cfg80211_mgmt_tx_cancel_wait, */
	.mgmt_frame_register = mtk_cfg80211_mgmt_frame_register,
#ifdef CONFIG_NL80211_TESTMODE
	.testmode_cmd = mtk_cfg80211_testmode_cmd,
#endif
#if (CFG_SUPPORT_TDLS == 1)
	.tdls_mgmt = TdlsexCfg80211TdlsMgmt,
	.tdls_oper = TdlsexCfg80211TdlsOper,
#endif /* CFG_SUPPORT_TDLS */
#if (CFG_SUPPORT_PNO == 1)
	.sched_scan_start = mtk_cfg80211_sched_scan_start,
	.sched_scan_stop = mtk_cfg80211_sched_scan_stop,
#endif
#if CFG_SUPPORT_802_11R
	.update_ft_ies = mtk_cfg80211_update_ft_ies,
#endif

};

static const struct wiphy_vendor_command mtk_wlan_vendor_ops[] = {
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_GET_CHANNEL_LIST
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_channel_list
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_SET_COUNTRY_CODE
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_country_code
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_GET_ROAMING_CAPABILITIES
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_roaming_capabilities
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_CONFIG_ROAMING
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_config_roaming
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_ENABLE_ROAMING
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_enable_roaming
	},
	/* RTT */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = RTT_SUBCMD_GETCAPABILITY
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_rtt_capabilities
	},
	/* RSSI Monitoring */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_SET_RSSI_MONITOR
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_rssi_monitoring
	},
	/* Packet Keep Alive */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_OFFLOAD_START_MKEEP_ALIVE
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_packet_keep_alive_start
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_OFFLOAD_STOP_MKEEP_ALIVE
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_packet_keep_alive_stop
	},
    {
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_GET_CAPABILITIES
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_capabilities
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_SET_CONFIG
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_config
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_SET_SCAN_CONFIG
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV,
		.doit = mtk_cfg80211_vendor_set_scan_config
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_ENABLE_GSCAN
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_enable_scan
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_ENABLE_FULL_SCAN_RESULTS
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_enable_full_scan_results
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_GET_SCAN_RESULTS
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_scan_results
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_SET_SIGNIFICANT_CHANGE_CONFIG
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_significant_change
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = GSCAN_SUBCMD_SET_HOTLIST
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_hotlist
	},
	/*Link Layer Statistics */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = LSTATS_SUBCMD_GET_INFO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_llstats_get_info
	},
	/* Get Supported Feature Set */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_GET_FEATURE_SET
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
				WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_supported_feature_set
	},
	/* Get Driver Version or Firmware Version */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = LOGGER_GET_VER
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
				WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_get_version
	},
	/* Set Tx Power Scenario */
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_SELECT_TX_POWER_SCENARIO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
				WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_tx_power_scenario
	},
	{
		{
			.vendor_id = GOOGLE_OUI,
			.subcmd = WIFI_SUBCMD_SET_PNO_RANDOM_MAC_OUI
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV
			| WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_scan_mac_oui
	},
	{
		{
			.vendor_id = OUI_QCA,
			.subcmd = QCA_NL80211_VENDOR_SUBCMD_ROAMING
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = mtk_cfg80211_vendor_set_roaming_policy
	},
};

static const struct nl80211_vendor_cmd_info mtk_wlan_vendor_events[] = {
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_SIGNIFICANT_CHANGE_RESULTS
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_HOTLIST_RESULTS_FOUND
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_SCAN_RESULTS_AVAILABLE
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_FULL_SCAN_RESULTS
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = RTT_EVENT_COMPLETE
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_COMPLETE_SCAN
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = GSCAN_EVENT_HOTLIST_RESULTS_LOST
	},
	{
		.vendor_id = GOOGLE_OUI,
		.subcmd = WIFI_EVENT_RSSI_MONITOR
	},
	{
		.vendor_id = OUI_AMAZON,
		.subcmd = AMZN_NL80211_VENDOR_SUBCMD_ROAMING_INFO
	},
};

/* There isn't a lot of sense in it, but you can transmit anything you like */
static const struct ieee80211_txrx_stypes
mtk_cfg80211_ais_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_ADHOC] = {
				  .tx = 0xffff,
				  .rx = BIT(IEEE80211_STYPE_ACTION >> 4)
				  },
	[NL80211_IFTYPE_STATION] = {
				    .tx = 0xffff,
				    .rx = BIT(IEEE80211_STYPE_ACTION >> 4) | BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
				    },
	[NL80211_IFTYPE_AP] = {
			       .tx = 0xffff,
			       .rx = BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4)
			       },
	[NL80211_IFTYPE_AP_VLAN] = {
				    /* copy AP */
				    .tx = 0xffff,
				    .rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
				    BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
				    BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
				    BIT(IEEE80211_STYPE_DISASSOC >> 4) |
				    BIT(IEEE80211_STYPE_AUTH >> 4) |
				    BIT(IEEE80211_STYPE_DEAUTH >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4)
				    },
	[NL80211_IFTYPE_P2P_CLIENT] = {
				       .tx = 0xffff,
				       .rx = BIT(IEEE80211_STYPE_ACTION >> 4) | BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
				       },
	[NL80211_IFTYPE_P2P_GO] = {
				   .tx = 0xffff,
				   .rx = BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4)
				   }
};

#if AMZN_PWR_TABLE_ENABLE
static COUNTRY_POWER_TABLE asCountryPwrTbl[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x1D,
			{0x00, 0x00, 0x00,},
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1c, 0x1b, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1e, 0x1e
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x1D,
			{0x00, 0x00, 0x00,},
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1c, 0x1b, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1e, 0x1e
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x1F,
			{0x00, 0x00, 0x00,},
			0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1B, 0x21
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x1F, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x21, 0x21
			},
		},
};

static COUNTRY_POWER_TABLE power_table_abf123[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x1C,
			{0x00, 0x00, 0x00,},
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1B,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x26, 0x22, 0x1A,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x22, 0x18
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x1C,
			{0x00, 0x00, 0x00,},
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1B,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x26, 0x22, 0x1A,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x22, 0x18
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x24, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x20, 0x20
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x24, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x20, 0x20
			},
		},
};


static COUNTRY_POWER_TABLE power_table_abc123[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x26, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1E, 0x1E
			},
		},
		{
			{'A', 'U',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'N', 'Z',}, /*country code*/
			{/*tx power*/
			0x20,
			{0x00, 0x00, 0x00,},
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1D, 0x1D
			},
		},
};

static COUNTRY_POWER_TABLE power_table_mustang[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1B, 0x1A
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1B, 0x1A
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1B, 0x1C
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1B, 0x1C
			},
		},
		{
			{'A', 'U',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'N', 'Z',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x20, 0x20, 0x20,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1D, 0x1D
			},
		},
		{
			{'C', 'A',}, /*country code*/
			{/*tx power*/
			0x20, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1F, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{/*5G band edge power*/
			0x1, 0x1C, 0x1A
			},
		},
};


static COUNTRY_POWER_TABLE power_table_abh123[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24, /* OFDM */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, /* HT20 */
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* HT40 */
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20, /* 5G_OFDM */
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C, /* 5G_HT_20 */
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C, /* 5G_HT_40 */
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x0,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x24, 0x1E
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x2A,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20,
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C,
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x2A, 0x26, 0x0,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x24, 0x1E
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20,
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C,
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x0, 0x24, 0x0
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20,
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C,
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x0, 0x24, 0x0
			},
		},
};

static COUNTRY_POWER_TABLE power_table_abd123[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24, /* OFDM */
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, /* HT20 */
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* HT40 */
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20, /* 5G_OFDM */
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C, /* 5G_HT_20 */
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C, /* 5G_HT_40 */
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x0,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x24, 0x1E
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x2A,
			{0x00, 0x00, 0x00,},
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20,
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C,
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x2A, 0x26, 0x0,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x24, 0x1E
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x20,
			0x24, 0x24, 0x24, 0x1C, 0x1C, 0x1C,
			0x22, 0x22, 0x22, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x0, 0x24, 0x0
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x22, /* 24CCK */
			{0x00, 0x00, 0x00,},
			/*BPSK, QPSK, 16QAM, rsv,48M, 54M */
			0x24, 0x24, 0x24, 0x26, 0x22, 0x22,
			/* HT20BSPSK, QPSK, 16QAM, MCS5,6,7 */
			0x24, 0x24, 0x24, 0x20, 0x20, 0x20,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 5G BPSK,QPSK,16QAM, rsv, 48,54 */
			0x20, 0x20, 0x20, 0x24, 0x1E, 0x1E,
			/* 5GHT20 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x20, 0x20, 0x20, 0x1E, 0x1E, 0x1E,
			/* 5GHT40 BPSK,QPSK,16QAM, MCS 5,6,7 */
			0x20, 0x20, 0x20, 0x1C, 0x1C, 0x1C,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x26, 0x00,
			},
			0x1,/*5G band support flag*/
			{
			0x0, 0x24, 0x0
			},
		},
};

static COUNTRY_POWER_TABLE power_table_abg123[] = {
		{
			{'W', 'W',}, /*country code*/
			{/*tx power*/
			0x1E,
			{0x00, 0x00, 0x00,},
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1E, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1E, 0x1A
			},
		},
		{
			{'U', 'S',}, /*country code*/
			{/*tx power*/
			0x1E,
			{0x00, 0x00, 0x00,},
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
			0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x1E, 0x1E, 0x1E,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x1E, 0x1A
			},
		},
		{
			{'E', 'U',}, /*country code*/
			{/*tx power*/
			0x24,
			{0x00, 0x00, 0x00,},
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x24, 0x21, 0x21,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x21, 0x21
			},
		},
		{
			{'J', 'P',}, /*country code*/
			{/*tx power*/
			0x25,
			{0x00, 0x00, 0x00,},
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
			},
			0x1,/*tx power valid flag*/
			{/*2.4G band edge power*/
			0x1, 0x25, 0x21, 0x21,
			},
			0x1,/*5G band support flag*/
			{
			0x1, 0x21, 0x21
			},
		},
};

struct board_id_power_table_map board_id_power_table_list[] = {
	{BOARD_ID_MUSTANG_STR, power_table_mustang, ARRAY_SIZE(power_table_mustang)},
	{BOARD_ID_abc123_STR, power_table_abc123, ARRAY_SIZE(power_table_abc123)},
	{BOARD_ID_abf123_STR, power_table_abf123, ARRAY_SIZE(power_table_abf123)},
	{BOARD_ID_abh123_STR, power_table_abh123, ARRAY_SIZE(power_table_abh123)},
	{BOARD_ID_abc123_STR, power_table_abh123, ARRAY_SIZE(power_table_abh123)},
	{BOARD_ID_abd123_STR, power_table_abd123, ARRAY_SIZE(power_table_abd123)},
	{BOARD_ID_abg123_STR, power_table_abg123, ARRAY_SIZE(power_table_abg123)},
};

static const struct ieee80211_iface_limit iface_limits_mcc[] = {
	{ .max = 2,	.types = (BIT(NL80211_IFTYPE_STATION) |
				BIT(NL80211_IFTYPE_ADHOC) |
				BIT(NL80211_IFTYPE_P2P_CLIENT))
	}
};

static const struct ieee80211_iface_combination iface_comb_mcc[] = {
	{
		.limits = iface_limits_mcc,
		.n_limits = ARRAY_SIZE(iface_limits_mcc),
		.max_interfaces = 2,
		.num_different_channels = 2,
		.beacon_int_infra_match = false,
	},
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

/*----------------------------------------------------------------------------*/
/*!
* \brief Override the implementation of select queue
*
* \param[in] dev Pointer to struct net_device
* \param[in] skb Pointer to struct skb_buff
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
unsigned int _cfg80211_classify8021d(struct sk_buff *skb)
{
	unsigned int dscp = 0;

	/* skb->priority values from 256->263 are magic values
	 * directly indicate a specific 802.1d priority.  This is
	 * to allow 802.1d priority to be passed directly in from
	 * tags
	 */

	if (skb->priority >= 256 && skb->priority <= 263)
		return skb->priority - 256;
	switch (skb->protocol) {
	case htons(ETH_P_IP):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	}
	return dscp >> 5;
}

static const UINT_16 au16Wlan1dToQueueIdx[8] = { 1, 0, 0, 1, 2, 2, 3, 3 };

static UINT_16 wlanSelectQueue(struct net_device *dev, struct sk_buff *skb,
				void *accel_priv, select_queue_fallback_t fallback)
{
	skb->priority = _cfg80211_classify8021d(skb);

	return au16Wlan1dToQueueIdx[skb->priority];
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Load NVRAM data and translate it into REG_INFO_T
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
* \param[out] prRegInfo  Pointer to struct REG_INFO_T
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static void glLoadNvram(IN P_GLUE_INFO_T prGlueInfo, OUT P_REG_INFO_T prRegInfo)
{
	UINT_32 i, j;
	UINT_8 aucTmp[2];
	PUINT_8 pucDest;

	ASSERT(prGlueInfo);
	ASSERT(prRegInfo);

	if ((!prGlueInfo) || (!prRegInfo))
		return;

	if (kalCfgDataRead16(prGlueInfo, sizeof(WIFI_CFG_PARAM_STRUCT) - sizeof(UINT_16), (PUINT_16) aucTmp) == TRUE) {
		prGlueInfo->fgNvramAvailable = TRUE;

		/* load MAC Address */
#if !defined(CONFIG_MTK_TC1_FEATURE)
		for (i = 0; i < PARAM_MAC_ADDR_LEN; i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo,
					 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucMacAddress) + i,
					 (PUINT_16) (((PUINT_8) prRegInfo->aucMacAddr) + i));
		}
#else
		TC1_FAC_NAME(FacReadWifiMacAddr) ((unsigned char *)prRegInfo->aucMacAddr);
#endif

		/* load country code */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucCountryCode[0]), (PUINT_16) aucTmp);

		/* cast to wide characters */
		if ('X' == aucTmp[0] && 'X' == aucTmp[1])
			aucTmp[0] = aucTmp[1] = 'W';
		prRegInfo->au2CountryCode[0] = (UINT_16) aucTmp[0];
		prRegInfo->au2CountryCode[1] = (UINT_16) aucTmp[1];

		/* load default normal TX power */
		for (i = 0; i < sizeof(TX_PWR_PARAM_T); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo,
					 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, rTxPwr) + i,
					 (PUINT_16) (((PUINT_8) &(prRegInfo->rTxPwr)) + i));
		}

		/* load feature flags */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucTxPwrValid), (PUINT_16) aucTmp);
		prRegInfo->ucTxPwrValid = aucTmp[0];
		prRegInfo->ucSupport5GBand = aucTmp[1];

		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, uc2G4BwFixed20M), (PUINT_16) aucTmp);
		prRegInfo->uc2G4BwFixed20M = aucTmp[0];
		prRegInfo->uc5GBwFixed20M = aucTmp[1];

		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucEnable5GBand), (PUINT_16) aucTmp);
		prRegInfo->ucEnable5GBand = aucTmp[0];

		/* load EFUSE overriding part */
		for (i = 0; i < sizeof(prRegInfo->aucEFUSE); i += sizeof(UINT_16)) {
			kalCfgDataRead16(prGlueInfo,
					 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucEFUSE) + i,
					 (PUINT_16) (((PUINT_8) &(prRegInfo->aucEFUSE)) + i));
		}

		/* load band edge tx power control */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, fg2G4BandEdgePwrUsed), (PUINT_16) aucTmp);
		prRegInfo->fg2G4BandEdgePwrUsed = (BOOLEAN) aucTmp[0];
		if (aucTmp[0]) {
			prRegInfo->cBandEdgeMaxPwrCCK = (INT_8) aucTmp[1];

			kalCfgDataRead16(prGlueInfo,
					 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, cBandEdgeMaxPwrOFDM20), (PUINT_16) aucTmp);
			prRegInfo->cBandEdgeMaxPwrOFDM20 = (INT_8) aucTmp[0];
			prRegInfo->cBandEdgeMaxPwrOFDM40 = (INT_8) aucTmp[1];
		}

		/* load regulation subbands */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, ucRegChannelListMap), (PUINT_16) aucTmp);
		prRegInfo->eRegChannelListMap = (ENUM_REG_CH_MAP_T) aucTmp[0];
		prRegInfo->ucRegChannelListIndex = aucTmp[1];

		if (prRegInfo->eRegChannelListMap == REG_CH_MAP_CUSTOMIZED) {
			for (i = 0; i < MAX_SUBBAND_NUM; i++) {
				pucDest = (PUINT_8) &prRegInfo->rDomainInfo.rSubBand[i];
				for (j = 0; j < 6; j += sizeof(UINT_16)) {
					kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucRegSubbandInfo)
							 + (i * 6 + j), (PUINT_16) aucTmp);

					*pucDest++ = aucTmp[0];
					*pucDest++ = aucTmp[1];
				}
			}
		}
		/* load RSSI compensation */
		kalCfgDataRead16(prGlueInfo, OFFSET_OF(WIFI_CFG_PARAM_STRUCT, uc2GRssiCompensation), (PUINT_16) aucTmp);
		prRegInfo->uc2GRssiCompensation = aucTmp[0];
		prRegInfo->uc5GRssiCompensation = aucTmp[1];

		kalCfgDataRead16(prGlueInfo,
				 OFFSET_OF(WIFI_CFG_PARAM_STRUCT, fgRssiCompensationValidbit), (PUINT_16) aucTmp);
		prRegInfo->fgRssiCompensationValidbit = aucTmp[0];
		prRegInfo->ucRxAntennanumber = aucTmp[1];
	} else {
		prGlueInfo->fgNvramAvailable = FALSE;
	}

}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Update Channel table for cfg80211 for Wi-Fi Direct based on current country code
 *
 * \param[in] prGlueInfo      Pointer to glue info
 *
 * \return   none
 */
/*----------------------------------------------------------------------------*/
P_COUNTRY_POWER_TABLE wlanGetUpdatedPowerTable(P_UINT_8 paucCountry)
{
#if AMZN_PWR_TABLE_ENABLE
	UINT_8 i = 0;
	COUNTRY_POWER_TABLE *country_pwr_tbl = asCountryPwrTbl;
	int pwr_tbl_size = ARRAY_SIZE(asCountryPwrTbl);

	if (NULL == paucCountry)
		return NULL;

	DBGLOG(INIT, INFO, "aucCountry:%c%c\n", paucCountry[0], paucCountry[1]);

	for (i = 0; i < ARRAY_SIZE(board_id_power_table_list); i++) {
		if (!strncmp(idme_board_id, board_id_power_table_list[i].board_id,
			     strlen(board_id_power_table_list[i].board_id))) {
			country_pwr_tbl = board_id_power_table_list[i].power_table;
			pwr_tbl_size = board_id_power_table_list[i].tbl_size;
			DBGLOG(INIT, INFO, "board_id:%s\n", board_id_power_table_list[i].board_id);
			break;
		}
	}

	for (i = 0; i < pwr_tbl_size; i++) {
		if (paucCountry[0] == country_pwr_tbl[i].auCountryCode[0] &&
		    paucCountry[1] == country_pwr_tbl[i].auCountryCode[1])
			return &country_pwr_tbl[i];
	}

	return NULL;
#else
	return NULL;
#endif
}

#if CFG_ENABLE_WIFI_DIRECT
/*----------------------------------------------------------------------------*/
/*!
* \brief called by txthread, run sub module init function
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID wlanSubModRunInit(P_GLUE_INFO_T prGlueInfo)
{
	/*now, we only have p2p module */
	if (rSubModHandler[P2P_MODULE].fgIsInited == FALSE) {
		rSubModHandler[P2P_MODULE].subModInit(prGlueInfo);
		rSubModHandler[P2P_MODULE].fgIsInited = TRUE;
	}

}

/*----------------------------------------------------------------------------*/
/*!
* \brief called by txthread, run sub module exit function
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID wlanSubModRunExit(P_GLUE_INFO_T prGlueInfo)
{
	/*now, we only have p2p module */
	if (rSubModHandler[P2P_MODULE].fgIsInited == TRUE) {
		rSubModHandler[P2P_MODULE].subModExit(prGlueInfo);
		rSubModHandler[P2P_MODULE].fgIsInited = FALSE;
	}
}

/*----------------------------------------------------------------------------*/
/*!
* \brief set sub module init flag, force TxThread to run sub modle init
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
BOOLEAN wlanSubModInit(P_GLUE_INFO_T prGlueInfo)
{
	/* 4  Mark HALT, notify main thread to finish current job */
	prGlueInfo->ulFlag |= GLUE_FLAG_SUB_MOD_INIT;
	/* wake up main thread */
	wake_up_interruptible(&prGlueInfo->waitq);
	/* wait main thread  finish sub module INIT */
	wait_for_completion_interruptible(&prGlueInfo->rSubModComp);

#if 0
	if (prGlueInfo->prAdapter->fgIsP2PRegistered)
		p2pNetRegister(prGlueInfo);
#endif

	return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief set sub module exit flag, force TxThread to run sub modle exit
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
BOOLEAN wlanSubModExit(P_GLUE_INFO_T prGlueInfo)
{
#if 0
	if (prGlueInfo->prAdapter->fgIsP2PRegistered)
		p2pNetUnregister(prGlueInfo);
#endif

	/* 4  Mark HALT, notify main thread to finish current job */
	prGlueInfo->ulFlag |= GLUE_FLAG_SUB_MOD_EXIT;
	/* wake up main thread */
	wake_up_interruptible(&prGlueInfo->waitq);
	/* wait main thread finish sub module EXIT */
	wait_for_completion_interruptible(&prGlueInfo->rSubModComp);

	return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
* \brief set by sub module, indicate sub module is already inserted
*
* \param[in]  rSubModInit, function pointer point to sub module init function
* \param[in]  rSubModExit,  function pointer point to sub module exit function
* \param[in]  eSubModIdx,  sub module index
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
VOID
wlanSubModRegisterInitExit(SUB_MODULE_INIT rSubModInit, SUB_MODULE_EXIT rSubModExit, ENUM_SUB_MODULE_IDX_T eSubModIdx)
{
	rSubModHandler[eSubModIdx].subModInit = rSubModInit;
	rSubModHandler[eSubModIdx].subModExit = rSubModExit;
	rSubModHandler[eSubModIdx].fgIsInited = FALSE;
}

#if 0
/*----------------------------------------------------------------------------*/
/*!
* \brief check wlan is launched or not
*
* \param[in]  (none)
*
* \return TRUE, wlan is already started
*             FALSE, wlan is not started yet
*/
/*----------------------------------------------------------------------------*/
BOOLEAN wlanIsLaunched(VOID)
{
	struct net_device *prDev = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;

	/* 4 <0> Sanity check */
	ASSERT(u4WlanDevNum <= CFG_MAX_WLAN_DEVICES);
	if (0 == u4WlanDevNum)
		return FALSE;

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;

	ASSERT(prDev);
	if (NULL == prDev)
		return FALSE;

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);
	if (NULL == prGlueInfo)
		return FALSE;

	return prGlueInfo->prAdapter->fgIsWlanLaunched;
}

#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Export wlan GLUE_INFO_T pointer to p2p module
*
* \param[in]  prGlueInfo Pointer to struct GLUE_INFO_T
*
* \return TRUE: get GlueInfo pointer successfully
*            FALSE: wlan is not started yet
*/
/*---------------------------------------------------------------------------*/
BOOLEAN wlanExportGlueInfo(P_GLUE_INFO_T *prGlueInfoExpAddr)
{
	struct net_device *prDev = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (0 == u4WlanDevNum)
		return FALSE;

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	if (NULL == prDev)
		return FALSE;

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	if (NULL == prGlueInfo)
		return FALSE;

	if (FALSE == prGlueInfo->prAdapter->fgIsWlanLaunched)
		return FALSE;

	*prGlueInfoExpAddr = prGlueInfo;
	return TRUE;
}

#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Release prDev from wlandev_array and free tasklet object related to it.
*
* \param[in] prDev  Pointer to struct net_device
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static void wlanClearDevIdx(struct net_device *prDev)
{
	int i;

	ASSERT(prDev);

	for (i = 0; i < CFG_MAX_WLAN_DEVICES; i++) {
		if (arWlanDevInfo[i].prDev == prDev) {
			arWlanDevInfo[i].prDev = NULL;
			u4WlanDevNum--;
		}
	}

}				/* end of wlanClearDevIdx() */

/*----------------------------------------------------------------------------*/
/*!
* \brief Allocate an unique interface index, net_device::ifindex member for this
*        wlan device. Store the net_device in wlandev_array, and initialize
*        tasklet object related to it.
*
* \param[in] prDev  Pointer to struct net_device
*
* \retval >= 0      The device number.
* \retval -1        Fail to get index.
*/
/*----------------------------------------------------------------------------*/
static int wlanGetDevIdx(struct net_device *prDev)
{
	int i;

	ASSERT(prDev);

	for (i = 0; i < CFG_MAX_WLAN_DEVICES; i++) {
		if (arWlanDevInfo[i].prDev == (struct net_device *)NULL) {
			/* Reserve 2 bytes space to store one digit of
			 * device number and NULL terminator.
			 */
			arWlanDevInfo[i].prDev = prDev;
			u4WlanDevNum++;
			return i;
		}
	}

	return -1;
}				/* end of wlanGetDevIdx() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A method of struct net_device, a primary SOCKET interface to configure
*        the interface lively. Handle an ioctl call on one of our devices.
*        Everything Linux ioctl specific is done here. Then we pass the contents
*        of the ifr->data to the request message handler.
*
* \param[in] prDev      Linux kernel netdevice
*
* \param[in] prIFReq    Our private ioctl request structure, typed for the generic
*                       struct ifreq so we can use ptr to function
*
* \param[in] cmd        Command ID
*
* \retval WLAN_STATUS_SUCCESS The IOCTL command is executed successfully.
* \retval OTHER The execution of IOCTL command is failed.
*/
/*----------------------------------------------------------------------------*/
int wlanDoIOCTL(struct net_device *prDev, struct ifreq *prIFReq, int i4Cmd)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	int ret = 0;

	/* Verify input parameters for the following functions */
	ASSERT(prDev && prIFReq);
	if (!prDev || !prIFReq) {
		DBGLOG(INIT, WARN, "%s Invalid input data\n", __func__);
		return -EINVAL;
	}

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		DBGLOG(INIT, WARN, "%s No glue info\n", __func__);
		return -EFAULT;
	}

	if (prGlueInfo->u4ReadyFlag == 0)
		return -EINVAL;

	if (i4Cmd == SIOCGIWPRIV) {
		/* 0x8B0D, get private ioctl table */
		ret = wext_get_priv(prDev, prIFReq);
	} else if ((i4Cmd >= SIOCIWFIRST) && (i4Cmd < SIOCIWFIRSTPRIV)) {
		/* 0x8B00 ~ 0x8BDF, wireless extension region */
		ret = wext_support_ioctl(prDev, prIFReq, i4Cmd);
	} else if ((i4Cmd >= SIOCIWFIRSTPRIV) && (i4Cmd < SIOCIWLASTPRIV)) {
		/* 0x8BE0 ~ 0x8BFF, private ioctl region */
		ret = priv_support_ioctl(prDev, prIFReq, i4Cmd);
	} else if (i4Cmd == SIOCDEVPRIVATE + 1) {
		ret = priv_support_driver_cmd(prDev, prIFReq, i4Cmd);
	} else {
		DBGLOG(INIT, WARN, "Unexpected ioctl command: 0x%04x\n", i4Cmd);
		/* return 0 for safe? */
	}

	return ret;
}				/* end of wlanDoIOCTL() */

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is to set multicast list and set rx mode.
*
* \param[in] prDev  Pointer to struct net_device
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/

static struct delayed_work workq;
static struct net_device *gPrDev;
static BOOLEAN fgIsWorkMcStart = FALSE;
static BOOLEAN fgIsWorkMcEverInit = FALSE;
static struct wireless_dev *gprWdev;
#ifdef CONFIG_PM
static const struct wiphy_wowlan_support wlan_wowlan_support = {
	.flags = WIPHY_WOWLAN_DISCONNECT | WIPHY_WOWLAN_ANY,
};
#endif
static void createWirelessDevice(void)
{
	struct wiphy *prWiphy = NULL;
	struct wireless_dev *prWdev = NULL;
#if CFG_SUPPORT_PERSIST_NETDEV
	struct net_device *prNetDev = NULL;
#endif

	/* <1.1> Create wireless_dev */
	prWdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!prWdev) {
		DBGLOG(INIT, ERROR, "Allocating memory to wireless_dev context failed\n");
		return;
	}

	/* initialize semaphore for halt */
	sema_init(&g_halt_sem, 1);
	g_u4HaltFlag = 1;

	/* <1.2> Create wiphy */
	prWiphy = wiphy_new(&mtk_wlan_ops, sizeof(GLUE_INFO_T));
	if (!prWiphy) {
		DBGLOG(INIT, ERROR, "Allocating memory to wiphy device failed\n");
		goto free_wdev;
	}

	/* <1.3> configure wireless_dev & wiphy */
	prWdev->iftype = NL80211_IFTYPE_STATION;
	prWiphy->max_scan_ssids   = 4;    /* FIXME: for combo scan */
	prWiphy->max_scan_ie_len = 512;

	prWiphy->max_sched_scan_ssids	= CFG_SCAN_SSID_MAX_NUM;
	prWiphy->max_match_sets		= CFG_SCAN_SSID_MATCH_MAX_NUM;
	prWiphy->max_sched_scan_ie_len	= CFG_CFG80211_IE_BUF_LEN;

	prWiphy->interface_modes	= BIT(NL80211_IFTYPE_STATION) |
					BIT(NL80211_IFTYPE_ADHOC) |
					BIT(NL80211_IFTYPE_P2P_CLIENT);
	prWiphy->bands[NL80211_BAND_2GHZ] = &mtk_band_2ghz;
	/* always assign 5Ghz bands here, if the chip is not support 5Ghz,
		bands[IEEE80211_BAND_5GHZ] will be assign to NULL */
	prWiphy->bands[NL80211_BAND_5GHZ] = &mtk_band_5ghz;
	prWiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	prWiphy->cipher_suites = (const u32 *)mtk_cipher_suites;
	prWiphy->n_cipher_suites = ARRAY_SIZE(mtk_cipher_suites);
	prWiphy->flags = WIPHY_FLAG_SUPPORTS_FW_ROAM
			| WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL
			| WIPHY_FLAG_SUPPORTS_SCHED_SCAN;
	prWiphy->regulatory_flags = REGULATORY_CUSTOM_REG;
#if (CFG_SUPPORT_TDLS == 1)
	TDLSEX_WIPHY_FLAGS_INIT(prWiphy->flags);
#endif /* CFG_SUPPORT_TDLS */
	prWiphy->max_remain_on_channel_duration = 5000;
	prWiphy->mgmt_stypes = mtk_cfg80211_ais_default_mgmt_stypes;
	prWiphy->vendor_commands = mtk_wlan_vendor_ops;
	prWiphy->n_vendor_commands = sizeof(mtk_wlan_vendor_ops) / sizeof(struct wiphy_vendor_command);
	prWiphy->vendor_events = mtk_wlan_vendor_events;
	prWiphy->n_vendor_events = ARRAY_SIZE(mtk_wlan_vendor_events);

	/* <1.4> wowlan support */
#ifdef CONFIG_PM
	prWiphy->wowlan = &wlan_wowlan_support;
#endif
#ifdef CONFIG_CFG80211_WEXT
	 /* <1.5> Use wireless extension to replace IOCTL */
	prWiphy->wext = &wext_handler_def;
#endif

	/* For Jungle request to support M channel setting here */
	prWiphy->iface_combinations =  iface_comb_mcc;
	prWiphy->n_iface_combinations = ARRAY_SIZE(iface_comb_mcc);
	DBGLOG(INIT, ERROR, "The num_different_channels is %d\n",
			iface_comb_mcc[0].num_different_channels);
	if (wiphy_register(prWiphy) < 0) {
		DBGLOG(INIT, ERROR, "wiphy_register error\n");
		goto free_wiphy;
	}

	prWdev->wiphy = prWiphy;

#if CFG_SUPPORT_PERSIST_NETDEV
	/* <2> allocate and register net_device */
#if CFG_TC1_FEATURE
	if (wlan_if_changed)
		prNetDev = alloc_netdev_mq(sizeof(P_GLUE_INFO_T), NIC_INF_NAME_IN_AP_MODE, NET_NAME_PREDICTABLE,
									ether_setup, CFG_MAX_TXQ_NUM);
	else
#else
		prNetDev = alloc_netdev_mq(sizeof(P_GLUE_INFO_T), NIC_INF_NAME, NET_NAME_PREDICTABLE,
									ether_setup, CFG_MAX_TXQ_NUM);
#endif
	if (!prNetDev) {
		DBGLOG(INIT, ERROR, "Allocating memory to net_device context failed\n");
		goto unregister_wiphy;
	}

	*((P_GLUE_INFO_T *) netdev_priv(prNetDev)) = (P_GLUE_INFO_T) wiphy_priv(prWiphy);

	 prNetDev->netdev_ops = &wlan_netdev_ops;
#ifdef CONFIG_WIRELESS_EXT
	prNetDev->wireless_handlers = &wext_handler_def;
#endif
	netif_carrier_off(prNetDev);
	netif_tx_stop_all_queues(prNetDev);

	/* <2.1> co-relate with wireless_dev bi-directionally */
	prNetDev->ieee80211_ptr = prWdev;
	prWdev->netdev = prNetDev;
#if CFG_TCP_IP_CHKSUM_OFFLOAD
	prNetDev->features = NETIF_F_HW_CSUM;
#endif

	/* <2.2> co-relate net device & device tree */
	SET_NETDEV_DEV(prNetDev, wiphy_dev(prWiphy));

	/* <2.3> register net_device */
	if (register_netdev(prWdev->netdev) < 0) {
		DBGLOG(INIT, ERROR, "wlanNetRegister: net_device context is not registered.\n");
		goto unregister_wiphy;
	}
#endif /* CFG_SUPPORT_PERSIST_NETDEV */

	gprWdev = prWdev;
	DBGLOG(INIT, INFO, "create wireless device success\n");
	return;

#if CFG_SUPPORT_PERSIST_NETDEV
unregister_wiphy:
	wiphy_unregister(prWiphy);
#endif
free_wiphy:
	wiphy_free(prWiphy);
free_wdev:
	kfree(prWdev);
}

static void destroyWirelessDevice(void)
{
#if CFG_SUPPORT_PERSIST_NETDEV
	unregister_netdev(gprWdev->netdev);
	free_netdev(gprWdev->netdev);
#endif
	wiphy_unregister(gprWdev->wiphy);
	wiphy_free(gprWdev->wiphy);
	kfree(gprWdev);
	gprWdev = NULL;
}

static void wlanSetMulticastList(struct net_device *prDev)
{
	gPrDev = prDev;
	schedule_delayed_work(&workq, 0);
}

/* FIXME: Since we cannot sleep in the wlanSetMulticastList, we arrange
 * another workqueue for sleeping. We don't want to block
 * tx_thread, so we can't let tx_thread to do this */

static void wlanSetMulticastListWorkQueue(struct work_struct *work)
{

	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4PacketFilter = 0;
	UINT_32 u4SetInfoLen;
	struct net_device *prDev = gPrDev;

	fgIsWorkMcStart = TRUE;

	DBGLOG(INIT, INFO, "wlanSetMulticastListWorkQueue start...\n");

	down(&g_halt_sem);
	if (g_u4HaltFlag) {
		fgIsWorkMcStart = FALSE;
		up(&g_halt_sem);
		return;
	}

	prGlueInfo = (NULL != prDev) ? *((P_GLUE_INFO_T *) netdev_priv(prDev)) : NULL;
	ASSERT(prDev);
	ASSERT(prGlueInfo);
	if (!prDev || !prGlueInfo) {
		DBGLOG(INIT, WARN, "abnormal dev or skb: prDev(0x%p), prGlueInfo(0x%p)\n", prDev, prGlueInfo);
		fgIsWorkMcStart = FALSE;
		up(&g_halt_sem);
		return;
	}

	if (prDev->flags & IFF_PROMISC)
		u4PacketFilter |= PARAM_PACKET_FILTER_PROMISCUOUS;

	if (prDev->flags & IFF_BROADCAST)
		u4PacketFilter |= PARAM_PACKET_FILTER_BROADCAST;

	if (prDev->flags & IFF_MULTICAST) {
		if ((prDev->flags & IFF_ALLMULTI) ||
		    (netdev_mc_count(prDev) > MAX_NUM_GROUP_ADDR)) {

			u4PacketFilter |= PARAM_PACKET_FILTER_ALL_MULTICAST;
		} else {
			u4PacketFilter |= PARAM_PACKET_FILTER_MULTICAST;
		}
	}

	up(&g_halt_sem);

	if (kalIoctl(prGlueInfo,
		     wlanoidSetCurrentPacketFilter,
		     &u4PacketFilter,
		     sizeof(u4PacketFilter), FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen) != WLAN_STATUS_SUCCESS) {
		fgIsWorkMcStart = FALSE;
		return;
	}

	if (u4PacketFilter & PARAM_PACKET_FILTER_MULTICAST) {
		/* Prepare multicast address list */
		struct netdev_hw_addr *ha;
		PUINT_8 prMCAddrList = NULL;
		UINT_32 i = 0;

		down(&g_halt_sem);
		if (g_u4HaltFlag) {
			fgIsWorkMcStart = FALSE;
			up(&g_halt_sem);
			return;
		}

		prMCAddrList = kalMemAlloc(MAX_NUM_GROUP_ADDR * ETH_ALEN, VIR_MEM_TYPE);

		netdev_for_each_mc_addr(ha, prDev) {
			if (i < MAX_NUM_GROUP_ADDR) {
				memcpy((prMCAddrList + i * ETH_ALEN), ha->addr, ETH_ALEN);
				i++;
			}
		}

		up(&g_halt_sem);

		kalIoctl(prGlueInfo,
			 wlanoidSetMulticastList,
			 prMCAddrList, (i * ETH_ALEN), FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

		kalMemFree(prMCAddrList, VIR_MEM_TYPE, MAX_NUM_GROUP_ADDR * ETH_ALEN);
	}

	fgIsWorkMcStart = FALSE;
	DBGLOG(INIT, INFO, "wlanSetMulticastListWorkQueue end\n");

}				/* end of wlanSetMulticastList() */

/*----------------------------------------------------------------------------*/
/*!
* \brief    To indicate scheduled scan has been stopped
*
* \param[in]
*           prGlueInfo
*
* \return
*           None
*/
/*----------------------------------------------------------------------------*/
VOID wlanSchedScanStoppedWorkQueue(struct work_struct *work)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct net_device *prDev = gPrDev;

	prGlueInfo = (NULL != prDev) ? *((P_GLUE_INFO_T *) netdev_priv(prDev)) : NULL;
	if (!prGlueInfo) {
		DBGLOG(SCN, ERROR, "prGlueInfo == NULL unexpected\n");
		return;
	}

	/* 2. indication to cfg80211 */
	/* 20150205 change cfg80211_sched_scan_stopped to work queue due to sched_scan_mtx dead lock issue */
	cfg80211_sched_scan_stopped(priv_to_wiphy(prGlueInfo));
	DBGLOG(SCN, INFO,
		"cfg80211_sched_scan_stopped event send done\n");

}

/* FIXME: Since we cannot sleep in the wlanSetMulticastList, we arrange
 * another workqueue for sleeping. We don't want to block
 * tx_thread, so we can't let tx_thread to do this */

void p2pSetMulticastListWorkQueueWrapper(P_GLUE_INFO_T prGlueInfo)
{

	ASSERT(prGlueInfo);

	if (!prGlueInfo) {
		DBGLOG(INIT, WARN, "abnormal dev or skb: prGlueInfo(0x%p)\n", prGlueInfo);
		return;
	}
#if CFG_ENABLE_WIFI_DIRECT
	if (prGlueInfo->prAdapter->fgIsP2PRegistered)
		mtk_p2p_wext_set_Multicastlist(prGlueInfo);
#endif

}				/* end of p2pSetMulticastListWorkQueueWrapper() */

/*----------------------------------------------------------------------------*/
/*!
* \brief This function is TX entry point of NET DEVICE.
*
* \param[in] prSkb  Pointer of the sk_buff to be sent
* \param[in] prDev  Pointer to struct net_device
*
* \retval NETDEV_TX_OK - on success.
* \retval NETDEV_TX_BUSY - on failure, packet will be discarded by upper layer.
*/
/*----------------------------------------------------------------------------*/
int wlanHardStartXmit(struct sk_buff *prSkb, struct net_device *prDev)
{
	P_GLUE_INFO_T prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

	P_QUE_ENTRY_T prQueueEntry = NULL;
	P_QUE_T prTxQueue = NULL;
	UINT_16 u2QueueIdx = 0;
#if (CFG_SUPPORT_TDLS_DBG == 1)
	UINT16 u2Identifier = 0;
#endif

#if CFG_BOW_TEST
	UINT_32 i;
#endif

	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prSkb);
	ASSERT(prDev);
	ASSERT(prGlueInfo);

#if (CFG_SUPPORT_TDLS_DBG == 1)
	{
		UINT8 *pkt = prSkb->data;

		if ((*(pkt + 12) == 0x08) && (*(pkt + 13) == 0x00)) {
			/* ip */
			u2Identifier = ((*(pkt + 18)) << 8) | (*(pkt + 19));
			/* u2TdlsTxSeq[u4TdlsTxSeqId ++] = u2Identifier; */
			DBGLOG(INIT, INFO, "<s> %d\n", u2Identifier);
		}
	}
#endif
	/* check if WiFi is halt */
	if (prGlueInfo->ulFlag & GLUE_FLAG_HALT) {
		DBGLOG(INIT, INFO, "GLUE_FLAG_HALT skip tx\n");
		dev_kfree_skb(prSkb);
		return NETDEV_TX_OK;
	}
#if CFG_SUPPORT_HOTSPOT_2_0
	if (prGlueInfo->fgIsDad) {
		/* kalPrint("[Passpoint R2] Due to ipv4_dad...TX is forbidden\n"); */
		dev_kfree_skb(prSkb);
		return NETDEV_TX_OK;
	}
	if (prGlueInfo->fgIs6Dad) {
		/* kalPrint("[Passpoint R2] Due to ipv6_dad...TX is forbidden\n"); */
		dev_kfree_skb(prSkb);
		return NETDEV_TX_OK;
	}
#endif

	STATS_TX_TIME_ARRIVE(prSkb);
	prQueueEntry = (P_QUE_ENTRY_T) GLUE_GET_PKT_QUEUE_ENTRY(prSkb);
	prTxQueue = &prGlueInfo->rTxQueue;

#if CFG_BOW_TEST
	DBGLOG(BOW, TRACE, "sk_buff->len: %d\n", prSkb->len);
	DBGLOG(BOW, TRACE, "sk_buff->data_len: %d\n", prSkb->data_len);
	DBGLOG(BOW, TRACE, "sk_buff->data:\n");

	for (i = 0; i < prSkb->len; i++) {
		DBGLOG(BOW, TRACE, "%4x", prSkb->data[i]);

		if ((i + 1) % 16 == 0)
			DBGLOG(BOW, TRACE, "\n");
	}

	DBGLOG(BOW, TRACE, "\n");
#endif

	if (wlanProcessSecurityFrame(prGlueInfo->prAdapter, (P_NATIVE_PACKET) prSkb) == FALSE) {

		/* non-1x packets */

#if CFG_DBG_GPIO_PINS
		{
			/* TX request from OS */
			mtk_wcn_stp_debug_gpio_assert(IDX_TX_REQ, DBG_TIE_LOW);
			kalUdelay(1);
			mtk_wcn_stp_debug_gpio_assert(IDX_TX_REQ, DBG_TIE_HIGH);
		}
#endif

		u2QueueIdx = skb_get_queue_mapping(prSkb);
		ASSERT(u2QueueIdx < CFG_MAX_TXQ_NUM);

#if CFG_ENABLE_PKT_LIFETIME_PROFILE
		GLUE_SET_PKT_ARRIVAL_TIME(prSkb, kalGetTimeTick());
#endif
		GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingFrameNum);
		if (u2QueueIdx < CFG_MAX_TXQ_NUM)
			GLUE_INC_REF_CNT(prGlueInfo->ai4TxPendingFrameNumPerQueue[NETWORK_TYPE_AIS_INDEX][u2QueueIdx]);

		GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
		QUEUE_INSERT_TAIL(prTxQueue, prQueueEntry);
		GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);

/* GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingFrameNum); */
/* GLUE_INC_REF_CNT(prGlueInfo->ai4TxPendingFrameNumPerQueue[NETWORK_TYPE_AIS_INDEX][u2QueueIdx]); */

		if (u2QueueIdx < CFG_MAX_TXQ_NUM) {
			if (prGlueInfo->ai4TxPendingFrameNumPerQueue[NETWORK_TYPE_AIS_INDEX][u2QueueIdx] >=
			    CFG_TX_STOP_NETIF_PER_QUEUE_THRESHOLD) {
				netif_stop_subqueue(prDev, u2QueueIdx);

#if (CONF_HIF_LOOPBACK_AUTO == 1)
			prGlueInfo->rHifInfo.HifLoopbkFlg |= 0x01;
#endif /* CONF_HIF_LOOPBACK_AUTO */
			}
		}
	} else {
		GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingSecurityFrameNum);
	}

	DBGLOG(TX, EVENT, "\n+++++ pending frame %d len = %d +++++\n", prGlueInfo->i4TxPendingFrameNum, prSkb->len);
	prGlueInfo->rNetDevStats.tx_bytes += prSkb->len;
	prGlueInfo->rNetDevStats.tx_packets++;

	/* set GLUE_FLAG_TXREQ_BIT */

	/* pr->u4Flag |= GLUE_FLAG_TXREQ; */
	/* wake_up_interruptible(&prGlueInfo->waitq); */
	kalSetEvent(prGlueInfo);

	/* For Linux, we'll always return OK FLAG, because we'll free this skb by ourself */
	return NETDEV_TX_OK;
}				/* end of wlanHardStartXmit() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A method of struct net_device, to get the network interface statistical
*        information.
*
* Whenever an application needs to get statistics for the interface, this method
* is called. This happens, for example, when ifconfig or netstat -i is run.
*
* \param[in] prDev      Pointer to struct net_device.
*
* \return net_device_stats buffer pointer.
*/
/*----------------------------------------------------------------------------*/
struct net_device_stats *wlanGetStats(IN struct net_device *prDev)
{
	P_GLUE_INFO_T prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

#if 0
	WLAN_STATUS rStatus;
	UINT_32 u4XmitError = 0;
	UINT_32 u4XmitOk = 0;
	UINT_32 u4RecvError = 0;
	UINT_32 u4RecvOk = 0;
	UINT_32 u4BufLen;

	ASSERT(prDev);

	/* @FIX ME: need a more clear way to do this */

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryXmitError, &u4XmitError, sizeof(UINT_32), TRUE, TRUE, TRUE, &u4BufLen);

	rStatus = kalIoctl(prGlueInfo, wlanoidQueryXmitOk, &u4XmitOk, sizeof(UINT_32), TRUE, TRUE, TRUE, &u4BufLen);
	rStatus = kalIoctl(prGlueInfo, wlanoidQueryRcvOk, &u4RecvOk, sizeof(UINT_32), TRUE, TRUE, TRUE, &u4BufLen);
	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryRcvError, &u4RecvError, sizeof(UINT_32), TRUE, TRUE, TRUE, &u4BufLen);
	prGlueInfo->rNetDevStats.rx_packets = u4RecvOk;
	prGlueInfo->rNetDevStats.tx_packets = u4XmitOk;
	prGlueInfo->rNetDevStats.tx_errors = u4XmitError;
	prGlueInfo->rNetDevStats.rx_errors = u4RecvError;
	/* prGlueInfo->rNetDevStats.rx_bytes   = rCustomNetDevStats.u4RxBytes; */
	/* prGlueInfo->rNetDevStats.tx_bytes   = rCustomNetDevStats.u4TxBytes; */
	/* prGlueInfo->rNetDevStats.rx_errors  = rCustomNetDevStats.u4RxErrors; */
	/* prGlueInfo->rNetDevStats.multicast  = rCustomNetDevStats.u4Multicast; */
#endif
	/* prGlueInfo->rNetDevStats.rx_packets = 0; */
	/* prGlueInfo->rNetDevStats.tx_packets = 0; */
	prGlueInfo->rNetDevStats.tx_errors = 0;
	prGlueInfo->rNetDevStats.rx_errors = 0;
	/* prGlueInfo->rNetDevStats.rx_bytes   = 0; */
	/* prGlueInfo->rNetDevStats.tx_bytes   = 0; */
	prGlueInfo->rNetDevStats.rx_errors = 0;
	prGlueInfo->rNetDevStats.multicast = 0;

	return &prGlueInfo->rNetDevStats;

}				/* end of wlanGetStats() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A function for prDev->init
*
* \param[in] prDev      Pointer to struct net_device.
*
* \retval 0         The execution of wlanInit succeeds.
* \retval -ENXIO    No such device.
*/
/*----------------------------------------------------------------------------*/
static int wlanInit(struct net_device *prDev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (fgIsWorkMcEverInit == FALSE) {
		if (!prDev)
			return -ENXIO;

		prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
		INIT_DELAYED_WORK(&workq, wlanSetMulticastListWorkQueue);

		/* 20150205 work queue for sched_scan */
		INIT_DELAYED_WORK(&sched_workq, wlanSchedScanStoppedWorkQueue);

		fgIsWorkMcEverInit = TRUE;
	}

	return 0;		/* success */
}				/* end of wlanInit() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A function for prDev->uninit
*
* \param[in] prDev      Pointer to struct net_device.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static void wlanUninit(struct net_device *prDev)
{

}				/* end of wlanUninit() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief A method of struct net_device, to set the randomized mac address
 *
 * This method is called before Wifi Framework requests a new conenction with
 * enabled feature "Connected Random Mac".
 *
 * \param[in] ndev	Pointer to struct net_device.
 * \param[in] addr	Randomized Mac address passed from WIFI framework.
 *
 * \return int.
 */
/*----------------------------------------------------------------------------*/
static int wlanSetMacAddress(struct net_device *ndev, void *addr)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_BSS_INFO_T prAisBssInfo = NULL;
	struct sockaddr *sa = NULL;
	UINT_8 aucMacAddr[MAC_ADDR_LEN];
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen = 0;

	/**********************************************************************
	 * Check if kernel passes valid data to us                            *
	 **********************************************************************
	 */
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(ndev));
	if (!ndev || !addr || !prGlueInfo) {
		DBGLOG(INIT, ERROR,
		       "Set macaddr with ndev(%d), glue(%d) and addr(%d)\n",
		       (ndev == NULL) ? 0 : 1, (prGlueInfo == NULL) ? 0 : 1,
		       (addr == NULL) ? 0 : 1);
		return WLAN_STATUS_INVALID_DATA;
	}

	/**********************************************************************
	 * 1. Change OwnMacAddr which will be updated to FW through           *
	 *    rlmActivateNetwork later.                                       *
	 * 2. Change dev_addr stored in kernel to notify framework that the   *
	 *    mac addr has been changed and what the new value is.            *
	 **********************************************************************
	 */
	sa = (struct sockaddr *)addr;
	COPY_MAC_ADDR(aucMacAddr, sa->sa_data);

	rStatus = kalIoctl(prGlueInfo, wlanoidSetRandomMac,
			   (PVOID)(&aucMacAddr), sizeof(PARAM_MAC_ADDR_LEN),
			   FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(REQ, ERROR, "set random mac failed:%x\n", rStatus);
		return -EINVAL;
	}

	return rStatus;
}				/* end of wlanSetMacAddr() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A function for prDev->open
*
* \param[in] prDev      Pointer to struct net_device.
*
* \retval 0     The execution of wlanOpen succeeds.
* \retval < 0   The execution of wlanOpen failed.
*/
/*----------------------------------------------------------------------------*/
static int wlanOpen(struct net_device *prDev)
{
#if CFG_SUPPORT_WAKEUP_STATISTICS
	P_GLUE_INFO_T prGlueInfo = NULL;
#endif

	ASSERT(prDev);

	netif_tx_start_all_queues(prDev);

#if CFG_SUPPORT_WAKEUP_STATISTICS
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

	/* Initialize arWakeupStatistic */
	kalMemSet(prGlueInfo->prAdapter->arWakeupStatistic, 0,
		  sizeof(prGlueInfo->prAdapter->arWakeupStatistic));
	/* Initialize wake_event_count */
	kalMemSet(prGlueInfo->prAdapter->wake_event_count, 0,
		  sizeof(prGlueInfo->prAdapter->wake_event_count));
#endif
#if CFG_SUPPORT_EXCEPTION_STATISTICS
	kalMemSet(prGlueInfo->prAdapter->beacon_timeout_count, 0,
			sizeof(prGlueInfo->prAdapter->beacon_timeout_count));
	prGlueInfo->prAdapter->total_beacon_timeout_count = 0;

	kalMemSet(prGlueInfo->prAdapter->tx_done_fail_count, 0,
			sizeof(prGlueInfo->prAdapter->tx_done_fail_count));
	prGlueInfo->prAdapter->total_tx_done_fail_count = 0;

	kalMemSet(prGlueInfo->prAdapter->deauth_rx_count, 0,
			sizeof(prGlueInfo->prAdapter->deauth_rx_count));
	prGlueInfo->prAdapter->total_deauth_rx_count = 0;

	prGlueInfo->prAdapter->total_scandone_timeout_count = 0;
	prGlueInfo->prAdapter->total_mgmtTX_timeout_count = 0;
	prGlueInfo->prAdapter->total_mgmtRX_timeout_count = 0;
#endif

	return 0;		/* success */
}				/* end of wlanOpen() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A function for prDev->stop
*
* \param[in] prDev      Pointer to struct net_device.
*
* \retval 0     The execution of wlanStop succeeds.
* \retval < 0   The execution of wlanStop failed.
*/
/*----------------------------------------------------------------------------*/
static int wlanStop(struct net_device *prDev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct cfg80211_scan_request *prScanRequest = NULL;
	struct cfg80211_scan_info info = {
		.aborted = (bool)TRUE,
    };

	GLUE_SPIN_LOCK_DECLARATION();

	ASSERT(prDev);

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

	/* CFG80211 down */
	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);
	if (prGlueInfo->prScanRequest != NULL) {
		prScanRequest = prGlueInfo->prScanRequest;
		prGlueInfo->prScanRequest = NULL;
		kalMemZero(prGlueInfo->rScanRequestChannel, sizeof(prGlueInfo->rScanRequestChannel));
	}
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);

	if (prScanRequest)
		cfg80211_scan_done(prScanRequest, &info);
		/*cfg80211_scan_done(prScanRequest, TRUE);*/
	netif_tx_stop_all_queues(prDev);

	return 0;		/* success */
}				/* end of wlanStop() */

/*----------------------------------------------------------------------------*/
/*!
 * \brief Update Channel table for cfg80211 for Wi-Fi Direct based on current country code
 *
 * \param[in] prGlueInfo      Pointer to glue info
 *
 * \return   none
 */
/*----------------------------------------------------------------------------*/
VOID wlanUpdateChannelTable(P_GLUE_INFO_T prGlueInfo)
{
	UINT_8 i, j;
	UINT_8 ucNumOfChannel;
	RF_CHANNEL_INFO_T aucChannelList[ARRAY_SIZE(mtk_2ghz_channels) + ARRAY_SIZE(mtk_5ghz_channels)];

	/* 1. Disable all channel */
	for (i = 0; i < ARRAY_SIZE(mtk_2ghz_channels); i++) {
		mtk_2ghz_channels[i].flags |= IEEE80211_CHAN_DISABLED;
		mtk_2ghz_channels[i].orig_flags |= IEEE80211_CHAN_DISABLED;
	}

	for (i = 0; i < ARRAY_SIZE(mtk_5ghz_channels); i++) {
		mtk_5ghz_channels[i].flags |= IEEE80211_CHAN_DISABLED;
		mtk_5ghz_channels[i].orig_flags |= IEEE80211_CHAN_DISABLED;
	}

	/* 2. Get current domain channel list */
	rlmDomainGetChnlList(prGlueInfo->prAdapter,
			     BAND_NULL, FALSE,
			     ARRAY_SIZE(mtk_2ghz_channels) + ARRAY_SIZE(mtk_5ghz_channels),
			     &ucNumOfChannel, aucChannelList);

	/* 3. Enable specific channel based on domain channel list */
	for (i = 0; i < ucNumOfChannel; i++) {
		switch (aucChannelList[i].eBand) {
		case BAND_2G4:
			for (j = 0; j < ARRAY_SIZE(mtk_2ghz_channels); j++) {
				if (mtk_2ghz_channels[j].hw_value == aucChannelList[i].ucChannelNum) {
					mtk_2ghz_channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
					mtk_2ghz_channels[j].orig_flags &= ~IEEE80211_CHAN_DISABLED;
					break;
				}
			}
			break;

		case BAND_5G:
			for (j = 0; j < ARRAY_SIZE(mtk_5ghz_channels); j++) {
				if (mtk_5ghz_channels[j].hw_value == aucChannelList[i].ucChannelNum) {
					mtk_5ghz_channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
					mtk_5ghz_channels[j].orig_flags &= ~IEEE80211_CHAN_DISABLED;
					break;
				}
			}
			break;

		default:
			break;
		}
	}

}

/*----------------------------------------------------------------------------*/
/*!
* \brief Register the device to the kernel and return the index.
*
* \param[in] prDev      Pointer to struct net_device.
*
* \retval 0     The execution of wlanNetRegister succeeds.
* \retval < 0   The execution of wlanNetRegister failed.
*/
/*----------------------------------------------------------------------------*/
static INT_32 wlanNetRegister(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T prGlueInfo;
	INT_32 i4DevIdx = -1;

	ASSERT(prWdev);

	do {
		if (!prWdev)
			break;

		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
		i4DevIdx = wlanGetDevIdx(prWdev->netdev);
		if (i4DevIdx < 0) {
			DBGLOG(INIT, ERROR, "wlanNetRegister: net_device number exceeds.\n");
			break;
		}

		/* adjust channel support status */
		wlanUpdateChannelTable(prGlueInfo);
#if !CFG_SUPPORT_PERSIST_NETDEV
		if (register_netdev(prWdev->netdev) < 0) {
			DBGLOG(INIT, ERROR, "wlanNetRegister: net_device context is not registered.\n");

			wiphy_unregister(prWdev->wiphy);
			wlanClearDevIdx(prWdev->netdev);
			i4DevIdx = -1;
		}
#endif
		if (i4DevIdx != -1)
			prGlueInfo->fgIsRegistered = TRUE;
	} while (FALSE);

	return i4DevIdx;	/* success */
}				/* end of wlanNetRegister() */

/*----------------------------------------------------------------------------*/
/*!
* \brief Unregister the device from the kernel
*
* \param[in] prWdev      Pointer to struct net_device.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID wlanNetUnregister(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T prGlueInfo;

	if (!prWdev) {
		DBGLOG(INIT, ERROR, "wlanNetUnregister: The device context is NULL\n");
		return;
	}
	DBGLOG(INIT, TRACE, "unregister net_dev(0x%p)\n", prWdev->netdev);
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
	wlanClearDevIdx(prWdev->netdev);
#if !CFG_SUPPORT_PERSIST_NETDEV
	unregister_netdev(prWdev->netdev);
#endif
	prGlueInfo->fgIsRegistered = FALSE;

	DBGLOG(INIT, INFO, "unregister wireless_dev(0x%p), ifindex=%d\n", prWdev, prWdev->netdev->ifindex);

}				/* end of wlanNetUnregister() */

static const struct net_device_ops wlan_netdev_ops = {
	.ndo_open = wlanOpen,
	.ndo_stop = wlanStop,
	.ndo_set_rx_mode = wlanSetMulticastList,
	.ndo_get_stats = wlanGetStats,
	.ndo_do_ioctl = wlanDoIOCTL,
	.ndo_start_xmit = wlanHardStartXmit,
	.ndo_init = wlanInit,
	.ndo_uninit = wlanUninit,
	.ndo_select_queue = wlanSelectQueue,
	.ndo_set_mac_address = wlanSetMacAddress,
};

/*----------------------------------------------------------------------------*/
/*!
* \brief A method for creating Linux NET4 struct net_device object and the
*        private data(prGlueInfo and prAdapter). Setup the IO address to the HIF.
*        Assign the function pointer to the net_device object
*
* \param[in] pvData     Memory address for the device
*
* \retval Not null      The wireless_dev object.
* \retval NULL          Fail to create wireless_dev object
*/
/*----------------------------------------------------------------------------*/
static struct lock_class_key rSpinKey[SPIN_LOCK_NUM];
static struct lock_class_key rLockKey;
static struct wireless_dev *wlanNetCreate(PVOID pvData)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct wireless_dev *prWdev = gprWdev;
	UINT_32 i;
	struct device *prDev;

	if (!prWdev) {
		DBGLOG(INIT, ERROR, "Allocating memory to wireless_dev context failed\n");
		return NULL;
	}
	/* 4 <1> co-relate wiphy & prDev */
#if MTK_WCN_HIF_SDIO
	mtk_wcn_hif_sdio_get_dev(*((MTK_WCN_HIF_SDIO_CLTCTX *) pvData), &prDev);
#else
/* prDev = &((struct sdio_func *) pvData)->dev; //samp */
	prDev = pvData;		/* samp */
#endif
	if (!prDev)
		DBGLOG(INIT, WARN, "unable to get struct dev for wlan\n");
	/* don't set prDev as parent of wiphy->dev, because we have done device_add
		in driver init. if we set parent here, parent will be not able to know this child,
		and may occurs a KE in device_shutdown, to free wiphy->dev, because his parent
		has been freed. */
	/*set_wiphy_dev(prWdev->wiphy, prDev);*/

#if !CFG_SUPPORT_PERSIST_NETDEV
	/* 4 <3> Initial Glue structure */
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
	kalMemZero(prGlueInfo, sizeof(GLUE_INFO_T));
	/* 4 <3.1> Create net device */
#if CFG_TC1_FEATURE
	if (wlan_if_changed) {
		prGlueInfo->prDevHandler = alloc_netdev_mq(sizeof(P_GLUE_INFO_T), NIC_INF_NAME_IN_AP_MODE,
							NET_NAME_PREDICTABLE, ether_setup, CFG_MAX_TXQ_NUM);
	} else {
		prGlueInfo->prDevHandler = alloc_netdev_mq(sizeof(P_GLUE_INFO_T), NIC_INF_NAME, NET_NAME_PREDICTABLE,
							ether_setup, CFG_MAX_TXQ_NUM);
	}
#else
	prGlueInfo->prDevHandler = alloc_netdev_mq(sizeof(P_GLUE_INFO_T), NIC_INF_NAME, NET_NAME_PREDICTABLE,
						ether_setup, CFG_MAX_TXQ_NUM);
#endif
	if (!prGlueInfo->prDevHandler) {
		DBGLOG(INIT, ERROR, "Allocating memory to net_device context failed\n");
		return NULL;
	}
	DBGLOG(INIT, INFO, "net_device prDev(0x%p) allocated ifindex=%d\n",
			prGlueInfo->prDevHandler, prGlueInfo->prDevHandler->ifindex);

	/* 4 <3.1.1> initialize net device varaiables */
	*((P_GLUE_INFO_T *) netdev_priv(prGlueInfo->prDevHandler)) = prGlueInfo;

	prGlueInfo->prDevHandler->netdev_ops = &wlan_netdev_ops;
#ifdef CONFIG_WIRELESS_EXT
	prGlueInfo->prDevHandler->wireless_handlers = &wext_handler_def;
#endif
	netif_carrier_off(prGlueInfo->prDevHandler);
	netif_tx_stop_all_queues(prGlueInfo->prDevHandler);

	/* 4 <3.1.2> co-relate with wiphy bi-directionally */
	prGlueInfo->prDevHandler->ieee80211_ptr = prWdev;
#if CFG_TCP_IP_CHKSUM_OFFLOAD
	prGlueInfo->prDevHandler->features = NETIF_F_HW_CSUM;
#endif
	prWdev->netdev = prGlueInfo->prDevHandler;

	/* 4 <3.1.3> co-relate net device & prDev */
	/*SET_NETDEV_DEV(prGlueInfo->prDevHandler, wiphy_dev(prWdev->wiphy));*/
	SET_NETDEV_DEV(prGlueInfo->prDevHandler, prDev);
#else /* CFG_SUPPORT_PERSIST_NETDEV */
	prGlueInfo->prDevHandler = gprWdev->netdev;
#endif /* CFG_SUPPORT_PERSIST_NETDEV */

	/* 4 <3.2> initiali glue variables */
	prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
	prGlueInfo->ePowerState = ParamDeviceStateD0;
	prGlueInfo->fgIsMacAddrOverride = FALSE;
	prGlueInfo->fgIsRegistered = FALSE;
	prGlueInfo->prScanRequest = NULL;
	kalMemZero(prGlueInfo->rScanRequestChannel, sizeof(prGlueInfo->rScanRequestChannel));

#if CFG_SUPPORT_HOTSPOT_2_0
	/* Init DAD */
	prGlueInfo->fgIsDad = FALSE;
	prGlueInfo->fgIs6Dad = FALSE;
	kalMemZero(prGlueInfo->aucDADipv4, 4);
	kalMemZero(prGlueInfo->aucDADipv6, 16);
#endif

	init_completion(&prGlueInfo->rScanComp);
	init_completion(&prGlueInfo->rHaltComp);
	init_completion(&prGlueInfo->rPendComp);
#if CFG_ENABLE_WIFI_DIRECT
	init_completion(&prGlueInfo->rSubModComp);
#endif

	/* initialize timer for OID timeout checker */
	kalOsTimerInitialize(prGlueInfo, kalTimeoutHandler);

	for (i = 0; i < SPIN_LOCK_NUM; i++) {
		spin_lock_init(&prGlueInfo->rSpinLock[i]);
		lockdep_set_class(&prGlueInfo->rSpinLock[i], &rSpinKey[i]);
	}

	/* initialize semaphore for ioctl */
	sema_init(&prGlueInfo->ioctl_sem, 1);

	glSetHifInfo(prGlueInfo, (ULONG) pvData);

	/* 4 <8> Init Queues */
	init_waitqueue_head(&prGlueInfo->waitq);
	QUEUE_INITIALIZE(&prGlueInfo->rCmdQueue);
	QUEUE_INITIALIZE(&prGlueInfo->rTxQueue);

	/* 4 <4> Create Adapter structure */
	prGlueInfo->prAdapter = (P_ADAPTER_T) wlanAdapterCreate(prGlueInfo);

	if (!prGlueInfo->prAdapter) {
		DBGLOG(INIT, ERROR, "Allocating memory to adapter failed\n");
		return NULL;
	}
	KAL_WAKE_LOCK_INIT(prAdapter, &prGlueInfo->rAhbIsrWakeLock, "WLAN AHB ISR");
#if CFG_SUPPORT_PERSIST_NETDEV
	dev_open(prGlueInfo->prDevHandler);
	netif_carrier_off(prGlueInfo->prDevHandler);
	netif_tx_stop_all_queues(prGlueInfo->prDevHandler);
#endif
	return prWdev;
}				/* end of wlanNetCreate() */

/*----------------------------------------------------------------------------*/
/*!
* \brief Destroying the struct net_device object and the private data.
*
* \param[in] prWdev      Pointer to struct wireless_dev.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID wlanNetDestroy(struct wireless_dev *prWdev)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(prWdev);

	if (!prWdev) {
		DBGLOG(INIT, ERROR, "wlanNetDestroy: The device context is NULL\n");
		return;
	}

	/* prGlueInfo is allocated with net_device */
	prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
	ASSERT(prGlueInfo);

	/* destroy kal OS timer */
	kalCancelTimer(prGlueInfo);

	glClearHifInfo(prGlueInfo);

	wlanAdapterDestroy(prGlueInfo->prAdapter);
	prGlueInfo->prAdapter = NULL;

#if CFG_SUPPORT_PERSIST_NETDEV
	/* take the net_device to down state */
	dev_close(prGlueInfo->prDevHandler);
#else
	/* Free net_device and private data prGlueInfo, which are allocated by alloc_netdev(). */
	free_netdev(prWdev->netdev);
#endif

}				/* end of wlanNetDestroy() */

#ifndef CONFIG_X86
UINT_8 g_aucBufIpAddr[64] = {0};
static void wlanNotifyFwSuspend(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgSuspend)
{
	WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
	UINT_32 u4SetInfoLen;

	rStatus = kalIoctl(prGlueInfo,
			wlanoidNotifyFwSuspend,
			(PVOID)&fgSuspend,
			sizeof(fgSuspend),
			FALSE,
			FALSE,
			TRUE,
			FALSE,
			&u4SetInfoLen);
	if (rStatus != WLAN_STATUS_SUCCESS)
		DBGLOG(INIT, INFO, "wlanNotifyFwSuspend fail\n");
}

void wlanHandleSystemSuspend(void)
{
	WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
	struct net_device *prDev = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_8 ip[4] = { 0 };
	UINT_32 u4NumIPv4 = 0;
#ifdef CONFIG_IPV6
	UINT_8 ip6[16] = { 0 };	/* FIX ME: avoid to allocate large memory in stack */
	UINT_32 u4NumIPv6 = 0;
	P_PARAM_NETWORK_ADDRESS_IPV6 prParamIpv6Addr;
#endif
	UINT_32 i;
	P_PARAM_NETWORK_ADDRESS_IP prParamIpAddr;

	DBGLOG(INIT, INFO, "*********wlan System Suspend************\n");

	/* <1> Sanity check and acquire the net_device */
	ASSERT(u4WlanDevNum <= CFG_MAX_WLAN_DEVICES);
	if (u4WlanDevNum == 0) {
		DBGLOG(INIT, ERROR, "wlanEarlySuspend u4WlanDevNum==0 invalid!!\n");
		return;
	}
	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
/* ASSERT(prDev); */

	fgIsUnderSuspend = true;
	/* <2> acquire the prGlueInfo */
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);

	/* <3> get the IPv4 address */
	if (!prDev || !(prDev->ip_ptr) ||
	    !((struct in_device *)(prDev->ip_ptr))->ifa_list ||
	    !(&(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local))) {
		DBGLOG(INIT, INFO, "ip is not available.\n");
		goto notify_suspend;
	}

	/* <4> copy the IPv4 address */
	kalMemCopy(ip, &(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local), sizeof(ip));
	DBGLOG(INIT, INFO, "ip is %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

	/* todo: traverse between list to find whole sets of IPv4 addresses */
	if (!((ip[0] == 0) && (ip[1] == 0) && (ip[2] == 0) && (ip[3] == 0)))
		u4NumIPv4++;
#ifdef CONFIG_IPV6
	/* <5> get the IPv6 address */
	if (!prDev || !(prDev->ip6_ptr) ||\
		!((struct inet6_dev *)(prDev->ip6_ptr))->addr_list.next) {
		DBGLOG(INIT, WARN, "ipv6 is not avaliable.\n");
		u4NumIPv6 = 0;
	} else {
			struct inet6_ifaddr *ifa;
		struct list_head *addr_list;
		addr_list = &(((struct inet6_dev *)(prDev->ip6_ptr))->addr_list);
		ifa = list_entry(addr_list->next, typeof (*ifa), if_list);

		/* <6> copy the IPv6 address*/
		kalMemCopy(ip6, &(ifa->addr.s6_addr[0]), sizeof (ip6));
		DBGLOG(INIT, INFO, "ipv6 is %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\n",
				ip6[0], ip6[1], ip6[2], ip6[3],
				ip6[4], ip6[5], ip6[6], ip6[7],
				ip6[8], ip6[9], ip6[10], ip6[11],
				ip6[12], ip6[13], ip6[14], ip6[15]);

		/* todo: traverse between list to find whole sets of IPv6 addresses*/
		for (i = 0; i < sizeof (ip6); i++) {
			if (ip6[i] != 0) {
				u4NumIPv6 = 1;
				break;
			}
		}
	}
#endif

	/* <7> set up the ARP filter */
	{
		UINT_32 u4SetInfoLen = 0;
/* UINT_8 aucBuf[32] = {0}; */
		UINT_32 u4Len = OFFSET_OF(PARAM_NETWORK_ADDRESS_LIST, arAddress);
		P_PARAM_NETWORK_ADDRESS_LIST prParamNetAddrList = (P_PARAM_NETWORK_ADDRESS_LIST) g_aucBufIpAddr;
		/* aucBuf; */
		P_PARAM_NETWORK_ADDRESS prParamNetAddr = prParamNetAddrList->arAddress;

		kalMemZero(g_aucBufIpAddr, sizeof(g_aucBufIpAddr));

#ifdef CONFIG_IPV6
		prParamNetAddrList->u4AddressCount = u4NumIPv4 + u4NumIPv6;
		DBGLOG(INIT, INFO, "u4NumIPv4:%d, u4NumIPv6:%d\n", u4NumIPv4, u4NumIPv6);
#else
		prParamNetAddrList->u4AddressCount = u4NumIPv4;
		DBGLOG(INIT, INFO, "u4NumIPv4:%d\n", u4NumIPv4);
#endif
		prParamNetAddrList->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
		if (u4NumIPv4 > 0) {
			DBGLOG(INIT, LOUD, "prParamNetAddr:%p\n", prParamNetAddr);
			prParamNetAddr->u2AddressLength = sizeof(PARAM_NETWORK_ADDRESS_IP);
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;;
			prParamIpAddr = (P_PARAM_NETWORK_ADDRESS_IP)prParamNetAddr->aucAddress;
			kalMemCopy(&prParamIpAddr->in_addr, ip, sizeof(ip));
			prParamNetAddr = (P_PARAM_NETWORK_ADDRESS)((ULONG)prParamNetAddr + prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress));
			u4Len += prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress);
			DBGLOG(INIT, LOUD, "prParamNetAddr:%p\n", prParamNetAddr);
		}
#ifdef CONFIG_IPV6
		if (u4NumIPv6 > 0) {
			DBGLOG(INIT, LOUD, "prParamNetAddr:%p\n", prParamNetAddr);
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
			prParamNetAddr->u2AddressLength = sizeof (PARAM_NETWORK_ADDRESS_IPV6);
			prParamIpv6Addr = (P_PARAM_NETWORK_ADDRESS_IPV6)prParamNetAddr->aucAddress;
			kalMemCopy(prParamIpv6Addr->addr, ip6, sizeof (ip6));
			prParamNetAddr = (P_PARAM_NETWORK_ADDRESS)((ULONG)prParamNetAddr + prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress));
			u4Len += prParamNetAddr->u2AddressLength + OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress);
			DBGLOG(INIT, LOUD, "prParamIpv6Addr:%p\n", prParamIpv6Addr);
		}
#endif
		ASSERT(u4Len <= sizeof(g_aucBufIpAddr /*aucBuf */));

		DBGLOG(INIT, LOUD, "kalIoctl (0x%p, 0x%p)\n", prGlueInfo, prParamNetAddrList);
		rStatus = kalIoctl(prGlueInfo,
				wlanoidSetNetworkAddress,
				(PVOID)prParamNetAddrList,
				u4Len,
				FALSE,
				FALSE,
				TRUE,
				FALSE,
				&u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, "set HW pattern filter fail 0x%x\n", rStatus);
		}
#ifdef CONFIG_IPV6
		DBGLOG(INIT, LOUD, "kalIoctl (0x%p, 0x%p)\n", prGlueInfo, prParamNetAddrList);
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
			DBGLOG(INIT, ERROR, "set IPv6 HW pattern filter fail 0x%x\n", rStatus);
		}
#endif
    }

notify_suspend:
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(INIT, WARN, "[Suspend Error] set HW pattern filter fail 0x%x\n", rStatus);
		wlanNotifyFwSuspend(prGlueInfo, TRUE);
	}
}

void wlanHandleSystemResume(void)
{
	struct net_device *prDev = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
	UINT_8 ip[4] = { 0 };
#ifdef CONFIG_IPV6
	UINT_8 ip6[16] = { 0 };	/* FIX ME: avoid to allocate large memory in stack */
#endif
	EVENT_AIS_BSS_INFO_T rParam;
	UINT_32 u4BufLen = 0;

	DBGLOG(INIT, INFO, "*********wlan System Resume************\n");

	/* <1> Sanity check and acquire the net_device */
	ASSERT(u4WlanDevNum <= CFG_MAX_WLAN_DEVICES);
	if (u4WlanDevNum == 0) {
		DBGLOG(INIT, ERROR, "wlanLateResume u4WlanDevNum==0 invalid!!\n");
		return;
	}
	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	/* ASSERT(prDev); */

	fgIsUnderSuspend = false;

	if (!prDev) {
		DBGLOG(INIT, INFO, "prDev == NULL!!!\n");
		return;
	}
	/* <3> acquire the prGlueInfo */
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);

#if 1
	/*
	   We will receive the event in rx, we will check if the status is the same in driver
	   and FW, if not the same, trigger disconnetion procedure.
	 */

	kalMemZero(&rParam, sizeof(EVENT_AIS_BSS_INFO_T));

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryBSSInfo,
			   &rParam, sizeof(EVENT_AIS_BSS_INFO_T), TRUE, TRUE, TRUE, FALSE, &u4BufLen);
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(INIT, ERROR, "Query BSSinfo fail 0x%x!!\n", rStatus);
	} else {
		DBGLOG(INIT, INFO,
		       "Status[%d], Mode[%d], Active[%d]\\n", rParam.eConnectionState, rParam.eCurrentOPMode,
			rParam.fgIsNetActive);
	}
#endif

	/* <2> get the IPv4 address */
	if (!(prDev->ip_ptr) ||
	    !((struct in_device *)(prDev->ip_ptr))->ifa_list ||
	    !(&(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local))) {
		DBGLOG(INIT, INFO, "ip is not available.\n");
		goto notify_resume;
	}
	/* <4> copy the IPv4 address */
	kalMemCopy(ip, &(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local), sizeof(ip));
	DBGLOG(INIT, INFO, "ip is %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

#ifdef CONFIG_IPV6
	/* <5> get the IPv6 address */
	if (!prDev || !(prDev->ip6_ptr) ||\
		!((struct inet6_dev *)(prDev->ip6_ptr))->addr_list.next) {
		DBGLOG(INIT, WARN, "ipv6 is not avaliable.\n");
	} else {
		struct inet6_ifaddr *ifa;
		struct list_head *addr_list;
		addr_list = &(((struct inet6_dev *)(prDev->ip6_ptr))->addr_list);
		ifa = list_entry(addr_list->next, typeof (*ifa), if_list);

		/* <6> copy the IPv6 address*/
		kalMemCopy(ip6, &(ifa->addr.s6_addr[0]), sizeof (ip6));
		DBGLOG(INIT, INFO, "ipv6 is %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\n",
				ip6[0], ip6[1], ip6[2], ip6[3],
				ip6[4], ip6[5], ip6[6], ip6[7],
				ip6[8], ip6[9], ip6[10], ip6[11],
				ip6[12], ip6[13], ip6[14], ip6[15]);
	}
#endif
	/* <7> clear the ARP filter */
	{
		UINT_32 u4SetInfoLen = 0;
/* UINT_8 aucBuf[32] = {0}; */
		UINT_32 u4Len = sizeof(PARAM_NETWORK_ADDRESS_LIST);
		P_PARAM_NETWORK_ADDRESS_LIST prParamNetAddrList = (P_PARAM_NETWORK_ADDRESS_LIST) g_aucBufIpAddr;
		/* aucBuf; */

		kalMemZero(g_aucBufIpAddr, sizeof(g_aucBufIpAddr));

		prParamNetAddrList->u4AddressCount = 0;
		prParamNetAddrList->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;

		ASSERT(u4Len <= sizeof(g_aucBufIpAddr /*aucBuf */));
		rStatus = kalIoctl(prGlueInfo,
				wlanoidSetNetworkAddress,
				(PVOID)prParamNetAddrList,
				u4Len,
				FALSE,
				FALSE,
				TRUE,
				FALSE,
				&u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, "clear HW pattern filter fail 0x%x\n", rStatus);
		}
#ifdef CONFIG_IPV6
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
			DBGLOG(INIT, ERROR, "clear IPv6 HW pattern filter fail 0x%x\n", rStatus);
		}
#endif
	}

notify_resume:
	if (rStatus != WLAN_STATUS_SUCCESS) {
		DBGLOG(INIT, WARN, "set HW pattern filter fail 0x%x\n", rStatus);
		wlanNotifyFwSuspend(prGlueInfo, FALSE);
	}
}
#endif /* ! CONFIG_X86 */

int set_p2p_mode_handler(struct net_device *netdev, PARAM_CUSTOM_P2P_SET_STRUCT_T p2pmode)
{
#if 0
	P_GLUE_INFO_T prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(netdev));
	PARAM_CUSTOM_P2P_SET_STRUCT_T rSetP2P;
	WLAN_STATUS rWlanStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	rSetP2P.u4Enable = p2pmode.u4Enable;
	rSetP2P.u4Mode = p2pmode.u4Mode;

	if (!rSetP2P.u4Enable)
		p2pNetUnregister(prGlueInfo, TRUE);

	rWlanStatus = kalIoctl(prGlueInfo,
			       wlanoidSetP2pMode,
			       (PVOID) &rSetP2P,
			       sizeof(PARAM_CUSTOM_P2P_SET_STRUCT_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
	DBGLOG(INIT, INFO, "ret = %d\n", rWlanStatus);
	if (rSetP2P.u4Enable)
		p2pNetRegister(prGlueInfo, TRUE);

	return 0;

#else

	P_GLUE_INFO_T prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(netdev));
	PARAM_CUSTOM_P2P_SET_STRUCT_T rSetP2P;
	PARAM_CUSTOM_P2P_SET_STRUCT_T rResetP2P;
	WLAN_STATUS rWlanStatus = WLAN_STATUS_SUCCESS;
	BOOLEAN fgIsP2PEnding;
	UINT_32 u4BufLen = 0;

	GLUE_SPIN_LOCK_DECLARATION();

	DBGLOG(INIT, INFO, "%u %u\n", (UINT_32) p2pmode.u4Enable, (UINT_32) p2pmode.u4Mode);

	/* avoid remove & p2p off command simultaneously */
	GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
	fgIsP2PEnding = g_u4P2PEnding;
	g_u4P2POnOffing = 1;
	GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);

	if (fgIsP2PEnding == 1) {
		/* skip the command if we are removing */
		GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
		g_u4P2POnOffing = 0;
		GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);
		return 0;
	}

#if !CFG_SUPPORT_PERSIST_NETDEV
	/* Resetting p2p mode if registered to avoid launch KE */
	if (p2pmode.u4Enable && (prGlueInfo->prAdapter->fgIsP2PRegistered) &&
		(fgIsResetting == FALSE)) {
		rResetP2P.u4Enable = 0;
		rResetP2P.u4Mode = 0;

		p2pNetUnregister(prGlueInfo, FALSE);

		rWlanStatus = kalIoctl(prGlueInfo,
								wlanoidSetP2pMode,
								(PVOID)&rResetP2P,/* pu4IntBuf[0]is used as input SubCmd */
								sizeof(PARAM_CUSTOM_P2P_SET_STRUCT_T),
								FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		DBGLOG(INIT, WARN, "Resetting p2p mode ret = 0x%08x\n", rWlanStatus);
	}
#endif

	rSetP2P.u4Enable = p2pmode.u4Enable;
	rSetP2P.u4Mode = p2pmode.u4Mode;

#if !CFG_SUPPORT_PERSIST_NETDEV
	if ((!rSetP2P.u4Enable) && (fgIsResetting == FALSE))
		p2pNetUnregister(prGlueInfo, TRUE);
#endif
	/* move out to caller to avoid kalIoctrl & suspend/resume deadlock problem ALPS00844864 */
	/*
	   Scenario:
	   1. System enters suspend/resume but not yet enter wlanearlysuspend()
	   or wlanlateresume();

	   2. System switches to do PRIV_CMD_P2P_MODE and execute kalIoctl()
	   and get g_halt_sem then do glRegisterEarlySuspend() or
	   glUnregisterEarlySuspend();

	   But system suspend/resume procedure is not yet finished so we
	   suspend;

	   3. System switches back to do suspend/resume procedure and execute
	   kalIoctl(). But driver does not yet release g_halt_sem so system
	   suspend in wlanearlysuspend() or wlanlateresume();

	   ==> deadlock occurs.
	 */

	rWlanStatus = kalIoctl(prGlueInfo, wlanoidSetP2pMode, (PVOID) &rSetP2P,/* pu4IntBuf[0]is used as input SubCmd */
			       sizeof(PARAM_CUSTOM_P2P_SET_STRUCT_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
#if !CFG_SUPPORT_PERSIST_NETDEV
	/* Need to check fgIsP2PRegistered, in case of whole chip reset.
	 * in this case, kalIOCTL return success always,
	 * and prGlueInfo->prP2pInfo may be NULL */
	if ((rSetP2P.u4Enable) && (prGlueInfo->prAdapter->fgIsP2PRegistered) && (fgIsResetting == FALSE))
		p2pNetRegister(prGlueInfo, TRUE);
#endif
	GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
	g_u4P2POnOffing = 0;
	GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);
	return 0;
#endif
}

#if 0
static void set_dbg_level_handler(unsigned char dbg_lvl[DBG_MODULE_NUM])
{
	kalMemCopy(aucDebugModule, dbg_lvl, sizeof(aucDebugModule));
	kalPrint("[wlan] change debug level");
}
#endif
static void wlanGetDefaultWifiMfg(P_REG_INFO_T prRegInfo)
{
	static TX_PWR_PARAM_T rTxPwr_default = {
		0x20, { 0x20, 0x00, 0x00 }, /*cTxPwr2G4Cck*/ /*cTxPwr2G4Dsss*/
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, /*cTxPwr2G4OFDM*/
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, /*cTxPwr2G4HT20*/
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, /*cTxPwr2G4HT40*/
		0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, /*cTxPwr5GOFDM*/
		0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, /*cTxPwr5GHT20*/
		0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D }; /*cTxPwr5GHT40*/ /* TX_PWR_PARAM_T */

	static UINT_8 aucEFUSE_default[144] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x20, 0x18, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* aucEFUSE */

	/* MAC Address */
#if !defined(CONFIG_MTK_TC1_FEATURE)
	memset(&prRegInfo->aucMacAddr, 0, PARAM_MAC_ADDR_LEN);
#else
	TC1_FAC_NAME(FacReadWifiMacAddr) ((unsigned char *)prRegInfo->aucMacAddr);
#endif

	/* country code */
	prRegInfo->au2CountryCode[0] = (UINT_16)0;
	prRegInfo->au2CountryCode[1] = (UINT_16)0;

	/* default normal TX power */
	prRegInfo->rTxPwr = rTxPwr_default;

	/* feature flags */
	prRegInfo->ucTxPwrValid = 1;
	prRegInfo->ucSupport5GBand = 1;

	prRegInfo->uc2G4BwFixed20M = 0;
	prRegInfo->uc5GBwFixed20M = 0;

	prRegInfo->ucEnable5GBand = 1;

	/* EFUSE overriding part */
	memcpy(prRegInfo->aucEFUSE, aucEFUSE_default, 144);

	/* band edge tx power control */
	prRegInfo->fg2G4BandEdgePwrUsed = 1;
	prRegInfo->cBandEdgeMaxPwrCCK = 0x26;
	prRegInfo->cBandEdgeMaxPwrOFDM20 = 0x1E;
	prRegInfo->cBandEdgeMaxPwrOFDM40 = 0x1A;

	/* regulation subbands */
	prRegInfo->eRegChannelListMap = (ENUM_REG_CH_MAP_T)0;
	prRegInfo->ucRegChannelListIndex = 0;

	/* RSSI compensation */
	prRegInfo->uc2GRssiCompensation = 0;
	prRegInfo->uc5GRssiCompensation = 0;

	prRegInfo->fgRssiCompensationValidbit = 0;
	prRegInfo->ucRxAntennanumber = 0;
}

#ifdef CONFIG_IDME

WIFI_CFG_PARAM_STRUCT idme_wifi_mfg;

static void wlanCopyIdmeWifiMfg(P_REG_INFO_T prRegInfo)
{
	int i, j;
	PUINT_8 pucDest;

	/* MAC Address */
#if !defined(CONFIG_MTK_TC1_FEATURE)
	memcpy(prRegInfo->aucMacAddr, idme_wifi_mfg.aucMacAddress,
		PARAM_MAC_ADDR_LEN);
#else
	TC1_FAC_NAME(FacReadWifiMacAddr) ((unsigned char *)prRegInfo->aucMacAddr);
#endif

	/* country code */

	/* cast to wide characters */
	if ('X' == idme_wifi_mfg.aucCountryCode[0] &&
		'X' == idme_wifi_mfg.aucCountryCode[1])
		idme_wifi_mfg.aucCountryCode[0] = idme_wifi_mfg.aucCountryCode[1] = 'W';

	prRegInfo->au2CountryCode[0] = (UINT_16)idme_wifi_mfg.aucCountryCode[0];
	prRegInfo->au2CountryCode[1] = (UINT_16)idme_wifi_mfg.aucCountryCode[1];

	/* default normal TX power */
	prRegInfo->rTxPwr = idme_wifi_mfg.rTxPwr;

	/* feature flags */
	prRegInfo->ucTxPwrValid = idme_wifi_mfg.ucTxPwrValid;
	prRegInfo->ucSupport5GBand = idme_wifi_mfg.ucSupport5GBand;

	prRegInfo->uc2G4BwFixed20M = idme_wifi_mfg.uc2G4BwFixed20M;
	prRegInfo->uc5GBwFixed20M = idme_wifi_mfg.uc5GBwFixed20M;

	prRegInfo->ucEnable5GBand = idme_wifi_mfg.ucEnable5GBand;

	/* EFUSE overriding part */
	memcpy(prRegInfo->aucEFUSE, idme_wifi_mfg.aucEFUSE, 144);

	/* band edge tx power control */
	prRegInfo->fg2G4BandEdgePwrUsed = idme_wifi_mfg.fg2G4BandEdgePwrUsed;
	if (prRegInfo->fg2G4BandEdgePwrUsed) {
		prRegInfo->cBandEdgeMaxPwrCCK = idme_wifi_mfg.cBandEdgeMaxPwrCCK;
		prRegInfo->cBandEdgeMaxPwrOFDM20 = idme_wifi_mfg.cBandEdgeMaxPwrOFDM20;
		prRegInfo->cBandEdgeMaxPwrOFDM40 = idme_wifi_mfg.cBandEdgeMaxPwrOFDM40;
	}

	/* regulation subbands */
	prRegInfo->eRegChannelListMap =
		(ENUM_REG_CH_MAP_T)idme_wifi_mfg.ucRegChannelListMap;
	prRegInfo->ucRegChannelListIndex = idme_wifi_mfg.ucRegChannelListIndex;

	if (prRegInfo->eRegChannelListMap == REG_CH_MAP_CUSTOMIZED) {
		for (i = 0; i < MAX_SUBBAND_NUM; i++) {
			pucDest = (PUINT_8)&prRegInfo->rDomainInfo.rSubBand[i];

			for (j = 0; j < 6; j++)
				*pucDest++ = idme_wifi_mfg.aucRegSubbandInfo[i * 6 + j];
		}
	}

	/* RSSI compensation */
	if ((kalStrnCmp(CONFIG_ARCH_MTK_PROJECT, "abf123", 4) == 0 || kalStrnCmp(CONFIG_ARCH_MTK_PROJECT, "abc123", 6) == 0) && (prRegInfo->fgRssiCompensationValidbit == FALSE)) {
		prRegInfo->uc2GRssiCompensation = 4;
		prRegInfo->uc5GRssiCompensation = 4;
		prRegInfo->fgRssiCompensationValidbit = TRUE;
	} else {
		prRegInfo->uc2GRssiCompensation =
					idme_wifi_mfg.uc2GRssiCompensation;
		prRegInfo->uc5GRssiCompensation =
					idme_wifi_mfg.uc5GRssiCompensation;

		prRegInfo->fgRssiCompensationValidbit =
			idme_wifi_mfg.fgRssiCompensationValidbit;
	}
	prRegInfo->ucRxAntennanumber = idme_wifi_mfg.ucRxAntennanumber;
}

static void idme_get_mac_addr(P_REG_INFO_T prRegInfo)
{
	struct device_node *ap;
	int len, i, ret;
	char buf[3] = {0};

	ap = of_find_node_by_path(IDME_OF_MAC_ADDR);
	if (likely(ap)) {
		const char *mac_addr = of_get_property(ap, "value", &len);

		if (likely(len >= 12)) {
		for (i = 0; i < 12; i += 2) {
			buf[0] = mac_addr[i];
			buf[1] = mac_addr[i + 1];
			ret = kstrtou8(buf, 16, &prRegInfo->aucMacAddr[i >> 1]);
			if (ret)
				DBGLOG(INIT, WARN, "idme_get_mac_addr kstrtou8 failed\n");
			}
		}
	}
}

static int idme_get_wifi_mfg(P_REG_INFO_T prRegInfo)
{
	struct device_node *ap;
	int i, len;
	int ret = 0;
	char buf[3] = {0};
	PUINT_8 p;

	ap = of_find_node_by_path(IDME_OF_WIFI_MFG);
	if (likely(ap)) {
		const char *wifi_mfg = of_get_property(ap, "value", &len);

		if (likely(len >= 1024)) {
			p = (PUINT_8) &idme_wifi_mfg;
			for (i = 0; i < 1024; i += 2) {
				buf[0] = wifi_mfg[i];
				buf[1] = wifi_mfg[i + 1];
				ret = kstrtou8(buf, 16, &p[i/2]);
				if (ret)
					DBGLOG(INIT, WARN, "kstrtou8 failed, i=%d\n", i);
			}
		} else {
			DBGLOG(INIT, WARN, "idme wifi_mfg len err=%d\n", len);
			ret = -1;
		}
	} else {
		DBGLOG(INIT, WARN, "no idme wifi_mfg\n");
		ret = -1;
	}

	return ret;
}

static void idme_get_board_id(P_REG_INFO_T prRegInfo)
{
	struct device_node *ap;
	int len;

	ap = of_find_node_by_path(IDME_OF_BOARD_ID);
	if (likely(ap)) {
		const char *board_id = of_get_property(ap, "value", &len);
		if (likely(len >= 16))
			memcpy(idme_board_id, board_id, sizeof(idme_board_id));
	}
}
#endif

INT_32 wlanRegulatoryHint(PUINT_8 pauCountryCode)
{
	if (NULL == gprWdev || NULL == pauCountryCode) {
		DBGLOG(INIT, WARN, "gprWdev:0x%p, pauCountryCode:0x%p\n", gprWdev, pauCountryCode);
		return -1;
	}
	return regulatory_hint(gprWdev->wiphy, pauCountryCode);
}

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
static VOID wlanDrvCommonWork(struct work_struct *work)
{
	struct DRV_COMMON_WORK_T *prDrvWork;
	QUE_T rTempQue;
	struct DRV_COMMON_WORK_FUNC_T *prWorkFunc;
	ULONG ulFlags;

	if (!work)
		return;
	prDrvWork = container_of(work, struct DRV_COMMON_WORK_T, rWork);
	spin_lock_irqsave(&prDrvWork->rWorkFuncQueLock, ulFlags);
	QUEUE_MOVE_ALL(&rTempQue, &prDrvWork->rWorkFuncQue);
	spin_unlock_irqrestore(&prDrvWork->rWorkFuncQueLock, ulFlags);

	DBGLOG(INIT, TRACE, "DRV common work, %u\n", rTempQue.u4NumElem);
	QUEUE_REMOVE_HEAD(&rTempQue, prWorkFunc, struct DRV_COMMON_WORK_FUNC_T *);
	while (prWorkFunc) {
		prWorkFunc->work_func(prWorkFunc->params);
		QUEUE_REMOVE_HEAD(&rTempQue, prWorkFunc, struct DRV_COMMON_WORK_FUNC_T *);
	}
}
#endif

/*----------------------------------------------------------------------------*/
/*!
* \brief Wlan probe function. This function probes and initializes the device.
*
* \param[in] pvData     data passed by bus driver init function
*                           _HIF_EHPI: NULL
*                           _HIF_SDIO: sdio bus driver handle
*
* \retval 0 Success
* \retval negative value Failed
*/
/*----------------------------------------------------------------------------*/
static INT_32 wlanProbe(PVOID pvData)
{
	struct wireless_dev *prWdev = NULL;
	P_WLANDEV_INFO_T prWlandevInfo = NULL;
	INT_32 i4DevIdx = 0;
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_ADAPTER_T prAdapter = NULL;
	INT_32 i4Status = 0;
	BOOLEAN bRet = FALSE;

	do {
		/* 4 <1> Initialize the IO port of the interface */
		/*  GeorgeKuo: pData has different meaning for _HIF_XXX:
		 * _HIF_EHPI: pointer to memory base variable, which will be
		 *      initialized by glBusInit().
		 * _HIF_SDIO: bus driver handle
		 */

		bRet = glBusInit(pvData);

		/* Cannot get IO address from interface */
		if (FALSE == bRet) {
			DBGLOG(INIT, ERROR, KERN_ALERT "wlanProbe: glBusInit() fail\n");
			i4Status = -EIO;
			break;
		}
		/* 4 <2> Create network device, Adapter, KalInfo, prDevHandler(netdev) */
		prWdev = wlanNetCreate(pvData);
		if (prWdev == NULL) {
			DBGLOG(INIT, ERROR, "wlanProbe: No memory for dev and its private\n");
			i4Status = -ENOMEM;
			break;
		}
		/* 4 <2.5> Set the ioaddr to HIF Info */
		prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
		gPrDev = prGlueInfo->prDevHandler;

		/* 4 <4> Setup IRQ */
		prWlandevInfo = &arWlanDevInfo[i4DevIdx];

		i4Status = glBusSetIrq(prWdev->netdev, NULL, *((P_GLUE_INFO_T *) netdev_priv(prWdev->netdev)));

		if (i4Status != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, "wlanProbe: Set IRQ error\n");
			break;
		}

		prGlueInfo->i4DevIdx = i4DevIdx;

		prAdapter = prGlueInfo->prAdapter;

		prGlueInfo->u4ReadyFlag = 0;
#if CFG_SUPPORT_IPI_HISTOGRAM
		prGlueInfo->ipi_thread = NULL;
#endif

		/* default set the FW roaming enable state to 'on' */
		prGlueInfo->u4FWRoamingEnable = 1;

#if CFG_TCP_IP_CHKSUM_OFFLOAD
		prAdapter->u4CSUMFlags = (CSUM_OFFLOAD_EN_TX_TCP | CSUM_OFFLOAD_EN_TX_UDP | CSUM_OFFLOAD_EN_TX_IP);
#endif
#if CFG_SUPPORT_CFG_FILE
		{
			PUINT_8 pucConfigBuf;
			UINT_32 u4ConfigReadLen;

			wlanCfgInit(prAdapter, NULL, 0, 0);
			pucConfigBuf = (PUINT_8) kalMemAlloc(WLAN_CFG_FILE_BUF_SIZE, VIR_MEM_TYPE);
			u4ConfigReadLen = 0;
			DBGLOG(INIT, LOUD, "CFG_FILE: Read File...\n");
			if (pucConfigBuf) {
				kalMemZero(pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE);
				if (kalReadToFile("/vendor/firmware/wifi.cfg",
						pucConfigBuf, WLAN_CFG_FILE_BUF_SIZE, &u4ConfigReadLen) == 0) {
					DBGLOG(INIT, LOUD, "CFG_FILE: Read /vendor/firmware/wifi.cfg\n");

				}
				if (pucConfigBuf[0] != '\0' && u4ConfigReadLen > 0)
					wlanCfgInit(prAdapter, pucConfigBuf, u4ConfigReadLen, 0);
				kalMemFree(pucConfigBuf, VIR_MEM_TYPE, WLAN_CFG_FILE_BUF_SIZE);
			}	/* pucConfigBuf */
		}
#endif
		/* 4 <5> Start Device */
		/*  */
#if CFG_ENABLE_FW_DOWNLOAD
		DBGLOG(INIT, TRACE, "start to download firmware...\n");

		/* before start adapter, we need to open and load firmware */
		{
			UINT_32 u4FwSize = 0;
			PVOID prFwBuffer = NULL;
			P_REG_INFO_T prRegInfo = &prGlueInfo->rRegInfo;

			/* P_REG_INFO_T prRegInfo = (P_REG_INFO_T) kmalloc(sizeof(REG_INFO_T), GFP_KERNEL); */
			kalMemSet(prRegInfo, 0, sizeof(REG_INFO_T));
			prRegInfo->u4StartAddress = CFG_FW_START_ADDRESS;
			prRegInfo->u4LoadAddress = CFG_FW_LOAD_ADDRESS;

#ifdef CONFIG_IDME
			idme_get_board_id(prRegInfo);
			if (idme_get_wifi_mfg(prRegInfo) == 0) { /* read idme OK */
				/* copy idme Wifi data to prRegInfo */
				wlanCopyIdmeWifiMfg(prRegInfo);
				prRegInfo->ManufactureSource = MANUFACTURE_IDME;
				prGlueInfo->fgNvramAvailable = TRUE;
			} else
#endif
			{
				/* Load NVRAM content to REG_INFO_T */
				glLoadNvram(prGlueInfo, prRegInfo);
				prRegInfo->ManufactureSource = MANUFACTURE_NVRAM;
				if (prGlueInfo->fgNvramAvailable == FALSE)
					wlanGetDefaultWifiMfg(prRegInfo);
			}

#ifdef CONFIG_IDME
			idme_get_mac_addr(prRegInfo);
#endif

#if CFG_SUPPORT_CFG_FILE
			wlanCfgApply(prAdapter);
#endif

			/* kalMemCopy(&prGlueInfo->rRegInfo, prRegInfo, sizeof(REG_INFO_T)); */

			prRegInfo->u4PowerMode = CFG_INIT_POWER_SAVE_PROF;
			prRegInfo->fgEnArpFilter = TRUE;

			if (kalFirmwareImageMapping(prGlueInfo, &prFwBuffer, &u4FwSize) == NULL) {
				i4Status = -EIO;
				DBGLOG(INIT, ERROR, "kalFirmwareImageMapping fail!\n");
				goto bailout;
			} else {

				if (wlanAdapterStart(prAdapter, prRegInfo, prFwBuffer,
					u4FwSize) != WLAN_STATUS_SUCCESS) {
					i4Status = -EIO;
				}
			}

			kalFirmwareImageUnmapping(prGlueInfo, NULL, prFwBuffer);

bailout:
			/* kfree(prRegInfo); */

			DBGLOG(INIT, TRACE, "download firmware status = %d\n", i4Status);

			if (i4Status < 0) {
				GL_HIF_INFO_T *HifInfo;
				UINT_32 u4FwCnt;

				DBGLOG(INIT, WARN, "CONNSYS FW CPUINFO:\n");
				HifInfo = &prAdapter->prGlueInfo->rHifInfo;
				for (u4FwCnt = 0; u4FwCnt < 16; u4FwCnt++)
					DBGLOG(INIT, WARN, "0x%08x ", MCU_REG_READL(HifInfo, CONN_MCU_CPUPCR));
				/* CONSYS_REG_READ(CONSYS_CPUPCR_REG) */

				/* dump HIF/DMA registers */
				HifRegDump(prGlueInfo->prAdapter);
/* if (prGlueInfo->rHifInfo.DmaOps->DmaRegDump != NULL) */
/* prGlueInfo->rHifInfo.DmaOps->DmaRegDump(&prGlueInfo->rHifInfo); */
				break;
			}
		}
#else
		/* P_REG_INFO_T prRegInfo = (P_REG_INFO_T) kmalloc(sizeof(REG_INFO_T), GFP_KERNEL); */
		kalMemSet(&prGlueInfo->rRegInfo, 0, sizeof(REG_INFO_T));
		P_REG_INFO_T prRegInfo = &prGlueInfo->rRegInfo;

		/* Load NVRAM content to REG_INFO_T */
		glLoadNvram(prGlueInfo, prRegInfo);

		prRegInfo->u4PowerMode = CFG_INIT_POWER_SAVE_PROF;

		if (wlanAdapterStart(prAdapter, prRegInfo, NULL, 0) != WLAN_STATUS_SUCCESS) {
			i4Status = -EIO;
			break;
		}
#endif
		if (FALSE == prAdapter->fgEnable5GBand)
			prWdev->wiphy->bands[NL80211_BAND_5GHZ] = NULL;

		prGlueInfo->main_thread = kthread_run(tx_thread, prGlueInfo->prDevHandler, "tx_thread");
		g_u4HaltFlag = 0;
#if CFG_SUPPORT_ROAMING_ENC
		/* adjust roaming threshold */
		{
			WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
			CMD_ROAMING_INFO_T rRoamingInfo;
			UINT_32 u4SetInfoLen = 0;

			prAdapter->fgIsRoamingEncEnabled = TRUE;

			/* suggestion from Tsaiyuan.Hsu */
			kalMemZero(&rRoamingInfo, sizeof(CMD_ROAMING_INFO_T));
			rRoamingInfo.fgIsFastRoamingApplied = TRUE;

			DBGLOG(INIT, TRACE, "Enable roaming enhance function\n");

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetRoamingInfo,
					   &rRoamingInfo, sizeof(rRoamingInfo), TRUE, TRUE, TRUE, FALSE, &u4SetInfoLen);

			if (rStatus != WLAN_STATUS_SUCCESS)
				DBGLOG(INIT, ERROR, "set roaming advance info fail 0x%x\n", rStatus);
		}
#endif /* CFG_SUPPORT_ROAMING_ENC */

#if (CFG_SUPPORT_TXR_ENC == 1)
		/* adjust tx rate switch threshold */
		rlmTxRateEnhanceConfig(prGlueInfo->prAdapter);
#endif /* CFG_SUPPORT_TXR_ENC */

		/* set MAC address */
		{
			WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
			struct sockaddr MacAddr;
			UINT_32 u4SetInfoLen = 0;

			kalMemZero(MacAddr.sa_data, sizeof(MacAddr.sa_data));
			rStatus = kalIoctl(prGlueInfo,
					   wlanoidQueryCurrentAddr,
					   &MacAddr.sa_data,
					   PARAM_MAC_ADDR_LEN, TRUE, TRUE, TRUE, FALSE, &u4SetInfoLen);

			if (rStatus != WLAN_STATUS_SUCCESS) {
				DBGLOG(INIT, WARN, "set MAC addr fail 0x%x\n", rStatus);
				prGlueInfo->u4ReadyFlag = 0;
			} else {
				ether_addr_copy(prGlueInfo->prDevHandler->dev_addr, (const u8 *)&(MacAddr.sa_data));
				ether_addr_copy(prGlueInfo->prDevHandler->perm_addr,
					prGlueInfo->prDevHandler->dev_addr);

				/* card is ready */
				prGlueInfo->u4ReadyFlag = 1;
#if CFG_SHOW_MACADDR_SOURCE
				DBGLOG(INIT, INFO, "MAC address: %pM ", (&MacAddr.sa_data));
#endif
			}
		}

#if CFG_TCP_IP_CHKSUM_OFFLOAD
		/* set HW checksum offload */
		{
			WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
			UINT_32 u4CSUMFlags = CSUM_OFFLOAD_EN_ALL;
			UINT_32 u4SetInfoLen = 0;

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetCSUMOffload,
					   (PVOID) &u4CSUMFlags,
					   sizeof(UINT_32), FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

			if (rStatus != WLAN_STATUS_SUCCESS)
				DBGLOG(INIT, WARN, "set HW checksum offload fail 0x%x\n", rStatus);
		}
#endif
#if CFG_SUPPORT_802_11K
		{
			WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
			UINT_32 u4SetInfoLen = 0;

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSync11kCapbilities,
					   NULL,
					   0, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

			if (rStatus != WLAN_STATUS_SUCCESS)
				DBGLOG(INIT, WARN, "set 11k Capabilities fail 0x%x\n", rStatus);
		}
#endif


		/* 4 <3> Register the card */
		DBGLOG(INIT, TRACE, "wlanNetRegister...\n");
		i4DevIdx = wlanNetRegister(prWdev);
		if (i4DevIdx < 0) {
			i4Status = -ENXIO;
			DBGLOG(INIT, ERROR, "wlanProbe: Cannot register the net_device context to the kernel\n");
			break;
		}

		wlanRegisterNotifier();
		/* 4 <6> Initialize /proc filesystem */
#ifdef WLAN_INCLUDE_PROC
		DBGLOG(INIT, TRACE, "init procfs...\n");
		i4Status = procCreateFsEntry(prGlueInfo);
		if (i4Status < 0) {
			DBGLOG(INIT, ERROR, "wlanProbe: init procfs failed\n");
			break;
		}
#endif /* WLAN_INCLUDE_PROC */

#if CFG_ENABLE_BT_OVER_WIFI
		prGlueInfo->rBowInfo.fgIsNetRegistered = FALSE;
		prGlueInfo->rBowInfo.fgIsRegistered = FALSE;
		glRegisterAmpc(prGlueInfo);
#endif

#if CFG_ENABLE_WIFI_DIRECT
		DBGLOG(INIT, TRACE, "wlanSubModInit...\n");

		/* wlan is launched */
		prGlueInfo->prAdapter->fgIsWlanLaunched = TRUE;
		/* if p2p module is inserted, notify tx_thread to init p2p network */
		if (rSubModHandler[P2P_MODULE].subModInit)
			wlanSubModInit(prGlueInfo);
		/* register set_p2p_mode handler to mtk_wmt_wifi */
		register_set_p2p_mode_handler(set_p2p_mode_handler);
#endif
#if CFG_SPM_WORKAROUND_FOR_HOTSPOT
		if (glIsChipNeedWakelock(prGlueInfo))
			KAL_WAKE_LOCK_INIT(prGlueInfo->prAdapter, &prGlueInfo->prAdapter->rApWakeLock, "WLAN AP");
#endif
#if CFG_SUPPORT_802_11R
		kalMemZero(&prGlueInfo->rFtIeForTx, sizeof(prGlueInfo->rFtIeForTx));
#endif

	} while (FALSE);

	if (i4Status != WLAN_STATUS_SUCCESS) {
		KAL_WAKE_LOCK_DESTROY(prGlueInfo->prAdapter, &prGlueInfo->rAhbIsrWakeLock);
		if (prWdev != NULL)
			glBusFreeIrq(prWdev->netdev, *((P_GLUE_INFO_T *) netdev_priv(prWdev->netdev)));
	}
#if CFG_ENABLE_WIFI_DIRECT
	{
		GLUE_SPIN_LOCK_DECLARATION();

		GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
		g_u4P2PEnding = 0;
		GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);
	}
#endif
#if CFG_SUPPORT_AGPS_ASSIST
	if (i4Status == WLAN_STATUS_SUCCESS)
		kalIndicateAgpsNotify(prAdapter, AGPS_EVENT_WLAN_ON, NULL, 0);
#endif
#if (CFG_SUPPORT_MET_PROFILING == 1)
	DBGLOG(INIT, TRACE, "init MET procfs...\n");
	i4Status = kalMetInitProcfs(prGlueInfo);
	if (i4Status < 0)
		DBGLOG(INIT, ERROR, "wlanProbe: init MET procfs failed\n");
#endif

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	kalTrafficStatInit(prGlueInfo);
	INIT_WORK(&prGlueInfo->rDrvWork.rWork, wlanDrvCommonWork);
	QUEUE_INITIALIZE(&prGlueInfo->rDrvWork.rWorkFuncQue);
	spin_lock_init(&prGlueInfo->rDrvWork.rWorkFuncQueLock);
	lockdep_set_class(&prGlueInfo->rDrvWork.rWorkFuncQueLock, &rLockKey);
#endif

	DBGLOG(INIT, TRACE, "wlanProbe ok\n");
	return i4Status;
}				/* end of wlanProbe() */

/*----------------------------------------------------------------------------*/
/*!
* \brief A method to stop driver operation and release all resources. Following
*        this call, no frame should go up or down through this interface.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
static VOID wlanRemove(VOID)
{
	struct net_device *prDev = NULL;
	P_WLANDEV_INFO_T prWlandevInfo = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_ADAPTER_T prAdapter = NULL;

	DBGLOG(INIT, LOUD, "Remove wlan!\n");

	/* 4 <0> Sanity check */
	ASSERT(u4WlanDevNum <= CFG_MAX_WLAN_DEVICES);
	if (0 == u4WlanDevNum) {
		DBGLOG(INIT, ERROR, "0 == u4WlanDevNum\n");
		return;
	}
	/* unregister set_p2p_mode handler to mtk_wmt_wifi */
	register_set_p2p_mode_handler(NULL);

	prDev = arWlanDevInfo[u4WlanDevNum - 1].prDev;
	prWlandevInfo = &arWlanDevInfo[u4WlanDevNum - 1];

	ASSERT(prDev);
	if (NULL == prDev) {
		DBGLOG(INIT, ERROR, "NULL == prDev\n");
		return;
	}

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);
	if (NULL == prGlueInfo) {
		DBGLOG(INIT, ERROR, "NULL == prGlueInfo\n");
		free_netdev(prDev);
		return;
	}
#if CFG_ENABLE_WIFI_DIRECT
	/* avoid remove & p2p off command simultaneously */
	{
		BOOLEAN fgIsP2POnOffing;

		GLUE_SPIN_LOCK_DECLARATION();

		GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
		g_u4P2PEnding = 1;
		fgIsP2POnOffing = g_u4P2POnOffing;
		GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);

		DBGLOG(INIT, TRACE, "waiting for fgIsP2POnOffing...\n");

		/* History: cannot use down() here, sometimes we cannot come back here */
		/* waiting for p2p off command finishes, we cannot skip the remove */
		while (1) {
			if (fgIsP2POnOffing == 0)
				break;
			GLUE_ACQUIRE_THE_SPIN_LOCK(&g_p2p_lock);
			fgIsP2POnOffing = g_u4P2POnOffing;
			GLUE_RELEASE_THE_SPIN_LOCK(&g_p2p_lock);
		}
	}
#endif

#if CFG_ENABLE_BT_OVER_WIFI
	if (prGlueInfo->rBowInfo.fgIsNetRegistered) {
		bowNotifyAllLinkDisconnected(prGlueInfo->prAdapter);
		/* wait 300ms for BoW module to send deauth */
		kalMsleep(300);
	}
#endif

	wlanReleasePendingOid(prGlueInfo->prAdapter, 0);

	/* 4 <1> Stopping handling interrupt and free IRQ */
	DBGLOG(INIT, TRACE, "free IRQ...\n");
	glBusFreeIrq(prDev, *((P_GLUE_INFO_T *) netdev_priv(prDev)));

	kalMemSet(&(prGlueInfo->prAdapter->rWlanInfo), 0, sizeof(WLAN_INFO_T));

	g_u4HaltFlag = 1;	/* before flush_delayed_work() */
	if (fgIsWorkMcStart == TRUE) {
		DBGLOG(INIT, TRACE, "flush_delayed_work...\n");
		flush_delayed_work(&workq);	/* flush_delayed_work_sync is deprecated */
	}

	flush_delayed_work(&sched_workq);

	DBGLOG(INIT, INFO, "down g_halt_sem...\n");
	down(&g_halt_sem);
#if CFG_SPM_WORKAROUND_FOR_HOTSPOT
	if (glIsChipNeedWakelock(prGlueInfo))
		KAL_WAKE_LOCK_DESTROY(prGlueInfo->prAdapter, &prGlueInfo->prAdapter->rApWakeLock);
#endif

/* flush_delayed_work_sync(&workq); */
/* flush_delayed_work(&workq); */ /* flush_delayed_work_sync is deprecated */

	/* 4 <2> Mark HALT, notify main thread to stop, and clean up queued requests */
/* prGlueInfo->u4Flag |= GLUE_FLAG_HALT; */
	set_bit(GLUE_FLAG_HALT_BIT, &prGlueInfo->ulFlag);
	DBGLOG(INIT, TRACE, "waiting for tx_thread stop...\n");

	/* wake up main thread */
	wake_up_interruptible(&prGlueInfo->waitq);

	DBGLOG(INIT, TRACE, "wait_for_completion\n");

	/* wait main thread stops */
	wait_for_completion(&prGlueInfo->rHaltComp);

	DBGLOG(INIT, TRACE, "mtk_sdiod stopped\n");

	KAL_WAKE_LOCK_DESTROY(prGlueInfo->prAdapter, &prGlueInfo->prAdapter->rTxThreadWakeLock);
	KAL_WAKE_LOCK_DESTROY(prGlueInfo->prAdapter, &prGlueInfo->rAhbIsrWakeLock);

	/* prGlueInfo->rHifInfo.main_thread = NULL; */
	prGlueInfo->main_thread = NULL;
#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	kalTrafficStatUnInit(prGlueInfo);
#endif

#if CFG_ENABLE_BT_OVER_WIFI
	if (prGlueInfo->rBowInfo.fgIsRegistered)
		glUnregisterAmpc(prGlueInfo);
#endif

	/* 4 <3> Remove /proc filesystem. */
#ifdef WLAN_INCLUDE_PROC
	procRemoveProcfs();
#endif /* WLAN_INCLUDE_PROC */

#if (CFG_SUPPORT_MET_PROFILING == 1)
	kalMetRemoveProcfs();
#endif

	/* Force to do DMA reset */
	DBGLOG(INIT, TRACE, "glResetHif\n");
	glResetHif(prGlueInfo);

	/* 4 <4> wlanAdapterStop */
	prAdapter = prGlueInfo->prAdapter;
#if CFG_SUPPORT_AGPS_ASSIST
	kalIndicateAgpsNotify(prAdapter, AGPS_EVENT_WLAN_OFF, NULL, 0);
#endif

	wlanAdapterStop(prAdapter);
	DBGLOG(INIT, TRACE, "Number of Stalled Packets = %d\n", prGlueInfo->i4TxPendingFrameNum);

#if CFG_ENABLE_WIFI_DIRECT
	prGlueInfo->prAdapter->fgIsWlanLaunched = FALSE;
	if (prGlueInfo->prAdapter->fgIsP2PRegistered) {
		DBGLOG(INIT, TRACE, "p2pNetUnregister...\n");
#if !CFG_SUPPORT_PERSIST_NETDEV
		p2pNetUnregister(prGlueInfo, FALSE);
#endif
		DBGLOG(INIT, INFO, "p2pRemove...\n");
		p2pRemove(prGlueInfo);
	}
#endif

	/* 4 <5> Release the Bus */
	glBusRelease(prDev);

	up(&g_halt_sem);

#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	flush_work(&prGlueInfo->rDrvWork.rWork);
#endif
	/* 4 <6> Unregister the card */
	wlanNetUnregister(prDev->ieee80211_ptr);

	/* 4 <7> Destroy the device */
	wlanNetDestroy(prDev->ieee80211_ptr);
	prDev = NULL;

	DBGLOG(INIT, LOUD, "wlanUnregisterNotifier...\n");
	wlanUnregisterNotifier();

	DBGLOG(INIT, INFO, "wlanRemove ok\n");
}				/* end of wlanRemove() */


#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
static VOID wlanPsyWorkFunc(PUINT_8 pucParams)
{
	UINT_32 u4InfoBufLen;
	P_GLUE_INFO_T prGlueInfo = *(P_GLUE_INFO_T *)pucParams;
	PUINT_8 pucCharingStatus = &pucParams[sizeof(prGlueInfo)];

	kalIoctl(prGlueInfo, wlanoidNotifyChargeStatus,
		 pucCharingStatus, kalStrLen(pucCharingStatus), FALSE, FALSE, FALSE, FALSE, &u4InfoBufLen);
}

static int wlan_psy_notification(struct notifier_block *nb, unsigned long event, void *data)
{
	struct power_supply *psy = (struct power_supply*)data;
	union power_supply_propval status;
	static int last_status = POWER_SUPPLY_STATUS_UNKNOWN;
	static struct power_supply *batt_psy;

	if (!batt_psy)
		batt_psy = power_supply_get_by_name("battery");

	if (event != PSY_EVENT_PROP_CHANGED || psy != batt_psy)
		return NOTIFY_OK;

	if (!power_supply_get_property(psy, POWER_SUPPLY_PROP_STATUS, &status) &&
		status.intval != last_status) {
		P_GLUE_INFO_T prGlueInfo = NULL;

		if (wlanExportGlueInfo(&prGlueInfo)  && (last_status == POWER_SUPPLY_STATUS_CHARGING ||
		    status.intval == POWER_SUPPLY_STATUS_CHARGING)) {
			static UINT_8 aucPsyWorkBuf[sizeof(struct DRV_COMMON_WORK_FUNC_T) + sizeof(prGlueInfo) + 18];
			struct DRV_COMMON_WORK_FUNC_T *prPsyWork = (struct DRV_COMMON_WORK_FUNC_T *)&aucPsyWorkBuf[0];
			P_GLUE_INFO_T *pprGlueInfo = (P_GLUE_INFO_T *)prPsyWork->params;

			*pprGlueInfo = prGlueInfo;
			if (last_status == POWER_SUPPLY_STATUS_CHARGING)
				kalStrCpy(&prPsyWork->params[sizeof(prGlueInfo)], "Charging finished");
			else
				kalStrCpy(&prPsyWork->params[sizeof(prGlueInfo)], "Charging started");

			prPsyWork->work_func = wlanPsyWorkFunc;
			kalScheduleCommonWork(&prGlueInfo->rDrvWork, prPsyWork);
		}
	}
	DBGLOG(INIT, INFO, "charge status, current %d, last %d\n", status.intval, last_status);
	last_status = status.intval;
	return NOTIFY_OK;
}
static struct notifier_block wlan_psy_nb = {
	.notifier_call = wlan_psy_notification,
};
#endif
/*----------------------------------------------------------------------------*/
/*!
* \brief Driver entry point when the driver is configured as a Linux Module, and
*        is called once at module load time, by the user-level modutils
*        application: insmod or modprobe.
*
* \retval 0     Success
*/
/*----------------------------------------------------------------------------*/
/* 1 Module Entry Point */
static int initWlan(void)
{
	int ret = 0, i;
#if DBG
	for (i = 0; i < DBG_MODULE_NUM; i++)
		aucDebugModule[i] = DBG_CLASS_MASK; /* enable all */
#else
	/* Initial debug level is D1 */
	for (i = 0; i < DBG_MODULE_NUM; i++)
		aucDebugModule[i] = DBG_CLASS_ERROR | DBG_CLASS_WARN | DBG_CLASS_INFO | DBG_CLASS_STATE;

	aucDebugModule[DBG_TX_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_TRACE | DBG_CLASS_INFO);
	aucDebugModule[DBG_RX_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_TRACE | DBG_CLASS_INFO);
	aucDebugModule[DBG_REQ_IDX] &= ~(DBG_CLASS_EVENT | DBG_CLASS_TRACE | DBG_CLASS_INFO);
	aucDebugModule[DBG_INIT_IDX] &= ~(DBG_CLASS_TRACE | DBG_CLASS_INFO);
	aucDebugModule[DBG_RLM_IDX] &= ~(DBG_CLASS_TRACE);
	aucDebugModule[DBG_BOW_IDX] &= ~(DBG_CLASS_TRACE | DBG_CLASS_INFO);
	aucDebugModule[DBG_QM_IDX] &= ~(DBG_CLASS_TRACE);
	/* Some Cisco APs set 100ms timeout in WPA 4-way handshake. Reduce some logging */
	aucDebugModule[DBG_SW4_IDX] &= ~(DBG_CLASS_INFO);
	aucDebugModule[DBG_INTR_IDX] = 0;
	aucDebugModule[DBG_MEM_IDX] = DBG_CLASS_ERROR | DBG_CLASS_WARN;
#endif /* DBG */
	DBGLOG(INIT, INFO, "initWlan\n");

	spin_lock_init(&g_p2p_lock);

	/* memory pre-allocation */
	kalInitIOBuffer();
	procInitFs();
	createWirelessDevice();
	g_u4HaltFlag = 1;
	if (gprWdev)
		glP2pCreateWirelessDevice((P_GLUE_INFO_T) wiphy_priv(gprWdev->wiphy));

	ret = ((glRegisterBus(wlanProbe, wlanRemove) == WLAN_STATUS_SUCCESS) ? 0 : -EIO);

	if (ret == -EIO) {
		kalUninitIOBuffer();
		return ret;
	}
#if (CFG_CHIP_RESET_SUPPORT)
	glResetInit();
#endif

	/* register set_dbg_level handler to mtk_wmt_wifi */
	/*register_set_dbg_level_handler(set_dbg_level_handler);*/

	glRegisterPlatformDev();
#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	power_supply_reg_notifier(&wlan_psy_nb);
#endif
	/* Set the initial DEBUG CLASS of each module */
	return ret;
}				/* end of initWlan() */

/*----------------------------------------------------------------------------*/
/*!
* \brief Driver exit point when the driver as a Linux Module is removed. Called
*        at module unload time, by the user level modutils application: rmmod.
*        This is our last chance to clean up after ourselves.
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
/* 1 Module Leave Point */
static VOID exitWlan(void)
{
	DBGLOG(INIT, INFO, "exitWlan\n");

	glUnregisterPlatformDev();

	/* unregister set_dbg_level handler to mtk_wmt_wifi */
	/*register_set_dbg_level_handler(NULL);*/

#if CFG_CHIP_RESET_SUPPORT
	glResetUninit();
#endif
	destroyWirelessDevice();
	glP2pDestroyWirelessDevice();

	glUnregisterBus(wlanRemove);

	/* free pre-allocated memory */
	kalUninitIOBuffer();

	DBGLOG(INIT, INFO, "exitWlan\n");
	procUninitProcFs();
#if CFG_SUPPORT_WAKEUP_REASON_DEBUG
	power_supply_unreg_notifier(&wlan_psy_nb);
#endif
}				/* end of exitWlan() */

#ifdef MTK_WCN_REMOVE_KERNEL_MODULE
int mtk_wcn_wlan_soc_init(void)
{
	return initWlan();
}
EXPORT_SYMBOL(mtk_wcn_wlan_soc_init);

void mtk_wcn_wlan_soc_exit(void)
{
	return exitWlan();
}
EXPORT_SYMBOL(mtk_wcn_wlan_soc_exit);
#else
module_init(initWlan);
module_exit(exitWlan);
#endif
