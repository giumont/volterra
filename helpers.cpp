#include "helpers.hpp"
#include "graph_renderer.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace pf {

void handleExecuteSimulation(pf::Simulation& sim)
{
  double T =
      handleAskInput<double>("\n\nInsert duration T for the simulation: ");

  auto [steps, adjusted_T] = sim.runSimulation(T);

  if (steps == 1 && adjusted_T >= T) {
    std::cout << "[Warning] T (" << T << ") is shorter than the time step dt ("
              << sim.getDt() << "). One step will be executed anyway.\n";
  }

  else if (adjusted_T > T) {
    std::cout << "[Warning] T (" << T
              << ") is not a multiple of the time step dt (" << sim.getDt()
              << "). Rounded up to " << adjusted_T << ".\n";
  }

  std::cout << "[Info] Simulation succesfully ended.\n";
}

void handleWriteOnFile(pf::Simulation& sim)
{
  std::ofstream outfile{"results.txt"};

  if (!outfile) {
    throw std::runtime_error{"[Error] Impossible to open file."};
  }

  auto total_states = sim.numSteps();

  outfile << "- Total states: " << total_states << '\n';

  for (auto const& state : sim.getAbsStates()) {
    outfile << "Time: " << state.t << '\t';
    outfile << "Preys: " << state.preys << '\t';
    outfile << "Predators: " << state.predators << '\t';
    outfile << "H: " << state.H << '\n';
  }

  std::cout << "[Info] Results wrote on file.\n";
}

void handleVisualizeResult(pf::Simulation& sim)
{
  std::thread viewer_thread([sim]() { // sim copied
    pf::GraphRenderer renderer(sim);
    renderer.drawTimeSeries();
    renderer.drawOrbits();
  });

  std::cout << "[Info] Results plotted on graphic window.\n";

  viewer_thread.join();
}
} // namespace pf
