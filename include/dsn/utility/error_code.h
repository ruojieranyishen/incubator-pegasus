/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

#include <dsn/utility/customizable_id.h>
#include <thrift/protocol/TProtocol.h>

namespace dsn {

class error_code
{
public:
    explicit error_code(const char *name);

    explicit constexpr error_code(int err) : _internal_code(err) {}

    constexpr error_code() = default;

    const char *to_string() const;

    constexpr bool operator==(const error_code &r) { return _internal_code == r._internal_code; }

    constexpr bool operator!=(const error_code &r) { return !(*this == r); }

    constexpr operator int() const { return _internal_code; }

    // for serialization in thrift format
    uint32_t read(::apache::thrift::protocol::TProtocol *iprot);
    uint32_t write(::apache::thrift::protocol::TProtocol *oprot) const;

    static int max();
    static bool is_exist(const char *name);
    static error_code try_get(const char *name, error_code default_value);
    static error_code try_get(const std::string &name, error_code default_value);

    friend std::ostream &operator<<(std::ostream &os, const error_code &ec)
    {
        return os << std::string(ec.to_string());
    }

private:
    int _internal_code{0};
};

#define DEFINE_ERR_CODE(x) __selectany const dsn::error_code x(#x);

// the following error codes are grouped per 10 items,
// so that we can get the integer representation for each error_code easily
DEFINE_ERR_CODE(ERR_OK)

DEFINE_ERR_CODE(ERR_UNKNOWN)
DEFINE_ERR_CODE(ERR_SERVICE_NOT_FOUND)
DEFINE_ERR_CODE(ERR_SERVICE_ALREADY_RUNNING)
DEFINE_ERR_CODE(ERR_IO_PENDING)
DEFINE_ERR_CODE(ERR_TIMEOUT)
DEFINE_ERR_CODE(ERR_SERVICE_NOT_ACTIVE)
DEFINE_ERR_CODE(ERR_BUSY)
DEFINE_ERR_CODE(ERR_NETWORK_INIT_FAILED)
DEFINE_ERR_CODE(ERR_FORWARD_TO_OTHERS)
DEFINE_ERR_CODE(ERR_OBJECT_NOT_FOUND)

DEFINE_ERR_CODE(ERR_HANDLER_NOT_FOUND)
DEFINE_ERR_CODE(ERR_LEARN_FILE_FAILED)
DEFINE_ERR_CODE(ERR_GET_LEARN_STATE_FAILED)
DEFINE_ERR_CODE(ERR_INVALID_VERSION)
DEFINE_ERR_CODE(ERR_INVALID_PARAMETERS)
DEFINE_ERR_CODE(ERR_CAPACITY_EXCEEDED)
DEFINE_ERR_CODE(ERR_INVALID_STATE)
DEFINE_ERR_CODE(ERR_INACTIVE_STATE)
DEFINE_ERR_CODE(ERR_NOT_ENOUGH_MEMBER)
DEFINE_ERR_CODE(ERR_FILE_OPERATION_FAILED)

DEFINE_ERR_CODE(ERR_HANDLE_EOF)
DEFINE_ERR_CODE(ERR_WRONG_CHECKSUM)
DEFINE_ERR_CODE(ERR_INVALID_DATA)
DEFINE_ERR_CODE(ERR_INVALID_HANDLE)
DEFINE_ERR_CODE(ERR_INCOMPLETE_DATA)
DEFINE_ERR_CODE(ERR_VERSION_OUTDATED)
DEFINE_ERR_CODE(ERR_PATH_NOT_FOUND)
DEFINE_ERR_CODE(ERR_PATH_ALREADY_EXIST)
DEFINE_ERR_CODE(ERR_ADDRESS_ALREADY_USED)
DEFINE_ERR_CODE(ERR_STATE_FREEZED)

DEFINE_ERR_CODE(ERR_LOCAL_APP_FAILURE)
DEFINE_ERR_CODE(ERR_BIND_IOCP_FAILED)
DEFINE_ERR_CODE(ERR_NETWORK_START_FAILED)
DEFINE_ERR_CODE(ERR_NOT_IMPLEMENTED)
DEFINE_ERR_CODE(ERR_CHECKPOINT_FAILED)
DEFINE_ERR_CODE(ERR_WRONG_TIMING)
DEFINE_ERR_CODE(ERR_NO_NEED_OPERATE)
DEFINE_ERR_CODE(ERR_CORRUPTION)
DEFINE_ERR_CODE(ERR_TRY_AGAIN)
DEFINE_ERR_CODE(ERR_CLUSTER_NOT_FOUND)

DEFINE_ERR_CODE(ERR_CLUSTER_ALREADY_EXIST)
DEFINE_ERR_CODE(ERR_SERVICE_ALREADY_EXIST)
DEFINE_ERR_CODE(ERR_INJECTED)
DEFINE_ERR_CODE(ERR_REPLICATION_FAILURE)
DEFINE_ERR_CODE(ERR_APP_EXIST)
DEFINE_ERR_CODE(ERR_APP_NOT_EXIST)
DEFINE_ERR_CODE(ERR_BUSY_CREATING)
DEFINE_ERR_CODE(ERR_BUSY_DROPPING)
DEFINE_ERR_CODE(ERR_NETWORK_FAILURE)
DEFINE_ERR_CODE(ERR_UNDER_RECOVERY)

DEFINE_ERR_CODE(ERR_LEARNER_NOT_FOUND)
DEFINE_ERR_CODE(ERR_OPERATION_DISABLED)
DEFINE_ERR_CODE(ERR_EXPIRED)
DEFINE_ERR_CODE(ERR_LOCK_ALREADY_EXIST)
DEFINE_ERR_CODE(ERR_HOLD_BY_OTHERS)
DEFINE_ERR_CODE(ERR_RECURSIVE_LOCK)
DEFINE_ERR_CODE(ERR_NO_OWNER)
DEFINE_ERR_CODE(ERR_NODE_ALREADY_EXIST)
DEFINE_ERR_CODE(ERR_INCONSISTENT_STATE)
DEFINE_ERR_CODE(ERR_ARRAY_INDEX_OUT_OF_RANGE)

DEFINE_ERR_CODE(ERR_DIR_NOT_EMPTY)
DEFINE_ERR_CODE(ERR_FS_INTERNAL)
DEFINE_ERR_CODE(ERR_IGNORE_BAD_DATA)
DEFINE_ERR_CODE(ERR_APP_DROPPED)
DEFINE_ERR_CODE(ERR_MOCK_INTERNAL)
DEFINE_ERR_CODE(ERR_ZOOKEEPER_OPERATION)
DEFINE_ERR_CODE(ERR_CHILD_REGISTERED)
DEFINE_ERR_CODE(ERR_INGESTION_FAILED)
DEFINE_ERR_CODE(ERR_UNAUTHENTICATED)
DEFINE_ERR_CODE(ERR_KRB5_INTERNAL)

DEFINE_ERR_CODE(ERR_SASL_INTERNAL)
DEFINE_ERR_CODE(ERR_SASL_INCOMPLETE)
DEFINE_ERR_CODE(ERR_ACL_DENY)
DEFINE_ERR_CODE(ERR_SPLITTING)
DEFINE_ERR_CODE(ERR_PARENT_PARTITION_MISUSED)
DEFINE_ERR_CODE(ERR_CHILD_NOT_READY)
DEFINE_ERR_CODE(ERR_DISK_INSUFFICIENT)
DEFINE_ERR_CODE(ERR_RETRY_EXHAUSTED)
} // namespace dsn
