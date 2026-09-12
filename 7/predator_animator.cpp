// predator animation
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cstdint>
#include <random>
#include <optional>

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdint>

int main(){
    sf::RenderWindow window(
        sf::VideoMode({800,600}),
        "testing"
    );
    
    window.setFramerateLimit(4);

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();

        sf::VertexArray shape(sf::PrimitiveType::LineStrip, 3);

        shape[0].position = {0.f, 50.f};
        shape[1].position = {25.f, 0.f};
        shape[2].position = {50.f, 50.f};

        window.draw(shape);


        window.display();
    }

    return 0;
}