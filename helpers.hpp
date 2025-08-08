#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "volterra.hpp"

#include <iostream>

namespace pf {


void handleSimulationMenu(const bool& sim_ready);
void handleExecuteSimulation(Simulation& sim, double T);

void handleWriteOnFile(Simulation& sim);

void handleVisualizeResult(Simulation& sim);

template<typename T>
T handleAskInput(const std::string& prompt)
{
  T value;
  while (true) {
    std::cout << prompt;
    std::cin >> value;
    if (!std::cin.fail())
      break;

    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cerr << "[Error] Invalid input. Try again.\n";
  }
  return value;
}

// (Facoltativo) altre funzioni di utilità
// double promptPositiveDouble(const std::string&);
// int promptIntInRange(const std::string&, int, int);

} // namespace pf

#endif // HELPERS_HPP
