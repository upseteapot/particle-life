#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <climits>

#include "utils.hpp"
#include "particle_handler.hpp"
#include "assets_manager.hpp"


struct FactorsDisplayerStates
{
    int transparency = 50;
    float offset = 2.0f;
    float matrix_total_size = 220.0f;
    float color_indicator_radius = 6.0f;
    float color_indicator_offset = 8.0f;
    float value_container_offset = 4.0f;
    sf::Vector2f value_text_offset = sf::Vector2f(10.0f, 12.0f);
    sf::Color max_color = sf::Color(50, 250, 80);
    sf::Color min_color = sf::Color(250, 50, 80);
    sf::Color neutral_color = sf::Color(23, 22, 30);
    sf::Color value_text_color = sf::Color(227, 210, 200); 
    sf::Color value_container_color = sf::Color(27, 27, 27);
};


class FactorsDisplayer
{
    public:
        FactorsDisplayer() = default;
        void create(AssetsManager *assets_manager, FactorsDisplayerStates *factors_displayer_states, ParticleHandlerStates *particle_handler_states, sf::Vector2f position);
        void update(sf::Vector2f gui_mouse_position, float elapsed_time);
        void render(sf::RenderWindow &renderer);

    private:
        AssetsManager *m_assets_manager;
        FactorsDisplayerStates *m_factors_displayer_states;
        ParticleHandlerStates *m_particle_handler_states;
        bool m_active = false;
        float m_square_size;
        int m_selected_index = 0;
        sf::Text m_value_text;
        sf::RectangleShape m_value_container;
        sf::Vector2f m_position;
        std::vector<sf::RectangleShape> m_cells_shape;
        std::vector<sf::CircleShape> m_colors_shape;

        void m_set_cells_colors(int transparency);
        bool m_is_mouse_hovering(sf::Vector2f mouse_position);
};
