/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2017                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_ACTOR_REGISTRY_HPP
#define CAF_ACTOR_REGISTRY_HPP

#include <mutex>
#include <atomic>
#include <cstdint>
#include <condition_variable>

#include "caf/fwd.hpp"
#include "caf/gp_cache.hpp"
#include "caf/abstract_actor.hpp"

namespace caf {

/// A registry is used to associate actors to IDs or atoms (names). This
/// allows a middleman to lookup actor handles after receiving actor IDs
/// via the network and enables developers to use well-known names to
/// identify important actors independent from their ID at runtime.
/// Note that the registry does *not* contain all actors of an actor system.
/// The middleman registers actors as needed.
class actor_registry : public gp_cache<actor_id, strong_actor_ptr>,
                       public gp_cache<atom_value, strong_actor_ptr> {
  using actor_id_cache = gp_cache<actor_id, strong_actor_ptr>;
  using atom_value_cache = gp_cache<atom_value, strong_actor_ptr>;
public:
  friend class actor_system;

  ~actor_registry();

  /// Returns the local actor associated to `key`.
  strong_actor_ptr get(actor_id key) const;

  /// Associates a local actor with its ID.
  void put(actor_id key, strong_actor_ptr val);

  /// Removes an actor from this registry,
  /// leaving `reason` for future reference.
  void erase(actor_id key);

  /// Returns the actor associated with `key` or `invalid_actor`.
  strong_actor_ptr get(atom_value id) const;

  /// Associates given actor to `key`.
  void put(atom_value key, strong_actor_ptr val);

  /// Removes a name mapping.
  void erase(atom_value key);

  /// Increases running-actors-count by one.
  void inc_running();

  /// Decreases running-actors-count by one.
  void dec_running();

  /// Returns the number of currently running actors.
  size_t running() const;

  /// Blocks the caller until running-actors-count becomes `expected`
  /// (must be either 0 or 1).
  void await_running_count_equal(size_t expected) const;

  /// @private
  auto get_cache() -> decltype(gp_cache<atom_value, strong_actor_ptr>::get_cache()) {
    return atom_value_cache::get_cache();
  }

private:
  /// Starts this component.
  void start();

  /// Stops this component.
  void stop();

  actor_registry(actor_system& sys);

  std::atomic<size_t> running_;
  mutable std::mutex running_mtx_;
  mutable std::condition_variable running_cv_;

  actor_system& system_;
};

} // namespace caf

#endif // CAF_ACTOR_REGISTRY_HPP
