/*
 * OwlModemSIM.cpp
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
 * \file OwlModemSIM.cpp - API for retrieving various data from the SIM card
 */

#include "OwlModemSIM.h"

#include <stdio.h>

static char URC_ID[] = "SIM";
OwlModemSIM::OwlModemSIM(OwlModemAT *atModem) : atModem_(atModem) {
  if (atModem_ != nullptr) {
    atModem_->registerUrcHandler(URC_ID, OwlModemSIM::processURC, this);
  }
}

static str s_cpin = {.s = "+CPIN", .len = 5};

int OwlModemSIM::handleCPIN(str urc, str data) {
  if (!str_equal(urc, s_cpin)) return 0;
  if (!this->handler_cpin) {
    LOG(L_NOTICE,
        "Received URC for PIN [%.*s]. Set a handler with setHandlerPIN() if you wish to receive this event "
        "in your application\r\n",
        data.len, data.s);
  } else {
    (this->handler_cpin)(data, this->handler_cpin_priv);
  }
  return 1;
}

bool OwlModemSIM::processURC(str urc, str data, void *instance) {
  OwlModemSIM *inst = reinterpret_cast<OwlModemSIM *>(instance);
  if (inst->handleCPIN(urc, data)) return true;
  return false;
}


static str s_ccid = STRDECL("+CCID: ");

int OwlModemSIM::getICCID(str *out_response) {
  str command_response;

  int result = atModem_->doCommandBlocking("AT+CCID", 1000, &command_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_ccid, command_response, out_response);
  return 1;
}

int OwlModemSIM::getIMSI(str *out_response) {
  auto result = atModem_->doCommandBlocking("AT+CIMI", 1000, out_response);
  str_strip(out_response);
  return (result == at_result_code::OK);
}

int OwlModemSIM::getPINStatus() {
  auto result = atModem_->doCommandBlocking("AT+CPIN?", 10 * 1000, nullptr) == at_result_code::OK;
  return result;
}

void OwlModemSIM::setHandlerPIN(OwlModem_PINHandler_f cb, void* priv) {
  this->handler_cpin = cb;
  this->handler_cpin_priv = priv;
}
