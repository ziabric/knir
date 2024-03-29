#include <MainWindow.hpp>

MainWindow::MainWindow(std::string title_, int width_, int height_) :
size(width_, height_)
{
    window = new sf::RenderWindow(sf::VideoMode(width_, height_), title_);
    window->setFramerateLimit(60);

    auto tmp = base64_decode(filesystemIconString);
    filesystemIcon.loadFromMemory(tmp.data(), tmp.size());
    tmp = base64_decode(zoominIconString);
    zoominIcon.loadFromMemory(tmp.data(), tmp.size());
    tmp = base64_decode(zoomoutIconString);
    zoomoutIcon.loadFromMemory(tmp.data(), tmp.size());

}
MainWindow::~MainWindow()
{
    ImGui::SFML::Shutdown();
    delete window;
}
int MainWindow::mainloop()
{
    if (ImGui::SFML::Init(*window))
    {
        auto tmp = base64_decode(fontString);
        ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(tmp.data(), tmp.size(), 13);
        if ( !ImGui::SFML::UpdateFontTexture() )
        {
            return 0;
        }
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
            ImGui::PushFont(font1);

            size = {(float)window->getSize().x, (float)window->getSize().y};
            
            drawInterface();
            drawImage();
            drawFilesystem();

            ImGui::PopFont();

            window->clear();
            ImGui::SFML::Render(*window);
            window->display();
        }
    }
    return 0;
}
void MainWindow::drawInterface()
{
    ImGui::Begin("KNIR", nullptr, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos({0,0});
    ImGui::SetWindowSize({ImGui::GetWindowSize().x, size.y + 10});
    settingsWindowSize = ImGui::GetWindowSize();

    if ( ImGui::ImageButton(filesystemIcon) ) 
    {
        filesystemOpenFlag = true;
    }
    
    // if ( ImGui::Button("Edit") )
    // {

    // }
    // if ( ImGui::Button("Move") )
    // {

    // }
    // if ( ImGui::Button("Rot") )
    // {

    // }

    if ( ImGui::ImageButton(zoominIcon) )
    {
        if (fileScale < maxFileScale) fileScale += 0.1;
    }
    if ( ImGui::ImageButton(zoomoutIcon) )
    {
        if (fileScale > minFileScale) fileScale -= 0.1;
    }
    if ( ImGui::Button("Medi") )
    {
        al.clearData();
        al.setOrigImageSize(fileImage.getSize().x, fileImage.getSize().y);
        for (size_t i = 0; i < fileImage.getSize().x; i += 1)
        {
            for (size_t j = 0; j < fileImage.getSize().y; j += 1 )
            {
                al.setOrigImagePixel(i, j, {fileImage.getPixel(i, j).b, fileImage.getPixel(i, j).g, fileImage.getPixel(i, j).r});
            }
        }
        al.medianFilter(medianFilterRadius);
        
        newFileImage = fileImage;
        for (size_t i = 0; i < newFileImage.getSize().x; i += 1)
        {
            for (size_t j = 0; j < newFileImage.getSize().y; j += 1 )
            {
                newFileImage.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
            }
        }
    }
    if ( ImGui::Button("Haar") )
    {

    }
    ImGui::End();
}
void MainWindow::drawImage()
{
    if (fileImage.getSize().x > 0 && fileImage.getSize().y > 0)
    {
        ImGui::Begin(file.filename().c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar + ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos({settingsWindowSize.x + 10, 10}, ImGuiCond_Once);
        fileWindowPos = ImGui::GetWindowPos();
        fileWindowSize = ImGui::GetWindowSize();

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
    if (newFileImage.getSize().x > 0 && newFileImage.getSize().y > 0)
    {
        ImGui::Begin(std::string("Median_" + file.filename().string()).c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar + ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos({settingsWindowSize.x + 10, 10}, ImGuiCond_Once);
        fileWindowPos = ImGui::GetWindowPos();
        fileWindowSize = ImGui::GetWindowSize();

        newFileTexture.loadFromImage(newFileImage);
        newFileSprite.setTexture(newFileTexture);
        newFileSprite.setScale(fileScale, fileScale);

        if ( ImGui::GetWindowPos().x < settingsWindowSize.x ) ImGui::SetWindowPos({settingsWindowSize.x, ImGui::GetWindowPos().y});
        if ( ImGui::GetWindowPos().x + ImGui::GetWindowWidth() > size.x ) ImGui::SetWindowPos({size.x - ImGui::GetWindowWidth(), ImGui::GetWindowPos().y});
        if ( ImGui::GetWindowPos().y < 0 ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, 0});
        if ( ImGui::GetWindowPos().y + ImGui::GetWindowHeight() > size.y ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, size.y - ImGui::GetWindowSize().y});

        imagePopupMenu();

        ImGui::Image(fileSprite);

        ImGui::End();
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
                        ImGui::PushStyleColor(ImGuiCol_Button, {0.55, 0.714, 0, 1});
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.7, 0.714, 0, 1});
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.56, 0.814, 0, 1});
                        if (ImGui::Button(item.path().filename().c_str(), {ImGui::GetWindowSize().x, 25})) openImage(item.path());
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
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
    fileImage.loadFromFile(file.string());
    fileScale = 1;
    filesystemOpenFlag = false;
}
void MainWindow::imagePopupMenu()
{
    if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) )
    {
        ImGui::OpenPopup("filePopup");
    }

    if (ImGui::BeginPopup("filePopup", ImGuiWindowFlags_Popup))
    {
        if(ImGui::Button("1x"))
        {
            fileScale = 1;
        }
        ImGui::SameLine();
        ImGui::SliderFloat("Scale", &fileScale, minFileScale, maxFileScale);
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