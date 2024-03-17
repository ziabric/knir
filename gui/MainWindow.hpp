#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <filesystem>
#include <opencv2/opencv.hpp>


namespace fs = std::filesystem;

class MainWindow
{
public:
    MainWindow(std::string title_ = "Example", int width_ = 1280, int height_ = 720);
    ~MainWindow();
    int mainloop();
private:
    void drawInterface();
    void drawImage();
    void drawImageSettings();
    void drawFilesystem();
    float GetColumnDistance(int n);
    void showInformationImage();
    void showInformationNewImage();
    void openImage(fs::path pathToImage);
    void imagePopupMenu();
private:
    enum class ChannelType
    {
        Blue = 0,
        Green,
        Red,
        All
    };
    fs::path currentPath = fs::current_path();
    std::set<std::string> fileExt = {
            ".png",
            ".jpeg",
            ".jpg"
    };
    fs::path file;
    cv::Mat fileMat;
    sf::Texture fileTexture;
    sf::Sprite fileSprite;
    float fileScale;
    ImVec2 fileWindowPos = {};
    ImVec2 fileWindowSize = {};
    ChannelType fileChannel = ChannelType::All;

    fs::path newFile;
    sf::Texture newFileTexture;
    sf::Sprite newFileSprite;
    std::vector<std::string> pathTmp;
    sf::RenderWindow* window = nullptr;
    sf::Clock deltaClock;
    ImVec2 size = {0,0};

    bool showHiddenFiles = false;

    ImVec2 settingsWindowSize = {0,0};

    int settingsType = -1;
    bool filesystemOpenFlag = false;
};