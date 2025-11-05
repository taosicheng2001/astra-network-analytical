/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_unaware/Switch.h"
#include <cassert>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionUnaware;

L2Switch::L2Switch(const int npus_count, const Bandwidth bandwidth, const Latency latency) noexcept
    : BasicTopology(npus_count, bandwidth, latency) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    // set the building block type
    basic_topology_type = TopologyBuildingBlock::L2Switch;
}

int L2Switch::compute_hops_count(const DeviceId src, const DeviceId dest) const noexcept {
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);
    assert(src != dest);

    // for L2Switch, hops_count is always 2 (src -> L2Switch -> dest)
    return 2;
}
