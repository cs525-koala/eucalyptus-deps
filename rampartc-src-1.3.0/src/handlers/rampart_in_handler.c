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

#include <axis2_handler_desc.h>
#include <axutil_array_list.h>
#include <axutil_hash.h>
#include <axiom_soap_const.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_header.h>
#include <axiom_soap_header_block.h>
#include <axis2_op.h>
#include <axis2_msg_ctx.h>
#include <axis2_msg_info_headers.h>
#include <axutil_property.h>
#include <rampart_constants.h>
#include <rampart_username_token.h>
#include <rampart_handler_util.h>
#include <rampart_timestamp_token.h>
#include <rampart_util.h>
#include <rampart_sec_header_processor.h>
#include <rampart_sec_processed_result.h>
#include <rampart_context.h>
#include <rampart_engine.h>

axis2_status_t AXIS2_CALL
rampart_in_handler_invoke(
    struct axis2_handler *handler,
    const axutil_env_t *env,
    struct axis2_msg_ctx *msg_ctx);

AXIS2_EXTERN axis2_handler_t *AXIS2_CALL
rampart_in_handler_create(
    const axutil_env_t *env,
    axutil_string_t *name)
{
    axis2_handler_t *handler = NULL;
    AXIS2_ENV_CHECK(env, NULL);

    handler = axis2_handler_create(env);
    if (!handler)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[rampart][rampart_in_handler] Cannot create in-handler.");
        return NULL;
    }

    /*Set the function to invoke*/
    axis2_handler_set_invoke(handler, env, rampart_in_handler_invoke);
    
    return handler;
}


axis2_status_t AXIS2_CALL
rampart_in_handler_invoke(
    struct axis2_handler *handler,
    const axutil_env_t *env,
    struct axis2_msg_ctx *msg_ctx)
{
    axiom_soap_envelope_t *soap_envelope = NULL;
    axiom_soap_header_t *soap_header = NULL;
    axis2_status_t status = AXIS2_FAILURE;
    axiom_node_t *sec_node = NULL;
    rampart_context_t *rampart_context = NULL;

    AXIS2_PARAM_CHECK(env->error, msg_ctx, AXIS2_FAILURE);

    /*
    * since rampart in_handler is a global handler we should
    * first check whether the rampart module is engaged.If not engaged we 
    * should not process the message but return success.
    */
    if(!rampart_is_rampart_engaged(env, msg_ctx))
    {
        AXIS2_LOG_INFO(env->log, 
            "[rampart][rampart_in_handler] Rampart is not engaged. No security checks needed. ");
        return AXIS2_SUCCESS;
    }

    soap_envelope =  axis2_msg_ctx_get_soap_envelope(msg_ctx, env);
    if(!soap_envelope)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[rampart][rampart_in_handler] SOAP envelope cannot be found.");
        return AXIS2_FAILURE;
    }

    soap_header = axiom_soap_envelope_get_header(soap_envelope, env);
    if (!soap_header)
    {
        /*No SOAP header, so no point of proceeding. FAIL*/
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[rampart][rampart_in_handler] SOAP header cannot be found.");
        return AXIS2_FAILURE;
    }

    rampart_context = rampart_engine_build_configuration(env, msg_ctx, AXIS2_TRUE);
    if(!rampart_context)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
            "[rampart][rampart_in_handler] rampart_context creation failed.");
        return AXIS2_FAILURE;
    }

    sec_node = rampart_get_security_header(env, msg_ctx, soap_header);

    /*We do not check for the security header in Transport binding */
    if(rampart_context_get_binding_type(rampart_context, env) != RP_PROPERTY_TRANSPORT_BINDING)
    {
        if(!sec_node)
        {
            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                "[rampart][rampart_in_handler] Security header cannot be found.");
            return AXIS2_FAILURE;
        }
    }

    status = rampart_set_security_processed_results_property(env, msg_ctx);
    if(status != AXIS2_SUCCESS)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
            "[rampart][rampart_in_handler] Unable to set the security processed results.");
        return status;
    }

    status = rampart_shp_process_sec_header(env, msg_ctx, rampart_context, soap_envelope, sec_node);                                         
    if(status != AXIS2_SUCCESS)
    {
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
            "[rampart][rampart_in_handler] Security Header processing failed.");
        return status;
    }
    
    rampart_context_free(rampart_context, env);

    return AXIS2_SUCCESS;
}
