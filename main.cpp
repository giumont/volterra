
#include <cstdlib>
#include <exception>
#include <iostream>

#include "volterra.hpp"

int main()
{
  double duration;
  std::string feedback;

  std::cout << "Run simulation with default values? y/n\n";
  std::cin >> feedback;
  if (feedback == "y" || "Y") {
    pf::Simulation sim;
  } else {
    double a, b, c, d;
    double x, y;
    std::cout
        << "Insert parameters for ode.\n Insert parameter a (default: 1): ";
    std::cin >> a;
    std::cout << "\nInsert parameter b (default: 1): ";
    std::cin >> b;
    std::cout << "\nInsert parameter c (default: 1): ";
    std::cin >> c;
    std::cout << "\nInsert parameter d (default: 1): ";
    std::cin >> d;
    std::cout << "\n\nInsert initial number of preys (default: 1): ";
    std::cin >> x;
    std::cout << "\nInsert initial number of predators (default: 1): ";
    std::cin >> y;

    pf::Point initial_abs_point{x, y};
    pf::Simulation sim{initial_abs_point, a, b, c, d};
  }

  std::cout << "\n\nInsert time duration for the simulation: ";
  std::cin >> duration;

  if (duration <= 0) {
    std::cerr << "Error: duration must be a positive number.\n";
    return 1;
  }

  pf::sim.run(duration);

  std::cout << "Simulazione completata\n";
  return 0;
}
/*
int main() {
  try {
    pf::Simulation data{};

    // ...

  } catch (std::exception const& e) {
    std::cerr << "Caught exception: '" << e.what() << "'\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Caught unknown exception\n";
    return EXIT_FAILURE;
  }
}
*/
