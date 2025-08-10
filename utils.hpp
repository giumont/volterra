#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "volterra.hpp"

#include <iostream>

namespace pf {

struct ExecSimResult
{
  bool hasWarning;
  std::string message;
};

ExecSimResult executeSim(Simulation& sim, double T);

void writeOnFile(const Simulation& sim);

void visualizeResult(const Simulation& sim);

template<typename T>
T askInput(const std::string& prompt)
{
  T value;
  std::cout << prompt;
  std::cin >> value;
  if (std::cin.fail()) {
    std::cin.clear();
    std::cin.ignore(1000, '\n'); // discard invalid input
    throw std::invalid_argument("Invalid input.\n");
  }
  return value;
}
} // namespace pf

#endif // HELPERS_HPP
