/**
 * @file main..c
 * @author Giuliano FRANCHETTO
 * @date 15 November 2021
 *
 * @copyright @verbatim
 * Intellinium SAS, 2014-present
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Intellinium SAS and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Intellinium SAS
 * and its suppliers and may be covered by French and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Intellinium SAS.
 * @endverbatim
 *
 * @brief
 **/

#ifndef COMMON_IDS_H_
#define COMMON_IDS_H_

#ifdef __cplusplus
extern "C" {
#endif

enum rpc_command {
	RPC_COMMAND_TX_POWER = 0x01,
	RPC_COMMAND_TRANSMIT_PATTERN = 0x02,
	RPC_COMMAND_START_CHANNEL = 0x03,
	RPC_COMMAND_END_CHANNEL = 0x04,
	RPC_COMMAND_TIME_ON_CHANNEL = 0x05,
	RPC_COMMAND_CANCEL = 0x06,
	RPC_COMMAND_DATA_RATE = 0x07,
	RPC_COMMAND_START_TX_CARRIER = 0x08,
	RPC_COMMAND_START_TX_MOD_CARRIER = 0x09,
	RPC_COMMAND_START_TX_DC_MOD_CARRIER = 0x0A,
	RPC_COMMAND_START_RX_SWEEP = 0x0B,
	RPC_COMMAND_START_TX_SWEEP = 0x0C,
	RPC_COMMAND_START_RX = 0x0D,
};

#ifdef __cplusplus
}
#endif

#endif /* COMMON_IDS_H_ */
