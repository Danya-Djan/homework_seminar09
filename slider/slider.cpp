#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

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

bool in_box(sf::Vector2f mouse, sf::Vector2f pos, sf::Vector2f siz ){
    if ((mouse.x > pos.x) && (mouse.x < pos.x + siz.x))
        if ((mouse.y > pos.y) && (mouse.y < pos.y + siz.y))
            return true;
    return false;

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
        isChoosen = true;
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

    }
};

struct Slider
{
private:
    int min;
    int max;
    public:
    sf::Vector2f position;
    sf::Vector2f slider_position;
    sf::Font font;
    string name;
    int value;
    bool isChoosen = false;
    Slider(int min,int max, sf::Vector2f position, string name, float value): min(min), max(max), position(position), slider_position({position.x + value/max * 200, position.y - 15}), name(name), value(value)
    {
        if (!font.loadFromFile("consolas.ttf")) {
        std::cout << "Can't load button font" << std::endl;
        }
    }

    void draw(sf::RenderWindow& window)
    {
        value = (slider_position.x - position.x)/200 * max + min;
        sf::Text Name;
        sf::Text Value;
        sf::RectangleShape slider;
        sf::RectangleShape rectangle;
        Name.setString(name);
        Value.setString(to_string(value));
        Name.setFont(font);
        Value.setFont(font);
        Name.setCharacterSize(30);
        Name.setFillColor(sf::Color::White);
        Name.setPosition(position-sf::Vector2f({70, 10}));
        Value.setCharacterSize(30);
        Value.setFillColor(sf::Color::White);
        Value.setPosition(position+sf::Vector2f({220, -10}));
        slider.setSize(sf::Vector2f(10, 40));
        rectangle.setSize(sf::Vector2f(200, 10));
        rectangle.setFillColor(sf::Color{255, 255, 255});
        slider.setFillColor(sf::Color{200, 200, 200});
        rectangle.setPosition(position);
        slider.setPosition(slider_position);
        window.draw(rectangle);
        window.draw(slider);
        window.draw(Name);
        window.draw(Value);
    }
};




int main() {

    srand(time(nullptr));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(2560, 1600), "Slider", sf::Style::Default, settings);
    window.setFramerateLimit(180);

    sf::Font font;
    if (!font.loadFromFile("consolas.ttf")) {
        std::cout << "Can't load button font" << std::endl;
    }

    list<Ball> balls;
    balls.push_back(Ball({1600, 800}, 20));

    vector<Slider> sliders;
    sliders.push_back(Slider(0,255,{1200, 950}, "B", 255));
    sliders.push_back(Slider(0,255,{1200, 850}, "G", 255));
    sliders.push_back(Slider(0,255,{1200, 750}, "R", 255));
    sliders.push_back(Slider(10,90,{1200, 650}, "Size", 0));

    sf::RectangleShape selectionRect;
    selectionRect.setFillColor(sf::Color(150, 150, 240, 50));
    selectionRect.setOutlineColor(sf::Color(200, 200, 255));
    selectionRect.setOutlineThickness(1);


    bool isSelecting = false;
    bool slider_moved = false;
    sf::Vector2f slider_position;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePosition = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});

                // Если мы находимся в режиме выделения, то меняем прямоугольник выделения
                if (slider_moved){
                    for (Slider& s: sliders){
                            if (s.isChoosen)
                                s.slider_position.x = min(max(mousePosition.x, s.position.x), s.position.x+200);
                            }

                    }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                if (event.mouseButton.button == sf::Mouse::Left) {
                    for (Slider& s : sliders) {
                        if (in_box(mousePosition, s.position, {200, 10}) && !in_box(mousePosition, s.slider_position, {10, 40})) {
                            s.slider_position.x = mousePosition.x;
                            break;
                        }
                    }
                    if (!slider_moved){
                        for (Slider& s : sliders) {
                        if (in_box(mousePosition, s.slider_position, {10, 40})) {
                            s.isChoosen = true;
                            slider_moved = true;
                            break;
                        }
                    }}
                }
            }
            // При отпускании кнопки мыши выходим из режима выделения
            if (event.type == sf::Event::MouseButtonReleased) {
                slider_moved = false;
                for (Slider& s : sliders) {
                    s.isChoosen = false;

                }

            }
        }

        window.clear(sf::Color::Black);
        // Рисуем все шарики
        for (Ball& b : balls) {
            for (int i = 0; i < 3; i ++)
                b.color[i] = sliders[2-i].value;
            b.radius = sliders[3].value;
            b.draw(window);
        }
        for (Slider& s: sliders)
            s.draw(window);
        window.display();
    }

    return 0;
}