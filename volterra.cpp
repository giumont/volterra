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

State Simulation::to_abs(State const& rel_state) const
{
  State abs_state;

  abs_state.x = rel_state.x * d_ / c_;
  abs_state.y = rel_state.y * a_ / b_;
  abs_state.h = rel_state.h;

  return abs_state;
}

std::vector<State> Simulation::states() const
{
  std::vector<State> result;
  for (auto const& rel_state : states_) {
    result.push_back(to_abs(rel_state));
  }
  return result;
}

void Simulation::evolve()
{
  State new_state;
  State const& last_state = get_last();

  new_state.x = last_state.x + a_ * (1 - last_state.y) * last_state.x * dt_;
  new_state.y = last_state.y + d_ * (last_state.x - 1) * last_state.y * dt_;
  new_state.h = last_state.h; // da cambiare???

  states_.push_back(new_state);
}
} // namespace pf
