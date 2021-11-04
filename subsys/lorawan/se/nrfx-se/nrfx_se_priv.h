/**
 * @file nrfx_se_priv.h
 * @author Giuliano FRANCHETTO
 * @date 08 November 2021
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

#ifndef NRFX_SE_PRIV_H
#define NRFX_SE_PRIV_H

#include <zephyr/types.h>
#include <LoRaMacTypes.h>

typedef struct nrfx_se_key {
	/*!
	 * Key value
	 */
	uint8_t KeyValue[16];

	/*!
	 * Random used for derivation
	 */
	uint8_t random[32];
} nrfx_se_key_t;

int nrfx_se_keys_load(KeyIdentifier_t id, nrfx_se_key_t *key);

int nrfx_se_keys_save(KeyIdentifier_t id, nrfx_se_key_t *key);

#endif /* NRFX_SE_PRIV_H */