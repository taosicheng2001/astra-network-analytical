/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_aware/BasicTopology.h"

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionAware {

/**
 * Implements a tree topology.

 */

class Tree final : public BasicTopology {
  public:
    /**
     * Constructor.
     *
     * @param npus_count number of npus in a tree
     * @param bandwidth bandwidth of link
     * @param latency latency of link
     * @param bidirectional true if tree is bidirectional, false otherwise
     * @param base_id base id of the topology
     */
    Tree(int npus_count, Bandwidth bandwidth, Latency latency, const int base_id=0) noexcept;

    /**
     * Implementation of route function in Topology.
     */
    [[nodiscard]] Route route(DeviceId src, DeviceId dest) const noexcept override;

  private:
    /// true if the tree is bidirectional, false otherwise
    bool bidirectional;
    int height=4;

    std::vector<int> level_counts = {1, 4, 2, 2};
    std::vector<double> level_bandwidths = {25.0, 50.0, 100.0, 200.0};
    std::vector<double> level_latencies = {2000.0, 1000.0, 500.0, 200.0};

    std::vector<int> father_map = {
      -1,0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12
    };
};

}  // namespace NetworkAnalyticalCongestionAware
