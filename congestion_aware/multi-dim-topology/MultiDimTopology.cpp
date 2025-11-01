/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/MultiDimTopology.h"
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
    npus_count_per_dim.push_back(npus_size);
}

MultiDimTopology::MultiDimAddress MultiDimTopology::translate_address(const DeviceId npu_id) const noexcept {
    // If units-count if [2, 8, 4], and the given id is 47, then the id should be
    // 47 // 16 = 2, leftover = 47 % 16 = 15
    // 15 // 2 = 7, leftover = 15 % 2 = 1
    // 1 // 1 = 1, leftover = 0
    // therefore the address is [1, 7, 2]

    // create empty address
    auto multi_dim_address = MultiDimAddress();
    auto treeGroup = npu_id / 8;
    auto GroupIndex = npu_id % 8;
    multi_dim_address.push_back(GroupIndex);
    multi_dim_address.push_back(treeGroup);

    // check address translation
    //for (auto i = 0; i < dims_count; i++) {
    //   assert(0 <= multi_dim_address[i]);
    //    assert(multi_dim_address[i] < npus_count_per_dim[i]);
    //}

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

void MultiDimTopology::addSwitches() noexcept {
    auto switches_count = 1;
    for (auto dim = dims_count - 1; dim > 0; dim--) {
        switches_count *= npus_count_per_dim[dim];
        devices_count += switches_count;
    }
    return;
}

void MultiDimTopology::connect_dimensions(const int dim1, const int dim2) noexcept {
    // Connection
    const auto& mesh_topos = topology_per_dim[dim1];
    const auto& tree_topo = topology_per_dim[dim2][0];
    auto tree_leaf = 13+128;
    for( auto& mesh_topo : mesh_topos) {
        auto devices = mesh_topo->get_devices();

        for(auto device : devices) {
            connect(device->get_id(), tree_leaf, 200.0, 200.0);
        }
        tree_leaf++;
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
                return route;
            }
        }
    }else{
        // transmit by tree topo
        auto tree_src_node = 13 + src_multi_dim_address[dim_to_transfer];
        auto tree_dest_node = 13 + dest_multi_dim_address[dim_to_transfer];
        const auto& mesh_topos = topology_per_dim[0];
        const auto& tree_topo = topology_per_dim[dim_to_transfer][0];

        for( auto& mesh_topo : mesh_topos) {
            if (mesh_topo->contains_device(src)) {
                route.push_back(mesh_topo->get_device(src));
                break;
            }
        }

        auto sub_route = tree_topo->route(tree_src_node, tree_dest_node);
        route.insert(route.end(), sub_route.begin(), sub_route.end());

        for( auto& mesh_topo : mesh_topos) {
            if (mesh_topo->contains_device(dest)) {
                route.push_back(mesh_topo->get_device(dest));
                break;
            }
        }
    }

    return route;
}


