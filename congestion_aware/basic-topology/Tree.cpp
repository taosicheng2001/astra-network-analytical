/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Tree.h"
#include <cassert>
#include <iostream>

using namespace NetworkAnalyticalCongestionAware;

Tree::Tree(const int npus_count, const Bandwidth bandwidth, const Latency latency, const int base_id) noexcept
    : bidirectional(true),
      BasicTopology(0, npus_count, bandwidth, latency, base_id) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    // set topology type
    basic_topology_type = TopologyBuildingBlock::Tree;

    // calculate the number of devices in each level
    auto current_level_npus_count = 1;
    auto current_npus_count = 0;
    auto next_npus_count = 0;
    for (auto i = 0; i < height-1; i++) {
      current_level_npus_count = current_level_npus_count * level_counts[i];
      next_npus_count = current_npus_count + current_level_npus_count;
      for (auto j = 0; j < current_level_npus_count; j++) {
        for (auto k = 0; k < level_counts[i+1]; k++) {
          connect(current_npus_count, next_npus_count++, level_bandwidths[i], level_latencies[i], true, false);
        }
        current_npus_count++;
      }
    }
    assert(next_npus_count == npus_count);

}

Route Tree::route(DeviceId src, DeviceId dest) const noexcept {
    // assert npus are in valid range
    assert(src >= 13 && src <= 28);
    assert(dest >= 13 && dest <= 28);

    Route route;
    std::vector<int> path_left;
    std::vector<int> path_right;
    auto cur_left = src;
    auto cur_right = dest;

    while(cur_left != cur_right) {
      path_left.push_back(cur_left);
      path_right.push_back(cur_right);
      cur_left = father_map[cur_left];
      cur_right = father_map[cur_right];
    }
    for(int i = 0; i < path_left.size(); i++) {
      route.push_back(devices[path_left[i]]);
    }
    route.push_back(devices[cur_left]);
    for(int i = path_right.size() - 1; i >= 0; i--) {
      route.push_back(devices[path_right[i]]);
    }

    return route;
}
