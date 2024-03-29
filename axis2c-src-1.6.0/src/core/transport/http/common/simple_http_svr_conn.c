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

#include <axis2_simple_http_svr_conn.h>
#include <axis2_http_transport.h>
#include <axutil_string.h>
#include <axutil_network_handler.h>
#include <axutil_http_chunked_stream.h>
#include <platforms/axutil_platform_auto_sense.h>
#include <string.h>
#include <axis2_http_simple_response.h>
#include <axis2_http_transport_utils.h>

struct axis2_simple_http_svr_conn
{
    int socket;
    axutil_stream_t *stream;
    axis2_bool_t keep_alive;
};

AXIS2_EXTERN axis2_simple_http_svr_conn_t *AXIS2_CALL
axis2_simple_http_svr_conn_create(
    const axutil_env_t * env,
    int sockfd)
{
    axis2_simple_http_svr_conn_t *svr_conn = NULL;

    svr_conn = (axis2_simple_http_svr_conn_t *)
        AXIS2_MALLOC(env->allocator, sizeof(axis2_simple_http_svr_conn_t));

    if (!svr_conn)
    {
        AXIS2_HANDLE_ERROR(env, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }
    memset ((void *)svr_conn, 0, sizeof (axis2_simple_http_svr_conn_t));
    svr_conn->socket = sockfd;
    svr_conn->stream = NULL;
    svr_conn->keep_alive = AXIS2_FALSE;

    if (-1 != svr_conn->socket)
    {
        svr_conn->stream = axutil_stream_create_socket(env, svr_conn->socket);
        if (!svr_conn->stream)
        {
            AXIS2_LOG_ERROR (env->log, AXIS2_LOG_SI, 
                             "socket creation failed, socket %d", (int)sockfd);
            axis2_simple_http_svr_conn_free((axis2_simple_http_svr_conn_t *)
                                            svr_conn, env);
            return NULL;
        }
    }
    return svr_conn;
}

AXIS2_EXTERN void AXIS2_CALL
axis2_simple_http_svr_conn_free(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    if (!svr_conn)
    {
        return;
    }

    axis2_simple_http_svr_conn_close(svr_conn, env);
    AXIS2_FREE(env->allocator, svr_conn);

    return;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
axis2_simple_http_svr_conn_close(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    axutil_stream_free(svr_conn->stream, env);
    if (-1 != svr_conn->socket)
    {
        axutil_network_handler_close_socket(env, svr_conn->socket);
        svr_conn->socket = -1;
    }
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
axis2_simple_http_svr_conn_is_open(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    if (-1 != svr_conn->socket)
    {
        return AXIS2_TRUE;
    }
    return AXIS2_FALSE;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
axis2_simple_http_svr_conn_set_keep_alive(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env,
    axis2_bool_t keep_alive)
{
    svr_conn->keep_alive = keep_alive;
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN axis2_bool_t AXIS2_CALL
axis2_simple_http_svr_conn_is_keep_alive(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    return svr_conn->keep_alive;
}


AXIS2_EXTERN axutil_stream_t *AXIS2_CALL
axis2_simple_http_svr_conn_get_stream(
    const axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    return svr_conn->stream;
}


AXIS2_EXTERN axis2_http_response_writer_t *AXIS2_CALL
axis2_simple_http_svr_conn_get_writer(
    const axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    return axis2_http_response_writer_create(env, svr_conn->stream);
}


AXIS2_EXTERN axis2_http_simple_request_t *AXIS2_CALL
axis2_simple_http_svr_conn_read_request(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    axis2_char_t* str_line = NULL;
    axis2_char_t tmp_buf[2048];
    int read = -1;
    axis2_bool_t end_of_line = AXIS2_FALSE;
    axis2_bool_t end_of_headers = AXIS2_FALSE;
    axis2_http_request_line_t *request_line = NULL;
    axis2_http_simple_request_t *request = NULL;

    while ((read =
            axutil_stream_peek_socket(svr_conn->stream, env, tmp_buf,
                                      2048 - 1)) > 0)
    {
        axis2_char_t *start = tmp_buf;
        axis2_char_t *end = NULL;
        tmp_buf[read] = AXIS2_ESC_NULL;
        end = strstr(tmp_buf, AXIS2_HTTP_CRLF);
        if (end)
        {
            read =
                axutil_stream_read(svr_conn->stream, env, tmp_buf,
                                   end - start + 2);
            if (read > 0)
            {
                axis2_char_t* tmp_str_line = NULL;
                tmp_buf[read] = AXIS2_ESC_NULL;
                tmp_str_line = axutil_stracat(env, str_line, tmp_buf);
                if(tmp_str_line)
                {
                    AXIS2_FREE(env->allocator, str_line);
                    str_line = tmp_str_line;
                }
                break;
            }
            else
            {
                /* read returns 0 or negative value, this could be an error */
                break;
            }
        }
        else
        {
            /* not reached end yet */
            read = axutil_stream_read(svr_conn->stream, env, tmp_buf, 2048 - 1);
            if (read > 0)
            {
                axis2_char_t* tmp_str_line = NULL;
                tmp_buf[read] = AXIS2_ESC_NULL;
                tmp_str_line = axutil_stracat(env, str_line, tmp_buf);
                if(tmp_str_line)
                {
                    AXIS2_FREE(env->allocator, str_line);
                    str_line = tmp_str_line;
                }
            }
        }
    }

    request_line = axis2_http_request_line_parse_line(env, str_line);
    AXIS2_FREE(env->allocator, str_line);
    str_line = NULL;

    if (!request_line)
    {
        AXIS2_HANDLE_ERROR(env,
                        AXIS2_ERROR_INVALID_HTTP_HEADER_START_LINE,
                        AXIS2_FAILURE);
        return NULL;
    }
    request = axis2_http_simple_request_create(env, request_line, NULL, 0,
                                               svr_conn->stream);
    /* now read the headers */
    end_of_line = AXIS2_FALSE;
    while (AXIS2_FALSE == end_of_headers)
    {
        axis2_bool_t is_read = AXIS2_FALSE;
        while ((read =
                axutil_stream_peek_socket(svr_conn->stream, env, tmp_buf,
                                          2048 - 1)) > 0)
        {
            axis2_char_t *start = tmp_buf;
            axis2_char_t *end = NULL;
            is_read = AXIS2_TRUE;
            tmp_buf[read] = AXIS2_ESC_NULL;
            end = strstr(tmp_buf, AXIS2_HTTP_CRLF);
            if (end)
            {
                read =
                    axutil_stream_read(svr_conn->stream, env, tmp_buf,
                                       end - start + 2);
                if (read > 0)
                {
		    axis2_char_t* tmp_str_line = NULL;
                    tmp_buf[read] = AXIS2_ESC_NULL;
                    tmp_str_line = axutil_stracat(env, str_line, tmp_buf);
                    if(tmp_str_line)
                    {
                        AXIS2_FREE(env->allocator, str_line);
                        str_line = tmp_str_line;
                    }
                    end_of_line = AXIS2_TRUE;
                    break;
                }
                else
                {
                    break;
                }
            }
            else
            {
                read =
                    axutil_stream_read(svr_conn->stream, env, tmp_buf,
                                       2048 - 1);
                if (read > 0)
                {
		    axis2_char_t* tmp_str_line = NULL;
                    tmp_buf[read] = AXIS2_ESC_NULL;
                    tmp_str_line = axutil_stracat(env, str_line, tmp_buf);
                    if(tmp_str_line)
                    {
                        AXIS2_FREE(env->allocator, str_line);
                        str_line = tmp_str_line;
                    }
                }
            }

        }

        if (AXIS2_TRUE == end_of_line)
        {
            if (0 == axutil_strcmp(str_line, AXIS2_HTTP_CRLF))
            {
                end_of_headers = AXIS2_TRUE;
            }
            else
            {
                axis2_http_header_t *tmp_header =
                    axis2_http_header_create_by_str(env, str_line);
                AXIS2_FREE(env->allocator, str_line);
                str_line = NULL;
                if (tmp_header)
                {
                    axis2_http_simple_request_add_header(request, env,
                                                         tmp_header);
                }
            }
        }
        end_of_line = AXIS2_FALSE;
        if(!is_read)
        {
            /*if nothing is read, this loop should be broken. Otherwise, going to be endless loop */
            break; 
        }
    }
    return request;
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL
axis2_simple_http_svr_conn_write_response(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env,
    axis2_http_simple_response_t * response)
{
    axis2_http_response_writer_t *response_writer = NULL;
    axutil_array_list_t *headers = NULL;
    axutil_stream_t *response_stream = NULL;
    axis2_char_t *response_body = NULL;
    int body_size = 0;
    int i = 0;
    axis2_http_header_t *enc_header = NULL;
    axis2_bool_t chuked_encoding = AXIS2_FALSE;
    axis2_char_t *status_line = NULL;
    axis2_bool_t binary_content = AXIS2_FALSE;
    axis2_char_t *content_type = NULL;

    AXIS2_PARAM_CHECK(env->error, response, AXIS2_FAILURE);

    response_writer = axis2_http_response_writer_create(env, svr_conn->stream);
    content_type =
        (axis2_char_t *) axis2_http_simple_response_get_content_type(response,
                                                                     env);
    if (content_type)
    {
        if (strstr(content_type, AXIS2_HTTP_HEADER_ACCEPT_MULTIPART_RELATED)
            && strstr(content_type, AXIS2_HTTP_HEADER_ACCEPT_XOP_XML))
            binary_content = AXIS2_TRUE;
    }
    if (!response_writer)
    {
        return AXIS2_FAILURE;
    }
    enc_header = axis2_http_simple_response_get_first_header(response, env,
                                                             AXIS2_HTTP_HEADER_TRANSFER_ENCODING);
    if (enc_header)
    {
        axis2_char_t *enc_value = axis2_http_header_get_value(enc_header, env);
        if (enc_value)
        {
            if (0 == axutil_strcmp(enc_value,
                                   AXIS2_HTTP_HEADER_TRANSFER_ENCODING_CHUNKED))
            {
                chuked_encoding = AXIS2_TRUE;
                /* remove the content length header */
                if (AXIS2_TRUE ==
                    axis2_http_simple_response_contains_header(response, env,
                                                               AXIS2_HTTP_HEADER_CONTENT_LENGTH))
                {
                    axis2_http_simple_response_remove_headers(response, env,
                                                              AXIS2_HTTP_HEADER_CONTENT_LENGTH);
                }
            }
        }
    }
    status_line = axis2_http_simple_response_get_status_line(response, env);
    if (!status_line)
    {
        AXIS2_HANDLE_ERROR(env,
                           AXIS2_ERROR_INVALID_HTTP_HEADER_START_LINE,
                           AXIS2_FAILURE);
        axis2_http_response_writer_free(response_writer, env);
        response_writer = NULL;
        return AXIS2_FAILURE;
    }
    axis2_http_response_writer_print_str(response_writer, env, status_line);
    headers = axis2_http_simple_response_get_headers(response, env);

    if (headers)
    {
        for (i = 0; i < axutil_array_list_size(headers, env); i++)
        {
            axis2_http_header_t *header = NULL;
            header = (axis2_http_header_t *) axutil_array_list_get(headers, env,
                                                                   i);
            if (header)
            {
                axis2_char_t *header_ext_form =
                    axis2_http_header_to_external_form((axis2_http_header_t *)
                                                       header, env);
                axis2_http_response_writer_print_str(response_writer, env,
                                                     header_ext_form);
                AXIS2_FREE(env->allocator, header_ext_form);
            }
        }
    }
    axis2_http_response_writer_println(response_writer, env);

    response_stream = axis2_http_simple_response_get_body(response, env);
    if (response_stream)
    {
        body_size = axutil_stream_get_len(response_stream, env);
        response_body = axutil_stream_get_buffer(response_stream, env);
        axutil_stream_flush_buffer(response_stream, env);
        response_body[body_size] = AXIS2_ESC_NULL;
    }

    if (body_size <= 0 && !binary_content)
    {
        axis2_http_response_writer_free(response_writer, env);
        return AXIS2_SUCCESS;
    }

    /* This sending a normal SOAP response without chunk transfer encoding */
    if (AXIS2_FALSE == chuked_encoding && !binary_content)
    {
        axis2_status_t write_stat = AXIS2_FAILURE;
        if (AXIS2_FALSE == binary_content)
        {
            write_stat = axis2_http_response_writer_println_str(response_writer,
                                                                env,
                                                                response_body);
        }
        else
        {
            write_stat = axis2_http_response_writer_write_buf(response_writer,
                                                              env,
                                                              response_body, 0,
                                                              body_size);
        }

        if (AXIS2_SUCCESS != write_stat)
        {
            AXIS2_HANDLE_ERROR(env, AXIS2_ERROR_WRITING_RESPONSE,
                               AXIS2_FAILURE);
            axis2_http_response_writer_free(response_writer, env);
            return AXIS2_FAILURE;
        }
    }
    
    /* In the MTOM case we enable chunking inorder to send the attachment */
    
    else if(binary_content)
    {
        axutil_http_chunked_stream_t *chunked_stream = NULL;
        axis2_status_t write_stat = AXIS2_FAILURE;
        axutil_array_list_t *mime_parts = NULL;
        axis2_char_t *mtom_sending_callback_name = NULL;
        
        mime_parts = axis2_http_simple_response_get_mime_parts(response, env);

        mtom_sending_callback_name = axis2_http_simple_response_get_mtom_sending_callback_name(
            response, env);

        /* If the callback name is not there, then we will check whether there 
         * is any mime_parts which has type callback. If we found then no point 
         * of continuing we should return a failure */

        if(!mtom_sending_callback_name)
        {
            if(axis2_http_transport_utils_is_callback_required(
                env, mime_parts))
            {
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Sender callback not specified");
                return AXIS2_FAILURE;
            }
        }

        chunked_stream = axutil_http_chunked_stream_create(env,
                                                          svr_conn->stream);

        if(mime_parts)
        {            
            write_stat = axis2_http_transport_utils_send_mtom_message(
                    chunked_stream, env, mime_parts, mtom_sending_callback_name);
            axutil_http_chunked_stream_free(chunked_stream, env);
            chunked_stream = NULL;
                    
            if(write_stat == AXIS2_FAILURE)
            {
                return write_stat;
            }
        }    
        else
        {
            return AXIS2_FAILURE;
        }
    } 
    
    /* Sending a normal SOAP response enabling htpp chunking */
    else 
    {    
        axutil_http_chunked_stream_t *chunked_stream = NULL;
        int left = body_size;
        chunked_stream = axutil_http_chunked_stream_create(env, svr_conn->stream);
        while (left > 0)
        {
            left -= axutil_http_chunked_stream_write(chunked_stream, env,
                response_body, body_size);
        }
        axutil_http_chunked_stream_write_last_chunk(chunked_stream, env);
        axutil_http_chunked_stream_free(chunked_stream, env);
    }

    axis2_http_response_writer_free(response_writer, env);
    return AXIS2_SUCCESS;
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
axis2_simple_http_svr_conn_set_rcv_timeout(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env,
    int timeout)
{
    return axutil_network_handler_set_sock_option(env,
                                                  svr_conn->socket, SO_RCVTIMEO,
                                                  timeout);
}


AXIS2_EXTERN axis2_status_t AXIS2_CALL
axis2_simple_http_svr_conn_set_snd_timeout(
    axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env,
    int timeout)
{
    return axutil_network_handler_set_sock_option(env,
                                                  svr_conn->socket, SO_SNDTIMEO,
                                                  timeout);
}


AXIS2_EXTERN axis2_char_t *AXIS2_CALL
axis2_simple_http_svr_conn_get_svr_ip(
    const axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    return axutil_network_handler_get_svr_ip(env, svr_conn->socket);
}


AXIS2_EXTERN axis2_char_t *AXIS2_CALL
axis2_simple_http_svr_conn_get_peer_ip(
    const axis2_simple_http_svr_conn_t * svr_conn,
    const axutil_env_t * env)
{
    return axutil_network_handler_get_peer_ip(env, svr_conn->socket);
}


