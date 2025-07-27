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
  std::vector<State> states_; //salvati in valori relativi

 private:
  const double a;
  const double b;
  const double c;
  const double d;

  const double dt;

  State last(); // recupera l'ultimo di states_ per usarlo in evolve()

  State abs(); // restituisce i valori numerici assoluti dei valori di States
               // per usarlo in states()

 public:
  int size() const;

  void evolve(); // aggiunge State a states_ (in relativi)

  std::vector<State> states() const; // restituisce gli stati in valori assoluti
};
} // namespace pf

#endif