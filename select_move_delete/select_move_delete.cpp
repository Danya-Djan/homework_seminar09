#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "context_menu.hpp"

using namespace std;

// Вспомогательные функции, вычисляет расстояние между двумя точками
float distance(sf::Vector2f start, sf::Vector2f finish)
{
    return sqrtf((start.x - finish.x)*(start.x - finish.x) + (start.y - finish.y)*(start.y - finish.y));
}

// Вспомогательные функции, рисует линию на холсте окна window
void drawLine(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f finish, sf::Color color = sf::Color::White)
{
    sf::Vertex line_vertices[2] = {sf::Vertex(start, color), sf::Vertex(finish, color)};
    window.draw(line_vertices, 2, sf::Lines);
}


// Вспомагательный класс, описывет шарик
// position - положение шарика; radius - радиус
// is_chosen - говорит о том, выбран ли шарик или нет
struct Ball
{
    sf::Vector2f position;
    float radius;
    bool isChoosen;
    vector<unsigned char> color = {255, 255, 255};

    Ball(sf::Vector2f position, float radius) : position(position), radius(radius)
    {
        isChoosen = false;
    }

    // Метод, который рисует шарик на холстек окна window
    void draw(sf::RenderWindow& window) const
    {
        // Тут рисуем белый кружочек
        sf::CircleShape circle(radius);
        circle.setFillColor(sf::Color{color[0], color[1], color[2]});
        circle.setOrigin({radius, radius});
        circle.setPosition(position);
        window.draw(circle);

        // Если шарик выбран
        if (isChoosen) {
            // То рисуем "уголки"
            const float fraction = 0.7;
            drawLine(window, {position.x - radius, position.y + radius}, {position.x - radius, position.y + radius*fraction});
            drawLine(window, {position.x - radius, position.y + radius}, {position.x - fraction * radius, position.y + radius});

            drawLine(window, {position.x + radius, position.y + radius}, {position.x + radius, position.y + radius*fraction});
            drawLine(window, {position.x + radius, position.y + radius}, {position.x + radius*fraction, position.y + radius});

            drawLine(window, {position.x + radius, position.y - radius}, {position.x + radius*fraction, position.y - radius});
            drawLine(window, {position.x + radius, position.y - radius}, {position.x + radius, position.y - radius*fraction});

            drawLine(window, {position.x - radius, position.y - radius}, {position.x - radius*fraction, position.y - radius});
            drawLine(window, {position.x - radius, position.y - radius}, {position.x - radius, position.y - radius*fraction});
        }
    }
};


