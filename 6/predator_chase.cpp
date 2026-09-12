// predator simulation
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cstdint>
#include <random>
#include <optional>

constexpr int FRACTIONAL_BITS = 16;
constexpr int32_t SCALE = 1 << FRACTIONAL_BITS;
// constexpr int32_t SCALE = 100;

constexpr int32_t ConvertToInt(double number){
    return static_cast<int32_t>(number * SCALE);
}

constexpr double ConvertToDouble(int32_t number){
    return static_cast<double>(number) / SCALE;
}

constexpr int32_t Multiply(int32_t num1, int32_t num2){
    return (static_cast<int64_t>(num1) * num2) >> FRACTIONAL_BITS;
}

constexpr int32_t Divide(int32_t num1, int32_t num2){
    return ( static_cast<int64_t>(num1) << FRACTIONAL_BITS ) / num2;
}

constexpr int32_t turnfactor = ConvertToInt(0.2);         // Raw int: 13107
int leftmargin {ConvertToInt(100.0)};
int bottommargin {ConvertToInt(100.0)};
int rightmargin {ConvertToInt(700.0)};
int topmargin {ConvertToInt(500.0)};

// int32_t angle = ConvertToInt(45.0); 
// // top right.

// int32_t speed = ConvertToInt(1.0);
// // 5 pixels per second.


// predator class
class predator{
public:
    int32_t x;
    int32_t y;
    int32_t vx;
    int32_t vy;
    int32_t angle;
    int32_t speed = ConvertToInt(1.0);
    float width = 25.0;
    float height = 25.0;
};

class predator_doubles{
public:
    double x;
    double y;
    double vx;
    double vy;
};

int32_t GenerateRandomInt(double start, double stop){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dist(start, stop);

    int32_t x = ConvertToInt(dist(gen));

    return x;
}

void ScreenEdge(predator &b){
    if ( b.x < leftmargin )
{ 
        b.vx = b.vx + turnfactor;
 }
    if ( b.x > rightmargin )
{ 
        b.vx = b.vx - turnfactor;
 }
    if ( b.y > bottommargin )
{ 
        b.vy = b.vy - turnfactor;
 }
    if ( b.y < topmargin )
{ 
        b.vy = b.vy + turnfactor;
 }
}

void UpdatePosition(predator &pred){
    pred.x = pred.x +pred.vx;
    pred.y = pred.y +pred.vy;

    std::cout << "postion   update " << pred.x << " " << pred.y << "\n";

    ScreenEdge(pred);
}

void UpdateVelocity(predator &pred){

    pred.vx = Multiply( ConvertToInt(cos(pred.angle)) , pred.speed);
    pred.vy = Multiply(ConvertToInt(sin(pred.angle)) , pred.speed);
    std::cout << "velocity update " << pred.vx << " " << pred.vy << "\n";
}

void UpdateRandomAngle(predator &pred){
    pred.angle = GenerateRandomInt(0.0, 360.0);
    std::cout << "angle update " << pred.angle;
}

void frame(predator &pred){
    UpdateRandomAngle(pred);
    UpdateVelocity(pred);
    UpdatePosition(pred);
}

predator InitSinglePredator(){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dist(0.0, 500.0);

    int32_t x = ConvertToInt(dist(gen));
    int32_t y = ConvertToInt(dist(gen));
    int32_t vx = ConvertToInt(dist(gen));
    int32_t vy = ConvertToInt(dist(gen));

    return predator{x,y,vx,vy};
}

std::vector<double> ConvertPredatorToDouble(predator pred){
    double x = ConvertToDouble(pred.x);
    double y = ConvertToDouble(pred.y);
    // double vx = ConvertToDouble(pred.vx);
    // double vy = ConvertToDouble(pred.vy);

    // return predator_doubles{x,y,vx,vy};

    return std::vector<double> {x,y};
}




int main(){

    predator pred = InitSinglePredator();


    sf::RenderWindow window(
        sf::VideoMode({800,600}),
        "predator"
    );
    
    window.setFramerateLimit(4);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        
        window.clear();

        std::vector<double> pred_dubs = ConvertPredatorToDouble(pred);

        std::cout << "pred in window " << pred.x << " " << pred.y << "\n";
        // CircleShape, setPosition, draw
        sf::CircleShape triangle(10.f, 3);
        triangle.setPosition({
            sf::Vector2f{static_cast<float>(pred_dubs[0]), static_cast<float>(pred_dubs[1])}
        });

        window.draw(triangle);

        frame(pred);
        std::cout << pred_dubs[0] << " " << pred_dubs[1] << "\n";


        window.display();
    }
}