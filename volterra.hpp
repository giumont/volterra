#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include <vector>

namespace pf {
struct Point
{
  double x;
  double y;
};

struct State : public Point
{
  double H;
};

class Simulation
{
  std::vector<State> states_; // salvati in valori relativi

 private:
  const double a_, b_, c_, d_;
  const double dt_;

  // const double eq_state

  State const& get_last()
      const; // recupera l'ultimo di states_ per usarlo in evolve(): è relativo

  State to_abs(State const&) const; // restituisce i valori numerici assoluti di
                                    // uno State per usarlo in get_states()
  State to_rel(State const&) const;

  auto size() const; // puo aver senso renderlo pubblico?

  double compute_H(double x, double y) const;

  void evolve(); // aggiunge State a states_ (in relativi)

 public:
  Simulation(double a, double b, double c, double d,
             const State& initial_abs_state, double dt);

  void run(double duration); // metodo pubblico per lanciare simulazione

  std::vector<State>
  get_states() const; // restituisce gli stati in valori assoluti

  double get_dt() const;
};
} // namespace pf

#endif