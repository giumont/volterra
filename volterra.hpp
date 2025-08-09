#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include <array>
#include <string>
#include <vector>

namespace pf {
struct SpeciesCount
{
  double preys;
  double predators;
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

class Simulation
{
 private:
  Parameters params_;
  double dt_;

  std::vector<SpeciesCount> rel_counts_; // salvati in valori relativi, SOLO
                                         // punti

  static void
  validatePositive(const std::vector<std::pair<std::string, double>>& items);

  void validateParameters(const Parameters& params);

  void validateInitialConditions(const SpeciesCount& count);

  void validateDt(const double dt);

  SpeciesCount const& getLast() const; // recupera l'ultimo di rel_counts()_ per
                                       // usarlo in evolve(): è relativo

  SpeciesCount toAbs(
      SpeciesCount const&) const; // restituisce i valori numerici assoluti di
                                  // uno SpeciesState per usarlo in get_states()
  SpeciesCount toRel(SpeciesCount const&) const;

  double computeH(const SpeciesCount& abs_count) const;

  void evolve(); // aggiunge SpeciesCount a rel_counts_ (in relativi)

 public:
  Simulation(SpeciesCount const& initial_abs_count, double const a,
             double const b, double const c, double const d,
             double const dt);

  // Simulation(SpeciesCount const initial_abs_count,
  //            std::array<double, 4> const params, double dt);

  Simulation(SpeciesCount const& initial_abs_count = {10.0, 5.0},
             Parameters const& params              = {1.0, 0.1, 0.1, 1.0},
             double const dt                      = 0.001);

  Simulation& operator=(const Simulation&) = default;

  std::pair<int, double>
  runSimulation(double T); // metodo pubblico per lanciare simulazione

  std::vector<SpeciesState>
  getAbsStates() const; // restituisce gli stati in valori assoluti

  std::vector<SpeciesCount> getRelPoints() const;

  std::vector<double> getXSeries() const;
  std::vector<double> getYSeries() const;
  std::vector<double> getHSeries() const;
  std::vector<double> getTimeSeries() const;

  double getDt() const;

  std::size_t numSteps() const;
};
} // namespace pf

#endif