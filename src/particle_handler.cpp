#include "particle_handler.hpp"


void ParticleHandlerStates::create_random(ParticleHandlerStates &states)
{
    states.factors.resize(PARTICLE_COLORS * PARTICLE_COLORS);
    for (std::size_t i=0; i < states.factors.size(); i++)
        states.factors[i] = (2.0f * rand() / RAND_MAX) - 1.0f;    
}

void ParticleHandlerStates::create_default(ParticleHandlerStates &states)
{
    states.factors.resize(PARTICLE_COLORS * PARTICLE_COLORS);
}


void ParticleHandler::create(ParticleHandlerStates *states, std::size_t particle_amount, float width, float height)
{
    m_states = states;

    m_container_size = sf::Vector2f(width, height);
    m_grid_size = sf::Vector2i((int)(2.0f * width / m_states->max_distance), (int)(2.0f * height / m_states->max_distance));
    m_grid_cell_size = sf::Vector2f(m_container_size.x / (float)m_grid_size.x, m_container_size.y / (float)m_grid_size.y);

    m_particle_shape.setRadius(m_states->radius);
    m_particle_shape.setOrigin(m_states->radius, m_states->radius);

    m_particle_amount = particle_amount;
    m_particles.resize(m_particle_amount);
    m_grid.resize(m_grid_size.x * m_grid_size.y);

    for (std::size_t i=0; i < m_particle_amount; i++) 
    {
        sf::Vector2f position = sf::Vector2f((m_container_size.x * rand() / RAND_MAX) - m_container_size.x / 2.0f, (m_container_size.y * rand() / RAND_MAX) - m_container_size.y / 2.0f);
        sf::Vector2i grid_coord = m_world_to_grid(position);
        std::size_t grid_index = m_grid_to_index(grid_coord);
        short int color = rand() % PARTICLE_COLORS;
        
        m_particles[i] = Particle{position, sf::Vector2f(0.0f, 0.0f), grid_coord, color};
        m_grid[grid_index].push_back(i);
    }
}

void ParticleHandler::apply_forces(float elapsed_time, std::size_t start, std::size_t threads_amount)
{
    float distance;
    float norm_value;
    sf::Vector2f virtual_pos; 
    sf::Vector2f diff;
    sf::Vector2i cell;
    sf::Vector2i virtual_cell;

    for (std::size_t i=start; i < m_particle_amount; i+=threads_amount) 
    for (int dy=-1; dy <= 1; dy++)
    for (int dx=-1; dx <= 1; dx++)
    {
        virtual_cell = m_particles[i].grid_coord + sf::Vector2i(dx, dy);
        cell = virtual_cell;

        if (cell.x < 0)
            cell.x += m_grid_size.x;
        else if (cell.x >= m_grid_size.x)
            cell.x -= m_grid_size.x;
        if (cell.y < 0)
            cell.y += m_grid_size.y;
        else if (cell.y >= m_grid_size.y)
            cell.y -= m_grid_size.y;
        
        for (std::size_t j : m_grid[m_grid_to_index(cell)])
            if (j != i)
            {
                virtual_pos = m_particles[j].position
                    + sf::Vector2f(virtual_cell.x * m_grid_cell_size.x, virtual_cell.y * m_grid_cell_size.y)
                    - sf::Vector2f(m_particles[j].grid_coord.x * m_grid_cell_size.x, m_particles[j].grid_coord.y * m_grid_cell_size.y);
                
                diff = virtual_pos - m_particles[i].position;
                distance = sqrt(pow(diff.x, 2) + pow(diff.y, 2));

                if (m_states->min_distance < distance && distance <= m_states->max_distance) 
                {
                    diff /= distance;
                    norm_value = 2.0f - (2.0f * (distance - m_states->min_distance) / (m_states->max_distance - m_states->min_distance));
                    m_particles[i].velocity += diff * (float)fabs(norm_value) * m_states->factors[m_particles[i].color + m_particles[j].color * PARTICLE_COLORS] * m_states->acceleration * elapsed_time;
                    m_particles[j].velocity -= diff * (float)fabs(norm_value) * m_states->factors[m_particles[j].color + m_particles[i].color * PARTICLE_COLORS] * m_states->acceleration * elapsed_time;
                } else if (distance <= m_states->min_distance)
                {
                    m_particles[i].velocity += diff * (distance / m_states->min_distance - 1.0f) * m_states->acceleration * elapsed_time;
                    m_particles[j].velocity -= diff * (distance / m_states->min_distance - 1.0f) * m_states->acceleration * elapsed_time;
                }
            }
    }
}

void ParticleHandler::update(float elapsed_time, std::size_t start, std::size_t threads_amount)
{
    for (std::size_t i=start; i < m_particle_amount; i+=threads_amount) 
    {
        m_particles[i].position += m_particles[i].velocity * elapsed_time;
        m_particles[i].velocity -= m_particles[i].velocity * m_states->friction_coeficient * elapsed_time;

        while (m_particles[i].position.x < -m_container_size.x / 2.0f)
            m_particles[i].position.x += m_container_size.x;
        while (m_particles[i].position.x > m_container_size.x / 2.0f)
            m_particles[i].position.x -= m_container_size.x;
            
        while (m_particles[i].position.y < -m_container_size.y / 2.0f)
            m_particles[i].position.y += m_container_size.y;
        while (m_particles[i].position.y > m_container_size.y / 2.0f)
            m_particles[i].position.y -= m_container_size.y;

        sf::Vector2i grid_coord = m_world_to_grid(m_particles[i].position);

        if (grid_coord.x != m_particles[i].grid_coord.x && grid_coord.y != m_particles[i].grid_coord.y)
        {
            const std::lock_guard<std::mutex> prev_lock(m_prev_cell_mutex);
            const std::lock_guard<std::mutex> next_lock(m_next_cell_mutex);
            std::size_t prev_index = m_grid_to_index(m_particles[i].grid_coord);
            std::size_t next_index = m_grid_to_index(grid_coord);
            m_grid[prev_index].erase(std::find(m_grid[prev_index].begin(), m_grid[prev_index].end(), i));
            m_grid[next_index].push_back(i);
            m_particles[i].grid_coord = grid_coord;
        }
    }
}

void ParticleHandler::render(sf::RenderWindow &renderer)
{
    for (auto &particle : m_particles)
    {
        m_particle_shape.setFillColor(m_states->colors[particle.color]);
        m_particle_shape.setPosition(particle.position);
        renderer.draw(m_particle_shape);
    }
}


inline sf::Vector2i ParticleHandler::m_world_to_grid(const sf::Vector2f &world)
{
    return sf::Vector2i((int)((world.x + m_container_size.x / 2.0f) / m_grid_cell_size.x), (int)((world.y + m_container_size.y / 2.0f) / m_grid_cell_size.y));
}

inline std::size_t ParticleHandler::m_grid_to_index(const sf::Vector2i &grid)
{
    return grid.x + grid.y * m_grid_size.x;
}
