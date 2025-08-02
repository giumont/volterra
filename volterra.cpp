#include "volterra.hpp"

#include <cmath>
#include <stdexcept>

namespace pf {

std::size_t Simulation::size() const
{
  return rel_points_
      .size(); // ha senso che usi rel_points_ per una cosa outputtata?
}

Point const& Simulation::get_last() const
{
  if (rel_points_.empty()) { // prob inutile visto che viene riempito gia dal
                             // costruttore: ha senso tenere questo metodo?
    throw std::runtime_error("No points available.");
  }
  return rel_points_.back();
}

Point Simulation::to_abs(Point const& rel_point) const
{
  State abs_point;

  abs_point.x = rel_point.x * d_ / c_;
  abs_point.y = rel_point.y * a_ / b_;

  return abs_point;
}

Point Simulation::to_rel(Point const& abs_point) const
{
  State rel_point;

  rel_point.x = abs_point.x * c_ / d_;
  rel_point.y = abs_point.y * b_ / a_;

  return rel_point;
}

double Simulation::compute_H(const Point& abs_point) const
{
  return -d_ * std::log(abs_point.x) + c_ * abs_point.x + b_ * abs_point.y
       - a_ * std::log(abs_point.y);
}

double Simulation::get_dt() const
{
  return dt_;
}

Simulation::Simulation(Point const initial_abs_point, double a, double b,
                       double c, double d, double dt)
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
  rel_points_.push_back(
      to_rel(initial_abs_point)); // va bene che il costruttore faccia questo?
}

void Simulation::evolve()
{
  Point new_point;
  Point const& last_point = get_last();

  new_point.x = last_point.x + a_ * (1 - last_point.y) * last_point.x * dt_;
  new_point.y = last_point.y + d_ * (last_point.x - 1) * last_point.y * dt_;

  if (new_point.x <= 0 || new_point.y <= 0) {
    throw std::logic_error(
        "Model produced non-positive population: this should not happen.");
  }

  rel_points_.push_back(new_point);
}

std::pair<int, double> Simulation::run(double duration)
{
  if (duration <= 0) {
    throw std::invalid_argument("Duration must be a positive number.");
  }

  int steps = static_cast<int>(std::ceil(duration / dt_));
  double adjusted_duration =
      steps * dt_; // conversione implicita di steps a double: è legale

  for (int i = 0; i < steps; ++i) {
    evolve();
  }

  return {steps, adjusted_duration};
}

std::vector<State> Simulation::get_abs_states() const
{
  std::vector<State> result;
  double time = 0.0;
  for (const Point& rel_point : rel_points_) {
    Point abs_point = to_abs(rel_point);
    State abs_state{abs_point, compute_H(abs_point), time};
    time += dt_;

    result.push_back(abs_state);
  }
  return result;
}

std::vector<double> get_x_series() 
{
  std::vector<double> result;
  std::vector<State> abs_states = get_abs_states();
  for (const State& abs_state : abs_states) {
    result.push_back(abs_state.x);
  }
  return result;
}
// std::vector<double> get_y_series() const;
// std::vector<double> get_H_series() const;
// std::vector<double> get_time_series() const;

std::vector<Point> Simulation::get_rel_points() const
{
  return rel_points_;
}

} // namespace pf
