/*
 * OwlModemSIM.h
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
 * \file OwlModemSIM.h - API for retrieving various data from the SIM card
 */

#ifndef __OWL_MODEM_SIM_H__
#define __OWL_MODEM_SIM_H__

#include "enums.h"
#include "OwlModemAT.h"

/**
 * Handler function signature for SIM card ready/not ready - use this to check the PIN status.
 * @param message - the last message regarding PIN from the card
 * @param priv    - private pointer given at callback registration
 */
typedef void (*OwlModem_PINHandler_f)(str message, void *priv);



/**
 * Twilio wrapper for the AT serial interface to a modem - Methods to get information from the SIM card
 */
class OwlModemSIM {
 public:
  OwlModemSIM(OwlModemAT *atModem);

  /**
   * Handler for Unsolicited Response Codes from the modem - called from OwlModem on timer, when URC is received
   * @param urc - event id
   * @param data - data of the event
   * @return true if the line was handled, false if no match here
   */
  static bool processURC(str urc, str data, void *instance);



  /**
   * Retrieve ICCID (SIM serial number)
   * @param out_response - str object that will point to the response buffer
   * @return 1 on success, 0 on failure
   */
  int getICCID(str *out_response);

  /**
   * Retrieve IMSI (Mobile subscriber identifier)
   * @param out_response - str object that will point to the response buffer
   * @return 1 on success, 0 on failure
   */
  int getIMSI(str *out_response);

  /**
   * Trigger retrieval of the current PIN status - setting the callback will trigger verification.
   *
   * Note: You must do setHandlerPIN() before calling this. That's where you get the actual status, not here!
   *
   * @return 1 on success, 0 on failure
   */
  int getPINStatus();

  /**
   * Set the function to handle PIN requests from the SIM card
   * @param cb - callback function
   */
  void setHandlerPIN(OwlModem_PINHandler_f cb, void *priv = nullptr);



  /** Not private because the initialization might call this in a special way */
  OwlModem_PINHandler_f handler_cpin = 0;
  void *handler_cpin_priv            = nullptr;

 private:
  OwlModemAT *atModem_ = 0;

  int handleCPIN(str urc, str data);
};

#endif
