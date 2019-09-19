#pragma once

#include <algorithm>
#include <cmath>

typedef unsigned char uchar;
std::vector<std::string> get_files_in_folder (std::string path, std::string type = "", bool return_full = false, bool sort = true){
    std::vector<std::string> file_vec;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.')
                file_vec.emplace_back(return_full? path+"/"+ent->d_name:ent->d_name);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        EXIT_FAILURE;
    }
    if (sort) std::sort(file_vec.begin(),file_vec.end());
    
    if(type == "") return file_vec;
    
    std::vector<std::string> filtered;
    for (auto name: file_vec) {
        if(name.size() > type.size()) {
            std::string tmp = name.substr(name.size()-type.size(), type.size());
            if (tmp == type) filtered.push_back(name);
        }
    }
    return filtered;
}

void create_folder(std::string name){
#ifdef WIN32
    _mkdir(name.c_str());
#else
    mkdir(name.c_str(), 0755);
#endif
}

bool checkfolderexist(const std::string& output_db_name){
#ifdef WIN32
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;  //something is wrong with your path!
    
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;   // this is a directory!
    
    return false;    // this is not a directory!
#else
    //        const char* folderr = output_db_name;
    //        folderr = output_db_name;
    struct stat sb;
    if (stat(output_db_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        //            printf("YES\n");
        //            remove(output_db_name.c_str());
        //            remove_directory(output_db_name.c_str());
        return true;
    }
    else
    {
        return false;
        //            printf("NO\n");
    }
#endif
}

void check_and_create_folder (const std::string& path){
    if(!checkfolderexist(path))create_folder(path);
}

#ifdef hasLabel
void saveColorMap(){
    std::ofstream file(pL);
    if(!file.is_open()){
        std::cerr << "Cannot open label file\n" << std::endl;
    }
    std::string label, r,g,b,a;
    for (auto list : labelColorList){
        label = std::to_string(list.first);
        r = std::to_string(list.second[0]);
        g = std::to_string(list.second[1]);
        b = std::to_string(list.second[2]);
        a = std::to_string(list.second[3]);
        file << label << " " << r << " " << g << " " << b << " " << a << std::endl;
    }
}

void generateColorTexture(){
    int edgeSize = 5;
    check_and_create_folder(textureFolderName);
    for(const auto& pair : labelColorList){
        FILE *imageFile;
        int x,y,height=100,width=100;
        std::string fileName = textureFolderName + "/" + std::to_string(pair.first) + ".ppm";
        imageFile = fopen(fileName.c_str(),"wb");
        if(imageFile == NULL){
            perror("ERROR: Cannot open output file");
            exit(EXIT_FAILURE);
        }
        
        fprintf(imageFile,"P6\n");           // P5 filetype
        fprintf(imageFile,"%d %d\n",width,height);   // dimensions
        fprintf(imageFile,"255\n");          // Max pixel
        
        /* Now write a greyscale ramp */
        for(x=0;x<height;x++){
            for(y=0;y<width;y++){
                if(x < edgeSize || y < edgeSize || x > height-1 - 5 || y > width-1 - 5){
                    fputc(0,imageFile);
                    fputc(0,imageFile);
                    fputc(0,imageFile);
                } else {
                    fputc(pair.second[0],imageFile);
                    fputc(pair.second[1],imageFile);
                    fputc(pair.second[2],imageFile);
                }
            }
        }
        fclose(imageFile);
    }
}

void loadColorTexture(){
    auto vec = get_files_in_folder(textureFolderName + "/", "ppm", true, true);
    if(vec.size() != labelColorList.size())
        printf("[WARNING] The label number (%zu) differ from the textures number (%zu).\n", labelColorList.size(), vec.size());
    if(vec.size())
        for(const auto& pair : labelColorList)
            labelImageList[pair.first] = vec[pair.first];
}

void generateRandomColorMap(){
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> dist(0, 255);
    
    static const int maxLabelNum = std::numeric_limits<ushort>::max() + 1;
    for (int i = 0; i < maxLabelNum; i++){
        int r=0,g=0,b=0;
        r = dist(e1);
        g = dist(e1);
        b = dist(e1);
        while (r + g + b < 255){
            r = dist(e1);
            g = dist(e1);
            b = dist(e1);
        }
        labelColorList[i][0] = static_cast<uchar>(r);
        labelColorList[i][1] = static_cast<uchar>(g);
        labelColorList[i][2] = static_cast<uchar>(b);
        labelColorList[i][3] = static_cast<uchar>(255);
    }
}

void loadLabelColor(){
    std::ifstream file(pL);
    if(!file.is_open()){
        return;
    }
    std::string label, r,g,b,a;
    labelColorList.clear();
    while(file >> label >> r >> g >> b >> a){
        labelColorList[uchar(std::stoi(label))][0] = static_cast<uchar>(std::stoi(r));
        labelColorList[uchar(std::stoi(label))][1] = static_cast<uchar>(std::stoi(g));
        labelColorList[uchar(std::stoi(label))][2] = static_cast<uchar>(std::stoi(b));
        labelColorList[uchar(std::stoi(label))][3] = static_cast<uchar>(std::stoi(a));
    }
}

#endif

void saveMeshWithPointPly(size_t i, const std::vector<float> &vPoints, const std::vector<int> & vTriangles, const std::vector<uchar> &vLabels){
    std::ofstream fout;
    fout.open(pO + std::to_string(i) + extension);
    if (fout.fail()){
        std::string errmsg = "Cannot open file at ";
        throw errmsg;
    }
    
    fout << "ply" << std::endl;
    fout << "format ascii 1.0" << std::endl;
    if(bbb)
        fout << "element vertex " << vPoints.size()/3 + 8 << std::endl;
    else
        fout << "element vertex " << vPoints.size()/3 << std::endl;
    fout << "property float x" << std::endl;
    fout << "property float y" << std::endl;
    fout << "property float z" << std::endl;
    fout << "property uchar red" << std::endl;
    fout << "property uchar green" << std::endl;
    fout << "property uchar blue" << std::endl;
    fout << "property uchar alpha" << std::endl;
    fout << "element face " << vTriangles.size()/3 << std::endl;
    fout << "property list uchar int vertex_indices" << std::endl;
    fout << "end_header" << std::endl;
    
    for(int p=0; p< vPoints.size()/3 ; p++){
        
        const float* pt = &vPoints[p*3];
        fout << pt[0] << " " << pt[1] << " " << pt[2] << " ";
#ifdef hasLabel
        const auto& co = labelColorList.at(vLabels[12*uint(p/8)]);
        fout << static_cast<int>(co[0]) << " " << static_cast<int>(co[1]) << " " << static_cast<int>(co[2]) << " " << static_cast<int>(co[3]) << " ";
#else
        fout << 255 << " " << 255 << " " << 255 << " " << 255 << " ";
#endif
        fout << std::endl;
    }
    
    // CREATE VETEX ON BOUNDARIES
    if(bbb){
        auto addPoint = [&] (float x, float y, float z){
            fout << x*sW << " " << y*sH << " " << z*sD << " ";
#ifdef hasLabel
            fout << 255 << " " << 255 << " " << 255 << " " << 255 << "\n";
#endif
        };
        addPoint(0,0,0);
        addPoint(0,0,1);
        addPoint(0,1,0);
        addPoint(0,1,1);
        addPoint(1,0,0);
        addPoint(1,0,1);
        addPoint(1,1,0);
        addPoint(1,1,1);
    }
    
    for(int p=0; p< vTriangles.size()/3; ++p){
        const int* vertice = &vTriangles[p*3];
        fout << 3;
        if(bff)
            for(int i=0; i < 3; ++i)
                fout << " " << vertice[i];
        else
            for(int i=2; i >=0; --i)
                fout << " " << vertice[i];
        fout << std::endl;
    }
    fout.close();
}

void saveMeshWithPoint3MF(size_t i, const std::vector<float> &vPoints, const std::vector<int> & vTriangles, const std::vector<uchar> &vLabels){
    #ifndef USE_3MF
    printf("[Warning] Did not compile with 3MF. Return\n");
    return;
    #else
    NMR::MF* mf = new NMR::MF();
    if(verbal)
        mf->setVerbal(verbal);
    mf->init();
    mf->setPoints(vPoints.data(), vPoints.size());

#ifdef hasLabel
    if(!bt)
        mf->setColors(vLabels.data(), labelColorList, vLabels.size());
    else
        mf->setTextures(vLabels.data(), labelImageList, vLabels.size());
#endif

    mf->setTriangles(vTriangles.data(), vTriangles.size());
    mf->build(pO + std::to_string(i));
    delete mf;
    #endif
}

template <typename T>
void loadVolume(){
    FILE *fp;
    if((fp = fopen(pI.c_str(), "rb")) == NULL ) {
        printf("[Error] unable to open file to read. [%s]", pI.c_str());
        exit(-1);
    }
    vVolumes.resize(sN);
    for(int i=0; i < sN; ++i){
        vVolumes[i] = new T[sW * sH * sD];
        fread(vVolumes[i], sizeof(T), sW * sH * sD,  fp);
    }
}

template <typename T>
void processRaw(size_t i, void* mat){
    std::vector<float> points;
    std::vector<uchar> labels;
    std::vector<int> triangles;
    
    float scaleX = s;
    float scaleY = s;
    float scaleZ = s;
    float scalehalf = s*sb/2;
    points.clear();
#ifdef hasLabel
    labels.clear();
#endif
    triangles.clear();
    
    // READ FROM DATA
    for(int z=0; z < sD; ++z){
        for(int y=0; y < sH; ++y){
            for(int x=0; x < sW; ++x){
                int idx = (z*sH+y)*sW+x;
#ifdef hasLabel
                T label = static_cast<T*>(mat)[ idx ];
                if(label ==0 || label == 255) continue;
#endif
#ifdef hasTSDF
                T tsdf = static_cast<T*>(mat)[idx];
                if(tsdf==0 || tsdf <= -0.99 | tsdf >= 0.99 )
                    continue;
                float tx = static_cast<float*>(mat)[(z*sW+y)*sH+x+1];
                float ty = static_cast<float*>(mat)[(z*sW+y+1)*sH+x];
                float tz = static_cast<float*>(mat)[((z+1)*sW+y)*sH+x];
                // Check zero-crossing
                bool isSurface = ( (tsdf > 0) && (tx < 0 || ty < 0 || tz < 0 ) ) || ( (tsdf < 0) && (tx > 0 || ty > 0 || tz > 0 ) );
                if( !isSurface  ) continue;
#endif
#ifdef hasBinary
                T tsdf = static_cast<T*>(mat)[idx];;
                bool isSurface = tsdf;
                if( !isSurface  ) continue;
#endif
#ifdef hasInvertTSDF
                T tsdf = static_cast<T*>(mat)[idx];
                if (!(std::abs(tsdf) > 0.8f && std::abs(tsdf) < 1.0f))
                    continue;
#endif
#ifdef hasOccupancy
                if(static_cast<T*>(mat)[idx] <= 1.0f) continue;
#endif
                auto addPoint = [&] (float x_, float y_, float z_) -> int{
                    int idx = points.size()/3;
                    points.push_back(x * scaleX + x_ * scalehalf);
                    points.push_back(y * scaleY + y_ * scalehalf);
                    points.push_back(z * scaleZ + z_ * scalehalf);
                    return idx;
                };
                // 8
                const auto& point000 = addPoint(-1, -1, -1);
                const auto& point002 = addPoint(-1, -1, 1);
                const auto& point020 = addPoint(-1, 1, -1);
                const auto& point022 = addPoint(-1, 1, 1);
                const auto& point200 = addPoint(1, -1, -1);
                const auto& point202 = addPoint(1, -1, 1);
                const auto& point220 = addPoint(1, 1, -1);
                const auto& point222 = addPoint(1, 1, 1);
                
                auto addPolygon = [&](int p1, int p2, int p3){
                    triangles.push_back(p1);
                    triangles.push_back(p2);
                    triangles.push_back(p3);
#ifdef hasLabel
                    labels.push_back(label);
#endif
                };
                // 12
                addPolygon(point000,point022,point002);
                addPolygon(point000,point020,point022);
                addPolygon(point200,point020,point000);
                addPolygon(point200,point220,point020);
                addPolygon(point202,point220,point200);
                addPolygon(point202,point222,point220);
                addPolygon(point002,point222,point202);
                addPolygon(point002,point022,point222);
                addPolygon(point020,point222,point022);
                addPolygon(point020,point220,point222);
                addPolygon(point002,point200,point000);
                addPolygon(point002,point202,point200);
            }
        }
    }
    if(points.size() == 0 ) return;
    
//    // CREATE VETEX ON BOUNDARIES
//    if(bbb){
//        auto addPoint = [&] (float x, float y, float z){
//            points.push_back(x*sW);
//            points.push_back(y*sH);
//            points.push_back(z*sD);
//        };
//        addPoint(0,0,0);
//        addPoint(0,0,1);
//        addPoint(0,1,0);
//        addPoint(0,1,1);
//        addPoint(1,0,0);
//        addPoint(1,0,1);
//        addPoint(1,1,0);
//        addPoint(1,1,1);
//        for(size_t i=0;i<12;++i)
//            labels.push_back(0);
//    }
    
    if(type>=2){
        saveMeshWithPointPly(i, points, triangles, labels);
        saveMeshWithPoint3MF(i, points, triangles, labels
                             //                                 , pO, pI, labelColorList, vPoints, vTriangels,  vLabels, labelImageList, sN, sW, sH, sD, bt
                             );
        return;
    }
    if(type<1){
        saveMeshWithPointPly(i, points, triangles, labels);
        return;
    }
    if(type<2 ){
        saveMeshWithPoint3MF(i, points, triangles, labels
                             //                                 , pO, pI, labelColorList, vPoints, vTriangels,  vLabels, labelImageList, sN, sW, sH, sD, bt
                             );
        return;
    }
    delete [] static_cast<T*>(vVolumes[i]);
//    vPoints.clear();
#ifdef hasLabel
//    vLabels.clear();
#endif
//    vTriangels.clear();
}


void saveLog(){
    if(par == NULL) return;
    std::string outputLog = pO.find_last_of('/') == std::string::npos? "./" : pO.substr(0, pO.find_last_of('/')+1);
    outputLog += "logToCube.txt";
    std::fstream file(outputLog, std::ios::out);
    par->outputLog(file);
    file.close();
    printf("Log file saved to %s\n", outputLog.c_str());
}
