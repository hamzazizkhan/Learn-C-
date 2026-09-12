#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <cstdint>
#include <SFML/Graphics.hpp>
#include <optional>
// ==========================================
// GLOBAL CONFIGURATION CONSTANTS (Read-Only)
// ==========================================
#include <cstdint>

constexpr int FRACTIONAL_BITS = 16;
constexpr int32_t SCALE = 1 << FRACTIONAL_BITS;

constexpr int32_t TO_FP(double x)
{
    return static_cast<int32_t>(x * SCALE);
}

constexpr double FROM_FP(int32_t x)
{
    return static_cast<double>(x) / SCALE;
}
constexpr int32_t FP_MUL(int32_t a, int32_t b)
{
    return (static_cast<int64_t>(a) * b) >> FRACTIONAL_BITS;
}

constexpr int32_t FP_DIV(int32_t a, int32_t b)
{
    return (static_cast<int64_t>(a) << FRACTIONAL_BITS) / b;
}
constexpr int32_t turnfactor = TO_FP(0.2);         // Raw int: 13107
constexpr int32_t visualRange = TO_FP(40.0);       // Raw int: 2621440
constexpr int32_t visualRangeSquared =FP_MUL(visualRange, visualRange);
constexpr int32_t protectedRange = TO_FP(8.0);     // Raw int: 524288
constexpr int32_t protectedRangeSquared =FP_MUL(protectedRange, protectedRange);
constexpr int32_t centeringfactor = TO_FP(0.0005); // Raw int: 33
constexpr int32_t avoidfactor = TO_FP(0.05);       // Raw int: 3277
constexpr int32_t matchingfactor = TO_FP(0.05);    // Raw int: 3277
constexpr int32_t maxspeed = TO_FP(5.0);           // Raw int: 393216
constexpr int32_t minspeed = TO_FP(1.0);           // Raw int: 196608
constexpr int32_t maxbias = TO_FP(0.01);           // Raw int: 655
constexpr int32_t bias_increment = TO_FP(0.00004); // Raw int: 3

int leftmargin {TO_FP(100.0)};
int bottommargin {TO_FP(100.0)};
int rightmargin {TO_FP(700.0)};
int topmargin {TO_FP(500.0)};

class boid
{
public:
    int32_t x;
    int32_t y;
    int32_t vx;
    int32_t vy;
};

// void print_list(800){

//     for (int i = 0; i<10; i++){
//         std::cout << list[i] << '\n';
//     }
// }

// =============================================================================================================
// helper functions.
// =============================================================================================================
void print(boid &b){
    std::cout << "x "<< FROM_FP(b.x) << " y "<< FROM_FP( b.y )<<" vx "<< FROM_FP( b.vx )<< " vy "<< FROM_FP( b.vy ) << '\n';
}

void printList(std::vector<boid> &boids){
    for(auto&boi : boids){
        print(boi);
    }
}


void fill_list(std::vector<boid> &boids1){

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dist(0.0, 500.0);

    for (auto &b : boids1)
    {
        b.x = TO_FP(dist(gen));
        b.y = TO_FP(dist(gen));
        b.vx = TO_FP(dist(gen));
        b.vy = TO_FP(dist(gen));
    }
}

double distance(boid b1, boid b2){
    double dx = b1.x - b2.x;
    double dy = b1.y - b2.y;

    double dist = std::sqrt((dx*dx) - (dy*dy));

    return dist;
}

// =============================================================================================================
// boid functions.
// =============================================================================================================
void update_position(boid &b)
{
    b.x += b.vx;
    b.y += b.vy;
}

void seperation(std::vector<boid> &boid1, boid &b ){
    double close_dx {0};
    double close_dy {0};

    for(auto&otherboid : boid1){
        if (&otherboid != &b){
            close_dx += b.x - otherboid.x;
            close_dy += b.y - otherboid.y;
        }
    }
    b.vx = FP_MUL(close_dx, avoidfactor);
    b.vy = FP_MUL(close_dy, avoidfactor);
}

void alignment(std::vector<boid> &boid1, boid &b){
    double xvel_avg {0}, yvel_avg{0}  ;
    int neighboring_boids {0};

    for(auto&otherboid : boid1){
        if (&otherboid != &b){
            double dist = distance(b, otherboid);
            if (dist<visualRange){
                xvel_avg += otherboid.vx;
                yvel_avg += otherboid.vy;
                neighboring_boids += 1;
            }
        }
    }

    if (neighboring_boids>0){
        xvel_avg /= neighboring_boids;
        yvel_avg /= neighboring_boids;
    }

    b.vx += FP_MUL(xvel_avg - b.vx, matchingfactor);
    b.vy += FP_MUL(yvel_avg - b.vy, matchingfactor);
}

void cohesion(std::vector<boid> &boid1, boid &b){
    double xpos_avg{0}, ypos_avg{0};
    int neighboring_boids = 0;

    for(auto&otherboid : boid1){
        if (&otherboid != &b){
            double dist = distance(b, otherboid);
            if (dist<visualRange){
                xpos_avg += otherboid.x;
                ypos_avg += otherboid.y;
                neighboring_boids += 1;
            }
        }
    }

    if (neighboring_boids>0){
        xpos_avg = xpos_avg / neighboring_boids;
        ypos_avg = ypos_avg / neighboring_boids;
        }

        b.vx += FP_MUL(xpos_avg - b.x, centeringfactor);
        b.vy += FP_MUL(ypos_avg - b.y, centeringfactor);
}

