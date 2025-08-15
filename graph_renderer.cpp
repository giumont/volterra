#include "graph_renderer.hpp"
#include "graph_options.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace pf {

GraphRenderer::GraphRenderer(const pf::Simulation sim)
    : sim_(sim)
{}

// --- GraphRenderer private methods ---

sf::Font GraphRenderer::initializeFont() const
{
  sf::Font font;
  if (!font.loadFromFile(font_path)) {
    throw std::runtime_error(
        "[Error] Font file not found in the project directory.\n");
  }
  return font;
}

void GraphRenderer::drawAxes(sf::RenderTarget& target, const sf::Font& font,
                             int width, int height, double t_min, double t_max,
                             double y_min, double y_max,
                             const std::string& x_label_str,
                             const std::string& y_label_str) const
{
  float f_margin = static_cast<float>(plot_margin);
  float f_width  = static_cast<float>(width);
  float f_height = static_cast<float>(height);

  // Axes lines
  sf::Vertex x_axis[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_height - f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White)};
  sf::Vertex y_axis[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin, f_height - f_margin),
                 sf::Color::White)};
  target.draw(x_axis, 2, sf::Lines);
  target.draw(y_axis, 2, sf::Lines);

  // Mapping functions from data to screen coordinates
  auto mapX = [&](double t) {
    return static_cast<float>(plot_margin
                              + (t - t_min) / (t_max - t_min)
                                    * (width - 2 * plot_margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - plot_margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * plot_margin));
  };

  // X axis ticks and labels
  for (int i = 0; i <= num_ticks; ++i) {
    double t          = t_min + (t_max - t_min) * i / num_ticks;
    float x           = mapX(t);
    sf::Vertex tick[] = {
        sf::Vertex(sf::Vector2f(x, f_height - f_margin), sf::Color::White),
        sf::Vertex(sf::Vector2f(x, f_height - f_margin + tick_length),
                   sf::Color::White)};
    target.draw(tick, 2, sf::Lines);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << t;
    sf::Text label(oss.str(), font, font_size_axes_labels);
    label.setFillColor(sf::Color::White);
    label.setPosition(x - 10.f, f_height - f_margin + tick_length + 3.f);
    target.draw(label);
  }

  // Y axis ticks and labels
  for (int i = 0; i <= num_ticks; ++i) {
    double y    = y_min + (y_max - y_min) * i / num_ticks;
    float y_pos = mapY(y);

    sf::Vertex tick[] = {
        sf::Vertex(sf::Vector2f(f_margin - tick_length, y_pos),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(f_margin, y_pos), sf::Color::White)};
    target.draw(tick, 2, sf::Lines);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << y;
    sf::Text label(oss.str(), font, font_size_axes_labels);
    label.setFillColor(sf::Color::White);
    label.setPosition(5.f, y_pos - 8.f);
    target.draw(label);
  }

  // Arrows at axes ends
  sf::Vertex arrow_x[] = {
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin - 10, f_height - f_margin - 5),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin - 10, f_height - f_margin + 5),
                 sf::Color::White)};
  target.draw(arrow_x, 4, sf::Lines);

  sf::Vertex arrow_y[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin - 5, f_margin + 10), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin + 5, f_margin + 10), sf::Color::White)};
  target.draw(arrow_y, 4, sf::Lines);

  // Axis labels
  sf::Text x_label(x_label_str, font, font_size_axis_titles);
  x_label.setFillColor(sf::Color::White);
  x_label.setPosition(f_width - f_margin - 30.f, f_height - f_margin + 25.f);
  target.draw(x_label);

  sf::Text y_label(y_label_str, font, font_size_axis_titles);
  y_label.setFillColor(sf::Color::White);
  y_label.setPosition(f_margin - 40.f, f_margin - 30.f);
  target.draw(y_label);
}

void GraphRenderer::plotTimeSeries(sf::RenderTarget& target,
                                   const sf::Font& font, int width,
                                   int height) const
{
  std::vector<pf::SpeciesState> abs_states = sim_.getAbsStates();

  std::vector<double> time = sim_.getSeries(abs_states, &pf::SpeciesState::t);
  std::vector<double> preys =
      sim_.getSeries(abs_states, &pf::SpeciesState::preys);
  std::vector<double> preds =
      sim_.getSeries(abs_states, &pf::SpeciesState::preds);
  std::vector<double> H = sim_.getSeries(abs_states, &pf::SpeciesState::H);

  if (time.empty()) {
    throw std::runtime_error("No data to plot.");
  }

  double t_min = std::min(0.0, time.front());
  double t_max = time.back();
  double y_min = std::min({*std::min_element(preys.begin(), preys.end()),
                           *std::min_element(preds.begin(), preds.end()),
                           *std::min_element(H.begin(), H.end())});
  double y_max = std::max({*std::max_element(preys.begin(), preys.end()),
                           *std::max_element(preds.begin(), preds.end()),
                           *std::max_element(H.begin(), H.end())});

  auto mapX = [&](double t) {
    return static_cast<float>(plot_margin
                              + (t - t_min) / (t_max - t_min)
                                    * (width - 2 * plot_margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - plot_margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * plot_margin));
  };

  drawAxes(target, font, width, height, t_min, t_max, y_min, y_max, "Time",
           "Population");

  auto drawCurve = [&](const std::vector<double>& ys, sf::Color color) {
    std::vector<sf::Vertex> vertices;
    for (size_t i = 0; i < ys.size(); ++i) {
      vertices.emplace_back(sf::Vector2f(mapX(time[i]), mapY(ys[i])), color);
    }
    target.draw(&vertices[0], vertices.size(), sf::LineStrip);
  };
  drawCurve(preys, sf::Color::Green);
  drawCurve(preds, sf::Color::Red);
  drawCurve(H, sf::Color::Cyan);

  // Legend
  sf::Text legend_preys("Prey", font, font_size_legend);
  legend_preys.setFillColor(sf::Color::Green);
  legend_preys.setPosition(static_cast<float>(width) - 50, 20.f);

  sf::Text legend_pred("Predator", font, font_size_legend);
  legend_pred.setFillColor(sf::Color::Red);
  legend_pred.setPosition(static_cast<float>(width) - 50, 45.f);

  sf::Text legend_H("H", font, font_size_legend);
  legend_H.setFillColor(sf::Color::Cyan);
  legend_H.setPosition(static_cast<float>(width) - 50, 70.f);

  target.draw(legend_preys);
  target.draw(legend_pred);
  target.draw(legend_H);
}

