/*
 * OwlModemRN4.h
 * Twilio Breakout SDK
 *
 * Copyright (c) 2018 Twilio, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file OwlModemRN4.h - wrapper for U-blox SARA-R4/SARA-N4 modems
 */

#ifndef __OWL_MODEM_RN4_H__
#define __OWL_MODEM_RN4_H__

#include "enums.h"
#include "OwlModemAT.h"
#include "OwlModemInformation.h"
#include "OwlModemNetwork.h"
#include "OwlModemPDN.h"
#include "OwlModemSIM.h"
#include "OwlModemSocket.h"
#include "OwlModemGNSS.h"
#include "OwlModemSSLRN4.h"
#include "OwlModemMQTTRN4.h"

/*
 * Constants and Parameters
 */

#define MODEM_HOSTDEVICE_INFORMATION_SIZE 256

/**
 * Twilio wrapper for the AT serial interface to a modem
 */
class OwlModemRN4 {
 public:
  /**
   * Constructor for OwlModemRN4
   * @param modem_port - mandatory modem port
   * @param debug_port - optional debug port, to use in the bypass functions
   */
  OwlModemRN4(IOwlSerial *modem_port_in, IOwlSerial *debug_port_in = 0, IOwlSerial *gnss_port_in = 0);

  /**
   * Destructror of OwlModemRN4
   */
  ~OwlModemRN4();


  /**
   * Power on the module and wait until the AT interface is up. Might take up to 10 seconds.
   * @return 1 on success, 0 on failure
   */
  int powerOn();

  /**
   * Power off the module and wait until the AT interface is up. Might take up to 10 seconds.
   * @return 1 on success, 0 on failure
   */
  void powerOff();

  /**
   * Check if the modem is powered on
   * @return 1 if the modem is powered on, 0 if not
   */
  int isPoweredOn();


  /**
   * Set the default parameters of the modem, to ensure that we have a consistent experience, no matter how they could
   * be originally configured by the vendors.
   * @return - 1 on success, 0 on failure
   */
  int initModem(int testing_variant = 0, const char* apn = "", const char* cops = nullptr, at_cops_format_e cops_format = AT_COPS__Format__Numeric);

  /**
   * Wait for the modem to fully attach to the network. Usually, without this, there is little use for this class.
   * This blocks until correctly attached (unless configured for testing variants.
   * @param purpose - purpose string identifying the use-case for the integration.
   * @return 1 on success, 0 on failure
   */
  int waitForNetworkRegistration(char *purpose, int testing_variant = 0);


  /**
   * Bypass the modem serial to the debug serial, so that you can directly issue AT commands yourself.
   * This is for debug purposes. Drains current buffers, then returns, so call it again in a loop.
   */
  void bypass();

  /**
   * Bypass the GNSS serial to the debug serial, so that you can directly issue AT commands yourself.
   * This is for debug purposes. Drains current buffers, then returns, so call it again in a loop.
   */
  void bypassGNSS();

  /**
   * Similar to bypass(), but with a loop that watches for a magic word "exitbypass" to quit.
   */
  void bypassCLI();

  /**
   * Similar to bypassGNSS(), but with a loop that watches for a magic word "exitbypass" to quit.
   */
  void bypassGNSSCLI();

  /**
   * Retrieve the full HostDevice Information
   * @return the HostDevice Information string
   */
  str getHostDeviceInformation();

  /**
   * Retrieve the short HostDevice Information
   * @return the short HostDevice Information string
   */
  str getShortHostDeviceInformation();

  /**
   * Set the global debug level
   * @param level
   */
  void setDebugLevel(int level);

 private:
  IOwlSerial *modem_port;
  IOwlSerial *debug_port;
  IOwlSerial *gnss_port;

 public:
  /*
   * Main APIs
   */
  OwlModemAT AT;
  /** Methods to get information from the modem */
  OwlModemInformation information;

  /** Method to get information and interact with the SIM card */
  OwlModemSIM SIM;

  /** Network Registration and Management */
  OwlModemNetwork network;

  /** APN Management */
  OwlModemPDN pdn;
  
  /** TLS set up */
  OwlModemSSLRN4 ssl;

  /** MQTT set up */
  OwlModemMQTTRN4 mqtt;

  /** TCP/UDP communication over sockets */
  OwlModemSocket socket;

  /** GNSS to get position, date, time, etc */
  OwlModemGNSS gnss = OwlModemGNSS(this);

 private:
  /** The execution is now in the modem bypass mode */
  volatile uint8_t in_bypass = 0;  // volatile might not do much here, as we're not multi-threaded, but just marking it
  /** The execution is now in a timer - not used at the moment, will probably be removed in the near future */
  volatile uint8_t in_timer = 0;  // volatile might not do much here, as we're not multi-threaded, but just marking it

  bool has_modem_port{false};
  bool has_debug_port{false};
  bool has_gnss_port{false};

  char c_hostdevice_information[MODEM_HOSTDEVICE_INFORMATION_SIZE + 1];
  str hostdevice_information = {.s = c_hostdevice_information, .len = 0};
  char c_short_hostdevice_information[MODEM_HOSTDEVICE_INFORMATION_SIZE + 1];
  str short_hostdevice_information = {.s = c_short_hostdevice_information, .len = 0};

  /**
   * Perform Object-16 network registration. Initial attempts often are not successful, so may take a bit of time
   * to complete. The purpose defaults to 'Dev-Kit'
   * @param purpose - purpose string identifying the usecase for the integration.
   * @return 1 on success, 0 on failure
   */
  int setHostDeviceInformation(char *purpose = 0);

  /**
   * Perform Object-16 network registration. Initial attempts often are not successful, so may take a bit of time
   * to complete.
   * @param purpose - purpose string identifying the usecase for the integration.
   * @return 1 on success, 0 on failure
   */
  int setHostDeviceInformation(str purpose);

  /**
   * Compute the HostDevice Information string
   * @param purpose - input string naming the purpose
   */
  void computeHostDeviceInformation(str purpose);

 public:  // These things are not part of the API. TODO - make them private
  int drainGNSSRx(str *gnss_buffer, int gnss_buffer_len);
};

/**
 * Internal testing bitmask
 */
typedef enum {
  Testing__default                             = 0,     //!< Testing__default
  Testing__Set_MNO_Profile_to_Default          = 0x01,  //!< Testing__Set_MNO_Profile_to_Default
  Testing__Set_APN_Bands_to_Berlin             = 0x02,  //!< Testing__Set_APN_Bands_to_Berlin
  Testing__Timeout_Network_Registration_30_Sec = 0x04,  //!< Testing__Timeout_Network_Registration_60_Sec
  Testing__Skip_Set_Host_Device_Information    = 0x08,  //!< Testing__Skip_Set_Host_Device_Information
  Testing__Set_APN_Bands_to_US                 = 0x10,  //!< Testing__Set_APN_Bands_to_US
} testing_variant_e;
#endif
