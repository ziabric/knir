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
    MainWindow(std::string title_ = "Example", int width_ = 800, int height_ = 600) :
    size(width_, height_)
    {
        window = new sf::RenderWindow(sf::VideoMode(width_, height_), title_);
        window->setFramerateLimit(60);
        ImGui::SFML::Init(*window);
    }
    ~MainWindow()
    {
        ImGui::SFML::Shutdown();
        delete window;
    }

    int mainloop()
    {
        while (window->isOpen())
        {
            sf::Event event {};
            while (window->pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(*window, event);

                if (event.type == sf::Event::Closed)
                {
                    window->close();
                }
            }
            ImGui::SFML::Update(*window, deltaClock.restart());

            size = {(float)window->getSize().x, (float)window->getSize().y};

            draw();

            window->clear();
            ImGui::SFML::Render(*window);
            window->display();
        }
        return 0;
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

        ImGui::Columns(2, "Tree widget and body");

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
                    }
                }
            }
            ImGui::EndChild();
        }

        ImGui::NextColumn();

        if (ImGui::BeginChild("Body", {-1, -1}, true))
        {
            if (ImGui::Button("Func 1")) func1();
            ImGui::SameLine();
            if (ImGui::Button("Func 2")) func2();
            ImGui::SameLine();
            if (ImGui::Button("Func 3")) func3();

            ImGui::Columns(2, "images");

            if (file.string() != "" && ImGui::Button(file.filename().c_str()))
            {
                showFileFlag_ = true;
            }

            if (file.string() != "" && fileTexture.loadFromFile(file.string()))
            {
                fileSprite = sf::Sprite(fileTexture);
                fileSprite.setScale((GetColumnDistance(0))/fileTexture.getSize().x, (GetColumnDistance(0))/fileTexture.getSize().x);
                
                ImGui::Image(fileSprite);
            }

            ImGui::NextColumn();

            if (newFile.string() != "" && ImGui::Button(newFile.filename().c_str()))
            {
                showNewFileFlag_ = true;
            }

            if (newFile.string() != "" && newFileTexture.loadFromFile(newFile.string()))
            {
                newFileSprite = sf::Sprite(newFileTexture);
                newFileSprite.setScale((GetColumnDistance(1))/newFileTexture.getSize().x, (GetColumnDistance(1))/newFileTexture.getSize().x);
                
                ImGui::Image(newFileSprite);
            }
            ImGui::EndChild();
        }
        
        ImGui::End();

        if (showFileFlag_) showInformationImage();

        if (showNewFileFlag_) showInformationNewImage();
    }
    float GetColumnDistance(int n)
    {
        return ImGui::GetColumnOffset(n + 1) - ImGui::GetColumnOffset(n);
    }
    inline void showInformationImage()
    {
        if (ImGui::Begin(file.filename().string().c_str(), &showFileFlag_, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::SetWindowFocus();
            ImGui::End();
        }
    }
    inline void showInformationNewImage()
    {
        
    }
    inline void func1()
    {

    }
    inline void func2()
    {
        
    }
    inline void func3()
    {
        
    }
private:
    fs::path currentPath = fs::current_path();
    std::set<std::string> fileExt = {
            ".png",
            ".jpeg",
            ".jpg"
    };
    ImColor imageColor();
    ImColor notImageColor();
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

    bool showFileFlag_ = false;
    bool showNewFileFlag_ = false;
};