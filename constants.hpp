#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// WARNING: Do not deviate too much from these values to avoid program crashes
// or unexpected behavior.
// All parameter values and time-related constants must be positive.

namespace pf {

// Default ODE parameters for the predator-prey model
constexpr double def_a = 1.0; // prey birth rate
constexpr double def_b = 0.1; // predation rate
constexpr double def_c = 0.1; // predator reproduction rate per prey eaten
constexpr double def_d = 1.0; // predator death rate

// Maximum allowed value for ODE parameters
constexpr double max_param_value = 100.0;

// Initial populations (can be non-integer as density)
constexpr double def_initial_preys = 10.0;
constexpr double def_initial_preds = 5.0;

// Maximum allowed value for initial populations
constexpr double max_initial_population = 500.0;

// Time step for numerical integration
constexpr double def_dt = 0.01;

// Maximum simulation duration allowed
constexpr double max_duration = 100.0;

// Number of decimal digits used when printing floating-point values in
// outputs
constexpr int precision = 1;

} // namespace pf

#endif // CONSTANTS_HPP
