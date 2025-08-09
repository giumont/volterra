#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "volterra.hpp"

#include <iostream>
#include <memory> //per std::unique_ptr

namespace pf {

std::pair<SpeciesCount, Parameters> setSimValues();

void showSimMenu(Simulation& sim);
void executeSim(Simulation& sim, double T);

void writeOnFile(const Simulation& sim);

void visualizeResult(const Simulation& sim);

void runSim(Simulation& sim);

// template<typename T>
// T askInput(const std::string& prompt)
// {
//   T value;
//   while (true) {
//     std::cout << prompt;
//     std::cin >> value;
//     if (!std::cin.fail())
//       break;

//     std::cin.clear();
//     std::cin.ignore(10000, '\n');
//     std::cerr << "[Error] Invalid input. Try again.\n\n";
//   }
//   return value;
// }

template<typename T>
T askInput(const std::string& prompt)
{
  T value;
  std::cout << prompt;
  std::cin >> value;
  if (std::cin.fail()) {
    throw std::invalid_argument("[Error] Invalid input.\n");
  }
  return value;
}
} // namespace pf

#endif // HELPERS_HPP