int main() {

    srand(time(nullptr));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(2560, 1600), "Select, Move, Delete!", sf::Style::Default, settings);
    window.setFramerateLimit(180);

    sf::Font font;
    if (!font.loadFromFile("consolas.ttf")) {
        std::cout << "Can't load button font" << std::endl;
    }

    std::vector<sf::String> contextMenuStrings {"Delete", "Create", "Random color", "Increase", "Decrease", "Copy", "Paste", "Cut"};
    ContextMenu contextMenu(window, font);
    for (const auto& el : contextMenuStrings) {
        contextMenu.addButton(el);
    }


    // Создаём связный список из шариков
    // Связный список -- потому что нам нужно будет постоянно удалять и добавлять в этот список
    std::list<Ball> balls;
    balls.push_back(Ball({float(200 + rand() % 2000), float(300 + rand() % 1100)}, 35 + rand() % 50));
    balls.push_back(Ball({float(200 + rand() % 2000), float(300 + rand() % 1100)}, 35 + rand() % 50));
    balls.push_back(Ball({float(200 + rand() % 2000), float(300 + rand() % 1100)}, 35 + rand() % 50));
    balls.push_back(Ball({float(200 + rand() % 2000), float(300 + rand() % 1100)}, 35 + rand() % 50));
    balls.push_back(Ball({float(200 + rand() % 2000), float(300 + rand() % 1100)}, 35 + rand() % 50));

    // Создаём прямоугольник выделения
    // Обратите внимание на четвёртый параметр sf::Color(150, 150, 240, 50)
    // Это alpha - прозрачность      0 = полностью прозрачный     255 = непрозрачный
    sf::RectangleShape selectionRect;
    selectionRect.setFillColor(sf::Color(150, 150, 240, 50));
    selectionRect.setOutlineColor(sf::Color(200, 200, 255));
    selectionRect.setOutlineThickness(1);

    // Специальная переменная, которая будет показывать, что мы находимся в режиме выделения
    bool isSelecting = false;
    bool ball_move = false;
    sf::Vector2f last_position;
    vector<Ball> buffer;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            int result = contextMenu.handleEvent(event);


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                for (Ball& b : balls) {
                    if (b.isChoosen) {
                        for (int i = 0; i < 3; i++) {
                            b.color[i] = rand() % 255;
                        }
                    }
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete)) {
                for (auto b = balls.begin(); b != balls.end(); b++) {
                    if ((*b).isChoosen){
                        b = balls.erase(b);
                    }

                }
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePosition = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
                {
                // Если мы находимся в режиме выделения, то меняем прямоугольник выделения
                if (isSelecting) {
                    selectionRect.setSize(mousePosition - selectionRect.getPosition());
                    for (Ball& b : balls) {
                        if (((b.position.x -b.radius > min(mousePosition.x , selectionRect.getPosition().x)) &&
                            (b.position.x + b.radius < max(mousePosition.x , selectionRect.getPosition().x))) && 
                            ((b.position.y -b.radius > min(mousePosition.y , selectionRect.getPosition().y)) &&
                            (b.position.y + b.radius < max(mousePosition.y , selectionRect.getPosition().y))))
                            b.isChoosen = true;
                        else
                            b.isChoosen = false;
                        }
                    }
                
                if (ball_move) {
                    if (last_position.x == 0 && last_position.y == 0) {
                        last_position.x = mousePosition.x;
                        last_position.y = mousePosition.y;
                    }
                    for (Ball& b : balls) {
                        if (b.isChoosen) {
                            b.position.x += mousePosition.x - last_position.x;
                            b.position.y += mousePosition.y - last_position.y;
                        }
                    }
                    last_position.x = mousePosition.x;
                    last_position.y = mousePosition.y;
                }
                }
            }


            if (event.type == sf::Event::MouseButtonPressed && result == -1) {
                sf::Vector2f mousePosition = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                if (event.mouseButton.button == sf::Mouse::Left) {

                    if (!ball_move){
                        for (Ball& b : balls) {
                            if ((distance(mousePosition, b.position) < b.radius) && (b.isChoosen)) {
                                last_position = mousePosition;
                                ball_move = true;
                                break;
                            }
                        }
                    }
                    
                    // Если не зажат левый Ctrl, то все выделения снимаются
                    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && !ball_move) {
                        for (Ball& b : balls) {
                            b.isChoosen = false;
                        }
                        isSelecting = true;
                    }


                    // Проверяем попал ли курсор в какой-нибудь шарик, если попал - выделяем
                    for (Ball& b : balls) {
                        if (distance(mousePosition, b.position) < b.radius) {
                            b.isChoosen = true;
                            isSelecting = false;
                            break;
                        }
                    }

                    /*for (Ball& b : balls) {
                        if (distance(mousePosition, b.position) < b.radius && b.isChoosen) {
                            last_position = mousePosition;
                            ball_move = true;
                            break;
                        }
                    }*/

                    // ЛКМ + левый Alt - добавляем новый случайный шарик
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                        balls.push_back(Ball(mousePosition, 5 + rand() % 40));
                    }
                    // Задаём новое положения прямоугольника выделения
                    selectionRect.setPosition(mousePosition);
                    selectionRect.setSize({0, 0});
                }
            }



            //Контекстное меню

            if (result == 0){
                for (auto b = balls.begin(); b != balls.end();) {
                    if ((*b).isChoosen){
                        // TODO: free memory
                        b = balls.erase(b);
                    }
                    else ++b;
                }
            }

            if (result == 1) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition());
                mousePosition.x -= 100;
                mousePosition.y -= 1900;
                std::cout << mousePosition.x << " ; " << mousePosition.y << std::endl ;
                balls.push_back(Ball(mousePosition, 35 + rand() % 50));
            }

            if (result == 2) {
                for (Ball& b : balls) {
                    if (b.isChoosen) {
                        for (int i = 0; i < 3; i++) {
                            b.color[i] = rand() % 255;
                        }
                    }
                }
            }

            if (result == 3) {
                 for (Ball& b: balls){
                    if (b.isChoosen)
                        b.radius *= 1.25;
                 }
            }
            if (result == 4){
                 for (Ball& b: balls){
                    if (b.isChoosen)
                        b.radius *= 0.75;
                 }
            }

            if (result == 5){
                buffer.clear();
                for (Ball& b : balls) {
                    if (b.isChoosen){
                        buffer.push_back(b);
                    }
                }
            }

            if (result == 6) {
                for (Ball &b : balls)
                        b.isChoosen = false;
                for (Ball& b : buffer) {
                    // cout << b.position.x << ";" << b.position.y << "|||||" << mousePosition.x << ";" << mousePosition.y << endl;
                    balls.push_back(b);

                }
            }

            if (result == 7){
                buffer.clear();
                for (Ball& b : balls) {
                    if (b.isChoosen){
                        buffer.push_back(b);
                    }
                }
                for (auto b = balls.begin(); b != balls.end();) {
                    if ((*b).isChoosen){
                        // TODO: free memory
                        b = balls.erase(b);
                    }
                    else ++b;
                }
            }


            // При отпускании кнопки мыши выходим из режима выделения
            if (event.type == sf::Event::MouseButtonReleased) {
                isSelecting = false;
                ball_move = false;
            }
        }

        window.clear(sf::Color::Black);
        // Рисуем все шарики
        for (Ball& b : balls) {
            b.draw(window);
        }
        // Рисуем прямоугольник выделения
        if (isSelecting) {
            window.draw(selectionRect);
        }

        contextMenu.draw();
        window.display();
    }

    return 0;
}