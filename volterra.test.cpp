#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "volterra.hpp"
#include "constants.hpp"

#include "doctest.h"

TEST_CASE("Testing Simulation constructor object - Default values")
{
  pf::Simulation sim; // default

  SUBCASE("Default constructor does not throw")
  {
    CHECK_NOTHROW(pf::Simulation sim2); // creation of sim2 prevents warning
  }

  SUBCASE("Conversion to relative values correctly computed - Default values")
  {
    REQUIRE(sim.numSteps() == 1);

    auto initial_rel_count =
        sim.getRelCounts()[0]; // getLast() tested separately

    CHECK(initial_rel_count.preys
          == doctest::Approx(1.0)); // computed from defaults
    CHECK(initial_rel_count.preds == doctest::Approx(0.5));

    // Check that first = last rel count if just one
    auto last_rel = sim.getRelCounts().back(); // as testing getLastRelCount()
    CHECK(last_rel.preys == doctest::Approx(initial_rel_count.preys));
    CHECK(last_rel.preds == doctest::Approx(initial_rel_count.preds));
  }

  SUBCASE(
      "Initial values correctly computed - Default values") // implicitly tests
                                                            // computeH() and
                                                            // toAbs()
  {
    REQUIRE(sim.numSteps() == 1);

    auto initial_abs_state = sim.getAbsStates()[0];

    CHECK(initial_abs_state.preys == doctest::Approx(pf::def_initial_preys));
    CHECK(initial_abs_state.preds == doctest::Approx(pf::def_initial_preds));

    double expected_H =
        -1.0 * std::log(pf::def_initial_preys) + 0.1 * pf::def_initial_preys
        + 0.1 * pf::def_initial_preds - 1.0 * std::log(pf::def_initial_preds);

    CHECK(initial_abs_state.H == doctest::Approx(expected_H).epsilon(0.0001));
  }
}

