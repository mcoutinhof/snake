#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <deque>
#include <ctime>
#include <cmath>

int board_width = 20, board_height = 20;
std::deque<std::pair<int, int>> snake;
std::pair<int, int> food;
bool is_over = false;
int steps = 0;

void replace_food() {
    if (snake.size() == board_width * board_height) { // Esconde a comida se a serpente já ocupou o tabuleiro
        food = std::make_pair(-1, -1);
    } else do { // Se não, coloca a comida em algum quadrado vago
        food = std::make_pair(rand() % board_width, rand() % board_height);
    } while (std::find(snake.begin(), snake.end(), food) != snake.end());
}

void init() {
    if (board_width  % 2) board_width++;
    if (board_height % 2) board_height++;
    snake.clear();
    snake.push_back(std::make_pair(0, board_height / 2));
    snake.push_back(std::make_pair(1, board_height / 2));
    snake.push_back(std::make_pair(2, board_height / 2));
    replace_food();
    is_over = false;
    steps = 0;
}

void update() {
    if (is_over) { return; }

    steps++;

    auto head = snake.back();
    auto tail = snake.front();

    snake.pop_front();

    // Cria uma lista com os quatro quadrados ao redor da cabeça.
    std::deque<std::pair<int, int>> positions;
    positions.push_back(std::make_pair(head.first, head.second - 1)); // Em cima
    positions.push_back(std::make_pair(head.first - 1, head.second)); // À esquerda
    positions.push_back(std::make_pair(head.first, head.second + 1)); // Em baixo
    positions.push_back(std::make_pair(head.first + 1, head.second)); // À direita

    // Cria uma lista com as direções possíveis com base na posição da cabeça.
    std::deque<int> directions;
    if (head.first % 2) { directions.push_back(0); } else { directions.push_back(2); }
    if (head.second % 2) { directions.push_back(3); } else { directions.push_back(1); }

    // Desconsidera as direções que a farão colidir na parede ou no corpo.
    for (auto &direction: directions) {
        auto &position = positions[direction];
        if (position.first < 0 || position.first >= board_width
            || position.second < 0 || position.second >= board_height
            || std::find(snake.begin(), snake.end(), position) != snake.end()) {
            directions.erase(std::find(directions.begin(), directions.end(), direction));
        }
    }

    // Se ainda houver mais de uma direção disponível, escolhe a que irá para mais perto da comida.
    if (directions.size() > 1) {
        double distanceA = std::sqrt(std::pow(food.first - positions[directions[0]].first, 2)
                                     + std::pow(food.second - positions[directions[0]].second, 2));
        double distanceB = std::sqrt(std::pow(food.first - positions[directions[1]].first, 2)
                                     + std::pow(food.second - positions[directions[1]].second, 2));
        if (distanceA < distanceB) {
            head = positions[directions[0]];
        } else if (distanceB < distanceA) {
            head = positions[directions[1]];
        } else { // Se as distâncias forem iguais, escolhe aleatoriamente para prevenir um loop infinito.
            head = positions[directions[rand() % 2]];
        }
    } else {
        head = positions[directions[0]];
    }

    if (head.first < 0 || head.first >= board_width || head.second < 0 || head.second >= board_height) {
        std::cout << "Você bateu na parede!" << std::endl;
        is_over = true;
        return;
    }

    if (std::find(snake.begin(), snake.end(), head) != snake.end()) {
        std::cout << "Você bateu em si mesmo!" << std::endl;
        is_over = true;
        return;
    }

    snake.push_back(head);

    // Checa se colidiu com a comida.
    if (std::find(snake.begin(), snake.end(), food) != snake.end()) {
        snake.push_front(tail);
        replace_food();
    }

    if (snake.size() == board_width * board_height) {
        std::cout << "Você ganhou com " << steps << " passos!" << std::endl;
        is_over = true;
        return;
    }
}

