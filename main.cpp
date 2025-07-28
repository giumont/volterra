
#include <cstdlib>
#include <exception>
#include <iostream>

#include "volterra.hpp"

int main() {
    pf::Simulation sim(1.0, 1.0, 1.0, 1.0, 0.001);

    // Inizializza stato iniziale
    pf::State initial_state{1.0, 1.0, 0.0};
    sim.evolve();  // serve almeno uno stato iniziale prima?

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
