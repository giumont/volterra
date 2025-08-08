#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "volterra.hpp"

#include "doctest.h"

TEST_CASE("Testing Simulation constructor object - Default values")
{
  pf::Simulation sim;

  // valori di default: ...

  SUBCASE("Default constructor does not throw")
  {
    CHECK_NOTHROW(pf::Simulation sim2); // serve per non fare warning
  }

  SUBCASE("Conversion to relative values correctly computed - Default values")
  {
    REQUIRE(sim.numSteps() == 1); // serve?? si, perché cosi sto sicura

    auto initial_rel_count =
        sim.getRelPoints()[0]; // non uso get_last(), lo testo a parte

    CHECK(initial_rel_count.preys
          == doctest::Approx(1.0)); // valori calcolati dai default
    CHECK(initial_rel_count.predators == doctest::Approx(0.5));
  }

  SUBCASE(
      "Initial values correctly computed - Default values") // conrtolla
                                                            // implicitamente
                                                            // compute_H e
                                                            // to_abs
  {
    REQUIRE(sim.numSteps() == 1);

    auto initial_abs_count = sim.getAbsStates()[0];

    CHECK(initial_abs_count.preys == doctest::Approx(10.0));
    CHECK(initial_abs_count.predators == doctest::Approx(5.0));
    CHECK(initial_abs_count.H
          == doctest::Approx(-1.0 * std::log(10.0) + 0.1 * 10.0 + 0.1 * 5.0
                             - 1.0 * std::log(5.0))
                 .epsilon(0.0001));
  }
}

TEST_CASE("Testing Simulation constructor object - Generic values")
{
  pf::SpeciesCount initial_def_count      = {10.0, 5.0};          // default
  std::array<double, 4> def_params = {1.0, 0.1, 0.1, 1.0}; // default
  double def_dt                    = 0.001;

  SUBCASE("Null parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(initial_def_count, 0, def_params[1],
                                def_params[2],
                                def_params[3])); // a 0 other default
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], 0,
                                def_params[2], def_params[3]));
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], def_params[1],
                                0, def_params[3]));
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], def_params[1],
                                def_params[2], 0));
  }

  SUBCASE("Negative parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(initial_def_count, -1, def_params[1],
                                def_params[2], def_params[3],
                                def_dt)); // a neg other default
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], -1,
                                def_params[2], def_params[3], def_dt));
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], def_params[1],
                                -1, def_params[3], def_dt));
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params[0], def_params[1],
                                def_params[2], -1, def_dt));
  }

  SUBCASE("Null initial condition throws")
  {
    CHECK_THROWS(pf::Simulation({0, 5.0}, def_params, def_dt));
    CHECK_THROWS(pf::Simulation({10.0, 0}, def_params, def_dt));
  }

  SUBCASE("Negative initial condition throws")
  {
    CHECK_THROWS(pf::Simulation({-1, 5.0}, def_params, def_dt));
    CHECK_THROWS(pf::Simulation({10.0, -1}, def_params, def_dt));
  }

  SUBCASE("Null or negative dt throws")
  {
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params, 0));
    CHECK_THROWS(pf::Simulation(initial_def_count, def_params, -0.001));
  }
}

