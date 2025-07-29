#include "volterra.hpp"

#include <cmath>
#include <stdexcept>

namespace pf {

auto Simulation::size() const
{
  return states_.size();
}

Point const& Simulation::get_last() const
{
  if (rel_points_.empty()) { // prob inutile visto che viene riempito gia dal
                             // costruttore: ha senso tenere questo metodo?
    throw std::runtime_error("No points available");
  }
  return rel_points_.back();
}

Point Simulation::to_abs(Point const& rel_point) const
{
  State abs_point;

  abs_point.x = rel_point.x * d_ / c_;
  abs_point.y = rel_point.y * a_ / b_;
  abs_point.H = rel_point.H;

  return abs_point;
}

Point Simulation::to_rel(Point const& abs_point) const
{
  State rel_point;

  rel_point.x = abs_point.x * c_ / d_;
  rel_point.y = abs_point.y * b_ / a_;
  rel_point.H = abs_point.H;

  return rel_point;
}

double Simulation::compute_H(const Point& abs_point) const
{
  return -d_ * std::log(abs_point.x) + c_ * abs_point.x + b_ * abs_point.y
       - a_ * std::log(abs_point.y);
}

double Simulation::get_dt() const // va tenuto??
{
  return dt_;
}

Simulation::Simulation(const Point& initial_abs_point = {1, 1}, double a = 1,
                       double b = 1, double c = 1, double d = 1,
                       double dt = 0.001)
    : a_{a}
    , b_{b}
    , c_{c}
    , d_{d}
    , dt_{dt}
{
  if (a <= 0 || b <= 0 || c <= 0 || d <= 0 || initial_abs_point.x <= 0
      || initial_abs_point.y <= 0 || dt <= 0) {
    throw std::invalid_argument(
        "All parameters must be positive."); // l'errore deve emergere qui
                                             // oppure all'inserimento da
                                             // terminale???
  }
  rel_points_.push_back(
      to_rel(initial_abs_point)); // va bene che il costruttore faccia questo?
}

void Simulation::evolve()
{
  State new_point;
  State const& last_point = get_last();

  new_point.x = last_point.x + a_ * (1 - last_point.y) * last_point.x * dt_;
  new_point.y = last_point.y + d_ * (last_point.x - 1) * last_point.y * dt_;

  rel_points_.push_back(new_point);
}

void Simulation::run(double duration)
{
  int steps                = static_cast<int>(std::ceil(duration / dt_));
  double adjusted_duration = steps * dt_;
  if (adjusted_duration > duration) {
    std::cout << "Notice: the entered duration ()" << duration << ") was rounded up to the nearest multiple of dt ()"
              << adjusted_duration
              << ").\n"; // va specificato il perché o "espongo" troppo?
  }
  for (int i = 0; i < steps; i++) {
    evolve();
  }
}

std::vector<State> Simulation::get_states() const
{
  std::vector<State> result;
  for (auto const& rel_point : rel_points_) {
    Point abs_point = to_abs(rel_point);
    State abs_state{abs_point, compute_H(abs_point)};

    result.push_back(abs_state);
  }
  return result;
}
} // namespace pf
