// Oiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<vector>
#include<raylib.h>
using namespace std;
class XY {
public:
    double x, y;
    float xf() { return (float)x; }
    float yf() { return (float)y; }
    int xi() { return (int)x; }
    int yi() { return (int)y; }

    XY() : x(0), y(0) {}
    //XY(XY& from) : x(from.x), y(from.y) {}
    XY(double x, double y) : x(x), y(y) {}

    XY operator+(const XY& rhs) const { return XY(x + rhs.x, y + rhs.y); }
    XY operator-(const XY& rhs) const { return XY(x - rhs.x, y - rhs.y); }
    XY operator*(const XY& rhs) const { return XY(x * rhs.x, y * rhs.y); }
    XY operator/(const XY& rhs) const { return XY(x / rhs.x, y / rhs.y); }
    XY operator*(double scale) const { return XY(x * scale, y * scale); }
    XY operator/(double scale) const { return XY(x / scale, y / scale); }
    XY operator+=(const XY& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
    double dot(const XY& rhs) const { return x * rhs.x + y * rhs.y; }
    XY normal() const { return *this / magnitude(); }
    double magnitude() const { return sqrt(x * x + y * y); }

};
const XY Zero = { 0, 0 };
class Particle {
public:
    XY pos;
    XY vel;
    XY deltaVel;
    void update(double deltaTime) {
        vel += deltaVel;
        auto a = vel + deltaVel;
        pos += vel * deltaTime;
        deltaVel = { 0, 0 };
    }
    void Draw() {
        DrawCircle(pos.xi(), pos.yi(), 4, BLUE);
    }
};
class Spring {
public:
    XY origin = { 0, 0 };
    XY offset = { 250, 0 };
    vector<Particle> particles;
    bool forward = true;
    double stiffness;
    XY rest_pos() { return origin + offset; };
    XY direction() { return offset.normal(); }

    XY computeSpringForce(const XY& pos) {
        return (rest_pos() - pos) * stiffness;
    }
    void ForwardEuler(double deltaTime) {
        for (auto& p : particles) {
            ForwardEuler(deltaTime, p);
        }
    }
    void ForwardEuler(double deltaTime, Particle& p) {
        p.deltaVel += computeSpringForce(p.pos) * deltaTime;
    }
    void BackwardEuler(double deltaTime) {
        for (auto& p : particles) {
            BackwardEuler(deltaTime, p);
        }
    }
    void BackwardEuler(double deltaTime, Particle& p) {
        auto force = computeSpringForce(p.pos).dot(direction());

        auto df_dx = stiffness;
        auto deltaTime2 = deltaTime * deltaTime;
        auto mass = 1;
        auto A = 1 + deltaTime2 * df_dx / mass;
        auto B = deltaTime / mass * (force - deltaTime * df_dx * direction().dot(p.vel));
        p.deltaVel += direction() * (B / A);
    }
    void Update(double deltaTime) {
        if (forward) {
            ForwardEuler(deltaTime);
        } else {
            BackwardEuler(deltaTime);
        }
        for (auto& p : particles) { p.update(deltaTime); }
    }
    void Draw() {
        for (auto& p : particles) {
            DrawLine(origin.xi(), origin.yi(), p.pos.xi(), p.pos.yi(), BLUE);
            p.Draw();
        }
        DrawCircle(origin.xi(), origin.yi(), 4, BLACK);
        auto rest_pos = this->rest_pos();
        DrawLine(origin.xi(), origin.yi(), rest_pos.xi(), rest_pos.yi(), RED);
    }

};

int main()
{
    XY mouseStart, mouseEnd;
    double timestep = 0;

    vector<Spring> springs = {
        { 
            XY(200, 100),
            XY(250, 0),
            {
                { XY(250, 100) }
            },
            true,
            160
        }, {
            XY(200, 300),
            XY(250, 0),
            {
                { XY(200 + 500, 300) }
            },
            false,
            160
        },
    };
    InitWindow(800, 450, "raylib [core] example - basic window");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto& s : springs) {
            s.Update(GetFrameTime());
            s.Draw();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

