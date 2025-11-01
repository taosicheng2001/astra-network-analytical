/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Helper.h"
#include "congestion_aware/MultiDimTopology.h"
#include "congestion_aware/FullyConnected.h"
#include "congestion_aware/Ring.h"
#include "congestion_aware/Switch.h"
#include "congestion_aware/Mesh2D.h"
#include <cstdlib>
#include <iostream>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

std::shared_ptr<Topology> NetworkAnalyticalCongestionAware::construct_topology(
    const NetworkParser& network_parser) noexcept {
    // get network_parser info
    const auto dims_count = network_parser.get_dims_count();
    const auto topologies_per_dim = network_parser.get_topologies_per_dim();
    const auto npus_counts_per_dim = network_parser.get_npus_counts_per_dim();
    const auto bandwidths_per_dim = network_parser.get_bandwidths_per_dim();
    const auto latencies_per_dim = network_parser.get_latencies_per_dim();

    if (dims_count == 1) {

    // retrieve basic basic-topology info
    const auto topology_type = topologies_per_dim[0];
    const auto npus_count = npus_counts_per_dim[0];
    const auto bandwidth = bandwidths_per_dim[0];
    const auto latency = latencies_per_dim[0];

    switch (topology_type) {
    case TopologyBuildingBlock::Ring:
        return std::make_shared<Ring>(npus_count, bandwidth, latency);
    case TopologyBuildingBlock::Switch:
        return std::make_shared<Switch>(npus_count, bandwidth, latency);
    case TopologyBuildingBlock::FullyConnected:
        return std::make_shared<FullyConnected>(npus_count, bandwidth, latency);
    case TopologyBuildingBlock::Mesh2D:
        return std::make_shared<Mesh2D>(npus_count, bandwidth, latency);
    default:
        // shouldn't reaach here
        std::cerr << "[Error] (network/analytical/congestion_aware) " << "not supported basic-topology" << std::endl;
        std::exit(-1);
    }
    }

    const auto multi_dim_topology = std::make_shared<MultiDimTopology>();
    for (auto dim = 0; dim < dims_count; dim++) {
        const auto topology_type = topologies_per_dim[dim];
        const auto npus_count = npus_counts_per_dim[dim];
        const auto bandwidth = bandwidths_per_dim[dim];
        const auto latency = latencies_per_dim[dim];
        
        // create a network dim
        std::unique_ptr<BasicTopology> dim_topology;
        switch (topology_type) {
        case TopologyBuildingBlock::Ring:
            dim_topology = std::make_unique<Ring>(npus_count, bandwidth, latency);
            break;
        case TopologyBuildingBlock::Switch:
            dim_topology = std::make_unique<Switch>(npus_count, bandwidth, latency);
            break;
        case TopologyBuildingBlock::FullyConnected:
            dim_topology = std::make_unique<FullyConnected>(npus_count, bandwidth, latency);
            break;
        case TopologyBuildingBlock::Mesh2D:
            dim_topology = std::make_unique<Mesh2D>(npus_count, bandwidth, latency);
            break;
        default:
            // shouldn't reach here
            std::cerr << "[Error] (network/analytical/congestion_aware) " << "not supported basic-topology" << std::endl;
            std::exit(-1);
        }
        multi_dim_topology->append_dimension(std::move(dim_topology));
    }

    multi_dim_topology->addSwitches();
    // Connect the dimensions
    for (auto dim = 0; dim < dims_count - 1; dim++) {
        multi_dim_topology->connect_dimensions(dim, dim + 1);
    }

    return multi_dim_topology;
}
