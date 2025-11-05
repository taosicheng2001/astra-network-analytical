/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_unaware/BasicTopology.h"

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionUnaware {

class Mesh1D final : public BasicTopology {
  public:
    /**
     * Constructor
     *
     * @param npus_count number of NPUs in the Mesh1D
     * @param bandwidth bandwidth of each link
     * @param latency latency of each link
     */
    Mesh1D(int npus_count, Bandwidth bandwidth, Latency latency) noexcept;

  private:
    /**
     * Implements the compute_hops_count method of BasicTopology.
     */
    [[nodiscard]] int compute_hops_count(DeviceId src, DeviceId dest) const noexcept override;
};

}  // namespace NetworkAnalyticalCongestionUnaware
