#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>


template <typename T>
struct LabeledElement
{
    std::string label;
    T element;
};

template <typename T>
struct LabeledElementArray
{
    T default_element;
    LabeledElement<T> *data;
    std::size_t size = 0;
    std::size_t index = 0;
};


class AssetsManager
{
    public:
        AssetsManager() = default;
        ~AssetsManager();

        void create_fonts(std::string default_font_path, std::size_t size);
        void add_font(std::string font_path, std::string label);
        const sf::Font &get_font(std::string label);

    private:
        LabeledElementArray<sf::Font> m_fonts;
};
