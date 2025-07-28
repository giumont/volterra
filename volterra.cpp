#include "volterra.hpp"

#include <stdexcept>

namespace pf {

auto Simulation::size() const
{
  return states_.size();
}

State const& Simulation::get_last() const
{
  if (states_.empty()) {
    throw std::runtime_error("No states available");
  }
  return states_.back();
}

State Simulation::to_abs(State const& rel_state)
{
  State abs_state;

  abs_state.x = rel_state.x * d_ / c_;
  abs_state.y = rel_state.y * a_ / b_;
  abs_state.h = rel_state.h;

  return abs_state;
}

State Simulation::to_rel(State const& abs_state)
{
  State rel_state;

  rel_state.x = abs_state.x * c_ / d_;
  rel_state.y = abs_state.y * b_ / a_;
  rel_state.h = abs_state.h;

  return rel_state;
}

std::vector<State> Simulation::states() const
{
  return states_;
}

void Simulation::evolve()
{
  State rel_new_state;

  State const& abs_last_state = get_last();
  State rel_last_state        = to_rel(abs_last_state);

  rel_new_state.x =
      rel_last_state.x + a_ * (1 - rel_last_state.y) * rel_last_state.x * dt_;
  rel_new_state.y =
      rel_last_state.y + d_ * (rel_last_state.x - 1) * rel_last_state.y * dt_;
  rel_new_state.h = rel_last_state.h; // da cambiare???

  State abs_new_state = to_abs(rel_new_state);

  states_.push_back(abs_new_state);
}
} // namespace pf
