#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include <vector>

namespace pf {
struct State
{
  double x;
  double y;
  double h;
};

class Simulation
{
  std::vector<State> states_; // salvati in valori relativi

 private:
  const double a_;
  const double b_;
  const double c_;
  const double d_;

  const double dt_;

  State const&
  get_last() const; // recupera l'ultimo di states_ per usarlo in evolve()

  State to_abs(State const&) const; // restituisce i valori numerici assoluti di uno
                              // State per usarlo in states()

 public:
  Simulation(double a, double b, double c, double d, double dt)
      : a_{a}
      , b_{b}
      , c_{c}
      , d_{d}
      , dt_{dt}
  {}

  auto size() const;

  void evolve(); // aggiunge State a states_ (in relativi)

  std::vector<State> states() const; // restituisce gli stati in valori assoluti
};
} // namespace pf

#endif