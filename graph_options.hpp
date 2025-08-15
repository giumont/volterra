#include <string>

namespace pf {

// Window settings
constexpr int window_width_single_plot  = 800;
constexpr int window_height_single_plot = 600;
constexpr int window_width_combined     = 1680;
constexpr int window_height_combined    = 960;

// Plot margin
constexpr int plot_margin = 50;

// Font settings
const std::string font_path = "assets/Open_Sans/static/OpenSans-Regular.ttf";
constexpr unsigned int font_size_axes_labels = 12;
constexpr unsigned int font_size_axis_titles = 14;
constexpr unsigned int font_size_legend      = 15;

// Axis ticks
constexpr int num_ticks     = 10;
constexpr float tick_length = 5.f;

// Orbit arrow settings
constexpr size_t orbit_arrow_step = 50;   // points between two arrows
constexpr float orbit_arrow_size  = 10.f; // arrow lenght

// Equilibrium point visualization
constexpr float eq_point_radius = 5.f; // pixels of radius

} // namespace pf