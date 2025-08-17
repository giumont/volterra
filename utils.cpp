#include "utils.hpp"
#include "graph_renderer.hpp"
#include "output_opt.hpp"

#include <filesystem> // for creating results dir
#include <fstream>
#include <iomanip> // for formatting .txt output
#include <iostream>
#include <stdexcept>

std::string outfile_title = "results/volterra.txt";

namespace pf {

ExecSimResult executeSim(pf::Simulation& sim, double duration)
{
  auto [steps, adjusted_T] = sim.run(duration);
  ExecSimResult result{false, ""};

  if (steps == 1 && adjusted_T >= duration) {
    result.hasWarning = true;
    result.message    = "Duration duration (" + std::to_string(duration)
                   + ") is shorter than the time step dt ("
                   + std::to_string(sim.getDt())
                   + "). One step will be executed anyway.\n";
  }

  else if (adjusted_T > duration) {
    result.hasWarning = true;
    result.message    = "Duration duration (" + std::to_string(duration)
                   + ") is not a multiple of the time step dt ("
                   + std::to_string(sim.getDt()) + "). Rounded up to "
                   + std::to_string(adjusted_T) + ".\n";
  }

  return result;
}

void writeOnFile(const pf::Simulation& sim)
{
  std::filesystem::create_directories("results");

  std::ofstream outfile{outfile_title};

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

  if (combined) {
    sf::Image img = renderer.drawCombinedPlots();
    saveToFile(img, "volterra");
  } else {
    std::vector<PlotConfig> plot_configs = {
        {PlotConfig::Type::TimeSeries, "time_series"},
        {PlotConfig::Type::Orbits, "orbits"}};

    for (const auto& plot_config : plot_configs) {
      sf::Image img = renderer.drawSinglePlot(plot_config);
      saveToFile(img, plot_config.title);
    }
  }
}
} // namespace pf