TEST_CASE("Testing Simulation constructor object - Generic values")
{
  // default
  pf::SpeciesCount def_initial_count = {pf::def_initial_preys,
                                        pf::def_initial_preys};
  pf::Parameters def_params = {pf::def_a, pf::def_b, pf::def_c, pf::def_d};

  SUBCASE(
      "Null parameter in constructor throws - Testing validateParameters() ")
  {
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, 0.0, pf::def_b,
                                    pf::def_c, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, 0.0,
                                    pf::def_c, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, pf::def_b,
                                    0.0, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, pf::def_b,
                                    pf::def_c, 0.0, pf::def_dt}),
                    std::invalid_argument);
  }

  SUBCASE("Negative parameter in constructor throws - Testing "
          "validateParameters() ")

  {
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, -1.0, pf::def_b,
                                    pf::def_c, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, -1.0,
                                    pf::def_c, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, pf::def_b,
                                    -1.0, pf::def_d, pf::def_dt}),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation{def_initial_count, pf::def_a, pf::def_b,
                                    pf::def_c, -1.0, pf::def_dt}),
                    std::invalid_argument);
  }

  SUBCASE("Null initial condition throws - Testing validateInitialConditions()")
  {
    CHECK_THROWS_AS(
        (pf::Simulation{{0.0, pf::def_initial_preys}, def_params, pf::def_dt}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        (pf::Simulation{{pf::def_initial_preys, 0.0}, def_params, pf::def_dt}),
        std::invalid_argument);
  }

  SUBCASE("Negative initial condition throws - Testing "
          "validateInitialConditions()")
  {
    CHECK_THROWS_AS(
        (pf::Simulation{{-1.0, pf::def_initial_preys}, def_params, pf::def_dt}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        (pf::Simulation{{pf::def_initial_preys, -1.0}, def_params, pf::def_dt}),
        std::invalid_argument);
  }

  SUBCASE("Null or negative dt throws - Testing validateDt()")
  {
    CHECK_THROWS_AS((pf::Simulation(def_initial_count, def_params, 0)),
                    std::invalid_argument);
    CHECK_THROWS_AS((pf::Simulation(def_initial_count, def_params, -0.001)),
                    std::invalid_argument);
  }
}

TEST_CASE("Testing Simulation run() method")
{
  // default
  pf::Simulation def_sim;
  pf::SpeciesCount def_initial_count = {pf::def_initial_preys,
                                        pf::def_initial_preys};
  pf::Parameters def_params = {pf::def_a, pf::def_b, pf::def_c, pf::def_d};

  SUBCASE("Null or negative duration throws")
  {
    CHECK_THROWS(def_sim.run(0));
    CHECK_THROWS(def_sim.run(-1));
  }
  SUBCASE("Single step updates the system correctly - Testing for evolve()")
  {
    pf::Simulation sim{
        {5.0, 5.0}, def_params, pf::def_dt}; // non-equilibrium initial position
    sim.run(pf::def_dt);

    pf::SpeciesState si = sim.getAbsStates()[0];
    pf::SpeciesState sf = sim.getAbsStates()[1];

    CHECK(sf.t == doctest::Approx(si.t + pf::def_dt));
    CHECK(sf.preys != doctest::Approx(si.preys).epsilon(1e-12));
    CHECK(sf.preds != doctest::Approx(si.preds).epsilon(1e-12));

    CHECK(sf.preys > 0);
    CHECK(sf.preds > 0);
  }

  SUBCASE("Run for long duration does not diverge - Default values")
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
              0.5)); // Bigger numerical drift allowed for these conditions
  }

  SUBCASE("Run for unbalanced parameters does not diverge")
  {
    pf::Simulation sim1{def_initial_count, 0.1,       1.0,
                        pf::def_c,         pf::def_d, pf::def_dt}; // b>>a
    pf::Simulation sim2{def_initial_count, pf::def_a, pf::def_b, 1.0, 0.1,
                        pf::def_dt}; // c>>d

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
                0.01)); // Small numerical drift allowed
    }
  }

  SUBCASE("Run for big parameters' values does not diverge")
  {
    pf::Simulation sim1{def_initial_count, pf::max_param_value,
                        pf::def_b,         pf::def_c,
                        pf::def_d,         pf::def_dt}; // a large
    pf::Simulation sim2{def_initial_count,
                        pf::max_param_value,
                        pf::max_param_value / 10,
                        pf::def_c,
                        pf::def_d,
                        pf::def_dt}; // b large (but a>>b)
    pf::Simulation sim3{def_initial_count,   pf::def_a,
                        pf::def_b,           pf::max_param_value / 10,
                        pf::max_param_value, pf::def_dt}; // c large (but d>>c)
    pf::Simulation sim4{def_initial_count,   pf::def_a, pf::def_b, pf::def_c,
                        pf::max_param_value, pf::def_dt}; // d large

    for (pf::Simulation sim : {sim1, sim2, sim3, sim4}) {
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
                20)); // Allow big numerical drift in H for these conditions
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
    pf::Simulation sim{{pf::max_initial_population, pf::max_initial_population},
                       def_params,
                       pf::def_dt};

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
              20)); // Allow big numerical drift in H for these conditions
  }

  SUBCASE("Run for unbalanced initial conditions does not diverge")
  {
    pf::Simulation sim1{
        {100.0, 10.0}, def_params, pf::def_dt}; // initial preys>>predators
    pf::Simulation sim2{
        {10.0, 100.0}, def_params, pf::def_dt}; // initial predators>>preys

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

  SUBCASE("Too short duration performs just one step other than the initial")
  {
    pf::Simulation sim{def_initial_count, def_params, pf::def_dt};
    sim.run(0.00001);           // < dt
    CHECK(sim.numSteps() == 2); // only initial state and first state
  }
}

TEST_CASE("Testing Simulation getSeries() template")
{
  pf::Simulation sim;

  std::vector<pf::SpeciesState> states = sim.getAbsStates();

  std::vector<double> preys_series =
      sim.getSeries(states, &pf::SpeciesState::preys);
  std::vector<double> preds_series =
      sim.getSeries(states, &pf::SpeciesState::preds);
  std::vector<double> H_series = sim.getSeries(states, &pf::SpeciesState::H);
  std::vector<double> t_series = sim.getSeries(states, &pf::SpeciesState::t);

  sim.run(0.1);

  SUBCASE("Series match getAbsStates() output")
  {
    for (std::size_t i = 0; i < states.size(); ++i) {
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

TEST_CASE("Testing directly Simulation validatePositive() method")
{
  CHECK_THROWS_AS(pf::validatePositive({{"a", -1.0}}), std::invalid_argument);
  CHECK_NOTHROW(pf::validatePositive({{"a", 1.0}}));
}
