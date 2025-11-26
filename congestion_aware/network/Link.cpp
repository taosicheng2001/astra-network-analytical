/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Link.h"
#include "common/NetworkFunction.h"
#include "congestion_aware/Chunk.h"
#include "congestion_aware/Device.h"
#include <cassert>
#include <iostream>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

// declaring static event_queue
std::shared_ptr<EventQueue> Link::event_queue;

void Link::link_become_free(void* const link_ptr) noexcept {
    assert(link_ptr != nullptr);

    // cast to Link*
    auto* const link = static_cast<Link*>(link_ptr);

    // set link free
    link->set_free();

    // process pending chunks if one exist
    if (link->pending_chunk_exists()) {
        link->process_pending_transmission();
    }
}

void Link::set_event_queue(std::shared_ptr<EventQueue> event_queue_ptr) noexcept {
    assert(event_queue_ptr != nullptr);

    // set the event queue
    Link::event_queue = std::move(event_queue_ptr);
}

Link::Link(const Bandwidth bandwidth, const Latency latency, const bool non_blocking) noexcept
    : bandwidth(bandwidth),
      latency(latency),
      non_blocking(non_blocking),
      pending_chunks(),
      busy(false) {
    assert(bandwidth > 0);
    assert(latency >= 0);

    // convert bandwidth from GB/s to B/ns
    bandwidth_Bpns = bw_GBps_to_Bpns(bandwidth);
}

void Link::send(std::unique_ptr<Chunk> chunk) noexcept {
    assert(chunk != nullptr);

    if (busy) {
        // link is busy, add to pending chunks
        chunk->stall_count++;
        chunk->stall_time_begin = Link::event_queue->get_current_time();
        pending_chunks.push_back(std::move(chunk));
    } else {
        // service this chunk immediately
        schedule_chunk_transmission(std::move(chunk));
    }
}

void Link::process_pending_transmission() noexcept {
    // pending chunk should exist
    assert(pending_chunk_exists());

    // get chunk to process
    auto chunk = std::move(pending_chunks.front());
    chunk->stall_times += (Link::event_queue->get_current_time() - chunk->stall_time_begin);
    pending_chunks.pop_front();

    // service this chunk
    schedule_chunk_transmission(std::move(chunk));
}

bool Link::pending_chunk_exists() const noexcept {
    // check pending chunks is not empty
    return !pending_chunks.empty();
}

void Link::set_busy() noexcept {
    // set busy to true
    busy = true;
}

void Link::set_free() noexcept {
    // set busy to false
    busy = false;
}

EventTime Link::serialization_delay(const ChunkSize chunk_size, const double last_bpns) const noexcept {
    assert(chunk_size > 0);

    // calculate serialization delay
    if (last_bpns <= bandwidth_Bpns) {
        return static_cast<EventTime>(0);
    }

    const auto delay = static_cast<Bandwidth>(chunk_size) * ( 1 / bandwidth_Bpns - 1 / last_bpns);

    // return serialization delay in EventTime type
    return static_cast<EventTime>(delay);
}

EventTime Link::communication_delay(const ChunkSize chunk_size, const double last_bpns) const noexcept {
    assert(chunk_size > 0);

    // calculate communication delay
    const auto delay = latency + (static_cast<Bandwidth>(chunk_size) / bandwidth_Bpns);

    // return communication delay in EventTime type
    return static_cast<EventTime>(delay);
}

void Link::schedule_chunk_transmission(std::unique_ptr<Chunk> chunk) noexcept {
    assert(chunk != nullptr);

    // link should be free
    assert(!busy);

    // set link busy
    if (!non_blocking)
        set_busy();

    // get metadata
    const auto chunk_size = chunk->get_size();
    const auto current_time = Link::event_queue->get_current_time();

    const auto last_bpns = chunk->last_bpns;
    chunk->last_bpns = bandwidth_Bpns;

    // schedule chunk arrival event
    const auto serialization_time = serialization_delay(chunk_size, last_bpns);

    const auto chunk_arrival_time = current_time + latency + serialization_time;
    const auto link_free_time = chunk_arrival_time;

    if (chunk->src_device_id == 0){
        auto cnt = 1;
    }

    auto* const chunk_ptr = static_cast<void*>(chunk.release());
    Link::event_queue->schedule_event(chunk_arrival_time, Chunk::chunk_arrived_next_device, chunk_ptr);

    auto* const link_ptr = static_cast<void*>(this);
    Link::event_queue->schedule_event(link_free_time, link_become_free, link_ptr);
}
