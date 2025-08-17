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

void saveToFile(const sf::Image&, const std::string& filename);

class GraphRenderer
{
 private:
  const pf::Simulation sim_;
  sf::Font initializeFont() const;
  void drawAxes(sf::RenderTarget& target, const sf::Font& font, int width,
                int height, double t_min, double t_max, double y_min,
                double y_max, const std::string& x_label_str,
                const std::string& y_label_str) const;

  void plotTimeSeries(sf::RenderTarget& target, const sf::Font& font, int width,
                      int height) const;
  void plotOrbits(sf::RenderTarget& target, const sf::Font& font, int width,
                  int height) const;

 public:
  GraphRenderer(const pf::Simulation sim);

  sf::Image drawSinglePlot(const PlotConfig& plot_config) const;

  sf::Image drawCombinedPlots();
};
} // namespace pf

#endif