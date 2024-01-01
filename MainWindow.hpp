#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <filesystem>
#include <set>
#include <sstream>

#include <func.h>

namespace fs = std::filesystem;

class MainWindow
{
public:
    MainWindow()
    {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "Window sample");
        window->setFramerateLimit(60);
    }
    ~MainWindow()
    {
        ImGui::SFML::Shutdown();
        delete window;
    }

    void mainloop()
    {
        while (window->isOpen())
        {
            sf::Event event {};
            while (window->pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(&window, event);

                if (event.type == sf::Event::Closed)
                {
                    window->close();
                }
            }
            ImGui::SFML::Update(&window, deltaClock.restart());

            size = {(float)window->getSize().x, (float)window->getSize().y};

            draw();

            window->clear();
            ImGui::SFML::Render(&window);
            window->display();
        }
    }
private:
    inline void draw()
    {
        auto font = ImGui::GetFont();
        font->FontSize = 25;
        ImGui::Begin("KNIR", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize(size);

        std::stringstream ss(currentPath.string());
        std::string s;
        pathTmp.clear();
        while(getline(ss, s, '/'))
        {
            if (s != "") pathTmp.push_back(s);
        }
        
        if (ImGui::Button("/"))
        {
            currentPath = "/";
        }

        for (int i = 0; i < pathTmp.size(); i += 1)
        {
            ImGui::SameLine();
            if(ImGui::Button(pathTmp.at(i).c_str()))
            {
                fs::path newPath;
                for (int j = 0; j <= i; j += 1)
                {
                    newPath += "/";
                    newPath += pathTmp[j];
                }
                newPath += "/";
                currentPath = newPath;
            }
            ImGui::SameLine();
            ImGui::Text("/");
        }

        ImGui::Columns(3);

        if (ImGui::BeginChild("fs", {-1,-1}, true))
        {
            if ( fs::exists(currentPath.parent_path()) )
            {
                if (ImGui::Button("..", {ImGui::GetWindowSize().x, 25}))
                {
                    currentPath = currentPath.parent_path();
                }
            }

            for ( const auto& item : fs::directory_iterator(currentPath) )
            {
                if (is_directory(item))
                {
                    if ( ImGui::Button(std::string(item.path().filename().string()).c_str(), {ImGui::GetWindowSize().x, 25}) )
                    {
                            currentPath = item;
                    }
                }
                else if ( item.is_regular_file() )
                {
                    if (ImGui::Button(item.path().filename().c_str(), {ImGui::GetWindowSize().x, 25}) && fileExt.find(fs::path(item).extension().string()) != fileExt.end())
                    {
                        file = item;
                        fileManagerOpen = false;
                    }
                }
            }
            ImGui::EndChild();
        }

        ImGui::NextColumn();

        if (file.string() != "" && fileTexture.loadFromFile(file.string()))
        {
            fileSprite = sf::Sprite(fileTexture);
            fileSprite.setScale((GetColumnDistance(1))/fileTexture.getSize().x, (GetColumnDistance(1))/fileTexture.getSize().x);
            
            ImGui::Image(fileSprite);
        }

        ImGui::NextColumn();

        if (newFile.string() != "" && newFileTexture.loadFromFile(newFile.string()))
        {
            newFileSprite = sf::Sprite(newFileTexture);
            newFileSprite.setScale((GetColumnDistance(2))/newFileTexture.getSize().x, (GetColumnDistance(2))/newFileTexture.getSize().x);
            
            ImGui::Image(newFileSprite);
        }
        
        ImGui::End();
    }
    float GetColumnDistance(int n)
    {
        return ImGui::GetColumnOffset(n + 1) - ImGui::GetColumnOffset(n);
    }
private:
    bool fileManagerOpen = false;
    fs::path currentPath = fs::current_path();
    std::set<std::string> fileExt = {
            ".png",
            ".jpeg",
            ".jpg"
    };
    fs::path file;
    sf::Texture fileTexture;
    sf::Sprite fileSprite;
    fs::path newFile;
    sf::Texture newFileTexture;
    sf::Sprite newFileSprite;
    std::vector<std::string> pathTmp;
    sf::RenderWindow* window = nullptr;
    sf::Clock deltaClock;
    ImVec2 size = {0,0};
};