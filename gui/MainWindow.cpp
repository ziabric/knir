#include <MainWindow.hpp>

MainWindow::MainWindow(std::string title_, int width_, int height_) :
size(width_, height_)
{
    window = new sf::RenderWindow(sf::VideoMode(width_, height_), title_);
    window->setFramerateLimit(60);

    try
    {
        std::string path = std::filesystem::current_path().string() + "/yolov5s.onnx";
        net = readNet(path);
        // net = readNetFromONNX(path); 
        canLoadModel = true;

        std::ifstream ifs( std::filesystem::current_path().string() + "coco.yaml");
        std::string line;
        while (getline(ifs, line))
        {
            class_list.push_back(line);
        }
    }
    catch (std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
        std::cout<<"Failed to load model :("<<std::endl;
        canLoadModel = false;
    }
    
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
        ImGui::SameLine();
        if ( canLoadModel && ImGui::ImageButton(detectIcon, {35, 35}) )
        {
            cv::Size size(fileImage[currentImage].image.getSize().x, fileImage[currentImage].image.getSize().y);
            cv::Mat cvImg(size, CV_8UC4, (void*)fileImage[currentImage].image.getPixelsPtr());
            cv::cvtColor(cvImg, cvImg, cv::COLOR_BGRA2BGR);
            std::vector<Mat> detections = pre_process(cvImg, net);
            Mat img = post_process(cvImg, detections, class_list); 
            imageStruct newImage;
            newImage.filename = "dec_" + fileImage[currentImage].filename;
            
            cv::Mat convertedImg;
            cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
            newImage.image.create(convertedImg.cols, convertedImg.rows, convertedImg.data); 

            fileImage.push_back(newImage);
        }
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
        ImGui::Text(std::string("Width  -- " + std::to_string(fileImage[currentImage].image.getSize().x)).c_str());
        ImGui::Text(std::string("Height -- " + std::to_string(fileImage[currentImage].image.getSize().y)).c_str());
        ImGui::Text("Point color");
        ImGui::InputInt2("##PointPos", fileImage[currentImage].pixelPos);
        if ( fileImage[currentImage].pixelPos[0] >= 0 && fileImage[currentImage].pixelPos[0] < fileImage[currentImage].image.getSize().x 
            && fileImage[currentImage].pixelPos[1] >= 0 && fileImage[currentImage].pixelPos[1] < fileImage[currentImage].image.getSize().y )
        {
            ImGui::Text("%s", std::string("B -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).b)).c_str());
            ImGui::Text("%s", std::string("G -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).g)).c_str());
            ImGui::Text("%s", std::string("R -- " + std::to_string(fileImage[currentImage].image.getPixel(fileImage[currentImage].pixelPos[0], fileImage[currentImage].pixelPos[1]).r)).c_str());
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
            al.medianFilter(medianFilterRadius);
            std::cout<<"End median filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "med" + std::to_string(medianFilterRadius) + "_" + fileImage[currentImage].filename;

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
            al.haar();
            std::cout<<"End haar filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "haar_" + fileImage[currentImage].filename;

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
            al.newMedianFilter(newMedianFilterRadius);
            std::cout<<"End new median filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "nmed" + std::to_string(newMedianFilterRadius) + "_" + fileImage[currentImage].filename;

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
        ImGui::SliderInt("Spatial_k##gaus", &gausSpatialSigma, 1, 50);
        ImGui::SliderInt("Intensity_k##gaus", &gausIntensitySigma, 1, 50);
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
            al.bilateralFilter(gausKernalSize, gausSpatialSigma, gausIntensitySigma);
            std::cout<<"End gaus filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "gb_" + std::to_string(gausKernalSize) + "_" + std::to_string(gausSpatialSigma) + "_" + std::to_string(gausIntensitySigma) + "_" + fileImage[currentImage].filename;

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
        ImGui::SliderInt("Kernal##gausRange", &gausRangeKernalSize, 1, 50);
        ImGui::SliderInt("Spatial_k##gausRange", &gausRangeSpatialSigma, 1, 50);
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
            al.bilateralFilterRange(gausRangeKernalSize, gausRangeSpatialSigma, gausRangeSigma);
            std::cout<<"End gaus range filter"<<std::endl;

            imageStruct newImage;
            newImage.filename = "gr_" + std::to_string(gausRangeKernalSize) + "_" + std::to_string(gausRangeSpatialSigma) + "_" + std::to_string(gausRangeSigma) + "_" + fileImage[currentImage].filename;

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
            // ImGui::SetWindowSize({(float)fileImage[i].image.getSize().x * fileImage[i].scale + 25, (float)fileImage[i].image.getSize().y * fileImage[i].scale + 40});
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

            // if (
            //     ImGui::GetWindowPos().x < ImGui::GetMousePos().x &&
            //     ImGui::GetWindowPos().y < ImGui::GetMousePos().y &&
            //     ImGui::GetWindowSize().x > ImGui::GetMousePos().x &&
            //     ImGui::GetWindowSize().y > ImGui::GetMousePos().y &&
            //     sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
            //     ImGui::GetIO().MouseWheel != 0
            // )
            // {
            //     std::cout<<"chack"<<std::endl;
            //     fileImage[i].scale += (ImGui::GetIO().MouseWheel > 0) ? scaleStep/10 : -scaleStep/10;
            // }

            
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
void MainWindow::draw_label(Mat& input_image, std::string label, int left, int top)
{
    int baseLine;
    Size label_size = getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
    top = max(top, label_size.height);
    Point tlc = Point(left, top);
    Point brc = Point(left + label_size.width, top + label_size.height + baseLine);
    rectangle(input_image, tlc, brc, BLACK, FILLED);
    putText(input_image, label, Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS);
}
std::vector<Mat> MainWindow::pre_process(Mat &input_image, Net &net)
{
    Mat blob;
    blobFromImage(input_image, blob, 1./255., Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(), true, false);
 
    net.setInput(blob);
 
    std::vector<Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
 
    return outputs;
}
Mat MainWindow::post_process(Mat &input_image, std::vector<Mat> &outputs, const std::vector<std::string> &class_name)
{
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<Rect> boxes;
    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;
    float *data = (float *)outputs[0].data;
    const int dimensions = 85;
    const int rows = 25200;
    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];
        if (confidence >= CONFIDENCE_THRESHOLD)
        {
            float * classes_scores = data + 5;
            Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
            Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > SCORE_THRESHOLD)
            {
                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);
                float cx = data[0];
                float cy = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((cx - 0.5 * w) * x_factor);
                int top = int((cy - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
        data += 85;
    }
    std::vector<int> indices;
    NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
    for (int i = 0; i < indices.size(); i++)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        int left = box.x;
        int top = box.y;
        int width = box.width;
        int height = box.height;
        rectangle(input_image, Point(left, top), Point(left + width, top + height), BLUE, 3*THICKNESS);
        std::string label = format("%.2f", confidences[idx]);
        label = class_name[class_ids[idx]] + ":" + label;
        draw_label(input_image, label, left, top);
    }
    return input_image;
}


