#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include "core/path_tracer.h"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed\n";
        return 1;
    }

    PathTracerState state = make_default_state();
    int W = state.cfg.image_width;
    int H = state.cfg.image_height;

    SDL_Window* window = SDL_CreateWindow(
        "APMA2822B Path Tracer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        W, H, SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Failed to create window\n";
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        W, H
    );

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        path_tracer_iteration(state);
        auto img = normalize_buffer(state);

        // Convert to ARGB
        std::vector<uint32_t> pixels(W * H);
        double scale = 1.0;
        for (int j = 0; j < H; ++j) {
            for (int i = 0; i < W; ++i) {
                vec3 c = img[j*W + i];
                double r = std::sqrt(c.x());
                double g = std::sqrt(c.y());
                double b = std::sqrt(c.z());
                uint8_t R = static_cast<uint8_t>(255 * std::clamp(r, 0.0, 0.999));
                uint8_t G = static_cast<uint8_t>(255 * std::clamp(g, 0.0, 0.999));
                uint8_t B = static_cast<uint8_t>(255 * std::clamp(b, 0.0, 0.999));
                pixels[j*W + i] = (255u << 24) | (R << 16) | (G << 8) | B;
            }
        }

        void* tex_pixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &tex_pixels, &pitch);
        std::memcpy(tex_pixels, pixels.data(), W * H * sizeof(uint32_t));
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
