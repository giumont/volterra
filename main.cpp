
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>

#include "volterra.hpp"

void execute_simulation(pf::Simulation& sim)
{
  double duration;
  std::cout << "\n\nInsert time duration for the simulation: ";
  std::cin >> duration;

  auto [steps, adjusted_duration] = sim.run_simulation(duration);

  if (steps == 1 && adjusted_duration >= duration) {
    std::cout << "Notice: duration (" << duration
              << ") is shorter than the time step dt (" << sim.get_dt()
              << "). One step will be executed anyway.\n";
  }

  else if (adjusted_duration > duration) {
    std::cout << "Notice: duration (" << duration
              << ") is not a multiple of the time step dt (" << sim.get_dt()
              << "). Rounded up to " << adjusted_duration << ".\n";
  }

  std::cout << "Simulation succesfully ended.\n";
}

void write_on_file(pf::Simulation sim)
{
  std::ofstream outfile{"results.txt"};

  if (!outfile) {
    throw std::runtime_error{"Impossible to open file!"};
  }

  auto total_states = sim.size();

  outfile << "- Total states: " << total_states << '\n';

  for (auto const& state : sim.get_abs_states()) {
    outfile << "Time: " << state.t << '\t';
    outfile << "Preys: " << state.x << '\t';
    outfile << "Predators: " << state.y << '\t';
    outfile << "H: " << state.H << '\n';
  }

  std::cout << "Results wrote on file.\n";
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

      write_on_file(sim);
      return 0;
    } else {
      double a, b, c, d;
      double x, y;
      std::cout
          << "Insert parameters for ode.\n Insert parameter a (default: 1): ";
      std::cin >> a;
      std::cout << "\nInsert parameter b (default: 0.1): ";
      std::cin >> b;
      std::cout << "\nInsert parameter c (default: 0.1): ";
      std::cin >> c;
      std::cout << "\nInsert parameter d (default: 1): ";
      std::cin >> d;
      std::cout << "\n\nInsert initial number of preys (default: 10.0): ";
      std::cin >> x;
      std::cout << "\nInsert initial number of predators (default: 5.0): ";
      std::cin >> y;

      pf::Point initial_abs_point{x, y};
      pf::Simulation sim{initial_abs_point, a, b, c, d};

      execute_simulation(sim);

      write_on_file(sim);
      return 0;
    }
  } catch (std::exception const& e) {
    std::cerr << "Caught exception: '" << e.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Caught unknown exception\n";
    return EXIT_FAILURE;
  }
}