void render(SDL_Renderer *renderer) {
    static Uint64 count = 0;
    if (is_over) { count++; } else { count = 0; }

    int screen_width, screen_height;
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);
    int square_size = std::min(screen_width / board_width, screen_height / board_height);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Desenha o plano de fundo
    SDL_SetRenderDrawColor(renderer, 223, 223, 223, SDL_ALPHA_OPAQUE);
    for (int x = 0; x < board_width; x++) {
        for (int y = 0; y < board_height; y++) {
            SDL_Rect rect = {x * square_size, y * square_size, square_size, square_size};
            SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
            SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
            SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y + rect.h, rect.x, rect.y + rect.h);
            SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x, rect.y);
        }
    }

    // Desenha a serpente
    for (int i = 0; i < snake.size(); i++) {
        auto &previous = i - 1 >= 0 ? snake[i - 1] : snake[i];
        auto &current  = snake[i];
        auto &next     = i + 1 < snake.size() ? snake[i + 1] : snake[i];

        if (!is_over || count / 30 % 2 || snake.size() == board_width * board_height) {
            SDL_SetRenderDrawColor(renderer, 0, 255 - 255 * i / snake.size(), 255 * i / snake.size(), SDL_ALPHA_OPAQUE);
        } else { // A serpente pisca em vermelho quando morre.
            SDL_SetRenderDrawColor(renderer, 255 * i / snake.size(), 0, 0, SDL_ALPHA_OPAQUE);
        }

        SDL_Rect rect = {current.first * square_size, current.second * square_size, square_size, square_size};
        SDL_RenderFillRect(renderer, &rect);

        if (!is_over || count / 30 % 2 || snake.size() != board_width * board_height) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        } else { // O contorno da serpente pisca em branco quando vence.
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        }

        // Contorno inteligente da serpente
        if ((current.first != previous.first || current.second - 1 != previous.second)
            && (current.first != next.first || current.second - 1 != next.second)) {
            SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
        }
        if ((current.first + 1 != previous.first || current.second != previous.second)
            && (current.first + 1 != next.first || current.second != next.second)) {
            SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
        }
        if ((current.first != previous.first || current.second + 1 != previous.second)
            && (current.first != next.first || current.second + 1 != next.second)) {
            SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y + rect.h, rect.x, rect.y + rect.h);
        }
        if ((current.first - 1 != previous.first || current.second != previous.second)
            && (current.first - 1 != next.first || current.second != next.second)) {
            SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x, rect.y);
        }
    }

    {   // Desenha a comida
        SDL_Rect rect = {food.first * square_size, food.second * square_size, square_size, square_size};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
        SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
        SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y + rect.h, rect.x, rect.y + rect.h);
        SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x, rect.y);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        if (SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 641, 641, SDL_WINDOW_RESIZABLE)) {
            if (SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) {
                init();
                Uint64 last_tick = SDL_GetPerformanceCounter();
                for (bool running = true, fast_forward = false; running;) {
                    for (SDL_Event e; SDL_PollEvent(&e);) {
                        switch (e.type) {
                            case SDL_QUIT:
                                running = false;
                                break;
                            case SDL_KEYDOWN:
                                switch (e.key.keysym.sym) {
                                    case SDLK_ESCAPE:
                                        if (is_over) {
                                            running = false;
                                        } else {
                                            is_over = true;
                                        }
                                        break;
                                    case SDLK_SPACE:
                                        fast_forward = true;
                                        break;
                                    case SDLK_RETURN:
                                    case SDLK_RETURN2:
                                    case SDLK_KP_ENTER:
                                        init();
                                        break;
                                }
                                break;
                            case SDL_KEYUP:
                                switch (e.key.keysym.sym) {
                                    case SDLK_SPACE:
                                        fast_forward = false;
                                        break;
                                }
                                break;
                        }
                    }

                    update();
                    auto delta_time = ((SDL_GetPerformanceCounter() - last_tick) * 1000 / (double) SDL_GetPerformanceFrequency());
                    if (33 > delta_time) {
                        if (fast_forward) {
                            continue;
                        } else {
                            SDL_Delay(33 - delta_time);
                        }
                    }
                    last_tick = SDL_GetPerformanceCounter();
                    render(renderer);
                }
                SDL_DestroyRenderer(renderer);
            } else fprintf(stderr, "O renderizador não pôde ser criado! SDL_Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
        } else fprintf(stderr, "A janela não pôde ser criada! SDL_Error: %s\n", SDL_GetError());
    } else fprintf(stderr, "O SDL não pôde ser inicializado! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
}