#include "volterra.hpp"

#include <cmath>
#include <stdexcept>

namespace pf {

void validatePositive(const std::vector<std::pair<std::string, double>>& items)
{
  for (const auto& item : items) {
    if (item.second <= 0) {
      throw std::invalid_argument(
          "[Error] Value '" + item.first
          + "' is invalid. All parameters must be positive.");
    }
  }
}

// --- Simulation private methods ---

void Simulation::validateParameters(const Parameters& params) const
{
  validatePositive(
      {{"a", params.a}, {"b", params.b}, {"c", params.c}, {"d", params.d}});
}

void Simulation::validateInitialConditions(const SpeciesCount& count) const
{
  validatePositive({{"Preys", count.preys}, {"Predators", count.preds}});
}

void Simulation::validateDt(const double dt) const
{
  validatePositive({{"dt", dt}});
}

SpeciesCount const& Simulation::getLastRelCount() const
{
  if (rel_counts_.empty()) {
    throw std::runtime_error(
        "[Error] No points available."); // Safety check: data should always
                                         // exist, but just in case
  }
  return rel_counts_.back();
}

SpeciesCount Simulation::toAbs(SpeciesCount const& rel_count) const
{
  SpeciesState abs_count;
  abs_count.preys = rel_count.preys * params_.d / params_.c;
  abs_count.preds = rel_count.preds * params_.a / params_.b;
  return abs_count;
}

SpeciesCount Simulation::toRel(SpeciesCount const& abs_count) const
{
  SpeciesState rel_count;
  rel_count.preys = abs_count.preys * params_.c / params_.d;
  rel_count.preds = abs_count.preds * params_.b / params_.a;
  return rel_count;
}

double Simulation::computeH(const SpeciesCount& abs_count) const
{
  return -params_.d * std::log(abs_count.preys) + params_.c * abs_count.preys
       + params_.b * abs_count.preds - params_.a * std::log(abs_count.preds);
}

void Simulation::evolve()
{
  SpeciesCount new_count;
  const SpeciesCount& last_count = getLastRelCount();

  new_count.preys = last_count.preys
                  + params_.a * (1 - last_count.preds) * last_count.preys * dt_;
  new_count.preds = last_count.preds
                  + params_.d * (last_count.preys - 1) * last_count.preds * dt_;

  if (new_count.preys <= 0 || new_count.preds <= 0) {
    throw std::logic_error("Model produced non-positive population: this "
                           "should not happen."); // Sanity check
  }

  rel_counts_.push_back(new_count);
}

// --- Simulation public methods ---

Simulation::Simulation(SpeciesCount const& initial_abs_count, double const a,
                       double const b, double const c, double const d,
                       double const dt)
    : params_{a, b, c, d}
    , dt_{dt}
{
  validateParameters(params_);
  validateInitialConditions(initial_abs_count);
  validateDt(dt);
  rel_counts_.push_back(toRel(initial_abs_count));
}

Simulation::Simulation(SpeciesCount const& initial_abs_count,
                       Parameters const& params, double const dt)
    : params_{params}
    , dt_{dt}
{
  validateParameters(params);
  validateInitialConditions(initial_abs_count);
  validateDt(dt);
  rel_counts_.push_back(toRel(initial_abs_count));
}

std::pair<int, double> Simulation::run(double T)
{
  if (T <= 0) {
    throw std::invalid_argument("Duration must be a positive number.");
  }

  int steps         = static_cast<int>(std::ceil(T / dt_));
  double adjusted_T = steps * dt_;

  for (int i = 0; i < steps; ++i) {
    evolve();
  }

  return {steps, adjusted_T};
}

std::vector<SpeciesState> Simulation::getAbsStates() const
{
  std::vector<SpeciesState> result;
  double time = 0.0;
  for (const SpeciesCount& rel_count : rel_counts_) {
    SpeciesCount abs_count = toAbs(rel_count);
    SpeciesState abs_state{abs_count, computeH(abs_count), time};
    time += dt_;

    result.push_back(abs_state);
  }
  return result;
}

std::vector<SpeciesCount> Simulation::getRelCounts() const
{
  return rel_counts_;
}

double Simulation::getDt() const
{
  return dt_;
}

std::size_t Simulation::numSteps() const
{
  return rel_counts_.size();
}

} // namespace pf
