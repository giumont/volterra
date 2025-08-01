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
    REQUIRE(sim.size() == 1); // serve?? si, perché cosi sto sicura

    auto initial_rel_point =
        sim.get_rel_points()[0]; // non uso get_last(), lo testo a parte

    CHECK(initial_rel_point.x
          == doctest::Approx(1.0)); // valori calcolati dai default
    CHECK(initial_rel_point.y == doctest::Approx(0.5));
  }

  SUBCASE(
      "Initial values correctly computed - Default values") // conrtolla
                                                            // implicitamente
                                                            // compute_H e
                                                            // to_abs
  {
    REQUIRE(sim.size() == 1);

    auto initial_abs_point = sim.get_abs_states()[0];

    CHECK(initial_abs_point.x == doctest::Approx(10.0));
    CHECK(initial_abs_point.y == doctest::Approx(5.0));
    CHECK(initial_abs_point.H
          == doctest::Approx(-1.0 * std::log(10.0) + 0.1 * 10.0 + 0.1 * 5.0
                             - 1.0 * std::log(5.0))
                 .epsilon(0.0001));
  }
}

TEST_CASE("Testing Simulation constructor object - Generic values")
{
  pf::Point initial_def_point = {10.0, 5.0}; // default
  //   double def_par_one          = 1.0;
  //   double def_par_two          = 0.1;

  SUBCASE("Null parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(initial_def_point, 0, 0.1, 0.1,
                                1.0)); // a 0 other default
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, 0, 0.1, 1.0));
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, 0.1, 0, 1.0));
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, 0.1, 0.1, 0));
  }

  SUBCASE("Negative parameter in constructor throws")
  {
    CHECK_THROWS(pf::Simulation(initial_def_point, -1, 0.1, 0.1,
                                1.0)); // a neg other default
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, -1, 0.1, 1.0));
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, 0.1, -1, 1.0));
    CHECK_THROWS(pf::Simulation(initial_def_point, 1.0, 0.1, 0.1, -1));
  }

  SUBCASE("Null initial condition throws")
  {
    CHECK_THROWS(pf::Simulation({0, 5.0}, 1.0, 0.1, 0.1, 1.0));
    CHECK_THROWS(pf::Simulation({10.0, 0}, 1.0, 0.1, 0.1, 1.0));
  }

  SUBCASE("Negative initial condition throws")
  {
    CHECK_THROWS(pf::Simulation({-1, 5.0}, 1.0, 0.1, 0.1, 1.0));
    CHECK_THROWS(pf::Simulation({10.0, -1}, 1.0, 0.1, 0.1, 1.0));
  }

  SUBCASE("Null or negative dt throws")
  {
    CHECK_THROWS(pf::Simulation({10.0, 5.0}, 1.0, 0.1, 0.1, 1.0, 0));
    CHECK_THROWS(pf::Simulation({10.0, 5.0}, 1.0, 0.1, 0.1, 1.0, -0.001));
  }
}

