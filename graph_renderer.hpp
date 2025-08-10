#ifndef GRAPH_RENDERER_HPP
#define GRAPH_RENDERER_HPP

#include "volterra.hpp"
#include <SFML/Graphics.hpp> // SFML Graphic library

namespace pf {

struct PlotConfig
{
  enum class Type
  {
    TimeSeries,
    Orbits
  };

  Type type;
  std::string title;
};

class GraphRenderer
{
 private:
  const pf::Simulation sim_;
  sf::Font initializeFont() const;
  void drawAxes(sf::RenderTarget& target, const sf::Font& font, int width,
                int height, int margin, double t_min, double t_max,
                double y_min, double y_max, const std::string& x_label_str,
                const std::string& y_label_str) const;

  void drawTimeSeries(sf::RenderTarget& target, const sf::Font& font, int width,
                      int height, int margin) const;
  void drawOrbits(sf::RenderTarget& target, const sf::Font& font, int width,
                  int height, int margin) const;

 public:
  GraphRenderer(const pf::Simulation sim);

  void drawSinglePlot(const PlotConfig& plot_config) const;

  void drawCombinedPlots() const;
};
} // namespace pf

#endif