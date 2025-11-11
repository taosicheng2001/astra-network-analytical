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


class SpinalSwitch final : public BasicTopology {
  public:
    /**
     * Constructor.
     *
     * @param leaf_nodes_count number of leaf nodes connected to the spinal switch
     * @param bandwidth bandwidth of link
     * @param latency latency of link
     * @param base_id base id of the topology
     */
    SpinalSwitch(int leaf_nodes_count, Bandwidth bandwidth, Latency latency, const int base_id) noexcept;

    /**
     * Implementation of route function in Topology.
     */
    [[nodiscard]] Route route(DeviceId global_src, DeviceId global_dest) const noexcept override;

  private:
    /// node_id of the spinal switch node
    DeviceId spinal_switch_id;
};

}  // namespace NetworkAnalyticalCongestionAware
