/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Mesh2D.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace NetworkAnalyticalCongestionAware;

Mesh2D::Mesh2D(const int npus_count, const Bandwidth bandwidth, const Latency latency, const int base_id) noexcept
    : BasicTopology(npus_count, npus_count, bandwidth, latency, base_id) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    // set topology type
    basic_topology_type = TopologyBuildingBlock::Mesh2D;

    // calculate the column and row size of the 2D array
    // Only support 32 and 64 PUs for now
    if (npus_count == 32) {
        col_size = 8;
        row_size = 4;
    } else if (npus_count == 64) {
        col_size = 8;
        row_size = 8;
    } else if (npus_count == 8) {
        col_size = 2;
        row_size = 4;
    } else if (npus_count == 768) {
        col_size = 24;
        row_size = 32;
    } else {
        assert(false);
    }
    
    assert(col_size * row_size == npus_count);

    // connect npus in a 2D mesh
    for (auto i = 0; i < npus_count; i++) {
        auto x = i % col_size;
        auto y = i / col_size; 

        if (x + 1 < col_size) {
            connect(i, i + 1, bandwidth, latency, true, false);
        }
        if (y + 1 < row_size) {
            connect(i, i + col_size, bandwidth, latency, true, false);
        }
    }
}

Route Mesh2D::route(DeviceId src, DeviceId dest) const noexcept {
    assert(contains_device(src));
    assert(contains_device(dest));

    auto local_src = src - base_id;
    auto local_dest = dest - base_id;

    // construct empty route
    auto route = Route();

    // 1. Add the source to the route
    route.push_back(devices[local_src]);

    // 2. Find the coordinates of src and dest
    auto src_col = local_src % col_size;
    auto src_row = local_src / col_size;
    auto dest_col = local_dest % col_size;
    auto dest_row = local_dest / col_size;

    // 3. Find the route
    auto current_col = src_col;
    auto current_row = src_row;

    while(current_col != dest_col) {
        if(current_col < dest_col) {
            current_col++;
        } else {
            current_col--;
        }
        route.push_back(devices[current_col + current_row * col_size]);
    }

    while(current_row != dest_row) {
        if(current_row < dest_row) {
            current_row++;
        } else {
            current_row--;
        }
        route.push_back(devices[current_col + current_row * col_size]);
    }

    // return the constructed route
    return route;
}
