//
//  pcl23mf.cpp
//  TSDF2Cubes
//
//  Created by Shun-Cheng Wu on 07/11/2018.
//

#include <Tool3mf/Tool3mf.h>

namespace NMR{
    MF::MF():pModel(NULL), p3MFWriter(NULL), pMeshObject(NULL), pBuildItem(NULL), pPropertyHandler(NULL), pDefaultPropertyHandler(NULL), pVertices(NULL), pTriangles(NULL), pColorIndices(NULL), sizeVertices(0), sizeTriangles(0), verbal_(false){
        
    }
    
    MF::~MF(){
        if(pVertices) delete [] pVertices;
        if(pTriangles) delete [] pTriangles;
        if(pColorIndices) delete [] pColorIndices;
    }
    int MF::setVerbal(bool verbal){
        verbal_ = verbal;
        return verbal_;
    }
    
    int MF::checkVersion(){
        // Check 3MF Library Version
        hResult = lib3mf_getinterfaceversion(&nInterfaceVersionMajor, &nInterfaceVersionMinor, &nInterfaceVersionMicro);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not get 3MF Library version: " << std::hex << hResult << std::endl;
            return -1;
        }
        
        if ((nInterfaceVersionMajor != NMR_APIVERSION_INTERFACE_MAJOR)) {
            std::cout << "invalid 3MF Library major version: " << NMR_APIVERSION_INTERFACE_MAJOR << std::endl;
            return -1;
        }
        if (!(nInterfaceVersionMinor >= NMR_APIVERSION_INTERFACE_MINOR)) {
            std::cout << "invalid 3MF Library minor version: " << NMR_APIVERSION_INTERFACE_MINOR << std::endl;
            return -1;
        }
        return 1;
    }
    
    MODELMESHVERTEX MF::fnCreateVertex(float x, float y, float z)
    {
        NMR::MODELMESHVERTEX result;
        result.m_fPosition[0] = x;
        result.m_fPosition[1] = y;
        result.m_fPosition[2] = z;
        return result;
    }
    MODELMESHTRIANGLE MF::fnCreateTriangle(int v0, int v1, int v2)
    {
        MODELMESHTRIANGLE result;
        result.m_nIndices[0] = v0;
        result.m_nIndices[1] = v1;
        result.m_nIndices[2] = v2;
        return result;
    }
    MODELMESHCOLOR_SRGB MF::fnCreateColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    {
        MODELMESHCOLOR_SRGB result;
        result.m_Red = red;
        result.m_Green = green;
        result.m_Blue = blue;
        result.m_Alpha = alpha;
        return result;
    }
    
    LIB3MFRESULT MF::fnLoadModelTexture(PLib3MFModel * pModel, const char * pszPath, const char * pszFile, eModelTexture2DType eType, eModelTextureTileStyle eTileStyleU, eModelTextureTileStyle eTileStyleV, DWORD * pTextureID)
    {
        LIB3MFRESULT hResult = 0;
        PLib3MFModelTexture2D * pTexture2D;
        
        hResult = lib3mf_model_addtexture2dutf8(pModel, pszPath, &pTexture2D);
        if (hResult != 0)
            return hResult;
        
        hResult = lib3mf_texture2d_setcontenttype(pTexture2D, eType);
        if (hResult != 0) {
            lib3mf_release(pTexture2D);
            return hResult;
        }
        
        // Read Texture From File
        hResult = lib3mf_texture2d_readfromfileutf8(pTexture2D, pszFile);
        if (hResult != 0) {
            lib3mf_release(pTexture2D);
            return hResult;
        }
        
        hResult = lib3mf_texture2d_settilestyleuv(pTexture2D, eTileStyleU, eTileStyleV);
        if (hResult != 0) {
            lib3mf_release(pTexture2D);
            return hResult;
        }
        
        // Retrieve Texture ID
        hResult = lib3mf_resource_getresourceid(pTexture2D, pTextureID);
        if (hResult != 0) {
            lib3mf_release(pTexture2D);
            return hResult;
        }
        
        // Release Texture Object from memory (does not delete texture from 3mf!)
        lib3mf_release(pTexture2D);
        return hResult;
    }
    
    MODELMESHTEXTURE2D MF::fnCreateTexture(float u1, float v1, float u2, float v2, float u3, float v3, unsigned int textureid)
    {
        MODELMESHTEXTURE2D result;
        result.m_fU[0] = u1;
        result.m_fV[0] = v1;
        result.m_fU[1] = u2;
        result.m_fV[1] = v2;
        result.m_fU[2] = u3;
        result.m_fV[2] = v3;
        result.m_nTextureID = textureid;
        return result;
    }
    
    bool SimpleCallback(int step, ProgressIdentifier identifier, void * pUserData) {
        
        std::cout << "Step = " << step;
        if (identifier != PROGRESS_QUERYCANCELED) {
            const char * progressMessage;
            NMR::lib3mf_retrieveprogressmessage(identifier, &progressMessage);
            std::cout << " \"" << progressMessage << "\"\n";
        }
        return true;    // Return false if you want Lib3MF to abort
    }

    int MF::init(){
        // Check 3MF Library Version
        hResult = NMR::lib3mf_getinterfaceversion(&nInterfaceVersionMajor, &nInterfaceVersionMinor, &nInterfaceVersionMicro);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not get 3MF Library version: " << std::hex << hResult << std::endl;
            return -1;
        }
        
        if ((nInterfaceVersionMajor != NMR_APIVERSION_INTERFACE_MAJOR)) {
            std::cout << "invalid 3MF Library major version: " << NMR_APIVERSION_INTERFACE_MAJOR << std::endl;
            return -1;
        }
        if (!(nInterfaceVersionMinor >= NMR_APIVERSION_INTERFACE_MINOR)) {
            std::cout << "invalid 3MF Library minor version: " << NMR_APIVERSION_INTERFACE_MINOR << std::endl;
            return -1;
        }
        
        // Create Model Instance
        hResult = NMR::lib3mf_createmodel(&pModel);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create model: " << std::hex << hResult << std::endl;
            return -1;
        }
        
        // Create Mesh Object
        hResult = NMR::lib3mf_model_addmeshobject(pModel, &pMeshObject);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not add mesh object: " << std::hex << hResult << std::endl;
            NMR::lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            NMR::lib3mf_release(pModel);
            return -1;
        }
        
        hResult = NMR::lib3mf_object_setnameutf8(pMeshObject, "Colored Box");
        if (hResult != LIB3MF_OK) {
            std::cout << "could not set object name: " << std::hex << hResult << std::endl;
            NMR::lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            NMR::lib3mf_release(pMeshObject);
            NMR::lib3mf_release(pModel);
            return -1;
        }
        
        return 1;
    }

    int MF::setPoints(const float** points, size_t size){
        if(pVertices) delete [] pVertices;
        pVertices = new MODELMESHVERTEX[size];
        sizeVertices = size;
        for(size_t i=0; i<size; ++i){
            pVertices[i] = fnCreateVertex(points[i][0], points[i][1], points[i][2]);
        }
        return 1;
    }
    int MF::setPoints(float** points, size_t size){
        if(pVertices) delete [] pVertices;
        pVertices = new MODELMESHVERTEX[size];
        sizeVertices = size;
        for(size_t i=0; i<size; ++i){
            pVertices[i] = fnCreateVertex(points[i][0], points[i][1], points[i][2]);
        }
        return 1;
    }
    int MF::setPoints(const float* points, size_t size){
        if(pVertices) delete [] pVertices;
        sizeVertices = size/3;
        pVertices = new MODELMESHVERTEX[sizeVertices];
        for(size_t i=0; i<sizeVertices; ++i){
            pVertices[i] = fnCreateVertex(points[i*3+0], points[i*3+1], points[i*3+2]);
        }
        return 1;
    }
    int MF::setPoints(float* points, size_t size){
        if(pVertices) delete [] pVertices;
        sizeVertices = size/3;
        pVertices = new MODELMESHVERTEX[sizeVertices];
        for(size_t i=0; i<sizeVertices; ++i){
            pVertices[i] = fnCreateVertex(points[i*3+0], points[i*3+1], points[i*3+2]);
        }
        return 1;
    }
    int MF::setTriangles(const int **indices, size_t size){
        if(pTriangles) delete [] pTriangles;
        pTriangles = new MODELMESHTRIANGLE[size];
        sizeTriangles = size;
        for(size_t i=0; i<size; ++i){
            pTriangles[i] = fnCreateTriangle(indices[i][0], indices[i][1], indices[i][2]);
        }
        return 1;
    }
    int MF::setTriangles(const int *indices, size_t size){
        if(pTriangles) delete [] pTriangles;
        sizeTriangles = size/3;
        pTriangles = new MODELMESHTRIANGLE[sizeTriangles];
        for(size_t i=0; i<sizeTriangles; ++i){
            pTriangles[i] = fnCreateTriangle(indices[i*3+0], indices[i*3+1], indices[i*3+2]);
        }
        return 1;
    }
    int MF::setTriangles(int **indices, size_t size){
        if(pTriangles) delete [] pTriangles;
        pTriangles = new MODELMESHTRIANGLE[size];
        sizeTriangles = size;
        for(size_t i=0; i<size; ++i){
            pTriangles[i] = fnCreateTriangle(indices[i][0], indices[i][1], indices[i][2]);
        }
        return 1;
    }
    int MF::setTriangles(int *indices, size_t size){
        if(pTriangles) delete [] pTriangles;
        sizeTriangles = size/3;
        pTriangles = new MODELMESHTRIANGLE[sizeTriangles];
        for(size_t i=0; i<sizeTriangles; ++i){
            pTriangles[i] = fnCreateTriangle(indices[i*3+0], indices[i*3+1], indices[i*3+2]);
        }
        return 1;
    }
    
    int MF::preBuild(){
        if(!sizeVertices || !sizeTriangles){
            printf("[ERROR] need to set vertices and triangles first.\n");
            exit(EXIT_FAILURE);
        }
        if(verbal_)
        if(!sizeColorIndices){
            printf("[WARNING] No Colors indices were given.\n");
        }
        
        if(verbal_)
        if(sizeColorIndices != sizeTriangles){
            printf("[WARNING] The size of color and triangles do not match.\n");
        }
        
        // Build Object
        hResult = lib3mf_meshobject_setgeometry(pMeshObject, pVertices, sizeVertices, pTriangles, sizeTriangles);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not set mesh geometry: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }
        return 1;
    }
    
    int MF::postBuild(std::string name){
        // Add Build Item for Mesh
        hResult = lib3mf_model_addbuilditem(pModel, pMeshObject, NULL, &pBuildItem);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create build item: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }
        
        // Output cube as STL and 3MF
        lib3mf_release(pMeshObject);
        lib3mf_release(pBuildItem);
        
        // Create Model Writer
        hResult = lib3mf_model_querywriter(pModel, "3mf", &p3MFWriter);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create model reader: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pModel);
            return -1;
        }
        
        // Set progress callback
        if(verbal_)
            lib3mf_writer_setprogresscallback(p3MFWriter, SimpleCallback, NULL);
        
        // Export Model into File
        if(verbal_)
            std::cout << "writing texturecube.3mf..." << std::endl;
        std::string outputName = name + ".3mf";
        hResult = lib3mf_writer_writetofileutf8(p3MFWriter, outputName.c_str());
        if (hResult != LIB3MF_OK) {
            std::cout << "could not write file: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(p3MFWriter, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pModel);
            lib3mf_release(p3MFWriter);
            return -1;
        }
        
        lib3mf_release(p3MFWriter);
        lib3mf_release(pModel);
        return 1;
    }
    
    int MF::build(std::string path){
        preBuild();
        if(colorMap_.size())
            colorObject();
        if(textureMap_.size())
            textureObject();
        postBuild(path);
        return 1;
    }
    
    int MF::colorObject(){
        // Create color entries for cube
        hResult = lib3mf_meshobject_createpropertyhandler(pMeshObject, &pPropertyHandler);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create property handler: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }
        
        for(size_t i=0; i < sizeColorIndices; ++i)
            lib3mf_propertyhandler_setsinglecolor(pPropertyHandler, i, &colorMap_[pColorIndices[i]]);
        
        // release property handler
        lib3mf_release(pPropertyHandler);
        
        // make sure to define a default property
        hResult = lib3mf_object_createdefaultpropertyhandler(pMeshObject, &pDefaultPropertyHandler);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create default property handler: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }
        MODELMESHCOLOR_SRGB sDefaultWhite = fnCreateColor (0, 0, 0, 255);
        lib3mf_defaultpropertyhandler_setcolor(pDefaultPropertyHandler, &sDefaultWhite);
        
        // release default property handler
        lib3mf_release(pDefaultPropertyHandler);
        return 1;
    }
    
    int MF::textureObject(){
        // Create texture entries for cube
        hResult = lib3mf_meshobject_createpropertyhandler(pMeshObject, &pPropertyHandler);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create property handler: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }

        // Set Texture Parameters of triangle
        MODELMESHTEXTURE2D sTexture1;
        MODELMESHTEXTURE2D sTexture2;
        
        // if rectangular
        if(*pColorIndices % 2 != 0)
            printf("[Warning] The size of textures provided must be the multiple of 2.\n");
        for(size_t i=0; i < sizeColorIndices; i += 2){
            sTexture1 = fnCreateTexture(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, textureMap_[pColorIndices[i]]);
            sTexture2 = fnCreateTexture(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, textureMap_[pColorIndices[i+1]]);
            lib3mf_propertyhandler_settexture(pPropertyHandler, i, &sTexture1);
            lib3mf_propertyhandler_settexture(pPropertyHandler, i+1, &sTexture2);
        }
        
        // release property handler
        lib3mf_release(pPropertyHandler);
        
        // make sure to define a default property
        hResult = lib3mf_object_createdefaultpropertyhandler(pMeshObject, &pDefaultPropertyHandler);
        if (hResult != LIB3MF_OK) {
            std::cout << "could not create default property handler: " << std::hex << hResult << std::endl;
            lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
            std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
            lib3mf_release(pMeshObject);
            lib3mf_release(pModel);
            return -1;
        }
        lib3mf_defaultpropertyhandler_settexture(pDefaultPropertyHandler, textureMap_[0], 1.0, 1.0);
        
        // release default property handler
        lib3mf_release(pDefaultPropertyHandler);
        return 1;
    }
}// end of namespace NMR
