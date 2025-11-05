/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_unaware/Mesh1D.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace NetworkAnalyticalCongestionUnaware;

// base_id is the global id of the first device in the mesh

Mesh1D::Mesh1D(const int npus_count, const Bandwidth bandwidth, const Latency latency) noexcept
    : BasicTopology(npus_count, bandwidth, latency) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    // set the building block type
    basic_topology_type = TopologyBuildingBlock::Mesh1D;
}

// Mesh 1D default distance is the absolute difference between the source and destination
int Mesh1D::compute_hops_count(const DeviceId src, const DeviceId dest) const noexcept {
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);
    assert(src != dest);

    return abs(dest - src);
}