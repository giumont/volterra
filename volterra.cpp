#include "volterra.hpp"

#include <cmath>
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
  abs_state.H = rel_state.H;

  return abs_state;
}

State Simulation::to_rel(State const& abs_state) const
{
  State rel_state;

  rel_state.x = abs_state.x * c_ / d_;
  rel_state.y = abs_state.y * b_ / a_;
  rel_state.H = abs_state.H;

  return rel_state;
}

double Simulation::compute_H(double x, double y) const
{
  return -d_ * std::log(x) + c_ * x + b_ * y - a_ * std::log(y);
}

double Simulation::get_dt() const { return dt_; }

Simulation::Simulation(const Point& initial_abs_point = {1, 1}, double a = 1, double b = 1, double c = 1, double d = 1,
                       double dt                      = 0.001)
    : a_{a}
    , b_{b}
    , c_{c}
    , d_{d}
    , dt_{dt}
{
  if (a <= 0 || b <= 0 || c <= 0 || d <= 0 || initial_abs_point.x <= 0
      || initial_abs_point.y <= 0 || dt <= 0) {
    throw std::invalid_argument("All parameters must be positive.");
  }
  states_.push_back(to_rel(initial_abs_state)); //DA MODIFICARE
}

std::vector<State> Simulation::get_states() const // DA MODIFICARE
{
  std::vector<State> result;
  for (auto const& rel_state : states_) {
    State abs_state;

    abs_state.x = to_abs(rel_state).x;
    abs_state.y = to_abs(rel_state).y;
    abs_state.H = compute_H(abs_state.x, abs_state.y); // NO!

    result.push_back(abs_state);
  }
  return result;
}

void Simulation::evolve()
{
  State new_state;
  State const& last_state = get_last();

  new_state.x = last_state.x + a_ * (1 - last_state.y) * last_state.x * dt_;
  new_state.y = last_state.y + d_ * (last_state.x - 1) * last_state.y * dt_;
  new_state.H = compute_H(
      new_state.x,
      new_state.y); // questo va levato e qui devono essere tutti points

  states_.push_back(new_state);
}

void Simulation::run(double duration)
{
    if (std::fmod(duration/dt_) > 1e-9){
        throw std::invalid_argument("Duration must be an integer multiple of dt.\n dt =" << dt_);
    }
}
} // namespace pf
