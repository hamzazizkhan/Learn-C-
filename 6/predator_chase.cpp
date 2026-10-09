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
int bottommargin {ConvertToInt(500.0)};
int rightmargin {ConvertToInt(700.0)};
int topmargin {ConvertToInt(100.0)};

// int32_t angle = ConvertToInt(45.0); 
// // top right.

// int32_t speed = ConvertToInt(1.0);
// // 5 pixels per second.


class static_prey{
private:

public:
    int32_t x;
    int32_t y;

    static_prey(int32_t start_x, int32_t start_y):
    x(start_x),
    y(start_y)
    {}
};

// predator class
class predator{
private:
    void ScreenEdge(){
        if ( x < leftmargin )
    { 
            vx = vx + turnfactor;
    }
        if ( x > rightmargin )
    { 
            vx = vx - turnfactor;
    }
        if ( y > bottommargin )
    { 
            vy = vy - turnfactor;
    }
        if ( y < topmargin )
    { 
            vy = vy + turnfactor;
    }
    }

    void SetRotation(){
        this->left_rot_x = 
            this->left_x * cos(this->angle) - 
            this->left_y * sin(this->angle);

        this->left_rot_y =
            this->left_x * sin(this->angle) +
            this->left_y * cos(this->angle);

        // Right wing
        this->right_rot_x =
            this->right_x * cos(this->angle) -
            this->right_y * sin(this->angle);

        this->right_rot_y =
            this->right_x * sin(this->angle) +
            this->right_y * cos(this->angle);
    }

    double GenerateRandomAngle(){
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<> dist(0.0, 360.0);

        double angle = dist(gen);
        double radians = angle * M_PI / 180.0;

        return radians;
    }

    double AngleRadians(double angle_degrees){
        double radians = angle_degrees * M_PI / 180.0;
        return radians;
    }

    void PrivateUpdateVelocity(){
        vx = Multiply( ConvertToInt(cos(angle)) , speed);
        vy = Multiply(ConvertToInt(sin(angle)) , speed);
        std::cout << "velocity update " << vx << " " << vy << "\n";
    }


public:
    double width;
    double height;

    const double left_x;
    const double left_y;
    const double right_x;
    const double right_y;

    double left_pos_x;
    double left_pos_y;
    double right_pos_x;
    double right_pos_y;

    double left_rot_x;
    double left_rot_y;
    double right_rot_x;
    double right_rot_y;

    double angle;
    double constant_angle;

    int32_t speed;
    int32_t x;
    int32_t y;
    int32_t vx;
    int32_t vy;

    bool use_rand_angle;

    predator(int32_t start_x, int32_t start_y, double start_angle, int32_t start_speed):
    x(start_x),
    y(start_y),
    angle(start_angle),
    speed(start_speed),
    left_x(-25.0),
    left_y(25.0),
    right_x(25.0),
    right_y(25.0),
    width(25.0),
    height(25.0),
    // testing constant movement
    constant_angle(-45.0),
    use_rand_angle(false)

    {
        SetRotation();
        PrivateUpdateVelocity();
        constant_angle = AngleRadians(constant_angle);
        left_pos_x = ConvertToDouble(x) + left_x;
        left_pos_y = ConvertToDouble(y) + left_y;
        right_pos_x = ConvertToDouble(x) + right_x;
        right_pos_y = ConvertToDouble(y) + right_y;
        std::cout << "initialisation \n";
        std::cout << "\n";
        std::cout << left_pos_x << " " << left_pos_y  << " " << right_pos_x << " " << right_pos_y; 
    }

    void SetX(int32_t new_x){
        x = new_x; 
    }

    void SetY(int32_t new_y){
        y = new_y; 
    }

    void SetVX(int32_t new_vx){
        vx = new_vx; 
    }

    void SetVY(int32_t new_vy){
        vy = new_vy; 
    }

    void SetAngle(double new_angle){
        angle = new_angle; 
    }
    
    void UpdateRandomAngle(){
        if (use_rand_angle){
            angle = GenerateRandomAngle();
            std::cout << "random angle update " << angle;
        }else{
            angle = constant_angle;
            std::cout << "using constant angle " << angle;

        }

    }

    void UpdatePosition(){
        x = x +vx;
        y = y +vy;

        std::cout << "postion   update " << x << " " << y << "\n";

    }

