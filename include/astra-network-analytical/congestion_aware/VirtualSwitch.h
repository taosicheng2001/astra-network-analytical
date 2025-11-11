/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_aware/BasicTopology.h"
#include <cassert>

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionAware {


class VirtualSwitch final : public BasicTopology {
  public:
    /**
     * Constructor.
     *
     * @param leaf_nodes_count number of leaf nodes connected to the virtual switch
     * @param bandwidth bandwidth of link
     * @param latency latency of link
     * @param base_id base id of the topology
     */
    VirtualSwitch(int leaf_nodes_count, Bandwidth bandwidth, Latency latency, const int base_id) noexcept;

    /**
     * Implementation of route function in Topology.
     */
    [[nodiscard]] Route route(DeviceId global_src, DeviceId global_dest) const noexcept override;

    /**
     * Get the number of leaf nodes connected to the virtual switch.
     *
     * @return number of leaf nodes connected to the virtual switch
     */
    int get_leaf_nodes_count() const noexcept;

  private:
    /// number of leaf nodes connected to the virtual switch
    int leaf_nodes_count;
};

}  // namespace NetworkAnalyticalCongestionAware
