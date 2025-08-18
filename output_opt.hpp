#ifndef OUTPUT_OPT_HPP
#define OUTPUT_OPT_HPP

#include <string>

namespace pf {

// --- .txt outputs (text) ---
// Columns' width in output file
constexpr int width_time = 10;
constexpr int width_prey = 10;
constexpr int width_pred = 12;
constexpr int width_H    = 10;

// --- .png outputs (images) ---
// Window settings
constexpr int window_width_single_plot  = 800;
constexpr int window_height_single_plot = 600;
constexpr int window_width_combined     = 1400;
constexpr int window_height_combined    = 800;

constexpr int plot_margin = 50;

constexpr float display_time = 5; // time (in seconds) graphic window stays
                                  // open if not manually closed

constexpr bool combined = true; // if true plots visualized together and saved
                                // in same file, else separately

// Font settings
const std::string font_path = "assets/Open_Sans/static/OpenSans-Regular.ttf";

constexpr float title_height = 50; // space reserved to title in graphs
constexpr unsigned int font_size_title = 24;

constexpr unsigned int font_size_axes_labels = 12; // axes labels and infos
constexpr unsigned int font_size_axis_titles = 14;
constexpr unsigned int font_size_legend      = 15;

// Axis ticks
constexpr int num_ticks     = 10;
constexpr float tick_length = 5;

// Orbit arrow settings
constexpr size_t orbit_arrow_step = 50; // points between two arrows
constexpr float orbit_arrow_size  = 10; // arrow lenght

// Equilibrium point visualization
constexpr float eq_point_radius = 5; // pixels of radius

// Number of decimal digits used when printing floating-point values in
// outputs
constexpr int precision = 1;

} // namespace pf

#endif // OUTPUT_OPT_HPP