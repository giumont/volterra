
#include <cstdlib>
#include <exception>
#include <iostream>

#include "volterra.hpp"

int main()
{
  double duration;
  std::cout << "Insert time duration for the simulation: ";
  std::cin >> duration;

  if (duration <= 0) {
    std::cerr << "Errore: la durata deve essere positiva.\n";
    return 1;
  }

  // pf::State x0 = {1.0, 1.0, 0.0};
  pf::Simulation sim;

  int steps = static_cast<int>(duration / sim.get_dt());

  for (int i = 0; i < steps; ++i) {
    sim.evolve();
  }

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
