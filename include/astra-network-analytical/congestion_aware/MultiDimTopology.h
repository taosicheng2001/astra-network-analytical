/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_aware/BasicTopology.h"
#include "congestion_aware/Topology.h"
#include <memory>

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionAware {

/**
 * MultiDimTopology implements multi-dimensional network topologies
 * which can be constructed by stacking up multiple BasicTopology instances.
 */
class MultiDimTopology : public Topology {
  public:
    /**
     * Constructor.
     */
    MultiDimTopology() noexcept;

    /**
     * Add a dimension to the multi-dimensional topology.
     *
     * @param topology BasicTopology instance to be added.
     */
    void append_dimension(std::vector<std::unique_ptr<BasicTopology>> topologies) noexcept;

    /**
     * Route a chunk from src to dest.
     *
     * @param src src NPU ID
     * @param dest dest NPU ID
     * @return route information
     */
    [[nodiscard]] Route route(DeviceId src, DeviceId dest) const noexcept;
    
    void connect_dimensions(const int dim1, const int dim2) noexcept;
  
    std::map<int, int> device_2_father_device_map;
    int npu_counts;
    int real_swtich_count;

  private:
    /// Each NPU ID can be broken down into multiple dimensions.
    /// for example, if the topology size is [2, 8, 4] and the NPU ID is 31,
    /// then the NPU ID can be broken down into [1, 7, 1].
    using MultiDimAddress = std::vector<DeviceId>;

    /// BasicTopology instances per dimension.
    std::vector<std::vector<std::unique_ptr<BasicTopology>>> topology_per_dim;

    /**
     * Translate the NPU ID into a multi-dimensional address.
     *
     * @param npu_id id of the NPU
     * @return the same NPU in multi-dimensional address representation
     */
    [[nodiscard]] MultiDimAddress translate_address(DeviceId npu_id) const noexcept;

    /**
     * Given src and dest address in multi-dimensional form,
     * return the dimension where the transfer should happen.
     * i.e., the dimension where the src and dest addresses differ.
     *
     * @param src_address src NPU ID in multi-dimensional form
     * @param dest_address dest NPU ID in multi-dimensional form
     * @return the dimension where the transfer should happen
     */
    [[nodiscard]] int get_dim_to_transfer(const MultiDimAddress& src_address,
                                          const MultiDimAddress& dest_address) const noexcept;

    int leaf_nodes_count;

};

}  // namespace NetworkAnalyticalCongestionAware