    void UpdateVelocity(){
        PrivateUpdateVelocity();
    }

    // in frame function:
    // update angle before this.
    // update postion should be done before this.
    void UpdateWingsRotation(){
        SetRotation();
    }

    // void UpdateWingsPosition()
    // {
    //     double predator_x = ConvertToDouble(x);
    //     double predator_y = ConvertToDouble(y);

    //     right_pos_x = predator_x + right_rot_x;
    //     right_pos_y = predator_y + right_rot_y;

    //     left_pos_x = predator_x + left_rot_x;
    //     left_pos_y = predator_y + left_rot_y;

    // }

    sf::VertexArray PredatorShape(){

        double predator_x = ConvertToDouble(x);
        double predator_y = ConvertToDouble(y); 
        
        sf::VertexArray shape(sf::PrimitiveType::LineStrip, 3);

        std::cout << "printing wings postions \n";
        std::cout << left_pos_x << " " << left_pos_y  << " " << right_pos_x << " " << right_pos_y; 

        shape[0].position = {static_cast<float>(left_pos_x),static_cast<float>(left_pos_y)};                                             // left bottom
        shape[1].position = {static_cast<float>(predator_x), static_cast<float>(predator_y)};                            // top point - should be head
        shape[2].position = {static_cast<float>(right_pos_x), static_cast<float>(right_pos_y)};

        return shape;
    }

    void frame(){
        UpdateRandomAngle();
        UpdateWingsRotation();
        UpdateVelocity();
        UpdatePosition();
        UpdateWingsPosition();
        ScreenEdge();
    }

    std::vector<double> ConvertPredatorToDouble(){
        double dub_x = ConvertToDouble(x);
        double dub_y = ConvertToDouble(y);

        return std::vector<double> {dub_x, dub_y};
    }

    // new update of wings postion
    void UpdateWingsPosition(){
        int32_t backward_vx = -vx;
        int32_t backward_vy = -vy;

        int32_t perpendicular_vx = vx;
        int32_t perpendicular_vy = -vy;

        int32_t right_wing_vx = backward_vx + Divide(perpendicular_vx, ConvertToInt(2.0));
        int32_t right_wing_vy = backward_vy + Divide(perpendicular_vy, ConvertToInt(2.0));

        int32_t left_wing_vx = backward_vx - Divide(perpendicular_vx, ConvertToInt(2.0));
        int32_t left_wing_vy = backward_vy - Divide(perpendicular_vx, ConvertToInt(2.0));

        right_wing_vx /= ConvertToInt(2.0);
        right_wing_vy /= ConvertToInt(2.0);
        left_wing_vx /= ConvertToInt(2.0);
        left_wing_vy /= ConvertToInt(2.0);

        right_pos_x = ConvertToDouble(x) + right_wing_vx;
        right_pos_y = ConvertToDouble(y) + right_wing_vy;
        
        left_pos_x = ConvertToDouble(x) + left_wing_vx;
        left_pos_y = ConvertToDouble(y) + left_wing_vy;
    } 

};

int main(){

    // predator pred = InitSinglePredator();
    double init_angle = 45.0;
    double radians = init_angle * M_PI / 180.0;
    int32_t init_x = ConvertToInt(400.0);
    int32_t init_y = ConvertToInt(300.0);
    int32_t init_speed = ConvertToInt(50.0);
    predator pred(init_x, init_y, radians, init_speed);


    static_prey prey(ConvertToInt(250.0), ConvertToInt(250.0));


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


        // predator
        std::vector<double> pred_dubs = pred.ConvertPredatorToDouble();

        std::cout << "pred in window " << pred.x << " " << pred.y << "\n";
        // CircleShape, setPosition, draw
        
        sf::VertexArray shape = pred.PredatorShape();

        //prey
        sf::CircleShape circle_static_prey(20.f);

        sf::Vector2<float> prey_pos = {static_cast<float>(ConvertToDouble(prey.x)),  static_cast<float>(ConvertToDouble(prey.y))};
        circle_static_prey.setPosition({prey_pos.x, prey_pos.y});   

        // drawing
        window.draw(shape);
        window.draw(circle_static_prey);

        pred.frame();
        std::cout << pred_dubs[0] << " " << pred_dubs[1] << "\n";


        window.display();
    }
}