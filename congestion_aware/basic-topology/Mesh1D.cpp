/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Mesh1D.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace NetworkAnalyticalCongestionAware;

// base_id is the global id of the first device in the mesh

Mesh1D::Mesh1D(const int npus_count, const Bandwidth bandwidth, const Latency latency, const int base_id) noexcept
    : BasicTopology(npus_count, npus_count, bandwidth, latency, base_id) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    // set topology type
    basic_topology_type = TopologyBuildingBlock::Mesh1D;

    // calculate the size of the 1D array
    assert(npus_count == 2);

    // connect npus in a 1D mesh
    for (auto i = 0; i < npus_count; i++) {
        if (i + 1 < npus_count) {
            connect(i, i + 1, bandwidth, latency, true, false);
        }
    }
}

Route Mesh1D::route(DeviceId global_src, DeviceId global_dest) const noexcept {
    assert(contains_device(global_src));
    assert(contains_device(global_dest));

    auto local_src = global_src - base_id;
    auto local_dest = global_dest - base_id;

    // construct empty route
    auto route = Route();

    // 1. Add the source to the route
    route.push_back(devices[local_src]);

    // 2. Find the coordinates of src and dest
    auto current_pos = local_src;

    // 3. Find the route
    while(current_pos != local_dest) {
        if(current_pos < local_dest) {
            current_pos++;
        } else {
            current_pos--;
        }
        route.push_back(devices[current_pos]);
    }

    return route;
}
