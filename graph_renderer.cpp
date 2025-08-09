#include "graph_renderer.hpp"

#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace pf {

GraphRenderer::GraphRenderer(const pf::Simulation sim)
    : sim_(sim)
{}

sf::Font GraphRenderer::initializeFont() const
{
  sf::Font font;
  if (!font.loadFromFile("assets/Open_Sans/static/OpenSans-Regular.ttf")) {
    throw std::runtime_error(
        "[Error] Font file not found in the project directory.\n");
  }
  return font;
}

void GraphRenderer::drawAxes(sf::RenderWindow& window, const sf::Font& font,
                             int width, int height, int margin, double t_min,
                             double t_max, double y_min, double y_max,
                             const std::string& x_label_str,
                             const std::string& y_label_str) const
{
  float f_margin = static_cast<float>(margin);
  float f_width  = static_cast<float>(width);
  float f_height = static_cast<float>(height);
  // Assi
  sf::Vertex x_axis[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_height - f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White)};
  sf::Vertex y_axis[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin, f_height - f_margin),
                 sf::Color::White)};
  window.draw(x_axis, 2, sf::Lines);
  window.draw(y_axis, 2, sf::Lines);

  auto mapX = [&](double t) {
    return static_cast<float>(
        margin + (t - t_min) / (t_max - t_min) * (width - 2 * margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * margin));
  };

  const int num_ticks = 10;

  // Ticks asse X
  for (int i = 0; i <= num_ticks; ++i) {
    double t          = t_min + (t_max - t_min) * i / num_ticks;
    float x           = mapX(t);
    sf::Vertex tick[] = {
        sf::Vertex(sf::Vector2f(x, f_height - f_margin), sf::Color::White),
        sf::Vertex(sf::Vector2f(x, f_height - f_margin + 5), sf::Color::White)};
    window.draw(tick, 2, sf::Lines);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << t;
    sf::Text label(oss.str(), font, 12);
    label.setFillColor(sf::Color::White);
    label.setPosition(x - 10.f, f_height - f_margin + 8);
    window.draw(label);
  }

  // Ticks asse Y
  for (int i = 0; i <= num_ticks; ++i) {
    double y    = y_min + (y_max - y_min) * i / num_ticks;
    float y_pos = mapY(y);

    sf::Vertex tick[] = {
        sf::Vertex(sf::Vector2f(f_margin - 5, y_pos), sf::Color::White),
        sf::Vertex(sf::Vector2f(f_margin, y_pos), sf::Color::White)};
    window.draw(tick, 2, sf::Lines);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << y;
    sf::Text label(oss.str(), font, 12);
    label.setFillColor(sf::Color::White);
    label.setPosition(5.f, y_pos - 8.f);
    window.draw(label);
  }

  // Frecce
  sf::Vertex arrow_x[] = {
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin - 10, f_height - f_margin - 5),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin, f_height - f_margin),
                 sf::Color::White),
      sf::Vertex(sf::Vector2f(f_width - f_margin - 10, f_height - f_margin + 5),
                 sf::Color::White)};
  window.draw(arrow_x, 4, sf::Lines);

  sf::Vertex arrow_y[] = {
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin - 5, f_margin + 10), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin, f_margin), sf::Color::White),
      sf::Vertex(sf::Vector2f(f_margin + 5, f_margin + 10), sf::Color::White)};
  window.draw(arrow_y, 4, sf::Lines);

  // Etichette assi
  sf::Text x_label(x_label_str, font, 14);
  x_label.setFillColor(sf::Color::White);
  x_label.setPosition(f_width - f_margin - 30.f, f_height - f_margin + 25.f);
  window.draw(x_label);

  sf::Text y_label(y_label_str, font, 14);
  y_label.setFillColor(sf::Color::White);
  y_label.setPosition(f_margin - 40.f, f_margin - 30.f);
  window.draw(y_label);
}

void GraphRenderer::drawTimeSeries() const
{
  const int width  = 800;
  const int height = 600;
  const int margin = 50;

  // Creo la finestra qui
  sf::RenderWindow window1(sf::VideoMode(width, height), "Time Series");
  // Inizializzo il font
  sf::Font font = initializeFont();

  std::vector<pf::SpeciesState> abs_states = sim_.getAbsStates();

  std::vector<double> time = sim_.getSeries(abs_states, &pf::SpeciesState::t);
  std::vector<double> preys =
      sim_.getSeries(abs_states, &pf::SpeciesState::preys);
  std::vector<double> preds =
      sim_.getSeries(abs_states, &pf::SpeciesState::preds);
  std::vector<double> H = sim_.getSeries(abs_states, &pf::SpeciesState::H);

  if (time.empty()) {
    throw std::runtime_error("Error: no data to plot.");
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
    return static_cast<float>(
        margin + (t - t_min) / (t_max - t_min) * (width - 2 * margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * margin));
  };

  while (window1.isOpen()) {
    sf::Event event;
    while (window1.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window1.close();
    }

    window1.clear(sf::Color::Black);

    drawAxes(window1, font, width, height, margin, t_min, t_max, y_min, y_max,
             "Time", "Population");

    auto drawCurve = [&](const std::vector<double>& ys, sf::Color color) {
      std::vector<sf::Vertex> vertices;
      for (size_t i = 0; i < ys.size(); ++i) {
        vertices.emplace_back(sf::Vector2f(mapX(time[i]), mapY(ys[i])), color);
      }
      window1.draw(&vertices[0], vertices.size(), sf::LineStrip);
    };
    drawCurve(preys, sf::Color::Green);
    drawCurve(preds, sf::Color::Red);
    drawCurve(H, sf::Color::Cyan);

    sf::Text legend_preys("Prey", font, 14);
    legend_preys.setFillColor(sf::Color::Green);
    legend_preys.setPosition(width - 100, 20.f);

    sf::Text legend_pred("Predator", font, 14);
    legend_pred.setFillColor(sf::Color::Red);
    legend_pred.setPosition(width - 100, 45.f);

    sf::Text legend_H("H", font, 14);
    legend_H.setFillColor(sf::Color::Cyan);
    legend_H.setPosition(width - 100, 70.f);

    window1.draw(legend_preys);
    window1.draw(legend_pred);
    window1.draw(legend_H);

    window1.display();
  }
}

void GraphRenderer::drawOrbits() const
{
  const int width  = 800;
  const int height = 600;
  const int margin = 50;

  sf::RenderWindow window2(sf::VideoMode(width, height),
                           "Orbit Plot (Prey vs Predator)");
  sf::Font font = initializeFont();

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
    return static_cast<float>(
        margin + (x - x_min) / (x_max - x_min) * (width - 2 * margin));
  };
  auto mapY = [&](double y) {
    return static_cast<float>(height - margin
                              - (y - y_min) / (y_max - y_min)
                                    * (height - 2 * margin));
  };

  while (window2.isOpen()) {
    sf::Event event;
    while (window2.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window2.close();
    }

    window2.clear(sf::Color::Black);

    drawAxes(window2, font, width, height, margin, x_min, x_max, y_min, y_max,
             "Prey", "Predator");

    std::vector<sf::Vertex> orbit;
    for (size_t i = 0; i < preys.size(); ++i) {
      orbit.emplace_back(sf::Vector2f(mapX(preys[i]), mapY(preds[i])),
                         sf::Color::Magenta);
    }
    window2.draw(&orbit[0], orbit.size(), sf::LineStrip);

    window2.display();
  }
}

} // namespace pf
