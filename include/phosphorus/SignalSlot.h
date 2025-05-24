//
// Created by Renatus Madrigal on 5/24/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_SIGNALSLOT_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_SIGNALSLOT_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace phosphorus {

/**
 * @brief A simple signal-slot implementation.
 * @details Currently we use signal and slot to implement a simple observer.
 * Thus, the Particle, Field and Coordinate classes can observe each other.
 * @tparam Args The argument types for the signal.
 */
template <typename... Args> class Signal {
  struct Slot {
    std::function<void(Args...)> callback;
    std::shared_ptr<std::atomic<bool>> valid;
  };

  std::vector<Slot> slots_;
  mutable std::mutex mutex_;
  std::atomic<size_t> version_{0};

  // Initial capacity for the slots vector
  static constexpr size_t initial_capacity = 8;
  size_t capacity_ = initial_capacity;

public:
  class Connection {
    std::weak_ptr<std::atomic<bool>> weak_valid_;
    friend class Signal;
    explicit Connection(std::shared_ptr<std::atomic<bool>> valid)
        : weak_valid_(valid) {}

  public:
    Connection() = default;
    ~Connection() { disconnect(); }

    void disconnect() const {
      if (auto valid = weak_valid_.lock()) {
        valid->store(false, std::memory_order_release);
      }
    }
  };

  /**
   * @brief Connect a callback function to the signal.
   * @param callback The callback function to be called when the signal is
   * emitted.
   * @return A Connection object that can be used to disconnect the slot.
   */
  Connection connect(std::function<void(Args...)> callback) {
    std::lock_guard lock(mutex_);

    // Auto expand the slots vector if needed
    if (slots_.size() >= capacity_) {
      capacity_ *= 2;
      slots_.reserve(capacity_);
    }

    auto valid = std::make_shared<std::atomic<bool>>(true);
    slots_.emplace_back(Slot{std::move(callback), valid});
    version_.fetch_add(1, std::memory_order_relaxed);
    return Connection(valid);
  }

  void emit(Args... args) {
    // Read the current version atomically
    // Fast path: if no slots are connected, return early
    if (const size_t current_version = version_.load(std::memory_order_acquire);
        current_version == 0)
      return;

    // Two phases:
    // 1. Fast path: call all valid slots
    // 2. Cleanup: remove invalid slots
    bool need_cleanup = false;

    for (const auto &slot : slots_) {
      if (slot.valid->load(std::memory_order_acquire)) {
        slot.callback(args...);
      } else {
        need_cleanup = true;
      }
    }

    // Cleanup phase: remove invalid slots if needed
    if (need_cleanup) {
      std::lock_guard lock(mutex_);
      auto new_end =
          std::remove_if(slots_.begin(), slots_.end(), [](const Slot &s) {
            return !s.valid->load(std::memory_order_relaxed);
          });
      slots_.erase(new_end, slots_.end());
      version_.store(slots_.size(), std::memory_order_release);

      // Shrink the slots vector if it is more than half empty
      if (slots_.capacity() > capacity_ && slots_.size() < capacity_ / 2) {
        slots_.shrink_to_fit();
        capacity_ = slots_.capacity();
      }
    }
  }

  size_t slot_count() const {
    std::lock_guard lock(mutex_);
    return slots_.size();
  }
};

/**
 * @brief The CRTP base class for updatable objects.
 * @tparam Impl The implementation type.
 */
template <typename Impl> class Updatable {
public:
  /**
   * @brief Update the object and emit the update signal.
   */
  void update() {
    static_cast<Impl *>(this)->updateImpl();
    update_signal_.emit(static_cast<Impl &>(*this));
  }

protected:
  Signal<Impl &> update_signal_;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_SIGNALSLOT_H
