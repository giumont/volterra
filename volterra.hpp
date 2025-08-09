#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

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
  double a, b, c, d;
};

void validatePositive(const std::vector<std::pair<std::string, double>>& items);

class Simulation
{
 private:
  Parameters const params_;
  double const dt_;

  std::vector<SpeciesCount> rel_counts_; // salvati in valori relativi, SOLO
                                         // punti

  void validateParameters(const Parameters& params) const;

  void validateInitialConditions(const SpeciesCount& count) const;

  void validateDt(const double dt) const;

  // void Simulation::validateConstructor(SpeciesCount const& initial_abs_count,
  //                                      Parameters const& params,
  //                                      double const dt) const;

  SpeciesCount const& getLast() const; // recupera l'ultimo di rel_counts()_
                                       // per usarlo in evolve(): è relativo

  SpeciesCount toAbs(
      SpeciesCount const&) const; // restituisce i valori numerici assoluti di
                                  // uno SpeciesState per usarlo in get_states()
  SpeciesCount toRel(SpeciesCount const&) const;

  double computeH(const SpeciesCount& abs_count) const;

  void evolve(); // aggiunge SpeciesCount a rel_counts_ (in relativi)

 public:
  Simulation(SpeciesCount const& initial_abs_count, double const a,
             double const b, double const c, double const d, double const dt);

  // Simulation(SpeciesCount const initial_abs_count,
  //            std::array<double, 4> const params, double dt);

  Simulation(SpeciesCount const& initial_abs_count = {10.0, 5.0},
             Parameters const& params              = {1.0, 0.1, 0.1, 1.0},
             double const dt                       = 0.001);

  Simulation& operator=(const Simulation&) = default;

  std::pair<int, double>
  run(double T); // metodo pubblico per lanciare simulazione

  std::vector<SpeciesState>
  getAbsStates() const; // restituisce gli stati in valori assoluti

  std::vector<SpeciesCount> getRelPoints() const;

  template<typename MemberPtr>
  std::vector<double> getSeries(const std::vector<SpeciesState>& state_vector, MemberPtr member) const
  {
    std::vector<double> result;
    for (const SpeciesState& state : state_vector) {
      result.push_back(state.*member);
    }
    return result;
  }

  double getDt() const;

  std::size_t numSteps() const;
};
} // namespace pf

#endif