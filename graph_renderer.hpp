#ifndef GRAPH_RENDERER_HPP
#define GRAPH_RENDERER_HPP

#include "volterra.hpp"
#include <SFML/Graphics.hpp>

class GraphRenderer
{
 public:
  GraphRenderer(const pf::Simulation& sim);
  void run_visual(); // mostra la finestra con i grafici

 private:
  const pf::Simulation& sim_;
  // void drawGraph(...); // helper privati
};

#endif