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

#include <gtest/gtest.h>
#include <dsn/utility/fail_point.h>
#include <dsn/dist/fmt_logging.h>

#include "replica/test/replica_test_base.h"
#include "replica/replica_disk_migrator.h"
#include "meta/meta_data.h"

namespace dsn {
namespace replication {
class replica_disk_migrator;

using reset_ddd_partition_rpc = rpc_holder<reset_ddd_partition_request,reset_ddd_partition_response>;

class replica_reset_test : public replica_stub_test_base
{
public:
   dsn::app_info app_info_1;
   dsn::gpid _gpid;
   dsn::replication::replica_configuration config_1;
   partition_configuration _config;

   int app_id_1_primary_count_for_disk =1;
   int app_id_1_secondary_count_for_disk=2;
   const rpc_address PRIMARY = rpc_address("127.0.0.1", 10086);
   const rpc_address SECONDARY1 = rpc_address("127.0.0.1", 10085);
   const rpc_address SECONDARY2 = rpc_address("127.0.0.1", 10087);
   const rpc_address SECONDARY3 = rpc_address("127.0.0.1", 10080);
   reset_ddd_partition_rpc fake_reset_ddd_partition_rpc;
   dsn::replication::mock_replica* _mock_replica_ptr;

public:
   void SetUp() override {}

   replica_reset_test()
   {
   }
   ~replica_reset_test() { }
    void init_env(){
        dsn::utils::filesystem::create_directory("./tmp_dir");
        mock_app_info();
        generate_mock_dir_nodes();
        _mock_replica_ptr = stub->add_primary_replica(app_info_1.app_id,2,"./tmp_dir");
        _mock_replica_ptr->_disk_migrator->set_status(disk_migration_status::CLOSED);
        _mock_replica_ptr->_config.primary = PRIMARY;
//        _mock_replica = create_mock_replica(stub.get(), 2, 2, "./tmp_dir");
//        stub->_replicas[_gpid] = _mock_replica.get();
//        _mock_replica->_dir = "./tmp_dir";
//
//        _config.pid = _gpid;
//        _config.max_replica_count = 3;
//        _config.ballot = 0;
//        _config.primary = PRIMARY;
//        _config.secondaries.emplace_back(SECONDARY1);
//        _config.secondaries.emplace_back(SECONDARY2);
//        _mock_replica->set_primary_partition_configuration(_config);
//
//        _mock_replica->_config.primary = PRIMARY;
//        mock_replica_disk_migrator();
//        fail::cfg("replica_update_local_configuration", "return()");
        for(auto iter : stub->_replicas){
            std::cout<<"_replicas:"<< iter.second->_config<<std::endl;
        }
    }
   void generate_fake_rpc()
   {
       // create RPC_DDD_RESET_PARTITION fake request
       auto reset_quest = dsn::make_unique<reset_ddd_partition_request>();
       config_1.status = partition_status::PS_PRIMARY;
       config_1.pid = dsn::gpid(app_info_1.app_id,2);
       config_1.ballot= 0 ;
       reset_quest->config = config_1;
       fake_reset_ddd_partition_rpc = reset_ddd_partition_rpc(std::move(reset_quest), RPC_DDD_RESET_PARTITION);
   }

   void mock_app_info()
   {
       app_info_1.app_id = 2;
       app_info_1.app_name = "reset_test_1";
       app_info_1.app_type = "replica";
       app_info_1.is_stateful = true;
       app_info_1.max_replica_count = 3;
       app_info_1.partition_count = 4;
       _gpid.set_app_id(app_info_1.app_id);
       _gpid.set_partition_index(2);
   }
   void mock_replica_disk_migrator(){
        dsn::replication::replica_disk_migrator disk_mig(_mock_replica_ptr);
        disk_mig._status = disk_migration_status::MOVED;
        _mock_replica_ptr->_disk_migrator = std::make_unique<dsn::replication::replica_disk_migrator>(disk_mig);
   }

   void generate_mock_dir_nodes()
   {
       int app_id_1_disk_holding_replica_count =
           app_id_1_primary_count_for_disk + app_id_1_secondary_count_for_disk;

       int app_id_1_partition_index = 1;

       int64_t disk_capacity_mb = 100;
       int count = 0;
           int64_t disk_available_mb = count * 50;
           int disk_available_ratio =
               static_cast<int>(std::round((double)100 * disk_available_mb / disk_capacity_mb));
           // create one mock dir_node and make sure disk_capacity_mb_ > disk_available_mb_
           dir_node *node_disk = new dir_node("tmp_dir" ,
                                              "./tmp_dir" ,
                                              disk_capacity_mb,
                                              disk_available_mb,
                                              disk_available_ratio);

           stub->_options.data_dirs.push_back(
               node_disk->full_dir); // open replica need the options
           utils::filesystem::create_directory(node_disk->full_dir);

           int app_1_replica_count_per_disk = app_id_1_disk_holding_replica_count;
           while (app_1_replica_count_per_disk-- > 0) {
               node_disk->holding_replicas[app_info_1.app_id].emplace(
                   gpid(app_info_1.app_id, app_id_1_partition_index++));
           }

           stub->_fs_manager._dir_nodes.emplace_back(node_disk);
           stub->_fs_manager._available_data_dirs.emplace_back(node_disk->full_dir);
   }
};



TEST_F(replica_reset_test, on_reset_primary_replica)
{
    init_env();
    dsn::replication::replica_configuration replica_config;
    replica_config.pid = _gpid;
    replica_config.ballot = 0;
    replica_config.status = partition_status::PS_PRIMARY;
    _mock_replica_ptr->set_replica_config(replica_config);
    std::cout<<_mock_replica_ptr->status()<<std::endl;

    generate_fake_rpc();
    stub->on_reset_ddd_replica(fake_reset_ddd_partition_rpc);

    reset_ddd_partition_response& response = fake_reset_ddd_partition_rpc.response();
    // test response disk_info
    std::cout<<"ddd_partition_dir: "<<response.ddd_partition_dir<<"  ERR: "<<response.err.to_string()<<std::endl;
}


} // namespace replication
} // namespace dsn
