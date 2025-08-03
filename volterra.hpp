#ifndef PF_VOLTERRA_HPP
#define PF_VOLTERRA_HPP

#include <array>
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
  double t;
};

// inline constexpr std::array<double, 4> def_params {1.0, 0.1, 0.1, 1.0};

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
  Simulation(Point const initial_abs_point = {10.0, 5.0}, double a = 1.0,
             double b = 0.1, double c = 0.1, double d = 1.0, double dt = 0.001);

  Simulation(Point const initial_abs_point, std::array<double, 4> const params,
             double dt);

  Simulation& operator=(const Simulation&) = default;

  std::pair<int, double>
  run_simulation(double duration); // metodo pubblico per lanciare simulazione

  std::vector<State>
  get_abs_states() const; // restituisce gli stati in valori assoluti

  std::vector<Point> get_rel_points() const;

  std::vector<double> get_x_series() const;
  std::vector<double> get_y_series() const;
  std::vector<double> get_H_series() const;
  std::vector<double> get_time_series() const;

  double get_dt() const;

  std::size_t num_steps() const;
};
} // namespace pf

#endif