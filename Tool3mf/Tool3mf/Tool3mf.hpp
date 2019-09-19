//
//  tool3mf.hpp
//  TSDF2Cubes
//
//  Created by Shun-Cheng Wu on 07/11/2018.
//
#ifndef tool3mf_hpp
#define tool3mf_hpp

namespace NMR {
    template <typename T>
    int MF::setColors(T *colorIndices, const std::map<T, unsigned char [4]>& colorMap, size_t size){
        // Find Maximum label
        size_t max=0;
        for(size_t i=0; i < size; ++i)
            if(colorIndices[i] > max) max = colorIndices[i];
        if(colorMap.size() < max)
            printf("[WARNING] The maximum label number (%zu) larger than the color map (%zu).\n", max, colorMap.size());
        
        // Copy color Map & build Color
        for (const auto& pair : colorMap)
            colorMap_[static_cast<size_t>(pair.first)] = fnCreateColor (pair.second[0], pair.second[1], pair.second[2], pair.second[3]);
        
        // copy Color indices
        if(pColorIndices) delete [] pColorIndices;
        sizeColorIndices = size;
        pColorIndices = new size_t[sizeColorIndices];
        for(size_t i=0; i < sizeColorIndices; ++i)
            pColorIndices[i] = static_cast<size_t>(colorIndices[i]);
        return 1;
    }
    
    template <typename T>
    int MF::setColors(const T *colorIndices, const std::map<T, unsigned char [4]>& colorMap, size_t size){
        // Find Maximum label
        size_t max=0;
        for(size_t i=0; i < size; ++i)
            if(colorIndices[i] > max) max = colorIndices[i];
        if(colorMap.size() < max)
            printf("[WARNING] The maximum label number (%zu) larger than the color map (%zu).\n", max, colorMap.size());
        
        // Copy color Map & build Color
        for (const auto& pair : colorMap)
            colorMap_[static_cast<size_t>(pair.first)] = fnCreateColor (pair.second[0], pair.second[1], pair.second[2], pair.second[3]);
        
        // copy Color indices
        if(pColorIndices) delete [] pColorIndices;
        sizeColorIndices = size;
        pColorIndices = new size_t[sizeColorIndices];
        for(size_t i=0; i < sizeColorIndices; ++i)
            pColorIndices[i] = static_cast<size_t>(colorIndices[i]);
        return 1;
    }
    
    template <typename T>
    int MF::setTextures(T* colorIndices, const std::map<T, std::string>& textureMap, size_t size){
        // Find Maximum label
        size_t max=0;
        for(size_t i=0; i < size; ++i)
            if(colorIndices[i] > max) max = colorIndices[i];
        if(textureMap.size() < max)
            printf("[WARNING] The maximum texture number (%zu) larger than the texture map (%zu).\n", max, textureMap.size());
        
        // Load textures
        for(const auto& pair: textureMap){
            hResult = fnLoadModelTexture(pModel, pair.second.c_str(), pair.second.c_str(), MODELTEXTURETYPE_PNG, MODELTEXTURETILESTYLE_WRAP, MODELTEXTURETILESTYLE_WRAP, &textureMap_[static_cast<size_t>(pair.first)]);
            if(hResult != 0)
                printf("[Warning] Cannot load texture from path %s.\n", pair.second.c_str());
        }
        
        // copy Color indices
        if(pColorIndices) delete [] pColorIndices;
        sizeColorIndices = size;
        pColorIndices = new size_t[sizeColorIndices];
        for(size_t i=0; i < sizeColorIndices; ++i)
            pColorIndices[i] = static_cast<size_t>(colorIndices[i]);
        
        if (hResult != LIB3MF_OK) {
            std::cout << "could not load model texture: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            lib3mf_release(pPropertyHandler);
            return -1;
        }
        return 1;
    }
    
    template <typename T>
    int MF::setTextures(const T* colorIndices, const std::map<T, std::string>& textureMap, size_t size){
        // Find Maximum label
        size_t max=0;
        for(size_t i=0; i < size; ++i)
            if(colorIndices[i] > max) max = colorIndices[i];
        if(textureMap.size() < max)
            printf("[WARNING] The maximum texture number (%zu) larger than the texture map (%zu).\n", max, textureMap.size());
        
        // Load textures
        for(const auto& pair: textureMap){
            hResult = fnLoadModelTexture(pModel, pair.second.c_str(), pair.second.c_str(), MODELTEXTURETYPE_PNG, MODELTEXTURETILESTYLE_WRAP, MODELTEXTURETILESTYLE_WRAP, &textureMap_[static_cast<size_t>(pair.first)]);
            if(hResult != 0)
                printf("[Warning] Cannot load texture from path %s.\n", pair.second.c_str());
        }
        
        // copy Color indices
        if(pColorIndices) delete [] pColorIndices;
        sizeColorIndices = size;
        pColorIndices = new size_t[sizeColorIndices];
        for(size_t i=0; i < sizeColorIndices; ++i)
            pColorIndices[i] = static_cast<size_t>(colorIndices[i]);
        
        if (hResult != LIB3MF_OK) {
            std::cout << "could not load model texture: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            lib3mf_release(pPropertyHandler);
            return -1;
        }
        return 1;
    }
} //end of namespace NMR
#endif /* tool3mf_hpp */
