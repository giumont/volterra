#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "constants.hpp"
#include "volterra.hpp"

#include <iomanip> // for methods in formatDouble template
#include <iostream>
#include <limits>   // for methods in askInput template
#include <optional> // for methods in askInput template
#include <sstream>  // for methods in formatDouble template

namespace pf {

struct ExecSimResult
{
  bool hasWarning;
  std::string message;
};

ExecSimResult executeSim(Simulation& sim, double duration);

void writeOnFile(const Simulation& sim);

void visualizeResult(const Simulation& sim);

template<typename T>
std::string formatNumeric(T value)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(precision) << value;
  return oss.str();
}

template<typename T>
T askInput(const std::string& prompt, std::optional<T> max_value = std::nullopt,
           std::optional<T> min_value = std::nullopt)
{
  T value;
  std::cout << prompt;
  std::cin >> value;

  if (std::cin.fail()) {
    std::cin.clear();
    std::cin.ignore(1000, '\n'); // discard invalid input
    throw std::invalid_argument("Invalid input.\n");
  }

  if (max_value && value > *max_value) {
    throw std::out_of_range("Value (" + formatNumeric(value)
                            + ") exceeds the maximum allowed ("
                            + formatNumeric(*max_value) + ").\n");
  }

  if (min_value && value < *min_value) {
    throw std::out_of_range("Value (" + formatNumeric(value)
                            + ") is below the minimum allowed ("
                            + formatNumeric(*min_value) + ").\n");
  }

  return value;
}

} // namespace pf

#endif // HELPERS_HPP
