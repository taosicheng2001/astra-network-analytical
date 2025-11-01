/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/MultiDimTopology.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

MultiDimTopology::MultiDimTopology() noexcept : Topology() {
    // initialize values
    topology_per_dim.clear();
    npus_count_per_dim = {};

    // initialize topology shape
    npus_count = 1;
    devices_count = 1;
    dims_count = 0;
}

void MultiDimTopology::append_dimension(std::unique_ptr<BasicTopology> topology) noexcept {
    // increment dims_count
    dims_count++;

    // increase npus_count
    const auto topology_size = topology->get_npus_count();
    npus_count *= topology_size;
    devices_count *= topology_size;

    // append bandwidth
    const auto bandwidth = topology->get_bandwidth_per_dim()[0];
    bandwidth_per_dim.push_back(bandwidth);

    // append devices
    auto topology_devices = topology->get_devices();
    devices.insert(devices.end(), topology_devices.begin(), topology_devices.end());
    
    // push back topology and npus_count
    topology_per_dim.push_back(std::move(topology));
    npus_count_per_dim.push_back(topology_size);

}

MultiDimTopology::MultiDimAddress MultiDimTopology::translate_address(const DeviceId npu_id) const noexcept {
    // If units-count if [2, 8, 4], and the given id is 47, then the id should be
    // 47 // 16 = 2, leftover = 47 % 16 = 15
    // 15 // 2 = 7, leftover = 15 % 2 = 1
    // 1 // 1 = 1, leftover = 0
    // therefore the address is [1, 7, 2]

    // create empty address
    auto multi_dim_address = MultiDimAddress();
    for (auto i = 0; i < dims_count; i++) {
        multi_dim_address.push_back(-1);
    }

    auto leftover = npu_id;
    auto denominator = npus_count;

    for (auto dim = dims_count - 1; dim >= 0; dim--) {
        // change denominator
        denominator /= npus_count_per_dim[dim];

        // get and update address
        const auto quotient = leftover / denominator;
        leftover %= denominator;

        // update address
        multi_dim_address[dim] = quotient;
    }

    // check address translation
    for (auto i = 0; i < dims_count; i++) {
        assert(0 <= multi_dim_address[i]);
        assert(multi_dim_address[i] < npus_count_per_dim[i]);
    }

    // return retrieved address
    return multi_dim_address;
}

int MultiDimTopology::get_dim_to_transfer(const MultiDimAddress& src_address,
                                          const MultiDimAddress& dest_address) const noexcept {
    for (auto dim = dims_count -1 ; dim >= 0; dim--) {
        // check the dim that has different address
        if (src_address[dim] != dest_address[dim]) {
            return dim;
        }
    }

    // shouldn't reach here
    std::cerr << "[Error] (network/analytical/congestion_unaware): " << "src and dest have the same address"
              << std::endl;
    std::exit(-1);
}

void MultiDimTopology::addSwitches() noexcept {
    auto switches_count = 1;
    for (auto dim = dims_count - 1; dim > 0; dim--) {
        switches_count *= npus_count_per_dim[dim];
        devices_count += switches_count;
    }
    return;
}

void MultiDimTopology::connect_dimensions(const int dim1, const int dim2) noexcept {
    // Connection
    return;
}

Route MultiDimTopology::route(DeviceId src, DeviceId dest) const noexcept {
    Route route;
    auto src_multi_dim_address = translate_address(src);
    auto dest_multi_dim_address = translate_address(dest);
    auto dim_to_transfer = get_dim_to_transfer(src_multi_dim_address, dest_multi_dim_address);
    
    if (dim_to_transfer == 0) {
        //  Located in the same dimension
        route = topology_per_dim[dim_to_transfer]->route(src, dest);
    }else{
        std::cout << src << std::endl;
        std::cout << dest << std::endl;
        std::cout << "dim_to_transfer: " << dim_to_transfer << std::endl;
        exit(-1);
    }

    return route;
}


