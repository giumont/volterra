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
  std::vector<Point> rel_points_; // salvati in valori relativi, SOLO punti

 private:
  const double a_, b_, c_, d_;
  const double dt_;

  // const double eq_state

  Point const& get_last() const; // recupera l'ultimo di rel_points()_ per
                                 // usarlo in evolve(): è relativo

  Point to_abs(Point const&) const; // restituisce i valori numerici assoluti di
                                    // uno State per usarlo in get_states()
  Point to_rel(Point const&) const;

  double compute_H(const Point& abs_point) const;

  void evolve(); // aggiunge Point a rel_points_ (in relativi)

 public:
  Simulation(const Point& initial_abs_point = {1, 1}, double a = 1,
             double b = 1, double c = 1, double d = 1, double dt = 0.001);

  std::pair<int, double>
  run(double duration); // metodo pubblico per lanciare simulazione

  std::vector<State>
  get_states() const; // restituisce gli stati in valori assoluti

  double get_dt() const;
  
  int size() const;

};
} // namespace pf

#endif