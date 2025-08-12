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
 * Implements a mesh topology.
 *
 * Ring(16) example:
 * 0 - 1 - 2 - 3
 * |   |   |   |
 * 3 - 5 - 6 - 7
 * |   |   |   |
 * 8 - 9 - 10 -11
 * |   |   |   |
 * 12-13- 14 - 15
 *
 * Therefore, the number of NPUs and devices are both 16.
 *
 */
class Mesh2D final : public BasicTopology {
  public:
    /**
     * Constructor.
     *
     * @param npus_count number of npus in a ring
     * @param bandwidth bandwidth of link
     * @param latency latency of link
     */
    Mesh2D(int npus_count, Bandwidth bandwidth, Latency latency) noexcept;

    /**
     * Implementation of route function in Topology.
     */
    [[nodiscard]] Route route(DeviceId src, DeviceId dest) const noexcept override;

  private:
    int col_size; // column size of the 2D array
    int row_size; // row size of the 2D array
};

}  // namespace NetworkAnalyticalCongestionAware
