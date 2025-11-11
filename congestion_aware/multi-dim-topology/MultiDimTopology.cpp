/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/MultiDimTopology.h"
#include "congestion_aware/VirtualSwitch.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

MultiDimTopology::MultiDimTopology() noexcept : Topology(0) {
    // initialize values
    topology_per_dim.clear();
    npus_count_per_dim = {};

    // initialize topology shape
    npus_count = 0;
    devices_count = 0;
    dims_count = 0;
}

void MultiDimTopology::append_dimension(std::vector<std::unique_ptr<BasicTopology>> topologies) noexcept {
    // increment dims_count
    dims_count++;

    // increase npus_count and devices_count
    auto npus_size = topologies[0]->get_npus_count();
    auto devices_size = topologies[0]->get_devices_count();
    npus_count += npus_size*topologies.size();
    devices_count += devices_size*topologies.size();

    // append bandwidth
    const auto bandwidth = topologies[0]->get_bandwidth_per_dim()[0];
    bandwidth_per_dim.push_back(bandwidth);

    // append devices
    for (auto& topology : topologies) {
        auto topology_devices = topology->get_devices();
        devices.insert(devices.end(), topology_devices.begin(), topology_devices.end());
    }
    
    // push back topology and npus_count
    topology_per_dim.push_back(std::move(topologies));
    if (npus_size == 0 || devices_size == 0) {
        npus_count_per_dim.push_back(1);
    }
    else {
        npus_count_per_dim.push_back(2*8*48);
    }
}

MultiDimTopology::MultiDimAddress MultiDimTopology::translate_address(const DeviceId npu_id) const noexcept {
    // If units-count if [2, 8, 4], and the given id is 47, then the id should be
    // 47 // 16 = 2, leftover = 47 % 16 = 15
    // 15 // 2 = 7, leftover = 15 % 2 = 1
    // 1 // 1 = 1, leftover = 0
    // therefore the address is [1, 7, 2]

    // create empty address
    auto multi_dim_address = MultiDimAddress();

    auto leaf_nodes_count = this->leaf_nodes_count;
    auto SpinalSwitchIndex = npu_id / (2*leaf_nodes_count);
    auto LeafGroupIndex = (npu_id / 2) % leaf_nodes_count;
    auto MeshGroupIndex = npu_id % 2;

    multi_dim_address.push_back(MeshGroupIndex);
    multi_dim_address.push_back(LeafGroupIndex);
    multi_dim_address.push_back(SpinalSwitchIndex);

    // return retrieved address
    return multi_dim_address;
}

int MultiDimTopology::get_dim_to_transfer(const MultiDimAddress& src_address,
                                          const MultiDimAddress& dest_address) const noexcept {
    for (auto dim = dims_count -1 ; dim >= 0; dim--) {
        // check the dim that has different address
        if (src_address[dim] != dest_address[dim]) {
            return dim;
        }
    }

    // shouldn't reach here
    std::cerr << "[Error] (network/analytical/congestion_unaware): " << "src and dest have the same address"
              << std::endl;
    std::exit(-1);
}


void MultiDimTopology::connect_dimensions(const int dim1, const int dim2) noexcept {
    // Connection
    auto& current_topologies = topology_per_dim[dim1];
    auto& next_topologies = topology_per_dim[dim2];

    auto& current_topology = current_topologies[0];
    auto& next_topology = next_topologies[0];

    if (current_topology->get_basic_topology_type() == NetworkAnalytical::TopologyBuildingBlock::VirtualSwitch) {
        // Skip virtual switch
        return;
    }

    assert(next_topology->get_basic_topology_type() == NetworkAnalytical::TopologyBuildingBlock::VirtualSwitch);
    
    auto virtual_bandwidth = next_topology->get_bandwidth();
    auto virtual_latency = next_topology->get_latency();
    auto* virtual_switch = static_cast<VirtualSwitch*>(next_topology.get());
    auto leaf_nodes_count = virtual_switch->get_leaf_nodes_count();
    this->leaf_nodes_count = leaf_nodes_count;

    auto& spinal_switch_topology = topology_per_dim[dim2+1][0];
    auto spinal_switches = spinal_switch_topology->get_devices();

    auto spinal_switch_index = 0;
    auto leaf_node_index = 0;
    for (auto& cur_topo : current_topologies) {
        // connect npu devices to switches
        auto cur_devices = cur_topo->get_devices();
        for (auto& cur_device : cur_devices) {
            cur_device->connect(spinal_switches[spinal_switch_index]->get_id(), virtual_bandwidth, virtual_latency);
            spinal_switches[spinal_switch_index]->connect(cur_device->get_id(), virtual_bandwidth, virtual_latency);
            device_2_father_device_map[cur_device->get_id()] = spinal_switches[spinal_switch_index]->get_id();
            leaf_node_index++;
            if (leaf_node_index == 2*leaf_nodes_count) {
                leaf_node_index = 0;
                spinal_switch_index++;
            }
        }
    }

    return;
}

Route MultiDimTopology::route(DeviceId src, DeviceId dest) const noexcept {
    Route route;

    auto src_multi_dim_address = translate_address(src);
    auto dest_multi_dim_address = translate_address(dest);
    auto dim_to_transfer = get_dim_to_transfer(src_multi_dim_address, dest_multi_dim_address);
    
    
    if (dim_to_transfer == 0) {
        //  Located in the same dimension
        for (auto& topology : topology_per_dim[dim_to_transfer]) {
            if (topology->contains_device(src) && topology->contains_device(dest)) {
                route = topology->route(src, dest);
                break;
            }
        }
    }else{
        auto src_father_device_id = device_2_father_device_map.at(src);
        auto dest_father_device_id = device_2_father_device_map.at(dest);
        if (src_father_device_id == dest_father_device_id) {
            route.push_back(get_device(src));
            route.push_back(get_device(src_father_device_id));
            route.push_back(get_device(dest));
        }
        else {
            route.push_back(get_device(src));
            route.push_back(get_device(src_father_device_id));
            route.push_back(get_device(816));
            route.push_back(get_device(dest_father_device_id));
            route.push_back(get_device(dest));        
        }
    }

    return route;
}


