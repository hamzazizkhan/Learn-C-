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

    static_prey(float start_x, float start_y) : x(start_x),
                                                y(start_y)
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

    void SetRotation()
    {
        this->left_rot_x =
            this->left_x * std::cos(this->angle) -
            this->left_y * std::sin(this->angle);

        this->left_rot_y =
            this->left_x * std::sin(this->angle) +
            this->left_y * std::cos(this->angle);

        // Right wing
        this->right_rot_x =
            this->right_x * std::cos(this->angle) -
            this->right_y * std::sin(this->angle);

        this->right_rot_y =
            this->right_x * std::sin(this->angle) +
            this->right_y * std::cos(this->angle);
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

    float AngleRadians(float angle_degrees)
    {
        float radians = angle_degrees * M_PI / 180.0f;
        return radians;
    }

    void PrivateUpdateVelocity()
    {
        vx = std::cos(angle) * speed;
        vy = std::sin(angle) * speed;

        // std::cout << "velocity update " << vx << " " << vy << "\n";
    }

public:
    float width;
    float height;

    const float left_x;
    const float left_y;
    const float right_x;
    const float right_y;

    const float wing_size;

    float left_pos_x;
    float left_pos_y;
    float right_pos_x;
    float right_pos_y;

    float left_rot_x;
    float left_rot_y;
    float right_rot_x;
    float right_rot_y;

    float angle;
    float constant_angle;

    float speed;
    float x;
    float y;
    float vx;
    float vy;

    bool use_rand_angle;

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
                                                                                   use_rand_angle(false),
                                                                                   wing_size(30.0f)
    {
        SetRotation();
        PrivateUpdateVelocity();


        left_pos_x = x + left_x;
        left_pos_y = y + left_y;

        right_pos_x = x + right_x;
        right_pos_y = y + right_y;

        // std::cout << "initialisation \n";
        // std::cout << "\n";

        // std::cout
        //     << left_pos_x << " "
        //     << left_pos_y << " "
        //     << right_pos_x << " "
        //     << right_pos_y;
    }

    void SetX(float new_x)
    {
        x = new_x;
    }

    void SetY(float new_y)
    {
        y = new_y;
    }

    void SetVX(float new_vx)
    {
        vx = new_vx;
    }

    void SetVY(float new_vy)
    {
        vy = new_vy;
    }

    void SetAngle(float new_angle)
    {
        angle = new_angle;
    }

    void UpdateRandomAngle()
    {
        if (use_rand_angle)
        {
            angle = GenerateRandomAngle();
            // std::cout << "random angle update " << angle;
        }
    }

    void UpdatePosition()
    {
        x = x + vx;
        y = y + vy;

        // std::cout << "postion update " << x << " " << y << "\n";
    }

    void UpdateVelocity()
    {
        PrivateUpdateVelocity();
    }

    // in frame function:
    // update angle before this.
    // update position should be done before this.
    void UpdateWingsRotation()
    {
        SetRotation();
    }

    sf::VertexArray PredatorShape()
    {

        float predator_x = x;
        float predator_y = y;

        sf::VertexArray shape(sf::PrimitiveType::LineStrip, 3);

        // std::cout << "printing wings postions \n";

        // std::cout
        //     << left_pos_x << " "
        //     << left_pos_y << " "
        //     << right_pos_x << " "
        //     << right_pos_y;

        shape[0].position = {left_pos_x, left_pos_y};
        shape[1].position = {predator_x, predator_y};
        shape[2].position = {right_pos_x, right_pos_y};

        return shape;
    }

    void frame()
    {
        UpdateRandomAngle();
        UpdateWingsRotation();
        UpdateVelocity();
        UpdatePosition();
        UpdateWingsPosition();
        ScreenEdge();
    }

    std::vector<float> ConvertPredatorToDouble()
    {

        float dub_x = x;
        float dub_y = y;

        return std::vector<float>{dub_x, dub_y};
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
};

int main()
{

    float init_angle = 145.0f;
    float radians = init_angle * M_PI / 180.0f;

    float init_x = 400.0f;
    float init_y = 300.0f;
    float init_speed = 1.0f;

    predator pred(init_x, init_y, radians, init_speed);

    static_prey prey(250.0f, 250.0f);

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "predator");

    window.setFramerateLimit(100);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        // predator
        std::vector<float> pred_dubs = pred.ConvertPredatorToDouble();

        // std::cout << "pred in window "
        //           << pred.x << " "
        //           << pred.y << "\n";

        sf::VertexArray shape = pred.PredatorShape();

        // prey
        sf::CircleShape circle_static_prey(20.f);

        sf::Vector2<float> prey_pos = {
            prey.x,
            prey.y};

        circle_static_prey.setPosition({prey_pos.x,
                                        prey_pos.y});

        // drawing
        window.draw(shape);
        window.draw(circle_static_prey);

        pred.frame();

        // std::cout
        //     << pred_dubs[0] << " "
        //     << pred_dubs[1] << "\n";

        window.display();
    }
}