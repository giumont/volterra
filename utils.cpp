#include "utils.hpp"
#include "graph_renderer.hpp"

#include <fstream>
#include <iomanip> // for formatting .txt output
#include <iostream>
#include <stdexcept>

namespace pf {

ExecSimResult executeSim(pf::Simulation& sim, double T)
{
  auto [steps, adjusted_T] = sim.run(T);
  ExecSimResult result{false, ""};

  if (steps == 1 && adjusted_T >= T) {
    result.hasWarning = true;
    result.message    = "Duration T (" + std::to_string(T)
                   + ") is shorter than the time step dt ("
                   + std::to_string(sim.getDt())
                   + "). One step will be executed anyway.\n";
  }

  else if (adjusted_T > T) {
    result.hasWarning = true;
    result.message    = "Duration T (" + std::to_string(T)
                   + ") is not a multiple of the time step dt ("
                   + std::to_string(sim.getDt()) + "). Rounded up to "
                   + std::to_string(adjusted_T) + ".\n";
  }

  return result;
}

void writeOnFile(const pf::Simulation& sim)
{
  // Columns' width in output file
  constexpr int width_time = 10;
  constexpr int width_prey = 10;
  constexpr int width_pred = 12;
  constexpr int width_H    = 10;

  std::ofstream outfile{"volterra.txt"};

  if (!outfile) {
    throw std::runtime_error{"Impossible to open output file.\n"};
  }

  auto total_states = sim.numSteps();

  outfile << "Total states: " << total_states << '\n';

  // Columns' intestations
  outfile << std::left << std::setw(width_time) << "Time"
          << std::setw(width_prey) << "Preys" << std::setw(width_pred)
          << "Predators" << std::setw(width_H) << "H" << '\n';

  // Line separator
  outfile << std::string(width_time + width_prey + width_pred + width_H, '-')
          << '\n';

  // Data
  for (const auto& state : sim.getAbsStates()) {
    outfile << std::left << std::setw(width_time) << state.t
            << std::setw(width_prey) << state.preys << std::setw(width_pred)
            << state.preds << std::setw(width_H) << state.H << '\n';
  }
}

void visualizeResult(const pf::Simulation& sim)
{
  pf::GraphRenderer renderer(sim);

  renderer.drawCombinedPlots();
}
} // namespace pf
