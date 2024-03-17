#include <MainWindow.hpp>

MainWindow::MainWindow(std::string title_, int width_, int height_) :
size(width_, height_)
{
    window = new sf::RenderWindow(sf::VideoMode(width_, height_), title_);
    window->setFramerateLimit(60);
    ImGui::SFML::Init(*window);
}
MainWindow::~MainWindow()
{
    ImGui::SFML::Shutdown();
    delete window;
}
int MainWindow::mainloop()
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

        drawInterface();
        drawImage();
        drawFilesystem();


        window->clear();
        ImGui::SFML::Render(*window);
        window->display();
    }
    return 0;
}
void MainWindow::drawInterface()
{
    ImGui::Begin("KNIR", nullptr, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos({0,0});
    ImGui::SetWindowSize({60, size.y});
    settingsWindowSize = ImGui::GetWindowSize();

    if ( ImGui::Button("Open") ) 
    {
        filesystemOpenFlag = true;
    }
    if ( ImGui::Button("Edit") )
    {
        if ( !file.empty() ) ImGui::OpenPopup("ImageSettings");
    }
    if ( ImGui::Button("Move") )
    {

    }
    if ( ImGui::Button("Rotate") )
    {

    }
    if ( ImGui::Button("Z +") )
    {
        if (fileScale < 2) fileScale += 0.1;
    }
    if ( ImGui::Button("Z -") )
    {
        if (fileScale > 0.1) fileScale -= 0.1;
    }

    drawImageSettings();
    ImGui::End();
}
void MainWindow::drawImage()
{
    if (!file.empty())
    {
        ImGui::Begin(file.filename().c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar + ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos({settingsWindowSize.x + 10, 10}, ImGuiCond_Once);
        fileWindowPos = ImGui::GetWindowPos();
        fileWindowSize = ImGui::GetWindowSize();

        cv::Mat tmpForChannel;
        if (fileChannel != ChannelType::All) cv::extractChannel(fileMat, tmpForChannel, (int)fileChannel);
        else tmpForChannel = fileMat;

        sf::Image fileImage;
        fileImage.create(tmpForChannel.cols, tmpForChannel.rows, tmpForChannel.ptr());
        fileTexture.loadFromImage(fileImage);
        fileSprite.setTexture(fileTexture);
        fileSprite.setScale(fileScale, fileScale);

        if ( ImGui::GetWindowPos().x < settingsWindowSize.x ) ImGui::SetWindowPos({settingsWindowSize.x, ImGui::GetWindowPos().y});
        if ( ImGui::GetWindowPos().x + ImGui::GetWindowWidth() > size.x ) ImGui::SetWindowPos({size.x - ImGui::GetWindowWidth(), ImGui::GetWindowPos().y});
        if ( ImGui::GetWindowPos().y < 0 ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, 0});
        if ( ImGui::GetWindowPos().y + ImGui::GetWindowHeight() > size.y ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, size.y - ImGui::GetWindowSize().y});

        imagePopupMenu();

        ImGui::Image(fileSprite);

        ImGui::End();
    }
}
void MainWindow::drawImageSettings()
{
    if (ImGui::BeginPopup("ImageSettings", ImGuiWindowFlags_NoMove))
    {
        ImGui::SetWindowPos({fileWindowPos.x, fileWindowPos.y + fileWindowSize.y + 5});
        if(ImGui::Button("1x"))
        {
            fileScale = 1;
        }
        ImGui::SameLine();
        ImGui::SliderFloat("Scale", &fileScale, 0.1, 2.0);
        ImGui::Separator();
        if (ImGui::RadioButton("B", fileChannel == ChannelType::Blue)) fileChannel = ChannelType::Blue;
        ImGui::SameLine();
        if (ImGui::RadioButton("G", fileChannel == ChannelType::Green)) fileChannel = ChannelType::Green;
        ImGui::SameLine();
        if (ImGui::RadioButton("R", fileChannel == ChannelType::Red)) fileChannel = ChannelType::Red;
        ImGui::SameLine();
        if (ImGui::RadioButton("All", fileChannel == ChannelType::All)) fileChannel = ChannelType::All;
        ImGui::EndPopup();
    }
}
float MainWindow::GetColumnDistance(int n)
{
    return ImGui::GetColumnOffset(n + 1) - ImGui::GetColumnOffset(n);
}
void MainWindow::showInformationImage()
{
}
void MainWindow::showInformationNewImage()
{
    
}
void MainWindow::drawFilesystem()
{
    if (filesystemOpenFlag && ImGui::Begin("Filesystem", &filesystemOpenFlag, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SetWindowFocus();
        std::stringstream ss(currentPath.string());
        std::string s;
        pathTmp.clear();
        while(getline(ss, s, '/'))
        {
            if (s != "") pathTmp.push_back(s);
        }
        
        ImGui::BeginChild("PathToFile", {-1, 50}, true, ImGuiWindowFlags_HorizontalScrollbar);
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
        ImGui::SetScrollHereX(1);
        ImGui::EndChild();

        if (ImGui::BeginChild("fs", {0,0}, true))
        {
            ImGui::GetStyle().ButtonTextAlign = {0,0.5};
            for ( const auto& item : fs::directory_iterator(currentPath) )
            {
                if (item.path().filename().c_str()[0] == '.') continue;

                if (is_directory(item))
                {
                    if ( ImGui::Button(std::string(item.path().filename().string()).c_str(), {ImGui::GetWindowSize().x, 25}) )
                    {
                        currentPath = item;
                    }
                }
                else if ( item.is_regular_file() )
                {
                    if (fileExt.find(fs::path(item).extension().string()) != fileExt.end())
                    {
                        if (ImGui::Button(item.path().filename().c_str(), {ImGui::GetWindowSize().x, 25})) openImage(item.path());
                    }
                    else
                    {
                        if (ImGui::Button(item.path().filename().c_str(), {ImGui::GetWindowSize().x, 25})){}
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

}
void MainWindow::openImage(fs::path pathToImage)
{
    file = pathToImage;
    cv::Mat tmp = cv::imread(file.string());
    cv::cvtColor(tmp, fileMat, cv::COLOR_BGR2RGBA);
    fileScale = 1;
}
void MainWindow::imagePopupMenu()
{
    if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) )
    {
        ImGui::OpenPopup("filePopup");
    }

    if (ImGui::BeginPopup("filePopup", ImGuiWindowFlags_Popup))
    {
        if (ImGui::Button("Button1"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}