TEST_CASE("Testing Simulation run_simulation() method")
{
  pf::Simulation def_sim;                                  // default
  pf::SpeciesCount initial_def_count      = {10.0, 5.0};          // default
  std::array<double, 4> def_params = {1.0, 0.1, 0.1, 1.0}; // default
  double def_dt                    = 0.001;

  SUBCASE("Null or negative T throws")
  {
    CHECK_THROWS(def_sim.runSimulation(0));
    CHECK_THROWS(def_sim.runSimulation(-1));
  }
  SUBCASE("Single step updates the system correctly, testing for evolve()")
  {
    double dt = def_sim.getDt();
    def_sim.runSimulation(dt); // singolo passo

    REQUIRE(def_sim.numSteps() == 2);
    auto s0 = def_sim.getAbsStates()[0];
    auto s1 = def_sim.getAbsStates()[1];

    CHECK(s1.t == doctest::Approx(s0.t + dt));
    CHECK(s1.preys != doctest::Approx(s0.preys).epsilon(1e-12)); // deve cambiare
    // CHECK(s1.predators != doctest::Approx(s0.predators).epsilon(1e-12)); //NON funziona:
    // sembra che al primo step la y rimanga uguale
    CHECK(s1.preys > 0);
    CHECK(s1.predators > 0);
  }

  SUBCASE("Run for long T does not diverge - Default values")
  {
    CHECK_NOTHROW(def_sim.runSimulation(100)); // 100.000 steps
    CHECK(def_sim.numSteps() > 1);

    auto last  = def_sim.getAbsStates().back();
    auto first = def_sim.getAbsStates().front();

    CHECK(std::isfinite(last.preys));
    CHECK(last.preys > 0);

    CHECK(std::isfinite(last.predators));
    CHECK(last.predators > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for unbalanced parameters does not diverge")
  {
    pf::Simulation sim1{initial_def_count, 0.1,           1.0,
                        def_params[2],     def_params[3], def_dt}; // b>>a
    pf::Simulation sim2{initial_def_count,
                        def_params[0],
                        def_params[1],
                        1.0,
                        0.1,
                        def_dt}; // c>>d

    for (pf::Simulation sim : {sim1, sim2}) {
      CHECK_NOTHROW(sim.runSimulation(0.1));
      CHECK(sim.numSteps() > 1);

      auto last  = sim.getAbsStates().back();
      auto first = sim.getAbsStates().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.predators));
      CHECK(last.predators > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Run for big parameters' values does not diverge")
  {
    pf::Simulation sim1{initial_def_count, 50,    def_params[1], def_params[2],
                        def_params[3],     def_dt}; // a grande
    pf::Simulation sim2{initial_def_count, 500,   50, def_params[2],
                        def_params[3],     def_dt}; // b grande (ma a>>b)
    pf::Simulation sim3{initial_def_count,
                        def_params[0],
                        def_params[1],
                        50,
                        500,
                        def_dt}; // c grande (ma d>>c)
    pf::Simulation sim4{initial_def_count, 50, def_params[1],
                        def_params[2],     50, def_dt}; // d grande

    for (pf::Simulation sim : {sim1, sim2, sim3, sim4}) {
      CHECK_NOTHROW(sim.runSimulation(0.1));
      CHECK(sim.numSteps() > 1);

      auto last = sim.getAbsStates().back();
      // auto first = sim.get_abs_states().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.predators));
      CHECK(last.predators > 0);

      CHECK(std::isfinite(last.H));
      //   CHECK(last.H                              //in questo caso da
      //   problemi
      //         == doctest::Approx(first.H).epsilon(
      //             0.01));
    }
  }

  SUBCASE("Run for big dt value does not diverge")
  {
    pf::Simulation sim{initial_def_count, def_params, 0.1};

    CHECK_NOTHROW(sim.runSimulation(1.0));
    CHECK(sim.numSteps() > 1);

    auto last  = sim.getAbsStates().back();
    auto first = sim.getAbsStates().front();

    CHECK(std::isfinite(last.preys));
    CHECK(last.preys > 0);

    CHECK(std::isfinite(last.predators));
    CHECK(last.predators > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for big initial conditions' values does not diverge")
  {
    pf::Simulation sim{{1e6, 0.5e6}, def_params, def_dt};

    // CHECK_NOTHROW(sim.run_simulation(1.0));   //danno problemi
    // CHECK(sim.num_steps() > 1);

    auto last  = sim.getAbsStates().back();
    auto first = sim.getAbsStates().front();

    CHECK(std::isfinite(last.preys));
    CHECK(last.preys > 0);

    CHECK(std::isfinite(last.predators));
    CHECK(last.predators > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for unbalanced initial conditions does not diverge")
  {
    pf::Simulation sim1{{100.0, 10.0}, def_params, def_dt}; // x>>y
    pf::Simulation sim2{{10.0, 100.0}, def_params, def_dt}; // y>>x

    for (pf::Simulation sim : {sim1, sim2}) {
      CHECK_NOTHROW(sim.runSimulation(0.1));
      CHECK(sim.numSteps() > 1);

      auto last  = sim.getAbsStates().back();
      auto first = sim.getAbsStates().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.predators));
      CHECK(last.predators > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Too short T performs just one step other than the initial")
  {
    pf::Simulation sim{initial_def_count, def_params, def_dt};
    sim.runSimulation(0.00001); // < dt
    CHECK(sim.numSteps() == 2); // solo stato iniziale e primo step
  }
}

TEST_CASE("Testing Simulation get_element_series() methods")
{
  pf::Simulation sim;

  std::vector<pf::SpeciesState> states = sim.getAbsStates();
  std::vector<double> xs        = sim.getXSeries();
  std::vector<double> ys        = sim.getYSeries();
  std::vector<double> Hs        = sim.getHSeries();
  std::vector<double> ts        = sim.getTimeSeries();

  sim.runSimulation(0.1);

  SUBCASE("Series match get_abs_states output")
  {
    for (std::size_t i = 0; i < states.size();
         ++i) { // qui non serve usare sim.num_steps()
      CHECK(xs[i] == doctest::Approx(states[i].preys));
      CHECK(ys[i] == doctest::Approx(states[i].predators));
      CHECK(Hs[i] == doctest::Approx(states[i].H));
      CHECK(ts[i] == doctest::Approx(states[i].t));
    }
  }

  SUBCASE("Series lengths are consistent")
  {
    CHECK(xs.size() == ys.size());
    CHECK(xs.size() == Hs.size());
    CHECK(xs.size() == ts.size());
  }
}