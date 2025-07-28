
#include <cstdlib>
#include <exception>
#include <iostream>

#include "volterra.hpp"

int main()
{
  pf::State x0 = {1.0, 1.0, 0.0};
  pf::Simulation sim(1.0, 1.0, 1.0, 1.0, x0, 0.001);

  // Inizializza stato iniziale
  sim.evolve(); // serve almeno uno stato iniziale prima?

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
