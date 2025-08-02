#ifndef GRAPH_RENDERER_HPP
#define GRAPH_RENDERER_HPP

#include "volterra.hpp"
#include <SFML/Graphics.hpp>

namespace pf {

class GraphRenderer
{
 private:
  const pf::Simulation& sim_;
  // void drawGraph(...); // helper privati

 public:
  GraphRenderer(const pf::Simulation& sim);
  void run_visual(); // mostra la finestra con i grafici
};
} // namespace pf

#endif