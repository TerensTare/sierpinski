#include <cstdio>
#include <span>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <SDL2/SDL.h>

void sierpinski(SDL_Window *win, SDL_Renderer *ren, unsigned n) noexcept;

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Sierpinski Triangle",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI //
    );

    if (!window)
    {
        std::printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!ren)
    {
        std::printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, ren);
    ImGui_ImplSDLRenderer_Init(ren);

    // imgui state
    int n = 1;

    bool running = true;
    while (running)
    {
        for (SDL_Event event; SDL_PollEvent(&event);)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE)
                running = false;
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Sierpinski Triangle");

            ImGui::SliderInt("Depth", &n, 1, 10);

            ImGui::End();
        }

        ImGui::Render();

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        sierpinski(window, ren, n);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(ren);
        SDL_Delay(10);
    }

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

constexpr SDL_Vertex vertex(
    float x, float y,
    SDL_Color color = {0, 0, 0, 255}) noexcept
{
    return SDL_Vertex{
        .position = {x, y},
        .color = color,
    };
}

constexpr SDL_Vertex vertex(
    SDL_FPoint pos,
    SDL_Color color = {0, 0, 0, 255}) noexcept
{
    return SDL_Vertex{
        .position = pos,
        .color = color,
    };
}

constexpr SDL_FPoint midpoint(SDL_FPoint left, SDL_FPoint right) noexcept
{
    return SDL_FPoint{
        .x = (left.x + right.x) / 2,
        .y = (left.y + right.y) / 2,
    };
}

void draw_impl(
    SDL_Renderer *ren,
    unsigned n,
    std::span<SDL_Vertex const, 3> triangle) noexcept
{
    if (n > 0)
    {
        SDL_Vertex center[3];
        for (unsigned i = 0; i < 3; ++i)
        {
            auto &l = triangle[i].position;
            auto &r = triangle[(i + 1) % 3].position;

            center[i] = vertex(midpoint(l, r));
        }

        SDL_RenderGeometry(
            ren, NULL,
            center, 3,
            NULL, 0 //
        );

        for (unsigned i = 0; i < 3; ++i)
        {
            auto &p0 = triangle[i].position;
            auto &p1 = triangle[(i + 1) % 3].position;
            auto &p2 = triangle[(i + 2) % 3].position;

            SDL_Vertex sub[]{
                vertex(p0),
                vertex(midpoint(p0, p1)),
                vertex(midpoint(p0, p2)),
            };

            draw_impl(ren, n - 1, sub);
        }
    }
}

void sierpinski(SDL_Window *win, SDL_Renderer *ren, unsigned n) noexcept
{
    int w;
    int h;

    SDL_GetWindowSize(win, &w, &h);

    float hw = w / 2;
    float hh = h / 2;
    float size = 200.0f;

    SDL_Vertex triangle[]{
        vertex(hw, hh - size, {.r = 0xff, .a = 0xff}),
        vertex(hw - size, hh + size, {.g = 0xff, .a = 0xff}),
        vertex(hw + size, hh + size, {.b = 0xff, .a = 0xff}),
    };

    SDL_RenderGeometry(
        ren, NULL,
        triangle, 3,
        NULL, 0 //
    );

    draw_impl(ren, n - 1, triangle);
}