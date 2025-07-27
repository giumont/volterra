#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include <vector>

namespace pf {
struct State
{
  double x;
  double y;
  double H;
};

class Simulation
{
  std::vector<State> states_;

 public:
  State evolve() const;

  const std::vector<State> states() const; // metodo per printare dati
                                           // evoluzione
}
} // namespace pf

#endif