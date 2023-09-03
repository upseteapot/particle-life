#pragma once
#include <SFML/Graphics.hpp>


inline sf::Color lerp_colors(float a, sf::Color from, sf::Color to)
{
    return sf::Color(
        (1 - a) * from.r + a * to.r,
        (1 - a) * from.g + a * to.g,
        (1 - a) * from.b + a * to.b
    );
}
