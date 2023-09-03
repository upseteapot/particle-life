#pragma once
#include <SFML/Graphics.hpp>
#include <mutex>
#include <cmath>
#include <random>
#include <ctime>

#define PARTICLE_COLORS 5


struct Particle
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2i grid_coord;
    short int color;
};


struct ParticleHandlerStates
{
    std::vector<float> factors;
    float max_distance = 110.0f;
    float min_distance = 20.0f;
    float acceleration = 10.0f;
    float friction_coeficient = 0.99f;
    float radius = 1.1f;
    
    std::vector<sf::Color> colors =
    {
        sf::Color(255, 50, 80),
        sf::Color(20, 255, 80),
        sf::Color(80, 50, 255),
        sf::Color(255, 180, 0),
        sf::Color(0, 255, 255)
    };

    static void create_random(ParticleHandlerStates &states);
    static void create_default(ParticleHandlerStates &states);
};


class ParticleHandler
{
    public:
        ParticleHandler() = default;
        void create(ParticleHandlerStates *states, std::size_t particle_amount, float width, float height);
        void apply_forces(float elapsed_time, std::size_t start, std::size_t threads_amount);
        void update(float elapsed_time, std::size_t start, std::size_t threads_amount);
        void render(sf::RenderWindow &renderer);

    private:
        ParticleHandlerStates *m_states;
        sf::Vector2f m_container_size;
        sf::Vector2i m_grid_size;
        sf::Vector2f m_grid_cell_size;
        std::mutex m_prev_cell_mutex;
        std::mutex m_next_cell_mutex;
        std::vector<std::vector<std::size_t>> m_grid;
        std::size_t m_particle_amount;
        std::vector<Particle> m_particles;
        sf::CircleShape m_particle_shape;

        inline sf::Vector2i m_world_to_grid(const sf::Vector2f &world);
        inline std::size_t m_grid_to_index(const sf::Vector2i &grid); 
};
