/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/SpinalSwitch.h"
#include <cassert>

using namespace NetworkAnalyticalCongestionAware;

/*

SpinalSwitch(4) example:
<-spinal-switch->
|       |         |       |
switch0 switch1 switch2 switch3

@param leaf_nodes_count: number of leaf nodes in the spine
@param bandwidth: bandwidth of the link
@param latency: latency of the link
@param base_id: base id of the topology
*/

SpinalSwitch::SpinalSwitch(const int leaf_nodes_count, const Bandwidth bandwidth, const Latency latency, const int base_id) noexcept
    : BasicTopology(0, leaf_nodes_count+1, bandwidth, latency, base_id) {
    
    // there ar no npus in this topology, only spinal switch
    assert(leaf_nodes_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    basic_topology_type = TopologyBuildingBlock::SpinalSwitch;

    // set spinal switch id
    spinal_switch_id = leaf_nodes_count;

    // connect npus and switches, the link should be bidirectional
    for (auto i = 0; i < leaf_nodes_count; i++) {
        connect(i, spinal_switch_id, bandwidth, latency, true, true);
    }
}

Route SpinalSwitch::route(DeviceId global_src, DeviceId global_dest) const noexcept {

    assert(contains_device(global_src));
    assert(contains_device(global_dest));

    auto local_src = global_src - base_id;
    auto local_dest = global_dest - base_id;

    // construct empty route
    auto route = Route();
    route.push_back(devices[local_src]);
    route.push_back(devices[spinal_switch_id]);
    route.push_back(devices[local_dest]);
    return route;
}
