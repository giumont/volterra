#include "graph_renderer.hpp"
#include "helpers.hpp"
#include "volterra.hpp"

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

int main()
{
  std::unique_ptr<pf::Simulation> sim = nullptr;
  bool sim_ready = false; // Flag se la simulazione è stata eseguita

  while (true) {
    try {
      std::cout << "\n-- Menu --\n"
                << "1) Set parameters (a,b,c,d) and initial conditions\n"
                << "2) Run simulation\n"
                << "3) Write results to file\n"
                << "4) Visualize results\n"
                << "5) Exit\n"
                << "Choose an option: ";

      int choice;
      std::cin >> choice;

      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid choice, please enter a number between 1 and 5.\n";
        continue;
      }

      switch (choice) {
      case 1: {
        std::string feedback = pf::handleAskInput<std::string>(
            "Run simulation with default values? y/n\n");

        if (feedback == "y" || feedback == "Y") {
          sim       = std::make_unique<pf::Simulation>(); // default constructor
          sim_ready = false;
          std::cout << "Default simulation parameters set.\n";
        } else {
          std::cout << "Insert parameters for ODE:\n";

          double a = pf::handleAskInput<double>("a (default 1): ");
          double b = pf::handleAskInput<double>("b (default 0.1):");
          double c = pf::handleAskInput<double>("c (default 0.1): ");
          double d = pf::handleAskInput<double>("d (default 1): ");

          std::cout << "Insert initial conditions:\n";

          double x = pf::handleAskInput<double>(
              "Initial number of preys (default 10): ");
          double y = pf::handleAskInput<double>(
              "Initial number of predators (default 5): ");

          pf::SpeciesCount initial_abs_count{x, y};
          sim = std::make_unique<pf::Simulation>(initial_abs_count, a, b, c, d);
          sim_ready = false;
          std::cout << "Custom simulation parameters set.\n";
        }
        break;
      }
      case 2: {
        if (!sim_ready) {
          if (!sim) {
            std::cout << "Warning: values not set. Simulation will run with "
                         "default values.\n";
            sim = std::make_unique<pf::Simulation>();
          }
          double T = pf::handleAskInput<double>(
              "\n\nInsert duration T for the simulation: ");
          handleExecuteSimulation(*sim, T);
          sim_ready = true;
        } else {
          std::cout << "Simulation already run. Set new parameters to rerun.\n";
        }
        break;
      }
      case 3: {
        if (!sim_ready) {
          std::cout << "Run simulation first before writing results.\n";
        } else {
          handleWriteOnFile(*sim);
        }
        break;
      }
      case 4: {
        if (!sim_ready) {
          std::cout << "Run simulation first before visualization.\n";
        } else {
          handleVisualizeResult(*sim);
        }
        break;
      }
      case 5: {
        std::cout << "Exiting program.\n";
        return 0;
      }
      default:
        std::cout << "Invalid choice, please select between 1 and 5.\n";
      }
    } catch (const std::exception& e) {
      std::cerr << "Caught exception: '" << e.what() << "'\n";
      return EXIT_FAILURE;
    } catch (...) {
      std::cerr << "Caught unknown exception\n";
      return EXIT_FAILURE;
    }
  }
}
