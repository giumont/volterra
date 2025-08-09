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
  try {
    // 1) Chiedi se usare valori default o personalizzati (una sola volta)
    std::string feedback =
        pf::askInput<std::string>("Run simulation with default values? y/n\n");

    if (feedback == "y" || feedback == "Y") {
      pf::Simulation sim; // default
      std::cout << "Default simulation values set.\n";

      runSim(sim&);

    } else if (feedback == "n" || feedback == "N") {
      auto sim_values = pf::setSimValues();

      pf::Simulation sim{sim_values.first, sim_values.second};
      std::cout << "Custom simulation parameters set.\n";

      runSim(sim);

    } else {
      std::cerr << "Invalid input. Program will exit.\n";
      return EXIT_FAILURE;
    }

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\nExiting program.\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error occurred. Exiting program.\n";
    return EXIT_FAILURE;
  }

  return 0;
}

// int main()
// {
//   std::unique_ptr<pf::Simulation> sim = nullptr;
//   bool sim_ready = false; // Flag se la simulazione è stata eseguita

//   while (true) {
//     try {
//       std::cout << "\n-- Menu --\n"
//                 << "1) Set parameters (a,b,c,d) and initial conditions\n"
//                 << "2) Run simulation\n"
//                 << "3) Write results to file\n"
//                 << "4) Visualize results\n"
//                 << "5) Exit\n"
//                 << "\nChoose an option: ";

//       int choice;
//       std::cin >> choice;

//       if (std::cin.fail()) {
//         std::cin.clear();
//         std::cin.ignore(10000, '\n');
//         std::cout << "Invalid choice, please enter a number between 1
//         and 5.\n\n"; continue;
//       }

//       switch (choice) {
//       case 1: {
//         std::string feedback = pf::askInput<std::string>(
//             "Run simulation with default values? y/n\n");

//         if (feedback == "y" || feedback == "Y") {
//           sim       = std::make_unique<pf::Simulation>(); // default
//           constructor sim_ready = false; std::cout << "Default simulation
//           parameters set.\n\n";
//         } else {
//           std::cout << "Insert parameters for ODE:\n";

//           double a = pf::askInput<double>("a (default 1): ");
//           double b = pf::askInput<double>("b (default 0.1): ");
//           double c = pf::askInput<double>("c (default 0.1): ");
//           double d = pf::askInput<double>("d (default 1): ");

//           std::cout << "Insert initial conditions:\n";

//           double x = pf::askInput<double>(
//               "Initial number of preys (default 10): ");
//           double y = pf::askInput<double>(
//               "Initial number of predators (default 5): ");

//           pf::SpeciesCount initial_abs_count{x, y};
//           pf::Parameters params{a, b, c, d};

//           sim = std::make_unique<pf::Simulation>(initial_abs_count, params);
//           sim_ready = false;
//           std::cout << "Custom simulation parameters set.\n\n";
//         }
//         break;
//       }
//       case 2: {
//         pf::showSimMenu(sim, sim_ready);
//         break;
//       }
//       case 3: {
//         if (!sim_ready) {
//           std::cout << "Run simulation first before writing results.\n\n";
//         } else {
//           writeOnFile(*sim);
//         }
//         break;
//       }
//       case 4: {
//         if (!sim_ready) {
//           std::cout << "Run simulation first before visualization.\n\n";
//         } else {
//           visualizeResult(*sim);
//         }
//         break;
//       }
//       case 5: {
//         std::cout << "Exiting program.\n\n";
//         return 0;
//       }
//       default:
//         std::cout << "Invalid choice, please select between 1 and 5.\n\n";
//       }
//     } catch (const std::exception& e) {
//       std::cerr << "Caught exception: '" << e.what() << '\n';
//       return EXIT_FAILURE;
//     } catch (...) {
//       std::cerr << "Caught unknown exception\n";
//       return EXIT_FAILURE;
//     }
//   }
// }
