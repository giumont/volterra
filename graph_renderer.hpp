#ifndef GRAPH_RENDERER_HPP
#define GRAPH_RENDERER_HPP

#include "volterra.hpp"
#include <SFML/Graphics.hpp>

namespace pf {

class GraphRenderer
{
 private:
  const pf::Simulation sim_;
  sf::Font initializeFont() const;
  void drawAxes(sf::RenderWindow& window, const sf::Font& font, int width,
                int height, int margin, double t_min, double t_max,
                double y_min, double y_max, const std::string& x_label_str,
                const std::string& y_label_str) const;

 public:
  GraphRenderer(const pf::Simulation sim);
  void drawTimeSeries() const; // mostra la finestra con i grafici
  void drawOrbits() const;
};
} // namespace pf

#endif