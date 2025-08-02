#include "graph_renderer.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

GraphRenderer::GraphRenderer(const pf::Simulation& sim)
    : sim_(sim)
{
}

void GraphRenderer::run_visual()
{
    // Parametri finestra
    const int width = 800;
    const int height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "Lotka-Volterra Simulation");

    // Prendo i dati da simulazione
    auto time = sim_.get_time_series();
    auto prey = sim_.get_prey_series();
    auto pred = sim_.get_predator_series();
    auto H    = sim_.get_H_series();

    if (time.empty()) {
        std::cerr << "No data to plot!" << std::endl;
        return;
    }

    // Trova i minimi e massimi per scalare
    double t_min = time.front();
    double t_max = time.back();

    auto minmax_prey = std::minmax_element(prey.begin(), prey.end());
    auto minmax_pred = std::minmax_element(pred.begin(), pred.end());
    auto minmax_H    = std::minmax_element(H.begin(), H.end());

    double y_min = std::min({*minmax_prey.first, *minmax_pred.first, *minmax_H.first});
    double y_max = std::max({*minmax_prey.second, *minmax_pred.second, *minmax_H.second});

    // Margine per il grafico
    const int margin = 50;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Disegno assi
        sf::Vertex x_axis[] =
        {
            sf::Vertex(sf::Vector2f(margin, height - margin), sf::Color::White),
            sf::Vertex(sf::Vector2f(width - margin, height - margin), sf::Color::White)
        };

        sf::Vertex y_axis[] =
        {
            sf::Vertex(sf::Vector2f(margin, margin), sf::Color::White),
            sf::Vertex(sf::Vector2f(margin, height - margin), sf::Color::White)
        };

        window.draw(x_axis, 2, sf::Lines);
        window.draw(y_axis, 2, sf::Lines);

        // Funzione lambda per mappare valori reali a coordinate pixel
        auto mapX = [&](double t) {
            return margin + (t - t_min) / (t_max - t_min) * (width - 2 * margin);
        };
        auto mapY = [&](double y) {
            // Y in pixel: invertito perché 0 pixel è in alto
            return height - margin - (y - y_min) / (y_max - y_min) * (height - 2 * margin);
        };

        // Funzione per disegnare una curva da vettore dati (line strip)
        auto drawCurve = [&](const std::vector<double>& ys, sf::Color color) {
            std::vector<sf::Vertex> vertices;
            for (size_t i = 0; i < ys.size(); ++i) {
                vertices.emplace_back(
                    sf::Vector2f(mapX(time[i]), mapY(ys[i])), color);
            }
            window.draw(&vertices[0], vertices.size(), sf::LineStrip);
        };

        // Disegno le curve
        drawCurve(prey, sf::Color::Green);
        drawCurve(pred, sf::Color::Red);
        drawCurve(H, sf::Color::Cyan);

        window.display();
    }
}