#include "volterra.hpp"

#include <stdexcept>

namespace pf {

int Simulation::size() const
{
  return states_.size();
}

State Simulation::last()
{
  if (states_.empty()) {
    throw std::runtime_error("No states available");
  }
  return states_.back();
}

std::vector<State> Simulation::abs()
{
  auto const N = size();
  std::vector<State> abs_states;
  for (const& auto rel_state : states_) {
    State abs_state;

    abs_state.x = rel_state.x * d / c;
    abs_state.y = rel_state.y * a / b;
    abs_state.h = rel_state.h;

    abs_states.push_back(abs_state);
  }

  return abs_states;
}

std::vector<State> Simulation::states() const
{
  auto const abs_states = abs();

  // manca parte di output
  return abs_states;
}

void Simulation::evolve() const
{
  State const& last_state = last();
  State new_state;

  new_state.x = last_state.x + a * (1 - last_state.y) * last_state.x * dt;
  new_state.y = last_state.y + d * (last_state.x - 1) * last_state.y * dt;
  new_state.h = last_state.h; // da cambiare???

  states_.push_back(new_state);
}
} // namespace pf
