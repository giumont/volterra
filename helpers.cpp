#include "helpers.hpp"
#include "graph_renderer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace pf {

// void showSimMenu(std::unique_ptr<Simulation>& sim, bool& sim_ready)
// {
//   if (!sim_ready) {
//     if (!sim) {
//       std::cout << "[Warning] Values not set. Simulation will run with "
//                    "default values.\n";
//       sim = std::make_unique<pf::Simulation>();
//     }
//     double T = pf::askInput<double>(
//         "\n\nInsert duration T for the simulation: ");
//     executeSim(*sim, T);
//     sim_ready = true;
//   } else {
//     std::cout
//         << "[Info] Simulation already run. Set new parameters to rerun.\n\n";
//   }
// }

void runSim(pf::Simulation& sim)
{
  // 2) Esegui simulazione una volta
  showSimMenu(sim);

  std::cout << "Simulation completed.\n";

  // 3) Scrivi risultati su file
  writeOnFile(sim);
  std::cout << "Results written to file.\n";

  // 4) Visualizza i risultati
  visualizeResult(sim);
  std::cout << "Visualization completed.\n";
}

std::pair<pf::SpeciesCount, pf::Parameters> setSimValues()
{
  double a = pf::askInput<double>("a (default 1): ");
  double b = pf::askInput<double>("b (default 0.1): ");
  double c = pf::askInput<double>("c (default 0.1): ");
  double d = pf::askInput<double>("d (default 1): ");

  double x = pf::askInput<double>("Initial number of preys (default 10): ");
  double y = pf::askInput<double>("Initial number of predators (default 5): ");

  pf::SpeciesCount initial_abs_count{x, y};
  pf::Parameters params{a, b, c, d};

  return {initial_abs_count, params};
}

void showSimMenu(pf::Simulation& sim)
{
  constexpr double T_MAX =
      1000.0; // limite massimo per la durata della simulazione

  double T = pf::askInput<double>("\n\nInsert duration T for the simulation: ");

  if (T > T_MAX) {
    std::cout << "[Error] Simulation duration T (" << T
              << ") exceeds the maximum allowed (" << T_MAX << ").\n"
              << "Please insert a smaller value.\n\n";
    return; // torna al menu senza eseguire la simulazione
  }

  executeSim(sim, T);
}

void executeSim(pf::Simulation& sim, double T)
{
  auto [steps, adjusted_T] = sim.run(T);

  if (steps == 1 && adjusted_T >= T) {
    std::cout << "[Warning] T (" << T << ") is shorter than the time step dt ("
              << sim.getDt() << "). One step will be executed anyway.\n";
  }

  else if (adjusted_T > T) {
    std::cout << "[Warning] T (" << T
              << ") is not a multiple of the time step dt (" << sim.getDt()
              << "). Rounded up to " << adjusted_T << ".\n";
  }

  std::cout << "[Info] Simulation succesfully executed.\n\n";
}

void writeOnFile(const pf::Simulation& sim)
{
  std::ofstream outfile{"results.txt"};

  if (!outfile) {
    throw std::runtime_error{"[Error] Impossible to open file.\n\n"};
  }

  auto total_states = sim.numSteps();

  outfile << "- Total states: " << total_states << '\n';

  for (auto const& state : sim.getAbsStates()) {
    outfile << "Time: " << state.t << '\t';
    outfile << "Preys: " << state.preys << '\t';
    outfile << "Predators: " << state.preds << '\t';
    outfile << "H: " << state.H << '\n';
  }

  std::cout << "[Info] Results wrote on file.\n\n";
}

void visualizeResult(const pf::Simulation& sim)
{
  // shared_ptr per gestire durata dell'oggetto
  auto renderer = std::make_shared<pf::GraphRenderer>(sim);

  std::thread series_thread([renderer]() {
    renderer->drawTimeSeries();
    // Al termine della funzione, shared_ptr viene distrutto,
    // quindi libera l'oggetto renderer automaticamente.
  });

  std::thread orbits_thread([renderer]() { renderer->drawOrbits(); });

  series_thread.detach(); // Thread gira in background
  orbits_thread.detach();
}
} // namespace pf
