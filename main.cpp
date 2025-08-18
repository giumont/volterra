#include "simulation_opt.hpp"
#include "graph_renderer.hpp"
#include "utils.hpp"
#include "volterra.hpp"

#include <cstdlib> // for EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>
#include <stdexcept>
#include <string>

pf::Simulation setSimValues()
{
  std::cout << "\nInsert the following values for simulation: \n";

  double a =
      pf::askInput<double>("a (default " + pf::formatNumeric(pf::def_a) + "): ",
                           pf::max_param_value);
  double b =
      pf::askInput<double>("b (default " + pf::formatNumeric(pf::def_b) + "): ",
                           pf::max_param_value);
  double c =
      pf::askInput<double>("c (default " + pf::formatNumeric(pf::def_c) + "): ",
                           pf::max_param_value);
  double d =
      pf::askInput<double>("d (default " + pf::formatNumeric(pf::def_d) + "): ",
                           pf::max_param_value);

  double initial_preys =
      pf::askInput<double>("Initial number of preys (default "
                           + pf::formatNumeric(pf::def_initial_preys) + "): ");
  double initial_preds =
      pf::askInput<double>("Initial number of predators (default "
                           + pf::formatNumeric(pf::def_initial_preds) + "): ");

  return pf::Simulation{{initial_preys, initial_preds}, {a, b, c, d}};
}

void runSimulation(pf::Simulation& sim)
{
  double duration = pf::askInput<double>(
      "\nInsert duration for the simulation: ", pf::max_duration);

  auto execResult = pf::executeSim(sim, duration);
  if (execResult.hasWarning) {
    std::cout << "[Warning] " << execResult.message;
  }

  std::cout << "[Info] Simulation successfully executed.\n";

  pf::writeOnFile(sim);
  std::cout << "\n[Info] Results written to file.\n";

  std::cout << "\n[Info] Visualization in progress. Close the graphic window "
               "to terminate the execution.\n";
  pf::visualizeResult(sim);
  std::cout << "[Info] Visualization completed. Image saved to file.\n";
}

int main()
{
  std::cout << "\n====================================\n";
  std::cout << "Volterra Predator-Prey Simulator\n";
  std::cout << "====================================\n\n";

  try {
    std::string feedback = pf::askInput<std::string>(
        "Run simulation with default values? [y/n]\n"
        "Press [q] to run a random simulation instead.\n"
        "Press any other key to exit the program.\n");

    if (feedback == "y" || feedback == "Y") {
      pf::Simulation sim; // default constructor
      std::cout << "\n[Info] Default simulation values set.\n";
      runSimulation(sim);

    } else if (feedback == "n" || feedback == "N") {
      pf::Simulation sim = setSimValues();
      std::cout << "\n[Info] Custom simulation parameters set.\n";
      runSimulation(sim);

    } else if (feedback == "q" || feedback == "Q") {
      pf::Simulation sim = pf::generateRandomSim();
      std::cout << "\n[Info] Random simulation parameters set.\n";
      runSimulation(sim);
    }

    else {
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
