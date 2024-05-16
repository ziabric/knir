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
    tmp = base64_decode(detectIconString);
    detectIcon.loadFromMemory(tmp.data(), tmp.size());
    tmp = base64_decode(mainIconString);
    mainIcon.loadFromMemory(tmp.data(), tmp.size());
    // window->setIcon(64, 64, tmp.data());
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
            drawMetricsWindow();
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
    ImGui::SameLine();
    if ( ImGui::ImageButton(mainIcon, {35,35}) )
    {
        if (fileImage.size() > 0) showMetricsWindowFlag = true;
    }
    if (currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "") 
    {
        ImGui::SameLine();
        if ( ImGui::ImageButton(exportIcon, {35, 35}) )
        {
            exportFileFlag = true;
        }
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::Text("%s", fileImage[currentImage].filename.c_str());
        ImGui::Separator();
        if (ImGui::ImageButton(zoomoutIcon))
        {
            fileImage[currentImage].scale -= scaleStep;
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(zoominIcon))
        {
            fileImage[currentImage].scale += scaleStep;
        }
        ImGui::SliderFloat("##ScaleFoat", &fileImage[currentImage].scale, minFileScale, maxFileScale);
        ImGui::Separator();
        ImGui::Text("%s", std::string("Work time => " + std::to_string(fileImage[currentImage].workTime) + " ms").c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("MSE => b: " + std::to_string(fileImage[currentImage].mseValue.b) + " g: " + std::to_string(fileImage[currentImage].mseValue.g) + " r: " + std::to_string(fileImage[currentImage].mseValue.r)).c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("PSNR => b: " + std::to_string(fileImage[currentImage].psnrValue.b) + " g: " + std::to_string(fileImage[currentImage].psnrValue.g) + " r: " + std::to_string(fileImage[currentImage].psnrValue.r)).c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("SSIM => b: " + std::to_string(fileImage[currentImage].ssimValue.b) + " g: " + std::to_string(fileImage[currentImage].ssimValue.g) + " r: " + std::to_string(fileImage[currentImage].ssimValue.r)).c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("Width  => " + std::to_string(fileImage[currentImage].image.getSize().x)).c_str());
        ImGui::Text("%s", std::string("Height => " + std::to_string(fileImage[currentImage].image.getSize().y)).c_str());
        ImGui::Separator();
        ImGui::Text("Point color");
        ImGui::InputInt2("##PointPos", fileImage[currentImage].pixelPos);
        if ( fileImage[currentImage].pixelPos[0] >= 0 && fileImage[currentImage].pixelPos[0] < fileImage[currentImage].image.getSize().x 
            && fileImage[currentImage].pixelPos[1] >= 0 && fileImage[currentImage].pixelPos[1] < fileImage[currentImage].image.getSize().y )
        {
            ImGui::Text("%s", std::string("B: " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).b)).c_str());
            ImGui::SameLine();
            ImGui::Text("%s", std::string(" G: " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).g)).c_str());
            ImGui::SameLine();
            ImGui::Text("%s", std::string(" R: " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).r)).c_str());
        }
        ImGui::Separator();
        ImGui::Text("Median");
        ImGui::SliderInt("radius##Median", &medianFilterRadius, 1, 50);
        if ( ImGui::Button("Start##Median") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start median filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.medianFilter(medianFilterRadius);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End median filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "med" + std::to_string(medianFilterRadius) + "_" + fileImage[currentImage].filename;

            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        ImGui::Separator();
        if ( ImGui::Button("Haar") )
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start haar filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.haar();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End haar filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "haar_" + fileImage[currentImage].filename;

            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        ImGui::Separator();
        ImGui::Text("new Median");
        ImGui::SliderInt("radius##newMedian", &newMedianFilterRadius, 1, 50);
        if ( ImGui::Button("Start##newMedian") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start new median filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.newMedianFilter(newMedianFilterRadius);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End new median filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "nmed" + std::to_string(newMedianFilterRadius) + "_" + fileImage[currentImage].filename;

            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        ImGui::Separator();
        ImGui::Text("Bilateral Gaus");
        ImGui::SliderInt("Kernal##gaus", &gausKernalSize, 1, 50);
        ImGui::SliderInt("Spatial_k##gaus", &gausSpatialSigma, 1, 500);
        ImGui::SliderInt("Intensity_k##gaus", &gausIntensitySigma, 1, 500);
        if ( ImGui::Button("Start##gausBase") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start gaus filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.bilateralFilter(gausKernalSize, gausSpatialSigma, gausIntensitySigma);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End gaus filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "gb_" + std::to_string(gausKernalSize) + "_" + std::to_string(gausSpatialSigma) + "_" + std::to_string(gausIntensitySigma) + "_" + fileImage[currentImage].filename;
            
            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        
        ImGui::Separator();
        ImGui::Text("Bilateral Gaus range");
        ImGui::SliderInt("Kernal##gausRange", &gausRangeKernalSize, 1, 500);
        ImGui::SliderInt("Spatial_k##gausRange", &gausRangeSpatialSigma, 1, 500);
        ImGui::SliderInt("Range_k##gausRange", &gausRangeSigma, 1, 50);
        if ( ImGui::Button("Start##gausRange") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start gaus range filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.bilateralFilterRange(gausRangeKernalSize, gausRangeSpatialSigma, gausRangeSigma);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End gaus range filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "gr_" + std::to_string(gausRangeKernalSize) + "_" + std::to_string(gausRangeSpatialSigma) + "_" + std::to_string(gausRangeSigma) + "_" + fileImage[currentImage].filename;

            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        
        ImGui::Separator();
        ImGui::Text("new Bilateral Gaus");
        ImGui::SliderInt("Kernal##newBilatRange", &bilatRadius, 1, 50);
        ImGui::SliderInt("Spatial_k##newBilatRange", &bilatSpatialSigma, 1, 500);
        ImGui::SliderInt("Intensity_k##newBilatRange", &bilatIntensitySigma, 1, 500);
        if ( ImGui::Button("Start##newBilatRange") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
        {
            std::cout<<"Start"<<std::endl;
            al.clearData();
            al.setOrigImageSize(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            for (size_t i = 0; i < fileImage[currentImage].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[currentImage].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[currentImage].image.getPixel(i, j).b, (unsigned int)fileImage[currentImage].image.getPixel(i, j).g, (unsigned int)fileImage[currentImage].image.getPixel(i, j).r});
                }
            }
            std::cout<<"Start new bilat filter"<<std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            al.bilateralFilterRange(bilatRadius, bilatSpatialSigma, bilatIntensitySigma);
            auto end = std::chrono::high_resolution_clock::now();
            std::cout<<"End gaus range filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "bl_" + std::to_string(bilatRadius) + "_" + std::to_string(bilatSpatialSigma) + "_" + std::to_string(bilatIntensitySigma) + "_" + fileImage[currentImage].filename;

            std::chrono::duration<double> duration = end - start;
            newImage.workTime = duration.count() * 1000;
            newImage.mseValue = al.getMSE();
            newImage.psnrValue = al.getPSNR();
            newImage.ssimValue = al.getSSIM(10, 1, 1);

            newImage.image = fileImage[currentImage].image;
            for (size_t i = 0; i < newImage.image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < newImage.image.getSize().y; j += 1 )
                {
                    newImage.image.setPixel(i, j, {(sf::Uint8)al.getModImagePixel(i, j).r, (sf::Uint8)al.getModImagePixel(i, j).g, (sf::Uint8)al.getModImagePixel(i, j).b});
                }
            }
            std::cout<<"End"<<std::endl;

            newImage.texture.loadFromImage(newImage.image);
            newImage.sprite.setTexture(newImage.texture);

            fileImage.push_back(newImage);
        }
        ImGui::Separator();
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

            ImGui::SetWindowSize({(float)fileImage[i].image.getSize().x + 25, (float)fileImage[i].image.getSize().y + 40}, ImGuiCond_Once);

            ImGui::End();
        }
    }
}
float MainWindow::GetColumnDistance(int n)
{
    return ImGui::GetColumnOffset(n + 1) - ImGui::GetColumnOffset(n);
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

        if (ImGui::BeginChild("fs", {0, ImGui::GetWindowSize().y - 125}, true))
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
            ImGui::SameLine();
            if (ImGui::Button("Save") && currentImage >= 0 && currentImage < fileImage.size() && fileImage[currentImage].filename != "")
            {
                fileImage[currentImage].image.saveToFile(currentPath.string() + "/" + exportFileName);
            }
        }
        ImGui::End();
    }
}
void MainWindow::drawMetricsWindow()
{
    if (showMetricsWindowFlag && ImGui::Begin("Metrics", &showMetricsWindowFlag, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Columns(2);

        if (ImGui::BeginChild("OrigImage", ImVec2(0, ImGui::GetWindowHeight() - 150), true))
        {
            ImGui::Text("Orig image");
            ImGui::Separator();

            for (int i = 0; i < fileImage.size(); i += 1) 
            {
                if (ImGui::RadioButton(std::string(fileImage[i].filename).c_str(), i == origImageM)) origImageM = i;
            }

            ImGui::EndChild();
        }

        ImGui::NextColumn();

        if (ImGui::BeginChild("ModImage", ImVec2(0, ImGui::GetWindowHeight() - 150), true))
        {
            ImGui::Text("Mod image");
            ImGui::Separator();

            for (int i = 0; i < fileImage.size(); i += 1) 
            {
                if (ImGui::RadioButton(std::string(fileImage[i].filename).c_str(), i == modImageM)) modImageM = i;
            }

            ImGui::EndChild();
        }

        ImGui::Columns(1);

        if (ImGui::Button("Get metrics"))
        {
            al.clearData();
            
            al.setOrigImageSize(fileImage[modImageM].image.getSize().x, fileImage[modImageM].image.getSize().y);
            for (size_t i = 0; i < fileImage[modImageM].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[modImageM].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[modImageM].image.getPixel(i, j).b, (unsigned int)fileImage[modImageM].image.getPixel(i, j).g, (unsigned int)fileImage[modImageM].image.getPixel(i, j).r});
                }
            }
            
            al.swapOrigMod();

            al.setOrigImageSize(fileImage[origImageM].image.getSize().x, fileImage[origImageM].image.getSize().y);
            for (size_t i = 0; i < fileImage[origImageM].image.getSize().x; i += 1)
            {
                for (size_t j = 0; j < fileImage[origImageM].image.getSize().y; j += 1 )
                {
                    al.setOrigImagePixel(i, j, {(unsigned int)fileImage[origImageM].image.getPixel(i, j).b, (unsigned int)fileImage[origImageM].image.getPixel(i, j).g, (unsigned int)fileImage[origImageM].image.getPixel(i, j).r});
                }
            }


            metricsMSE.b = (al.getMSE().b + al.getMSE().g + al.getMSE().r) / 3;
            metricsPSNR.b = (al.getPSNR().b + al.getPSNR().g + al.getPSNR().r) / 3;
            metricsSSIM.b = (al.getSSIM(10, 1, 1).b + al.getSSIM(10, 1, 1).g + al.getSSIM(10, 1, 1).r)/ 3;
        }
        ImGui::Separator();
        ImGui::Text("%s", std::string("MSE => " + std::to_string(metricsMSE.b)).c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("PSNR => " + std::to_string(metricsPSNR.b)).c_str());
        ImGui::Separator();
        ImGui::Text("%s", std::string("SSIM => " + std::to_string(metricsSSIM.b)).c_str());

        ImGui::End();
    }
}