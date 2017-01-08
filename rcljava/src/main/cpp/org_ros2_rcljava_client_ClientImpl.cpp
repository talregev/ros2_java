// Copyright 2016 Esteve Fernandez <esteve@apache.org>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <jni.h>

#include <cassert>
#include <cstdlib>
#include <string>

#include "rmw/rmw.h"
#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcl/node.h"
#include "rosidl_generator_c/message_type_support_struct.h"

#include "rcljava_common/exceptions.h"
#include "rcljava_common/signatures.h"

#include "org_ros2_rcljava_client_ClientImpl.h"

JNIEXPORT void JNICALL Java_org_ros2_rcljava_client_ClientImpl_nativeSendClientRequest(JNIEnv * env,
  jclass, jlong client_handle, jlong sequence_number, jlong jrequest_from_java_converter_handle,
  jlong jrequest_to_java_converter_handle, jobject jrequest_msg)
{
  assert(client_handle != 0);
  assert(jrequest_from_java_converter_handle != 0);
  assert(jrequest_to_java_converter_handle != 0);
  assert(jrequest_msg != nullptr);

  rcl_client_t * client = reinterpret_cast<rcl_client_t *>(client_handle);

  convert_from_java_signature convert_from_java =
    reinterpret_cast<convert_from_java_signature>(jrequest_from_java_converter_handle);

  void * request_msg = convert_from_java(jrequest_msg, nullptr);

  rcl_ret_t ret = rcl_send_request(client, request_msg, &sequence_number);

  if (ret != RCL_RET_OK) {
    rcljava_throw_exception(
      env, "java/lang/IllegalStateException",
      "Failed to send request from a client: " + std::string(rcl_get_error_string_safe()));
  }
}

JNIEXPORT void JNICALL Java_org_ros2_rcljava_client_ClientImpl_nativeDispose(JNIEnv * env, jclass,
  jlong node_handle, jlong client_handle)
{
  if (client_handle == 0) {
    // everything is ok, already destroyed
    return;
  }

  if (node_handle == 0) {
    // TODO(esteve): handle this, node is null, but client isn't
    return;
  }

  rcl_node_t * node = reinterpret_cast<rcl_node_t *>(node_handle);

  assert(node != NULL);

  rcl_client_t * client = reinterpret_cast<rcl_client_t *>(client_handle);

  assert(client != NULL);

  rcl_ret_t ret = rcl_client_fini(client, node);

  if (ret != RCL_RET_OK) {
    rcljava_throw_exception(
      env, "java/lang/IllegalStateException",
      "Failed to destroy client: " + std::string(rcl_get_error_string_safe()));
  }
}
