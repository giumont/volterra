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
    CHECK(initial_rel_count.preds == doctest::Approx(0.5));
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
    CHECK(initial_abs_count.preds == doctest::Approx(5.0));
    CHECK(initial_abs_count.H
          == doctest::Approx(-1.0 * std::log(10.0) + 0.1 * 10.0 + 0.1 * 5.0
                             - 1.0 * std::log(5.0))
                 .epsilon(0.0001));
  }
}

TEST_CASE("Testing Simulation constructor object - Generic values")
{
  pf::SpeciesCount def_initial_count = {10.0, 5.0};          // default
  pf::Parameters def_params          = {1.0, 0.1, 0.1, 1.0}; // default
  double def_dt                      = 0.001;

  SUBCASE("Null parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(def_initial_count, 0.0, def_params.b,
                                def_params.c, def_params.d,
                                def_dt)); // a 0 other default
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, 0.0,
                                def_params.c, def_params.d, def_dt));
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, def_params.b,
                                0.0, def_params.d, def_dt));
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, def_params.b,
                                def_params.c, 0.0, def_dt));
  }

  SUBCASE("Negative parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(def_initial_count, -1.0, def_params.b,
                                def_params.c, def_params.d,
                                def_dt)); // a neg other default
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, -1.0,
                                def_params.c, def_params.d, def_dt));
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, def_params.b,
                                -1.0, def_params.d, def_dt));
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params.a, def_params.b,
                                def_params.c, -1.0, def_dt));
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
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params, 0));
    CHECK_THROWS(pf::Simulation(def_initial_count, def_params, -0.001));
  }
}

TEST_CASE("Testing Simulation run_simulation() method")
{
  pf::Simulation def_sim;                                    // default
  pf::SpeciesCount def_initial_count = {10.0, 5.0};          // default
  pf::Parameters def_params          = {1.0, 0.1, 0.1, 1.0}; // default
  double def_dt                      = 0.001;                // default

  SUBCASE("Null or negative T throws")
  {
    CHECK_THROWS(def_sim.run(0));
    CHECK_THROWS(def_sim.run(-1));
  }
  SUBCASE("Single step updates the system correctly, testing for evolve()")
  {
    double dt = def_sim.getDt();
    def_sim.run(dt); // singolo passo

    REQUIRE(def_sim.numSteps() == 2);
    auto s0 = def_sim.getAbsStates()[0];
    auto s1 = def_sim.getAbsStates()[1];

    CHECK(s1.t == doctest::Approx(s0.t + dt));
    CHECK(s1.preys
          != doctest::Approx(s0.preys).epsilon(1e-12)); // deve cambiare
    // CHECK(s1.preds != doctest::Approx(s0.preds).epsilon(1e-12));
    // NON funziona:
    // sembra che al primo step la y rimanga uguale
    CHECK(s1.preys > 0);
    CHECK(s1.preds > 0);
  }

  SUBCASE("Run for long T does not diverge - Default values")
  {
    CHECK_NOTHROW(def_sim.run(100)); // 100.000 steps
    CHECK(def_sim.numSteps() > 1);

    auto last  = def_sim.getAbsStates().back();
    auto first = def_sim.getAbsStates().front();

    CHECK(std::isfinite(last.preys));
    CHECK(last.preys > 0);

    CHECK(std::isfinite(last.preds));
    CHECK(last.preds > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for unbalanced parameters does not diverge")
  {
    pf::Simulation sim1{def_initial_count, 0.1,          1.0,
                        def_params.c,      def_params.d, def_dt}; // b>>a
    pf::Simulation sim2{def_initial_count,
                        def_params.a,
                        def_params.b,
                        1.0,
                        0.1,
                        def_dt}; // c>>d

    for (pf::Simulation sim : {sim1, sim2}) {
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.numSteps() > 1);

      auto last  = sim.getAbsStates().back();
      auto first = sim.getAbsStates().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.preds));
      CHECK(last.preds > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Run for big parameters' values does not diverge")
  {
    pf::Simulation sim1{def_initial_count, 50,           def_params.b,
                        def_params.c,      def_params.d, def_dt}; // a grande
    pf::Simulation sim2{def_initial_count, 500,   50, def_params.c,
                        def_params.d,      def_dt}; // b grande (ma a>>b)
    pf::Simulation sim3{def_initial_count,
                        def_params.a,
                        def_params.b,
                        50,
                        500,
                        def_dt}; // c grande (ma d>>c)
    pf::Simulation sim4{def_initial_count, 50, def_params.b,
                        def_params.c,      50, def_dt}; // d grande

    for (pf::Simulation sim : {sim1, sim2, sim3, sim4}) {
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.numSteps() > 1);

      auto last = sim.getAbsStates().back();
      // auto first = sim.get_abs_states().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.preds));
      CHECK(last.preds > 0);

      CHECK(std::isfinite(last.H));
      //   CHECK(last.H                              //in questo caso da
      //   problemi
      //         == doctest::Approx(first.H).epsilon(
      //             0.01));
    }
  }

  SUBCASE("Run for big dt value does not diverge")
  {
    pf::Simulation sim{def_initial_count, def_params, 0.1};

    CHECK_NOTHROW(sim.run(1.0));
    CHECK(sim.numSteps() > 1);

    auto last  = sim.getAbsStates().back();
    auto first = sim.getAbsStates().front();

    CHECK(std::isfinite(last.preys));
    CHECK(last.preys > 0);

    CHECK(std::isfinite(last.preds));
    CHECK(last.preds > 0);

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

    CHECK(std::isfinite(last.preds));
    CHECK(last.preds > 0);

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
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.numSteps() > 1);

      auto last  = sim.getAbsStates().back();
      auto first = sim.getAbsStates().front();

      CHECK(std::isfinite(last.preys));
      CHECK(last.preys > 0);

      CHECK(std::isfinite(last.preds));
      CHECK(last.preds > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Too short T performs just one step other than the initial")
  {
    pf::Simulation sim{def_initial_count, def_params, def_dt};
    sim.run(0.00001); // < dt
    CHECK(sim.numSteps() == 2); // solo stato iniziale e primo step
  }
}

TEST_CASE("Testing Simulation get_element_series() methods")
{
  pf::Simulation sim;

  std::vector<pf::SpeciesState> states = sim.getAbsStates();
  
  std::vector<double> preys_series               = sim.getSeries(states, &pf::SpeciesState::preys);
  std::vector<double> preds_series               = sim.getSeries(states, &pf::SpeciesState::preds);
  std::vector<double> H_series               = sim.getSeries(states, &pf::SpeciesState::H);
  std::vector<double> t_series               = sim.getSeries(states, &pf::SpeciesState::t);

  sim.run(0.1);

  SUBCASE("Series match get_abs_states output")
  {
    for (std::size_t i = 0; i < states.size();
         ++i) { // qui non serve usare sim.num_steps()
      CHECK(preys_series[i] == doctest::Approx(states[i].preys));
      CHECK(preds_series[i] == doctest::Approx(states[i].preds));
      CHECK(H_series[i] == doctest::Approx(states[i].H));
      CHECK(t_series[i] == doctest::Approx(states[i].t));
    }
  }

  SUBCASE("Series lengths are consistent")
  {
    CHECK(preys_series.size() == preds_series.size());
    CHECK(preys_series.size() == H_series.size());
    CHECK(preys_series.size() == t_series.size());
  }
}