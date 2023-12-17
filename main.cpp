#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

#include <filesystem>
#include <iostream>
#include <set>

#include <func.h>

namespace fs = std::filesystem;

bool fileManagerOpen = false;
fs::path currentPath;
fs::path file;

std::set<std::string> fileExt = {
        ".png",
        ".jpeg",
        ".jpg"
};

sf::Texture fileTexture;
bool showImage = false;

void display(ImVec2 size = {0,0})
{
    auto font = ImGui::GetFont();
    font->Scale = 1.2;
    ImGui::Begin("KNIR", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos({0,0});
    ImGui::SetWindowSize(size);
    if (ImGui::Button("Open"))
    {
        fileManagerOpen = true;
        currentPath = fs::current_path();
    }
    ImGui::Text("%s", file.filename().c_str());
    ImGui::Checkbox("Show image", &showImage);
    if (ImGui::Button("tmp"))
    {
        arithmetic_mean_filter(file, 50);
    }

    if ( showImage && file.string() != "" && fileTexture.loadFromFile(file.string()))
    {
        ImGui::Image(fileTexture);
    }

    if (fileManagerOpen)
    {
        ImGui::SetNextWindowFocus();
        ImGui::Begin("Simple fm", &fileManagerOpen, ImGuiWindowFlags_Modal + ImGuiWindowFlags_NoCollapse);

        ImGui::Text("%s", currentPath.c_str());

        auto tmp = ImGui::GetIO();

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

        ImGui::End();
    }

    ImGui::End();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Window sample");
    window.setFramerateLimit(60);

    if(ImGui::SFML::Init(window))
    {
        sf::Clock deltaClock;
        while (window.isOpen())
        {
            sf::Event event {};
            while (window.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(window, event);

                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }
            ImGui::SFML::Update(window, deltaClock.restart());

            display({(float)window.getSize().x, (float)window.getSize().y});

            window.clear();
            ImGui::SFML::Render(window);
            window.display();
        }
        ImGui::SFML::Shutdown();
    }
    return 0;
}
