#include "volterra.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace pf {

void validateParameters(const Parameters& params)
{
  std::array<double, 7> values = {params.a,
                                  params.b,
                                  params.c,
                                  params.d,
                                  params.initial_preys,
                                  params.initial_predators,
                                  params.dt};
  if (std::any_of(values.begin(), values.end(),
                  [](double x) { return x <= 0; })) {
    throw std::invalid_argument(
        "[Error] All parameters and initial conditions must be positive.");
  }
}

std::size_t Simulation::numSteps() const
{
  return rel_counts_.size(); // Number of simulation steps performed (equals
                             // number of stored relative points)
}

SpeciesCount const& Simulation::getLast() const
{
  if (rel_counts_.empty()) { // prob inutile visto che viene riempito gia dal
                             // costruttore: ha senso tenere questo metodo?
    throw std::runtime_error("[Error] No points available.");
  }
  return rel_counts_.back();
}

SpeciesCount Simulation::toAbs(SpeciesCount const& rel_count) const
{
  SpeciesState abs_count;

  abs_count.preys     = rel_count.preys * d_ / c_;
  abs_count.predators = rel_count.predators * a_ / b_;

  return abs_count;
}

SpeciesCount Simulation::toRel(SpeciesCount const& abs_count) const
{
  SpeciesState rel_count;

  rel_count.preys     = abs_count.preys * c_ / d_;
  rel_count.predators = abs_count.predators * b_ / a_;

  return rel_count;
}

double Simulation::computeH(const SpeciesCount& abs_count) const
{
  return -d_ * std::log(abs_count.preys) + c_ * abs_count.preys
       + b_ * abs_count.predators - a_ * std::log(abs_count.predators);
}

double Simulation::getDt() const
{
  return dt_;
}

Simulation::Simulation(SpeciesCount const initial_abs_count, double a, double b,
                       double c, double d, double dt)
    : a_{a}
    , b_{b}
    , c_{c}
    , d_{d}
    , dt_{dt}
{
  rel_counts_.push_back(toRel(initial_abs_count));
}

Simulation::Simulation(SpeciesCount const initial_abs_count,
                       std::array<double, 4> const params, double dt)
    : a_{params[0]}
    , b_{params[1]}
    , c_{params[2]}
    , d_{params[3]}
    , dt_{dt}
{
  if (std::any_of(params.begin(), params.end(),
                  [](double p) { return p <= 0; })) {
    throw std::invalid_argument("[Error] All parameters must be positive.");
  }
  if (initial_abs_count.preys <= 0 || initial_abs_count.predators <= 0) {
    throw std::invalid_argument("[Error] Initial conditions must be positive.");
  }
  if (dt <= 0) {
    throw std::invalid_argument("[Error] dt must be a positive value.");
  }
  rel_counts_.push_back(toRel(initial_abs_count));
}

void Simulation::evolve()
{
  SpeciesCount new_count;
  SpeciesCount const& last_count = getLast();

  new_count.preys = last_count.preys
                  + a_ * (1 - last_count.predators) * last_count.preys * dt_;
  new_count.predators =
      last_count.predators
      + d_ * (last_count.preys - 1) * last_count.predators * dt_;

  if (new_count.preys <= 0 || new_count.predators <= 0) {
    throw std::logic_error("[Error] Model produced non-positive population: "
                           "this should not happen.");
  }

  rel_counts_.push_back(new_count);
}

std::pair<int, double> Simulation::runSimulation(double T)
{
  if (T <= 0) {
    throw std::invalid_argument("[Error] Duration must be a positive number.");
  }

  int steps = static_cast<int>(std::ceil(T / dt_));
  double adjusted_T =
      steps * dt_; // conversione implicita di steps a double: è legale

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

std::vector<double> Simulation::getXSeries() const
{
  std::vector<double> result;
  std::vector<SpeciesState> abs_states = getAbsStates();
  for (const SpeciesState& abs_state : abs_states) {
    result.push_back(abs_state.preys);
  }
  return result;
}
std::vector<double> Simulation::getYSeries() const
{
  std::vector<double> result;
  std::vector<SpeciesState> abs_states = getAbsStates();
  for (const SpeciesState& abs_state : abs_states) {
    result.push_back(abs_state.predators);
  }
  return result;
}
std::vector<double> Simulation::getHSeries() const
{
  std::vector<double> result;
  std::vector<SpeciesState> abs_states = getAbsStates();
  for (const SpeciesState& abs_state : abs_states) {
    result.push_back(abs_state.H);
  }
  return result;
}
std::vector<double> Simulation::getTimeSeries() const
{
  std::vector<double> result;
  std::vector<SpeciesState> abs_states = getAbsStates();
  for (const SpeciesState& abs_state : abs_states) {
    result.push_back(abs_state.t);
  }
  return result;
}

std::vector<SpeciesCount> Simulation::getRelPoints() const
{
  return rel_counts_;
}

} // namespace pf
