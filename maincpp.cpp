#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
const float M_PI = static_cast<float>(M_PI);

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    bool isAsh;
};

std::vector<Particle> particles;

void createParticle(float x, float y, float vx, float vy, float lifetime, bool isAsh = false) {
    Particle p;
    p.x = x;
    p.y = y;
    p.vx = vx;
    p.vy = vy;
    p.life = lifetime;
    p.isAsh = isAsh;

    // Set fire particles to spawn at the center of the screen
    if (!p.isAsh) {
        p.x = 320;
        p.y = 300;
    }

    // Slow down the particles and increase their lifetime to create a slow-burning effect
    if (!p.isAsh) {
        p.vx *= 0.2f;
        p.vy *= 0.2f;
        p.life *= 5.0f;
    }

    particles.push_back(p);
}

void updateParticles() {
    for (auto& p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        if (!p.isAsh) {
            p.vx += 0.01f * (std::rand() % 100 - 50);
            p.vy += 0.01f * (std::rand() % 100 - 50) - 0.1f;
            p.life -= 0.005f;
            if (p.life <= 0.0f) {
                // Convert the particle to ash
                createParticle(p.x, p.y, p.vx, p.vy, 0.5f, true);
                p.life = 0.0f;
            }
        }
        else {
            p.life -= 0.001f;
        }
    }
    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );
}

void drawParticles() {
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        float r, g, b;
        if (p.isAsh) {
            // Ash particles fade from red to gray
            r = 1.0f - p.life;
            g = 0.8f - p.life * 0.8f;
            b = 0.6f - p.life * 0.6f;
        }
        else {
            // Fire particles fade from yellow to red
            r = 1.0f;
            g = 0.5f + p.life * 0.5f;
            b = 0.2f;
        }
        glColor4f(r, g, b, p.life);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Fire Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    std::srand(std::time(nullptr));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Spawn fire particles within a circular area in the middle of the screen
        float cx = width / 2.;
        float cy = height / 2.;
        float radius = 100.0f;
        for (int i = 0; i < 10; ++i) {
            float angle = 2.0f * M_PI * i / 10.0f;
            float x = cx + radius * std::cos(angle);
            float y = cy + radius * std::sin(angle);
            createParticle(x, y, 0.0f, 0.0f, 1.0f);
        }

        // Spawn some burning logs
        if (particles.size() < 100) {
            for (int i = 0; i < 5; ++i) {
                float x = cx + (std::rand() % 201 - 100);
                float y = cy + (std::rand() % 201 - 100);
                createParticle(x, y, 0.0f, 0.0f, 2.0f);
            }
        }

        // Burn the logs and create ash particles
        for (auto& p : particles) {
            if (p.life <= 0.0f && !p.isAsh) {
                for (int i = 0; i < 50; ++i) {
                    float vx = 0.05f * (std::rand() % 100 - 50);
                    float vy = -0.2f - 0.05f * (std::rand() % 100);
                    createParticle(p.x, p.y, vx, vy, 1.0f, true);
                }
            }
        }

        updateParticles();
        drawParticles();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
