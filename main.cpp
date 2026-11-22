#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <optional>          
#include <SFML/Graphics.hpp>

using namespace std;

class CellularAutomaton {
protected:
    int width;
    int height;
    int cellSize;

public:
    CellularAutomaton(int w, int h, int size) : width(w), height(h), cellSize(size) {}
    virtual ~CellularAutomaton() {}
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

class CaveGenerator : public CellularAutomaton {
private:
    // 0 - пусто, 1 - стена
    vector<vector<int>> map;

    //параметры симуляции
    int birthLimit;
    int deathLimit;
    int chanceToStartAlive;

    //метод для подсчета живых соседей
    int countAliveNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                int neighborX = x + j;
                int neighborY = y + i;
                if (neighborX < 0 || neighborY < 0 || neighborX >= width || neighborY >= height) {
                    count++;
                } else if (map[neighborY][neighborX] == 1) {
                    count++;
                }
            }
        }
        return count;
    }

public:
    CaveGenerator(int w, int h, int size, int birth, int death, int chance)
        : CellularAutomaton(w, h, size), birthLimit(birth), deathLimit(death), chanceToStartAlive(chance) {
        //инициализация вектора нулями
        map.resize(height, vector<int>(width, 0));
    }

    //реализация инициализации
    void initialize() override {
        srand(static_cast<unsigned>(time(0)));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if ((rand() % 100) < chanceToStartAlive) {
                    map[y][x] = 1; //Живая (стена)
                } else {
                    map[y][x] = 0; //Мертвая (пустота)
                }
            }
        }
    }

    //реализация шага симуляции согласно правилам
    void update() override {
        vector<vector<int>> newMap = map;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int neighbors = countAliveNeighbors(x, y);
                if (map[y][x] == 1) {
                    if (neighbors < deathLimit) {
                        newMap[y][x] = 0;
                    } else {
                        newMap[y][x] = 1;
                    }
                } else {
                    if (neighbors > birthLimit) {
                        newMap[y][x] = 1;
                    } else {
                        newMap[y][x] = 0;
                    }
                }
            }
        }
        map = newMap;
    }

    //реализация отрисовки в окно SFML
    void draw(sf::RenderWindow& window) override {
        sf::RectangleShape cellShape(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (map[y][x] == 1) {
                    cellShape.setFillColor(sf::Color::Black); //стены черные
                } else {
                    cellShape.setFillColor(sf::Color::White); //пол белый
                }
                cellShape.setPosition({static_cast<float>(x * cellSize),
                                       static_cast<float>(y * cellSize)});
                window.draw(cellShape);
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "");
    int width = 50;    //ширина поля в клетках
    int height = 50;   //высота поля в клетках
    int cellSize = 12; //размер одной клетки в пикселях
    int birthLimit, deathLimit, chance;

    cout << "Генератор пещер" << endl;
    cout << "Введите предел рождения (например, 4): ";
    cin >> birthLimit;
    cout << "Введите предел смерти (например, 3): ";
    cin >> deathLimit;
    cout << "Введите шанс начального появления стены % (например, 30-45): ";
    cin >> chance;

    CellularAutomaton* cave = new CaveGenerator(width, height, cellSize, birthLimit, deathLimit, chance);

    //инициализация поля
    cave->initialize();

    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned>(width * cellSize),
                       static_cast<unsigned>(height * cellSize)}),
        "Cave Generator - Lab 5");
    window.setFramerateLimit(60); //ограничение FPS

    cout << "\nУправление в окне:" << endl;
    cout << "[Space] - Сделать один шаг симуляции" << endl;
    cout << "[R]     - Перегенерировать поле заново" << endl;
    cout << "[Esc]   - Выход" << endl;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            //обработка нажатий клавиш
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    cave->update();
                    cout << "Шаг симуляции выполнен." << endl;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                    cave->initialize();
                    cout << "Поле перегенерировано." << endl;
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color::White);
        //отрисовка пещеры
        cave->draw(window);

        window.display();
    }
    delete cave;
    return 0;
}