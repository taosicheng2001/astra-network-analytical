/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Topology.h"
#include "congestion_aware/Link.h"
#include <cassert>

using namespace NetworkAnalyticalCongestionAware;

void Topology::set_event_queue(std::shared_ptr<EventQueue> event_queue) noexcept {
    assert(event_queue != nullptr);

    // pass the given event_queue to Link
    Link::set_event_queue(std::move(event_queue));
}

Topology::Topology(int base_id) noexcept : npus_count(-1), devices_count(-1), dims_count(-1), base_id(base_id) {
    npus_count_per_dim = {};
}

int Topology::get_devices_count() const noexcept {
    assert(devices_count > 0);
    assert(npus_count >= 0);
    assert(devices_count >= npus_count);

    return devices_count;
}

bool Topology::contains_device(DeviceId device_id) const noexcept {
    for (auto device : devices) {
        if (device->get_id() == device_id) {
            return true;
        }
    }
    return false;
}

int Topology::get_npus_count() const noexcept {
    assert(devices_count > 0);
    assert(npus_count >= 0);
    assert(devices_count >= npus_count);

    return npus_count;
}

int Topology::get_dims_count() const noexcept {
    assert(dims_count > 0);

    return dims_count;
}

std::vector<std::shared_ptr<Device>> Topology::get_devices() const noexcept {
    return devices;
}

std::shared_ptr<Device> Topology::get_device(DeviceId device_id) const noexcept {
    for (auto device : devices) {
        if (device->get_id() == device_id) {
            return device;
        }
    }
    return nullptr;    
}

std::vector<int> Topology::get_npus_count_per_dim() const noexcept {
    assert(npus_count_per_dim.size() == dims_count);

    return npus_count_per_dim;
}

std::vector<Bandwidth> Topology::get_bandwidth_per_dim() const noexcept {
    assert(bandwidth_per_dim.size() == dims_count);

    return bandwidth_per_dim;
}

void Topology::send(std::unique_ptr<Chunk> chunk) noexcept {
    assert(chunk != nullptr);

    // get src npu node_id
    const auto src = chunk->current_device()->get_id();

    // assert src is valid
    assert(0 <= src && src < devices_count);

    // initiate transmission from src
    devices[src]->send(std::move(chunk));
}

void Topology::connect(const DeviceId src,
                       const DeviceId dest,
                       const Bandwidth bandwidth,
                       const Latency latency,
                       const bool bidirectional) noexcept {
    // assert the src and dest are valid
    auto global_src = src + base_id;
    auto global_dest = dest + base_id;

    // assert bandwidth and latency are valid
    assert(bandwidth > 0);
    assert(latency >= 0);

    // connect src -> dest
    devices[src]->connect(global_dest, bandwidth, latency);

    // if bidirectional, connect dest -> src
    if (bidirectional) {
        devices[dest]->connect(global_src, bandwidth, latency);
    }
}

void Topology::instantiate_devices() noexcept {
    // instantiate all devices
    for (auto i = 0; i < devices_count; i++) {
        devices.push_back(std::make_shared<Device>(this->base_id + i, this->base_id));
    }
}
