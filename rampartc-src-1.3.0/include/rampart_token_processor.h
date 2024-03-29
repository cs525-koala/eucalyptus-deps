/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <axis2_util.h>
#include <axis2_defines.h>
#include <axutil_utils_defines.h>
#include <axutil_env.h>
#include <axiom_node.h>
#include <oxs_x509_cert.h>

/**
  * @file rampart_token_processor.h
  * @brief Token processing of rampart
  */

/**
* @defgroup Token Processor
* @ingroup rampart_utils
* @{
*/

#ifndef RAMPART_TOKEN_PROCESSOR_H
#define RAMPART_TOKEN_PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /**
     * extract certificate related information using given token_reference node and scope node
     * @param env Environment structure
     * @param st_ref_node security token reference node. 
     * @param scope_node node where additional details should be found. Can be NULL for all other 
     *  scenarios but the Direct Reference
     * @param cert certificate where values extracted shuold be populated
     * @return status of the operation
     */
    AXIS2_EXTERN axis2_status_t AXIS2_CALL
    rampart_token_process_security_token_reference(
        const axutil_env_t *env,
        axiom_node_t *st_ref_node,
        axiom_node_t *scope_node,
        oxs_x509_cert_t *cert);

    /**
     * extract certificate using reference id given in reference node
     * @param env Environment structure
     * @param ref_node security token reference node. 
     * @param scope_node node where certificate details should be found using reference id
     * @param cert certificate where values extracted shuold be populated
     * @return status of the operation
     */
    AXIS2_EXTERN axis2_status_t AXIS2_CALL
    rampart_token_process_direct_ref(
        const axutil_env_t *env,
        axiom_node_t *ref_node,
        axiom_node_t *scope_node,
        oxs_x509_cert_t *cert);

    /**
     * extract embedded certificate from given embed_node
     * @param env Environment structure
     * @param embed_node node where certificate is embedded. 
     * @param cert certificate where values extracted shuold be populated
     * @return status of the operation
     */
    AXIS2_EXTERN axis2_status_t AXIS2_CALL
    rampart_token_process_embedded(
        const axutil_env_t *env,
        axiom_node_t *embed_node,
        oxs_x509_cert_t *cert);

    /**
     * extract key identifier and populate the certificate
     * @param env Environment structure
     * @param ki_node node where key identifier is available. 
     * @param cert certificate where values extracted shuold be populated
     * @return status of the operation
     */
    AXIS2_EXTERN axis2_status_t AXIS2_CALL
    rampart_token_process_key_identifier(
        const axutil_env_t *env,
        axiom_node_t *ki_node,
        oxs_x509_cert_t *cert);

    /**
     * extract key details from x509data node
     * @param env Environment structure
     * @param x509_data_node x509data node. 
     * @param cert certificate where values extracted shuold be populated
     * @return status of the operation
     */
    AXIS2_EXTERN axis2_status_t AXIS2_CALL
    rampart_token_process_x509_data(
        const axutil_env_t *env,
        axiom_node_t *x509_data_node,
        oxs_x509_cert_t *cert);

    /* @} */
#ifdef __cplusplus
}
#endif

#endif    /* !RAMPART_TOKEN_PROCESSOR_H */


