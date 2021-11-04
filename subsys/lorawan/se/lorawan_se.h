/**
 * @file lorawan_se.h
 * @author Giuliano FRANCHETTO
 * @date 05 November 2021
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

#ifndef LORAWAN_SE_H
#define LORAWAN_SE_H

struct SecureElementNvmData_t;

int lorawan_se_update(SecureElementNvmData_t *data);
#endif /* LORAWAN_SE_H */