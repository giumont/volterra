
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

#include "graph_renderer.hpp"
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

void write_on_file(pf::Simulation& sim)
{
  std::ofstream outfile{"results.txt"};

  if (!outfile) {
    throw std::runtime_error{"Impossible to open file!"};
  }

  auto total_states = sim.num_steps();

  outfile << "- Total states: " << total_states << '\n';

  for (auto const& state : sim.get_abs_states()) {
    outfile << "Time: " << state.t << '\t';
    outfile << "Preys: " << state.x << '\t';
    outfile << "Predators: " << state.y << '\t';
    outfile << "H: " << state.H << '\n';
  }

  std::cout << "Results wrote on file.\n";
}

void visualize_result(pf::Simulation& sim)
{
  std::thread viewer_thread([sim]() { // sim copied
    pf::GraphRenderer renderer(sim);
    renderer.drawTimeSeries();
    renderer.drawOrbits();
  });

  std::cout << "Results plotted on graphic window.\n";

  viewer_thread.join();
}
int main()
{
  std::unique_ptr<pf::Simulation> sim = nullptr;
  bool sim_ready = false; // Flag se la simulazione è stata eseguita

  auto ask_input = [](const std::string& prompt, double& var) {
    while (true) {
      std::cout << prompt;
      std::cin >> var;
      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid input. Please enter a positive numeric value.\n";
      } else if (var <= 0) {
        std::cout << "Invalid input. Please enter a positive value.\n";
      } else {
        break;
      }
    }
  };

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
        std::string feedback;
        std::cout << "Run simulation with default values? y/n\n";
        std::cin >> feedback;

        if (feedback == "y" || feedback == "Y") {
          sim       = std::make_unique<pf::Simulation>(); // default constructor
          sim_ready = false;
          std::cout << "Default simulation parameters set.\n";
        } else {
          double a, b, c, d;
          double x, y;

          std::cout << "Insert parameters for ODE:\n";
          ask_input("a (default 1): ", a);
          ask_input("b (default 0.1): ", b);
          ask_input("c (default 0.1): ", c);
          ask_input("d (default 1): ", d);

          std::cout << "Insert initial conditions:\n";
          ask_input("Initial number of preys (default 10): ", x);
          ask_input("Initial number of predators (default 5): ", y);

          pf::Point initial_abs_point{x, y};
          sim = std::make_unique<pf::Simulation>(initial_abs_point, a, b, c, d);
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
          execute_simulation(*sim);
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
          write_on_file(*sim);
        }
        break;
      }
      case 4: {
        if (!sim_ready) {
          std::cout << "Run simulation first before visualization.\n";
        } else {
          visualize_result(*sim);
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