void screen_edge(boid &b){
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

void speed_limit(boid &b, int64_t speed){
    double vx = FROM_FP(b.vx);
    double vy = FROM_FP(b.vy);

    // double speed = std::sqrt(vx * vx + vy * vy);
    if (speed>maxspeed){
        b.vx = FP_MUL(FP_DIV(b.vx, speed), maxspeed);
        b.vy = FP_MUL(FP_DIV(b.vy, speed), maxspeed);
    }

    if (speed < minspeed){
        b.vx = FP_MUL(FP_DIV(b.vx, speed), minspeed);
        b.vy = FP_MUL(FP_DIV(b.vy, speed), minspeed);
    }
}

void frame(std::vector<boid> &boids){
    for(auto &b : boids){
        int32_t xpos_avg{0}, ypos_avg{0}, yvel_avg{0}, xvel_avg{0}, close_dx{0}, close_dy{0};
        int neighboring_boids = 0;

        for(auto &otherboid : boids){
            if(&b != &otherboid){
                // # Compute differences in x and y coordinates
                int32_t dx = b.x - otherboid.x;
                int32_t dy = b.y - otherboid.y;

                // # Are both those differences less than the visual range?
                if (abs(dx)<visualRange and abs(dy)<visualRange){  

                    // # If so, calculate the squared distance
                    int64_t squared_distance = FP_MUL(dx,dx) + FP_MUL(dy,dy);

                    // # Is squared distance less than the protected range?
                    if (squared_distance < protectedRangeSquared){
                        // # If so, calculate difference in x/y-coordinates to nearfield boid
                        close_dx += b.x - otherboid.x ;
                        close_dy += b.y - otherboid.y;
                    }

                    // # If not in protected range, is the boid in the visual range?
                    else if (squared_distance < visualRangeSquared){
                        // # Add other boid's x/y-coord and x/y vel to accumulator variables
                        xpos_avg += otherboid.x; 
                        ypos_avg += otherboid.y ;
                        xvel_avg += otherboid.vx;
                        yvel_avg += otherboid.vy;

                        // # Increment number of boids within visual range
                        neighboring_boids += 1 ;
                    }

                }
            }
        }

        if (neighboring_boids > 0){

            // #Divide accumulator variables by number of boids in visual range
            xpos_avg /= neighboring_boids;
            ypos_avg /= neighboring_boids;
            xvel_avg /= neighboring_boids;
            yvel_avg /= neighboring_boids;
            // #Add the centering / matching contributions to velocity
            b.vx = (b.vx + 
                    FP_MUL((xpos_avg - b.x), centeringfactor) + 
                    FP_MUL((xvel_avg - b.vx), matchingfactor));

            b.vy = (b.vy + 
                    FP_MUL( (ypos_avg - b.y), centeringfactor  ) +
                    FP_MUL( (yvel_avg - b.vy), matchingfactor) );

        }

        // #Add the avoidance contribution to velocity
        b.vx = b.vx + FP_MUL(close_dx, avoidfactor);
        b.vy = b.vy + FP_MUL(close_dy, avoidfactor);

        screen_edge(b);

        double speed = std::sqrt(
            FROM_FP(
                FP_MUL(b.vx, b.vx) +
                FP_MUL(b.vy, b.vy)));

        speed_limit(b, TO_FP(speed));

        update_position(b);
        // go through speed calculations with sqrt and whether you are doing them correctly
        // when to add centering, cohesion, repulsion functions.

    }
}

int main(){
    std::vector<boid> boids(100);

    fill_list(boids);

    std::cout << "printing list of boids \n";
    printList(boids);


    // std::cout << "new postions \n";
    // for (auto&b :boids){
    //     update_position(b);
    //     print(b);
    // }

    // boid bi;

    // bi.x = TO_FP(10.0);
    // bi.y = TO_FP(20.0);
    // bi.vx = TO_FP(2.0);
    // bi.vy = TO_FP(3.0);

    // update_position(bi);

    // std::cout
    //     << FROM_FP(bi.x) << " "
    //     << FROM_FP(bi.y) << '\n';

    // int32_t a = TO_FP(2.5);
    // int32_t b = TO_FP(4.0);

    // std::cout << FROM_FP(FP_MUL(a, b)) << '\n';

    // std::cout << FROM_FP(FP_DIV(TO_FP(10.0), TO_FP(2.0))) << '\n';
    

    std::cout << "testing frame update \n";

    frame(boids);

    printList(boids);


    sf::RenderWindow window(

        sf::VideoMode({800,600}),
        "boids"
    );
    // Cap the execution speed at 30 frames per second
    window.setFramerateLimit(30);

    sf::Vector2u windowSize = window.getSize();
    float screenWidth = static_cast<float>(windowSize.x);
    float screenHeight = static_cast<float>(windowSize.y);

    std::cout << "size of window is " << screenWidth << screenHeight;

    // while (window.isOpen()){
    //     while (const std::optional event = window.pollEvent())
    //     {
    //         if (event->is<sf::Event::Closed>())
    //             window.close();
    //     }

    //     window.clear();

    //     sf::CircleShape circle(3);
    //     circle.setPosition({
    //         sf::Vector2f{200,200}
    //     });
    //     window.draw(circle);

    //     circle.setPosition({
    //         sf::Vector2f{500,300}
    //     });
    //     window.draw(circle);

    //     window.display();

    // }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        // CircleShape, setPosition, draw

        for(auto &b: boids){
            sf::CircleShape circle(3);
            circle.setPosition({
                sf::Vector2f{
                    static_cast<float>(FROM_FP(b.x)), 
                    static_cast<float>(FROM_FP(b.y)) 
                }
            });
            window.draw(circle);
        }

        frame(boids);

        // draw things here

        window.display();
    }

    return 0;
}