#include "graph_renderer.hpp"
#include "utils.hpp"
#include "volterra.hpp"

#include <cstdlib> // for EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>
#include <stdexcept>
#include <string>

pf::Simulation setSimValues()
{
  double a = pf::askInput<double>("a (default 1): ");
  double b = pf::askInput<double>("b (default 0.1): ");
  double c = pf::askInput<double>("c (default 0.1): ");
  double d = pf::askInput<double>("d (default 1): ");

  double x = pf::askInput<double>("Initial number of preys (default 10): ");
  double y = pf::askInput<double>("Initial number of predators (default 5): ");

  return pf::Simulation{{x, y}, {a, b, c, d}};
}

void runSimulation(pf::Simulation& sim)
{
  constexpr double T_max = 1000.0;

  double T = pf::askInput<double>("\n\nInsert duration T for the simulation: ");

  if (T > T_max) {
    throw std::invalid_argument{"Simulation duration T (" + std::to_string(T)
                                + ") exceeds the maximum allowed ("
                                + std::to_string(T_max)
                                + ").\nPlease insert a smaller value.\n\n"};
  }

  auto execResult = executeSim(sim, T);
  if (execResult.hasWarning) {
    std::cout << "[Warning] " << execResult.message;
  }

  std::cout << "[Info] Simulation successfully executed.\n";

  writeOnFile(sim);
  std::cout << "\n[Info] Results written to file.\n";

  visualizeResult(sim);
  std::cout << "\n[Info] Visualization completed.\n";
}

int main()
{
  std::cout << "\n====================================\n";
  std::cout << "Volterra Predator-Prey Simulator\n";
  std::cout << "====================================\n\n";

  try {
    std::string feedback =
        pf::askInput<std::string>("Run simulation with default values? [y/n]\n"
                                  "Press any other key to exit the program.\n");

    if (feedback == "y" || feedback == "Y") {
      pf::Simulation sim; // default constructor
      std::cout << "\n[Info] Default simulation values set.\n";
      runSimulation(sim);

    } else if (feedback == "n" || feedback == "N") {
      pf::Simulation sim = setSimValues();
      std::cout << "\n[Info] Custom simulation parameters set.\n";
      runSimulation(sim);

    } else {
      std::cerr << "\nExiting program.\n";
      return EXIT_FAILURE;
    }

  } catch (const std::exception& e) {
    std::cerr << "[Error] " << e.what() << "\nExiting program.\n";
    return EXIT_FAILURE;

  } catch (...) {
    std::cerr << "[Error] Unknown error occurred. Exiting program.\n";
    return EXIT_FAILURE;
  }

  std::cout << "\nExiting program.\n";
  return EXIT_SUCCESS;
}
