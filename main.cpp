
#include <cstdlib>
#include <exception>
#include <iostream>

#include "volterra.hpp"

void execute_simulation(pf::Simulation& sim)
{
  double duration;
  std::cout << "\n\nInsert time duration for the simulation: ";
  std::cin >> duration;

  auto [steps, adjusted_duration] = sim.run(duration);

  if (adjusted_duration > duration) {
    std::cout << "Notice: duration (" << duration
              << ") is not a multiple of the time step dt (" << sim.get_dt()
              << "). Rounded up to " << adjusted_duration << ".\n";
  }

  std::cout << "Simulation succesfully ended.\n";

  sim.get_states();
}

int main()
{
  std::string feedback;

  try {
    std::cout << "Run simulation with default values? y/n\n";
    std::cin >> feedback;
    if (feedback == "y" || feedback == "Y") {
      pf::Simulation sim;

      execute_simulation(sim);
      return 0;
    } else {
      double a, b, c, d;
      double x, y;
      std::cout
          << "Insert parameters for ode.\n Insert parameter a (default: 1): ";
      std::cin >> a;
      std::cout << "\nInsert parameter b (default: 1): ";
      std::cin >> b;
      std::cout << "\nInsert parameter c (default: 1): ";
      std::cin >> c;
      std::cout << "\nInsert parameter d (default: 1): ";
      std::cin >> d;
      std::cout << "\n\nInsert initial number of preys (default: 1): ";
      std::cin >> x;
      std::cout << "\nInsert initial number of predators (default: 1): ";
      std::cin >> y;

      pf::Point initial_abs_point{x, y};
      pf::Simulation sim{initial_abs_point, a, b, c, d};

      execute_simulation(sim);
      return 0;
    }
  } catch (std::exception const& e) {
    std::cerr << "Caught exception: '" << e.what() << "'\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Caught unknown exception\n";
    return EXIT_FAILURE;
  }
}
