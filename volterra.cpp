#include "volterra.hpp"

#include <cmath>
#include <iostream>
#include <random> // for random gen methods
#include <stdexcept>

namespace pf {

void validatePositive(const std::vector<std::pair<std::string, double>>& items)
{
  for (const auto& item : items) {
    if (item.second <= 0) {
      throw std::invalid_argument(
          "Value '" + item.first
          + "' is invalid. All parameters must be positive.");
    }
  }
}

Parameters randomParams(const double min, const double max)
{
  if (min >= max) {
    throw std::invalid_argument("Invalid random parameter bounds");
  }

  // Random engine seeded with current time
  static std::random_device rd;
  static std::default_random_engine gen(rd());

  std::uniform_real_distribution<double> param_dist(min, max);

  double a = param_dist(gen);
  double b = param_dist(gen);
  double c = param_dist(gen);
  double d = param_dist(gen);

  Parameters params{a, b, c, d};

  return params;
}

SpeciesCount randomInitialConditions(const double min, const double max)
{
  if (min >= max) {
    throw std::invalid_argument("Invalid random initial conditions bounds");
  }

  // Random engine seeded with current time
  static std::random_device rd;
  static std::default_random_engine gen(rd());

  std::uniform_real_distribution<double> init_cond_dist(min, max);

  double preys = init_cond_dist(gen);
  double preds = init_cond_dist(gen);

  SpeciesCount init_cond{preys, preds};

  return init_cond;
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
        "[Error] No points available."); // Safety check (data should always
                                         // exist)
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

SpeciesCount Simulation::getInitConditions() const
{
  return toAbs(rel_counts_.front());
}

double Simulation::getDt() const
{
  return dt_;
}

Parameters Simulation::getParams() const
{
  return params_;
}

std::size_t Simulation::numSteps() const
{
  return rel_counts_.size();
}

std::pair<int, double> Simulation::run(double duration)
{
  if (duration <= 0) {
    throw std::invalid_argument("Duration must be a positive number.");
  }

  int steps         = static_cast<int>(std::ceil(duration / dt_));
  double adjusted_T = steps * dt_;

  for (int i = 0; i < steps; ++i) {
    evolve();
  }

  return {steps, adjusted_T};
}

} // namespace pf
