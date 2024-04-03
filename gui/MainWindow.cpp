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
    tmp = base64_decode(exporString);
    exportIcon.loadFromMemory(tmp.data(), tmp.size());
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
        ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(tmp.data(), tmp.size(), 18, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
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
            drawExportFile();
            drawFilesystem();
            drawImage();

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
    ImGui::SetWindowSize({ImGui::GetWindowSize().x, size.y + 20});
    settingsWindowSize = ImGui::GetWindowSize();
    if ( ImGui::ImageButton(filesystemIcon, {35, 35}) ) 
    {
        filesystemOpenFlag = true;
    }
    if (currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "") 
    {
        ImGui::SameLine();
        if ( ImGui::ImageButton(exportIcon, {35, 35}) ) 
        {
            exportFileFlag = true;
        }
        ImGui::Separator();
        ImGui::Text("%s", fileImage[currentImage].filename.c_str());
        ImGui::Separator();
        if (ImGui::ImageButton(zoomoutIcon))
        {
            fileImage[currentImage].scale -= 0.1;
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(zoominIcon))
        {
            fileImage[currentImage].scale += 0.1;
        }
        ImGui::SliderFloat("##ScaleFoat", &fileImage[currentImage].scale, minFileScale, maxFileScale);
        ImGui::Separator();
        ImGui::Text("Point color");
        ImGui::InputInt2("PointPos", fileImage[currentImage].pixelPos);
        if ( fileImage[currentImage].pixelPos[0] >= 0 && fileImage[currentImage].pixelPos[0] <= fileImage[currentImage].image.getSize().x 
            && fileImage[currentImage].pixelPos[1] >= 0 && fileImage[currentImage].pixelPos[1] <= fileImage[currentImage].image.getSize().y )
        {
            ImGui::Text("%s", std::string("B -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).b)).c_str());
            ImGui::Text("%s", std::string("G -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).g)).c_str());
            ImGui::Text("%s", std::string("R -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).r)).c_str());
        }
        ImGui::Separator();
        if ( ImGui::Button("Median") )
        {
            // std::cout<<"Start"<<std::endl;
            // al.clearData();
            // al.setOrigImageSize(fileImage->getSize().x, fileImage->getSize().y);
            // for (size_t i = 0; i < fileImage->getSize().x; i += 1)
            // {
            //     for (size_t j = 0; j < fileImage->getSize().y; j += 1 )
            //     {
            //         al.setOrigImagePixel(i, j, {fileImage->getPixel(i, j).b, fileImage->getPixel(i, j).g, fileImage->getPixel(i, j).r});
            //     }
            // }
            // std::cout<<"Start median filter"<<std::endl;
            // al.medianFilter(medianFilterRadius);
            // std::cout<<"End median filter"<<std::endl;
            // // newFileImage = fileImage;
            // for (size_t i = 0; i < newFileImage.getSize().x; i += 1)
            // {
            //     for (size_t j = 0; j < newFileImage.getSize().y; j += 1 )
            //     {
            //         newFileImage.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
            //     }
            // }
            // std::cout<<"End"<<std::endl;
            // newFileImageFlag = true;
        }
        if ( ImGui::Button("Haar") )
        {

        }
    }
    if (fileImage.size()>0) ImGui::Separator();
    for (int i = 0; i < fileImage.size(); i +=1)
    {
        if ( ImGui::Button(std::string( fileImage[i].filename ).c_str()) )
        {
            fileImage[i].openFlag = true;
        }
        ImGui::SameLine();
        if ( ImGui::Button(std::string("X##" + std::to_string(i)).c_str()) )
        {
            fileImage.erase(std::next(fileImage.begin(), i));
        }
    }
    ImGui::Text("");
    ImGui::Text("");
    ImGui::Text("");

    ImGui::End();
}
void MainWindow::drawImage()
{
    for (int i = 0; i < fileImage.size(); i +=1)
    {
        if ( fileImage[i].openFlag )
        {
            ImGui::Begin(std::string( fileImage[i].filename ).c_str(), &(fileImage[i].openFlag), ImGuiWindowFlags_HorizontalScrollbar + ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowPos({settingsWindowSize.x + 10, 10}, ImGuiCond_Once);
            ImGui::SetWindowSize({(float)fileImage[i].image.getSize().x + 25, (float)fileImage[i].image.getSize().y + 40}, ImGuiCond_Once);
            fileWindowPos = ImGui::GetWindowPos();
            fileWindowSize = ImGui::GetWindowSize();

            if ( ImGui::IsWindowFocused() )
            {
                currentImage = i;
            }

            fileImage[i].texture.loadFromImage(fileImage[i].image);
            fileImage[i].sprite.setTexture(fileImage[i].texture);

            fileImage[i].sprite.setScale(fileImage[i].scale, fileImage[i].scale);
            ImGui::Image(fileImage[i].sprite);

            if ( ImGui::GetWindowPos().x < settingsWindowSize.x ) ImGui::SetWindowPos({settingsWindowSize.x, ImGui::GetWindowPos().y});
            if ( ImGui::GetWindowPos().x + ImGui::GetWindowWidth() > size.x ) ImGui::SetWindowPos({size.x - ImGui::GetWindowWidth(), ImGui::GetWindowPos().y});
            if ( ImGui::GetWindowPos().y < 0 ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, 0});
            if ( ImGui::GetWindowPos().y + ImGui::GetWindowHeight() > size.y ) ImGui::SetWindowPos({ImGui::GetWindowPos().x, size.y - ImGui::GetWindowSize().y});

            // imagePopupMenu();

            ImGui::End();
        }
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
    imageStruct tmp;
    std::cout<<tmp.image.loadFromFile(pathToImage)<<std::endl;
    tmp.filename = pathToImage.filename().string();
    tmp.scale = 1;

    fileImage.push_back(tmp);
    filesystemOpenFlag = false;
    tmp.openFlag = true;
}
void MainWindow::imagePopupMenu()
{
    if ( sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) )
    {
        ImGui::OpenPopup("filePopup");
    }

    if (ImGui::BeginPopup("filePopup", ImGuiWindowFlags_Popup))
    {
        ImGui::EndPopup();
    }
}
void MainWindow::drawExportFile()
{
    if (!filesystemOpenFlag && exportFileFlag && ImGui::Begin("Export to", &exportFileFlag, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SetWindowFocus();
        std::stringstream ss(currentPath.string());
        std::string s;
        pathTmp.clear();
        while(getline(ss, s, '/'))
        {
            if (s != "") pathTmp.push_back(s);
        }
        
        ImGui::BeginChild("PathToFileExport", {-1, 50}, true, ImGuiWindowFlags_HorizontalScrollbar);
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

        if (ImGui::BeginChild("fs", {0,ImGui::GetWindowSize().y - 150}, true))
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
                        // if (ImGui::Button(item.path().filename().c_str(), {ImGui::GetWindowSize().x, 25})) openImage(item.path());
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

            ImGui::InputTextWithHint("##FileName", "File name", exportFileName, 1024);
            if (ImGui::Button("Save") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
            {
                fileImage[currentImage].image.saveToFile(currentPath.string() + exportFileName);
            }
        }
        ImGui::End();
    }
}