void GraphRenderer::plotOrbits(sf::RenderTarget& target, const sf::Font& font,
                               int width, int height) const
{
  std::vector<pf::SpeciesState> abs_states = sim_.getAbsStates();

  std::vector<double> preys =
      sim_.getSeries(abs_states, &pf::SpeciesState::preys);
  std::vector<double> preds =
      sim_.getSeries(abs_states, &pf::SpeciesState::preds);

  if (preys.empty() || preds.empty()) {
    throw std::runtime_error("Error: simulation data is empty.");
  }

  double x_min = std::min(0.0, *std::min_element(preys.begin(), preys.end()));
  double x_max = *std::max_element(preys.begin(), preys.end());
  double y_min = std::min(0.0, *std::min_element(preds.begin(), preds.end()));
  double y_max = *std::max_element(preds.begin(), preds.end());

  auto mapX = [&](double x) {
    return static_cast<float>(plot_margin
                              + (x - x_min) / (x_max - x_min)
                                    * (width - 2 * plot_margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - plot_margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * plot_margin));
  };

  drawAxes(target, font, width, height, x_min, x_max, y_min, y_max, "Preys",
           "Predators");

  std::vector<sf::Vertex> orbit;
  for (size_t i = 0; i < preys.size(); ++i) {
    orbit.emplace_back(sf::Vector2f(mapX(preys[i]), mapY(preds[i])),
                       sf::Color::Magenta);
  }
  target.draw(&orbit[0], orbit.size(), sf::LineStrip);

  // Draw arrows on the orbit

  for (size_t i = 0; i + 1 < preys.size(); i += orbit_arrow_step) {
    sf::Vector2f p1(mapX(preys[i]), mapY(preds[i]));
    sf::Vector2f p2(mapX(preys[i + 1]), mapY(preds[i + 1]));

    sf::Vector2f dir = p2 - p1; // distance between two points
    float length     = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0)
      continue;
    dir /= length; // unitary vector

    sf::Vector2f perp(-dir.y, dir.x); // vector perp to distance

    sf::ConvexShape arrow;
    arrow.setPointCount(3); // drawing a triangle
    arrow.setPoint(0, p2);
    arrow.setPoint(1,
                   p2 - dir * orbit_arrow_size + perp * (orbit_arrow_size / 2));
    arrow.setPoint(2,
                   p2 - dir * orbit_arrow_size - perp * (orbit_arrow_size / 2));
    arrow.setFillColor(sf::Color::Magenta);

    target.draw(arrow);
  }

  // Draw equilibrium points
  Parameters params                                = sim_.getParams();
  std::vector<std::pair<double, double>> eq_points = {
      {0.0, 0.0}, {params.d / params.c, params.a / params.b}};

  for (const auto& eq_point : eq_points) {
    sf::CircleShape point(eq_point_radius);
    point.setFillColor(sf::Color::Yellow);
    point.setOrigin(eq_point_radius, eq_point_radius); // center the circle
    point.setPosition(mapX(eq_point.first), mapY(eq_point.second));
    target.draw(point);
  }

  // Legend
  sf::Text legend_eq_points("Equilibrium points", font, font_size_legend);
  legend_eq_points.setFillColor(sf::Color::Yellow);
  legend_eq_points.setPosition(static_cast<float>(width) - 100, 20.f);

  target.draw(legend_eq_points);
}

// --- GraphRenderer public methods ---

void GraphRenderer::drawSinglePlot(const PlotConfig& plot_config) const
{
  sf::RenderWindow window(
      sf::VideoMode(window_width_single_plot, window_height_single_plot),
      plot_config.title);

  sf::Font font = initializeFont();

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);

    if (plot_config.type == PlotConfig::Type::TimeSeries) {
      plotTimeSeries(window, font, window_width_single_plot,
                     window_height_single_plot);
    } else {
      plotOrbits(window, font, window_width_single_plot,
                 window_height_single_plot);
    }

    window.display();
  }
}

void GraphRenderer::drawCombinedPlots() const
{
  sf::RenderWindow window(
      sf::VideoMode(window_width_combined, window_height_combined),
      "Time Series & Orbits");

  sf::Font font = initializeFont();

  // Division of graphic window in two parts
  sf::View leftView(sf::FloatRect(0, 0,
                                  static_cast<float>(window_width_combined / 2),
                                  static_cast<float>(window_height_combined)));
  leftView.setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 1.f));

  sf::View rightView(
      sf::FloatRect(0, 0, static_cast<float>(window_width_combined / 2),
                    static_cast<float>(window_height_combined)));
  rightView.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 1.f));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);

    // Drawing TimeSeries in the upper half of window
    window.setView(leftView);
    plotTimeSeries(window, font, window_width_combined / 2 - plot_margin,
                   window_height_combined - plot_margin);

    // Drawing Orbits in the lower half of window
    window.setView(rightView);
    plotOrbits(window, font, window_width_combined / 2 - plot_margin,
               window_height_combined - plot_margin);

    window.display();
  }
}

} // namespace pf
