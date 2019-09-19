//
//  tool3mf.h
//  TSDF2Cubes
//
//  Created by Shun-Cheng Wu on 07/11/2018.
//

#ifndef tool3mf_h
#define tool3mf_h

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
//#include <NMR_DLLInterfaces.h>
#include <Model/COM/NMR_DLLInterfaces.h>
namespace NMR {
    class MF{
    public:
        MF();
        ~MF();
        int init();
        int checkVersion();
        int setPoints(const float** points, size_t size);
        int setPoints(const float* points, size_t size);
        int setPoints(float** points, size_t size);
        int setPoints(float* points, size_t size);
        int setTriangles(const int** indices, size_t size);
        int setTriangles(const int* indices, size_t size);
        int setTriangles(int** indices, size_t size);
        int setTriangles(int* indices, size_t size);
        int setVerbal(bool verbal);
        //    int setColors(unsigned char** colors, size_t size);
        //    int setColors(unsigned char* colors, size_t size);
        template <typename T>
        int setColors(T* colors, const std::map<T, unsigned char[4]>& colorMap, size_t size);
        template <typename T>
        int setColors(const T* colors, const std::map<T, unsigned char[4]>& colorMap, size_t size);
        /**
         <#Description#>
         
         @param textures The corresponding index for texturing
         @param paths The path to where the texutre files locate. Should have file name x.ppm
         @param size The total size of textures
         @return 1: success
         */
        template <typename T>
        int setTextures(T* textures, const std::map<T, std::string>& textureMap, size_t size);
        template <typename T>
        int setTextures(const T* textures, const std::map<T, std::string>& textureMap, size_t size);
        int build(std::string path = "output");
    private:
        HRESULT hResult;
        DWORD nInterfaceVersionMajor, nInterfaceVersionMinor, nInterfaceVersionMicro;
        DWORD nErrorMessage;
        LPCSTR pszErrorMessage;
        PLib3MFModel * pModel;
        PLib3MFModelWriter * p3MFWriter;
        PLib3MFModelMeshObject * pMeshObject;
        PLib3MFModelBuildItem * pBuildItem;
        PLib3MFPropertyHandler * pPropertyHandler;
        PLib3MFDefaultPropertyHandler * pDefaultPropertyHandler;
        
        MODELMESHVERTEX* pVertices;
        MODELMESHTRIANGLE* pTriangles;
        size_t *pColorIndices;
        std::map<size_t, DWORD> textureMap_;
        std::map<size_t, MODELMESHCOLOR_SRGB> colorMap_;
        size_t sizeVertices, sizeTriangles, sizeColorIndices;
        /// need to call setColors first
        int colorObject();
        /// need to call setTextures first
        int textureObject();
        int preBuild();
        int postBuild(std::string name);
        bool verbal_;
        
        MODELMESHVERTEX fnCreateVertex(float x, float y, float z);
        MODELMESHTRIANGLE fnCreateTriangle(int v0, int v1, int v2);
        MODELMESHCOLOR_SRGB fnCreateColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
        LIB3MFRESULT fnLoadModelTexture(PLib3MFModel * pModel, const char * pszPath, const char * pszFile, eModelTexture2DType eType, eModelTextureTileStyle eTileStyleU, eModelTextureTileStyle eTileStyleV, DWORD * pTextureID);
        MODELMESHTEXTURE2D fnCreateTexture(float u1, float v1, float u2, float v2, float u3, float v3, unsigned int textureid);
    };
} //end of namespace NMR
#include <Tool3mf/Tool3mf.hpp>
#endif /* tool3mf_h */
