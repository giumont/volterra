#include "graph_renderer.hpp"

#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace pf {

GraphRenderer::GraphRenderer(const pf::Simulation& sim)
    : sim_(sim)
{}

void GraphRenderer::run_visual()
{
  const int width  = 800;
  const int height = 600;

  sf::RenderWindow window(sf::VideoMode(width, height),
                          "Lotka-Volterra Simulation");

  sf::Font font;
  if (!font.loadFromFile("assets/fonts/OpenSans-Regular.ttf")) {
    std::cerr << "Could not load font from file!\n";
    return;
  }

  std::vector<double> time = sim_.get_time_series();
  std::vector<double> prey = sim_.get_x_series();
  std::vector<double> pred = sim_.get_y_series();
  std::vector<double> H    = sim_.get_H_series();

  if (time.empty()) {
    std::cerr << "No data to plot!" << std::endl;
    return;
  }

  double t_min = time.front();
  double t_max = time.back();

  auto minmax_prey = std::minmax_element(prey.begin(), prey.end());
  auto minmax_pred = std::minmax_element(pred.begin(), pred.end());
  auto minmax_H    = std::minmax_element(H.begin(), H.end());

  double y_min =
      std::min({*minmax_prey.first, *minmax_pred.first, *minmax_H.first});
  double y_max =
      std::max({*minmax_prey.second, *minmax_pred.second, *minmax_H.second});

  const int margin = 50;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);

    // Assi
    sf::Vertex x_axis[] = {
        sf::Vertex(sf::Vector2f(static_cast<float>(margin),
                                static_cast<float>(height - margin)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(static_cast<float>(width - margin),
                                static_cast<float>(height - margin)),
                   sf::Color::White)};
    sf::Vertex y_axis[] = {
        sf::Vertex(sf::Vector2f(static_cast<float>(margin),
                                static_cast<float>(margin)),
                   sf::Color::White),
        sf::Vertex(sf::Vector2f(static_cast<float>(margin),
                                static_cast<float>(height - margin)),
                   sf::Color::White)};
    window.draw(x_axis, 2, sf::Lines);
    window.draw(y_axis, 2, sf::Lines);

    // Funzioni di mappatura
    auto mapX = [&](double t) {
      return static_cast<float>(
          margin + (t - t_min) / (t_max - t_min) * (width - 2 * margin));
    };
    auto mapY = [&](double y) {
      return static_cast<float>(height - margin
                                - (y - y_min) / (y_max - y_min)
                                      * (height - 2 * margin));
    };

    // Disegna ticks asse X
    int num_ticks = 10;
    for (int i = 0; i <= num_ticks; ++i) {
      double t = t_min + (t_max - t_min) * i / num_ticks;
      float x  = mapX(t);

      sf::Vertex tick[] = {
          sf::Vertex(sf::Vector2f(x, static_cast<float>(height - margin)),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(x, static_cast<float>(height - margin + 5)),
                     sf::Color::White)};
      window.draw(tick, 2, sf::Lines);

      std::ostringstream oss;
      oss << std::fixed << std::setprecision(1) << t;
      sf::Text label(oss.str(), font, 12);
      label.setFillColor(sf::Color::White);
      label.setPosition(x - 10.f, static_cast<float>(height - margin + 8));
      window.draw(label);
    }

    // Disegna ticks asse Y
    for (int i = 0; i <= num_ticks; ++i) {
      double y    = y_min + (y_max - y_min) * i / num_ticks;
      float y_pos = mapY(y);

      sf::Vertex tick[] = {
          sf::Vertex(sf::Vector2f(static_cast<float>(margin - 5), y_pos),
                     sf::Color::White),
          sf::Vertex(sf::Vector2f(static_cast<float>(margin), y_pos),
                     sf::Color::White)};
      window.draw(tick, 2, sf::Lines);

      std::ostringstream oss;
      oss << std::fixed << std::setprecision(1) << y;
      sf::Text label(oss.str(), font, 12);
      label.setFillColor(sf::Color::White);
      label.setPosition(5.f, y_pos - 8.f);
      window.draw(label);
    }

    // Disegna curve
    auto drawCurve = [&](const std::vector<double>& ys, sf::Color color) {
      std::vector<sf::Vertex> vertices;
      for (size_t i = 0; i < ys.size(); ++i) {
        vertices.emplace_back(sf::Vector2f(mapX(time[i]), mapY(ys[i])), color);
      }
      window.draw(&vertices[0], vertices.size(), sf::LineStrip);
    };
    drawCurve(prey, sf::Color::Green);
    drawCurve(pred, sf::Color::Red);
    drawCurve(H, sf::Color::Cyan);

    // Legenda
    sf::Text legend_prey("Prey", font, 14);
    legend_prey.setFillColor(sf::Color::Green);
    legend_prey.setPosition(static_cast<float>(width - 100), 20.f);

    sf::Text legend_pred("Predator", font, 14);
    legend_pred.setFillColor(sf::Color::Red);
    legend_pred.setPosition(static_cast<float>(width - 100), 45.f);

    sf::Text legend_H("H", font, 14);
    legend_H.setFillColor(sf::Color::Cyan);
    legend_H.setPosition(static_cast<float>(width - 100), 70.f);

    window.draw(legend_prey);
    window.draw(legend_pred);
    window.draw(legend_H);

    window.display();
  }
}

} // namespace pf
