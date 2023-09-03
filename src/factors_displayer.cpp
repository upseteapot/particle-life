#include "factors_displayer.hpp"


void FactorsDisplayer::create(AssetsManager *assets_manager, FactorsDisplayerStates *factors_displayer_states, ParticleHandlerStates *particle_handler_states, sf::Vector2f position)
{
    m_assets_manager = assets_manager;
    m_factors_displayer_states = factors_displayer_states;
    m_particle_handler_states = particle_handler_states;
    m_position = position;

    m_value_text.setFont(m_assets_manager->get_font("pt_mono"));
    m_value_text.setCharacterSize(18);
    m_value_text.setFillColor(m_factors_displayer_states->value_text_color);
    m_value_text.setString(" 0.000");

    sf::FloatRect m_value_size = m_value_text.getLocalBounds();
    m_value_container.setSize(sf::Vector2f(m_value_size.width + m_factors_displayer_states->value_container_offset * 2.0f, m_value_size.height + m_factors_displayer_states->value_container_offset * 2.0f));
    m_value_container.setOrigin(-m_value_size.left + m_factors_displayer_states->value_container_offset, -m_value_size.top + m_factors_displayer_states->value_container_offset);
    m_value_container.setFillColor(m_factors_displayer_states->value_container_color);

    m_square_size = m_factors_displayer_states->matrix_total_size / (float)PARTICLE_COLORS;
    m_cells_shape.resize(PARTICLE_COLORS * PARTICLE_COLORS);
    for (int y=0; y < PARTICLE_COLORS; y++)
    for (int x=0; x < PARTICLE_COLORS; x++)
    {
        m_cells_shape[x + y * PARTICLE_COLORS].setSize(sf::Vector2f(m_square_size - m_factors_displayer_states->offset * 2, m_square_size - m_factors_displayer_states->offset * 2));
        m_cells_shape[x + y * PARTICLE_COLORS].setOrigin(-m_factors_displayer_states->offset, -m_factors_displayer_states->offset);
        m_cells_shape[x + y * PARTICLE_COLORS].setPosition(m_position + sf::Vector2f(x * m_square_size, y * m_square_size));
        m_cells_shape[x + y * PARTICLE_COLORS].setOutlineColor(sf::Color::White);
    }

    m_colors_shape.resize(PARTICLE_COLORS);
    for (std::size_t i=0; i < m_colors_shape.size(); i++)
    {
        m_colors_shape[i].setRadius(m_factors_displayer_states->color_indicator_radius);
        m_colors_shape[i].setOrigin(m_factors_displayer_states->color_indicator_radius, m_factors_displayer_states->color_indicator_radius);
    }

    m_set_cells_colors(m_factors_displayer_states->transparency);
}

void FactorsDisplayer::render(sf::RenderWindow &renderer)
{
    for (sf::RectangleShape &cell_shape : m_cells_shape)
        renderer.draw(cell_shape);

    for (int y=0; y < PARTICLE_COLORS; y++)
    {
        m_colors_shape[y].setPosition(m_position + sf::Vector2f(-m_factors_displayer_states->color_indicator_offset, m_square_size * (y + 0.5f)));
        renderer.draw(m_colors_shape[y]);
    }

    for (int x=0; x < PARTICLE_COLORS; x++)
    {
        m_colors_shape[x].setPosition(m_position + sf::Vector2f(m_square_size * (x + 0.5f), -m_factors_displayer_states->color_indicator_offset));
        renderer.draw(m_colors_shape[x]);
    }

    if (m_active) 
    {
        renderer.draw(m_value_container);
        renderer.draw(m_value_text);
    }
}

void FactorsDisplayer::update(sf::Vector2f gui_mouse_position, float elapsed_time)
{
    bool is_mouse_hovering = m_is_mouse_hovering(gui_mouse_position);
    if (m_active && !is_mouse_hovering) 
    {
        m_cells_shape[m_selected_index].setOutlineThickness(0);
        m_active = false;
        m_set_cells_colors(m_factors_displayer_states->transparency);
    } else if (!m_active && is_mouse_hovering) 
    {
        m_active = true;
        m_set_cells_colors(255);
    }

    if (m_active) {
        sf::Vector2i selected = static_cast<sf::Vector2i>((gui_mouse_position - m_position) / m_square_size);

        if (selected.x < 0)
            selected.x = 0;
        else if (selected.x >= PARTICLE_COLORS)
            selected.x = PARTICLE_COLORS - 1;
        if (selected.y < 0)
            selected.y = 0;
        else if (selected.y >= PARTICLE_COLORS)
            selected.y = PARTICLE_COLORS - 1;
        
        int index = selected.x + selected.y * PARTICLE_COLORS;

        std::stringstream formated_value;
        if (m_particle_handler_states->factors[index] >= 0)
            formated_value << '+';
        formated_value << std::fixed << std::setprecision(3) << m_particle_handler_states->factors[index];

        m_cells_shape[index].setOutlineThickness(1);
        m_value_text.setString(formated_value.str());
        m_value_text.setPosition(gui_mouse_position + m_factors_displayer_states->value_text_offset);
        m_value_container.setPosition(gui_mouse_position + m_factors_displayer_states->value_text_offset);

        if (index != m_selected_index)
            m_cells_shape[m_selected_index].setOutlineThickness(0);

        m_selected_index = index;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            m_particle_handler_states->factors[index] -= 0.5f * elapsed_time;
            m_set_cells_colors(255);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            m_particle_handler_states->factors[index] += 0.5f * elapsed_time;
            m_set_cells_colors(255);
        }
    }
}

void FactorsDisplayer::m_set_cells_colors(int transparency)
{
    sf::Color color;
    for (std::size_t i=0; i < m_colors_shape.size(); i++) 
    {
        color = m_particle_handler_states->colors[i];
        m_colors_shape[i].setFillColor(sf::Color(color.r, color.g, color.b, transparency));  
    }

    float min_value = -1.0f, max_value = 1.0f;
    for (std::size_t i=0; i < m_particle_handler_states->factors.size(); i++) 
    {
        if (m_particle_handler_states->factors[i] > max_value)
            max_value = m_particle_handler_states->factors[i];
        else if (m_particle_handler_states->factors[i] < min_value)
            min_value = m_particle_handler_states->factors[i];
    }

    sf::Color cell_color;
    for (std::size_t i=0; i < m_particle_handler_states->factors.size(); i++)
    {
        if (m_particle_handler_states->factors[i] < 0) 
            cell_color = lerp_colors(
                m_particle_handler_states->factors[i] / min_value,
                m_factors_displayer_states->neutral_color,
                m_factors_displayer_states->min_color
            );
        else if (m_particle_handler_states->factors[i] > 0)
            cell_color = lerp_colors(
                m_particle_handler_states->factors[i] / max_value,
                m_factors_displayer_states->neutral_color,
                m_factors_displayer_states->max_color
            );
        else 
            cell_color = m_factors_displayer_states->neutral_color;

        m_cells_shape[i].setFillColor(sf::Color(cell_color.r, cell_color.g, cell_color.b, transparency));
    }
}

bool FactorsDisplayer::m_is_mouse_hovering(sf::Vector2f mouse_position)
{
    return m_position.x <= mouse_position.x && mouse_position.x <= m_position.x + m_factors_displayer_states->matrix_total_size 
        && m_position.y <= mouse_position.y && mouse_position.y <= m_position.y + m_factors_displayer_states->matrix_total_size;
}
