#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <vector>
#include <thread>

#include "assets_manager.hpp"
#include "particle_handler.hpp"
#include "factors_displayer.hpp"


int main(void)
{
    srand(time(0));
    const int width = 1920, height = 1080;

    int style = sf::Style::Close | sf::Style::Fullscreen;
    sf::VideoMode shape(width, height);
    sf::ContextSettings context;
    context.antialiasingLevel = 4;

    sf::RenderWindow renderer(shape, "Particle Life", style, context);
    
    float elapsed_time;
    sf::Event event;
    sf::Clock clock;
    sf::Vector2i mouse_position;

    sf::View default_view;
    default_view.setSize(width, height);
    default_view.setCenter(0, 0);

    sf::View gui_view;
    gui_view.setSize(width, height);
    gui_view.setCenter(width / 2.0f, height / 2.0f);

    
    std::size_t threads_amount = 5;
    std::thread threads[threads_amount];

    AssetsManager assets_manager;
    assets_manager.create_fonts("assets/pt_mono.ttf", 1);
    assets_manager.add_font("pt_mono", "assets/pt_mono.ttf");

    ParticleHandlerStates particle_handler_states;
    ParticleHandler particle_handler;
    ParticleHandlerStates::create_random(particle_handler_states);
    particle_handler.create(&particle_handler_states, 3000, 1920, 1080);

    FactorsDisplayerStates factors_displayer_states;
    FactorsDisplayer factors_displayer;
    factors_displayer.create(&assets_manager, &factors_displayer_states, &particle_handler_states, sf::Vector2f(30, 30));

    while (renderer.isOpen())
    {

        elapsed_time = clock.restart().asSeconds();

        while (renderer.pollEvent(event)) 
        {
            switch (event.type) 
            {
                case sf::Event::Closed:
                    renderer.close();
                break;

                default:
                break;
            }
        }

        mouse_position = sf::Mouse::getPosition(renderer);

        renderer.clear(sf::Color::Black);

        // Default view
        renderer.setView(default_view);
        
        for (std::size_t i=0; i < threads_amount; i++)
            threads[i] = std::thread(&ParticleHandler::apply_forces, &particle_handler, elapsed_time, i, threads_amount);
        for (std::size_t i=0; i < threads_amount; i++)
            threads[i].join();

        for (std::size_t i=0; i < threads_amount; i++)
            threads[i] = std::thread(&ParticleHandler::update, &particle_handler, elapsed_time, i, threads_amount);
        for (std::size_t i=0; i < threads_amount; i++)
            threads[i].join();

       //particle_handler.update(elapsed_time, 0, 1);

        particle_handler.render(renderer);

        // GUI view
        renderer.setView(gui_view);
        factors_displayer.update(renderer.mapPixelToCoords(mouse_position), elapsed_time);
        factors_displayer.render(renderer);

        renderer.display();

    }

    return 0;

}
