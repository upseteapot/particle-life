#include "assets_manager.hpp"


AssetsManager::~AssetsManager()
{
    delete[] m_fonts.data;
}

void AssetsManager::create_fonts(std::string default_font_path, std::size_t size)
{
    m_fonts.size = size;
    m_fonts.data = new LabeledElement<sf::Font>[size];
    
    if (!m_fonts.default_element.loadFromFile(default_font_path))
        exit(-1);
}

void AssetsManager::add_font(std::string label, std::string font_path)
{
    if (m_fonts.index < m_fonts.size)
    {
        sf::Font font;
        if (!font.loadFromFile(font_path))
            return;
        m_fonts.data[m_fonts.index++] = LabeledElement<sf::Font>{label, font};
    }
}

const sf::Font &AssetsManager::get_font(std::string label)
{
    if (m_fonts.size == 0)
    {
        std::cout << "Fonts haven't been initialised yet.\n";
        return m_fonts.default_element;
    }

    for (std::size_t i=0; i < m_fonts.size; i++)
        if (m_fonts.data[i].label == label)
            return m_fonts.data[i].element;

    std::cout << "Could not found font labeled '" << label << "'.\n";
    return m_fonts.default_element;
}
