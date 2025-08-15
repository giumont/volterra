#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include "constants.hpp"

#include <array>
#include <string>
#include <vector>

namespace pf {

struct SpeciesCount
{
  double preys;
  double preds;
};

struct SpeciesState : public SpeciesCount
{
  double H;
  double t;
};

struct Parameters
{
  double a;
  double b;
  double c;
  double d;
};

void validatePositive(const std::vector<std::pair<std::string, double>>& items);

class Simulation
{
 private:
  Parameters const params_;
  double const dt_;
  std::vector<SpeciesCount> rel_counts_;

  void validateParameters(const Parameters& params) const;
  void validateInitialConditions(const SpeciesCount& count) const;
  void validateDt(double dt) const;

  SpeciesCount const& getLastRelCount() const;

  SpeciesCount toAbs(SpeciesCount const&) const;
  SpeciesCount toRel(SpeciesCount const&) const;

  double computeH(const SpeciesCount& abs_count) const;

  void evolve();

 public:
  Simulation(SpeciesCount const& initial_abs_count, double a, double b,
             double c, double d, double dt);

  Simulation(SpeciesCount const& initial_abs_count = {def_initial_preys, def_initial_preds},
             Parameters const& params              = {def_a, def_b, def_c, def_d},
             double dt                             = def_dt);

  Simulation& operator=(const Simulation&) = default;

  std::vector<SpeciesState> getAbsStates() const;
  std::vector<SpeciesCount> getRelCounts() const;

  double getDt() const;
  Parameters getParams() const;
  std::size_t numSteps() const;

  std::pair<int, double> run(double duration);

  template<typename MemberPtr>
  std::vector<double> getSeries(const std::vector<SpeciesState>& state_vector,
                                MemberPtr member) const
  {
    std::vector<double> result;
    for (const SpeciesState& state : state_vector)
      result.push_back(state.*member);
    return result;
  }
};

} // namespace pf

#endif // PF_VOLTERRA_HPP
