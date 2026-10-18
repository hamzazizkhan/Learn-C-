// predator simulation
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <random>
#include <optional>

constexpr float turnfactor = 0.2f;

float leftmargin{100.0f};
float bottommargin{500.0f};
float rightmargin{700.0f};
float topmargin{100.0f};

// static prey
class static_prey
{
private:
public:
    float x;
    float y;

    float radius;

    static_prey(float start_x, float start_y) : x(start_x),
                                                y(start_y),
                                                radius(20.0f)
    {
    }
};

// predator class
class predator
{
private:
    void ScreenEdge()
    {
        if (x < leftmargin)
        {
            vx = vx + turnfactor;
        }

        if (x > rightmargin)
        {
            vx = vx - turnfactor;
        }

        if (y > bottommargin)
        {
            vy = vy - turnfactor;
        }

        if (y < topmargin)
        {
            vy = vy + turnfactor;
        }
    }

    float GenerateRandomAngle()
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<float> dist(0.0f, 360.0f);

        float new_angle = dist(gen);
        float radians = new_angle * M_PI / 180.0f;

        return radians;
    }

    float GenerateRandomTurnFactor(){
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);

        float new_turn_factor = dist(gen);

        return new_turn_factor;
    }

    float AngleRadians(float angle_degrees)
    {
        float radians = angle_degrees * M_PI / 180.0f;
        return radians;
    }

    void PrivateUpdateVelocity()
    {
        vx = std::cos(angle) * speed;
        vy = std::sin(angle) * speed;
    }
    
    void UpdateConePosition(){
        float normalised_vx = vx/speed; 
        float normalised_vy = vy/speed; 

        float forward_vx = normalised_vx;
        float forward_vy = normalised_vy;

        float perpendicular_vx = normalised_vy * cone_width;
        float perpendicular_vy = -normalised_vx * cone_width;

        float right_wing_vx = forward_vx + perpendicular_vx;
        float right_wing_vy = forward_vy + perpendicular_vy;

        float left_wing_vx = forward_vx - perpendicular_vx;
        float left_wing_vy = forward_vy - perpendicular_vy;

        right_cone_x = x + (right_wing_vx * cone_length);
        right_cone_y = y + (right_wing_vy * cone_length);

        left_cone_x =  x + (left_wing_vx  * cone_length);
        left_cone_y =  y + (left_wing_vy  * cone_length);
    }

    void UpdateWingsPosition()
    {

        float normalised_vx = vx/speed; 
        float normalised_vy = vy/speed; 

        float backward_vx = -normalised_vx;
        float backward_vy = -normalised_vy;

        float perpendicular_vx = normalised_vy;
        float perpendicular_vy = -normalised_vx;

        float right_wing_vx = backward_vx + perpendicular_vx;
        float right_wing_vy = backward_vy + perpendicular_vy;

        float left_wing_vx = backward_vx - perpendicular_vx;
        float left_wing_vy = backward_vy - perpendicular_vy;

        right_pos_x = x + (right_wing_vx * wing_size);
        right_pos_y = y + (right_wing_vy * wing_size);

        left_pos_x =  x + (left_wing_vx  * wing_size);
        left_pos_y =  y + (left_wing_vy  * wing_size);
    }


    void UpdateTurnFactor(){
        turn_factor = GenerateRandomTurnFactor(); 
    }

    void UpdateRandomAngle()
    {
        UpdateTurnFactor();
        angle = angle + turn_factor;
    }

    void UpdatePosition()
    {
        x = x + vx;
        y = y + vy;
    }

    void UpdateVelocity()
    {
        PrivateUpdateVelocity();
    }


public:
    float width;
    float height;

    const float left_x;
    const float left_y;
    const float right_x;
    const float right_y;

    const float wing_size;
    const float cone_length;
    const float cone_width;

    float cone_head_x;
    float cone_head_y;
    float left_cone_x;
    float left_cone_y;
    float right_cone_x;
    float right_cone_y;

    float left_pos_x;
    float left_pos_y;
    float right_pos_x;
    float right_pos_y;

    float angle;
    float constant_angle;

    float speed;
    float turn_factor;

    float x;
    float y;
    float vx;
    float vy;


    predator(float start_x, float start_y, float start_angle, float start_speed) : x(start_x),
                                                                                   y(start_y),
                                                                                   angle(start_angle),
                                                                                   speed(start_speed),
                                                                                    constant_angle(start_angle),
                                                                                   left_x(-25.0f),
                                                                                   left_y(25.0f),
                                                                                   right_x(25.0f),
                                                                                   right_y(25.0f),
                                                                                   width(25.0f),
                                                                                   height(25.0f),
                                                                                   wing_size(10.0f),
                                                                                   turn_factor(0.02f),
                                                                                   cone_length(50.0f),
                                                                                   cone_width(0.5f)
    {
        PrivateUpdateVelocity();

        left_pos_x = x + left_x;
        left_pos_y = y + left_y;

        right_pos_x = x + right_x;
        right_pos_y = y + right_y;

        UpdateConePosition();
    }

    sf::VertexArray PredatorShape()
    {
        sf::VertexArray shape(sf::PrimitiveType::LineStrip, 3);

        shape[0].position = {left_pos_x, left_pos_y};
        shape[1].position = {x, y};
        shape[2].position = {right_pos_x, right_pos_y};

        return shape;
    }

    sf::VertexArray LeftPredatorConeShape(){
        sf::VertexArray shape (sf::PrimitiveType::LineStrip, 2);

        shape[0].position = {x, y};
        shape[1].position = {left_cone_x, left_cone_y};

        shape[0].color = sf::Color::Red;
        shape[1].color = sf::Color::Red;

        return shape;
    }

    sf::VertexArray RightPredatorConeShape(){
        sf::VertexArray shape (sf::PrimitiveType::LineStrip, 2);

        shape[0].position = {x, y};
        shape[1].position = {right_cone_x, right_cone_y};

        shape[0].color = sf::Color::Red;
        shape[1].color = sf::Color::Red;

        return shape;
    }

    void frame()
    {
        UpdateRandomAngle();
        UpdateVelocity();
        UpdatePosition();
        UpdateWingsPosition();
        UpdateConePosition();
        ScreenEdge();
    }
};

int main()
{

    float init_angle = 145.0f;
    float radians = init_angle * M_PI / 180.0f;

    float init_x = 400.0f;
    float init_y = 300.0f;
    float init_speed = 0.5f;

    predator pred(init_x, init_y, radians, init_speed);

    static_prey prey(250.0f, 250.0f);

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "predator");

    window.setFramerateLimit(70);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        sf::VertexArray predator_shape = pred.PredatorShape();
        sf::VertexArray left_cone_shape = pred.LeftPredatorConeShape();
        sf::VertexArray right_cone_shape = pred.RightPredatorConeShape();

        // prey
        sf::CircleShape circle_static_prey(prey.radius);

        sf::Vector2<float> prey_pos = {
            prey.x,
            prey.y};

        sf::Vector2<float> cone_pos = {pred.cone_head_x, pred.cone_head_y};

        circle_static_prey.setPosition({prey_pos.x,
                                        prey_pos.y});

        // drawing
        window.draw(predator_shape);
        window.draw(circle_static_prey);
        window.draw(left_cone_shape);
        window.draw(right_cone_shape);

        pred.frame();

        window.display();
    }
}