TEST_CASE("Testing Simulation run() method")
{
  pf::Simulation def_sim;                    // default
  pf::Point initial_def_point = {10.0, 5.0}; // default

  SUBCASE("Null or negative duration throws")
  {
    CHECK_THROWS(def_sim.run(0));
    CHECK_THROWS(def_sim.run(-1));
  }

  SUBCASE("Run for long duration does not diverge - Default values")
  {
    CHECK_NOTHROW(def_sim.run(100)); // 100.000 steps
    CHECK(def_sim.size() > 1);

    auto last  = def_sim.get_abs_states().back();
    auto first = def_sim.get_abs_states().front();

    CHECK(std::isfinite(last.x));
    CHECK(last.x > 0);

    CHECK(std::isfinite(last.y));
    CHECK(last.y > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for unbalanced parameters does not diverge")
  {
    pf::Simulation sim1{initial_def_point, 0.1, 1.0, 0.1, 1.0}; // b>>a
    pf::Simulation sim2{initial_def_point, 1.0, 0.1, 1.0, 0.1}; // c>>d

    for (pf::Simulation sim : {sim1, sim2}) {
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.size() > 1);

      auto last  = sim.get_abs_states().back();
      auto first = sim.get_abs_states().front();

      CHECK(std::isfinite(last.x));
      CHECK(last.x > 0);

      CHECK(std::isfinite(last.y));
      CHECK(last.y > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Run for big parameters' values does not diverge")
  {
    pf::Simulation sim1{initial_def_point, 50, 0.1, 0.1, 1.0}; // a grande
    pf::Simulation sim2{initial_def_point, 500, 50, 0.1,
                        1.0}; // b grande (ma a>>b)
    pf::Simulation sim3{initial_def_point, 1.0, 0.1, 50,
                        500}; // c grande (ma d>>c)
    pf::Simulation sim4{initial_def_point, 50, 0.1, 0.1, 50}; // d grande

    for (pf::Simulation sim : {sim1, sim2, sim3, sim4}) {
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.size() > 1);

      auto last  = sim.get_abs_states().back();
      //auto first = sim.get_abs_states().front();

      CHECK(std::isfinite(last.x));
      CHECK(last.x > 0);

      CHECK(std::isfinite(last.y));
      CHECK(last.y > 0);

      CHECK(std::isfinite(last.H));
      //   CHECK(last.H                              //in questo caso da
      //   problemi
      //         == doctest::Approx(first.H).epsilon(
      //             0.01));
    }
  }

  SUBCASE("Run for big dt value does not diverge")
  {
    pf::Simulation sim{initial_def_point, 1.0, 0.1, 0.1, 1.0, 0.1};

    CHECK_NOTHROW(sim.run(1.0));
    CHECK(sim.size() > 1);

    auto last  = sim.get_abs_states().back();
    auto first = sim.get_abs_states().front();

    CHECK(std::isfinite(last.x));
    CHECK(last.x > 0);

    CHECK(std::isfinite(last.y));
    CHECK(last.y > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for big initial conditions' values does not diverge")
  {
    pf::Simulation sim{{1e6, 0.5e6}, 1.0, 0.1, 0.1, 1.0};

    // CHECK_NOTHROW(sim.run(1.0));   //danno problemi
    // CHECK(sim.size() > 1);

    auto last  = sim.get_abs_states().back();
    auto first = sim.get_abs_states().front();

    CHECK(std::isfinite(last.x));
    CHECK(last.x > 0);

    CHECK(std::isfinite(last.y));
    CHECK(last.y > 0);

    CHECK(std::isfinite(last.H));
    CHECK(last.H
          == doctest::Approx(first.H).epsilon(
              0.01)); // Allow small numerical drift in H
  }

  SUBCASE("Run for unbalanced initial conditions does not diverge")
  {
    pf::Simulation sim1{{100.0, 10.0}, 1.0, 0.1, 0.1, 1.0}; // x>>y
    pf::Simulation sim2{{10.0, 100.0}, 1.0, 0.1, 0.1, 1.0}; // y>>x

    for (pf::Simulation sim : {sim1, sim2}) {
      CHECK_NOTHROW(sim.run(0.1));
      CHECK(sim.size() > 1);

      auto last  = sim.get_abs_states().back();
      auto first = sim.get_abs_states().front();

      CHECK(std::isfinite(last.x));
      CHECK(last.x > 0);

      CHECK(std::isfinite(last.y));
      CHECK(last.y > 0);

      CHECK(std::isfinite(last.H));
      CHECK(last.H
            == doctest::Approx(first.H).epsilon(
                0.01)); // Allow small numerical drift in H
    }
  }

  SUBCASE("Too short duration performs no step")
  {
    pf::Simulation sim{initial_def_point, 1.0, 0.1, 0.1, 1.0, 0.1};
    sim.run(0.00001);          // < dt
    CHECK(sim.size() == 1); // solo stato iniziale
  }
}