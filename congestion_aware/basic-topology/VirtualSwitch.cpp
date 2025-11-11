/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/VirtualSwitch.h"
#include <cassert>

using namespace NetworkAnalyticalCongestionAware;

/*

VirtualSwitch(4) example:
<-virtual-switch->
|       |         |       |
virtual-switch0 virtual-switch1 virtual-switch2 virtual-switch3

@param leaf_nodes_count: number of leaf nodes in the virtual switch
@param bandwidth: bandwidth of the link
@param latency: latency of the link
@param base_id: base id of the topology
*/

VirtualSwitch::VirtualSwitch(const int leaf_nodes_count, const Bandwidth bandwidth, const Latency latency, const int base_id) noexcept
    : BasicTopology(0, 0, bandwidth, latency, base_id) {
    
    // there ar no npus in this topology, only virtual switch
    assert(leaf_nodes_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    basic_topology_type = TopologyBuildingBlock::VirtualSwitch;

    // set leaf nodes count
    this->leaf_nodes_count = leaf_nodes_count;
}

int VirtualSwitch::get_leaf_nodes_count() const noexcept {
    return leaf_nodes_count;
}

Route VirtualSwitch::route(DeviceId global_src, DeviceId global_dest) const noexcept {

    assert(0); // should not reach here
    return Route();
}
