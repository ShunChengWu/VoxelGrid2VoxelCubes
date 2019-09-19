//////////////
// Convert a float volume which contains label infromation into a cube
//////////////
#include <stdio.h>
#ifdef USE_3MF
#include <Tool3mf/Tool3mf.h>
#endif
#include "../Utils/parser.hpp"
#include "../Utils/thread_pool.hpp"
#include <fstream>
#include <random>

#ifdef WIN32 //For create/delete files
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

typedef unsigned char uchar;

int sN = 1;
int sW = 80; // x
int sH = 48; // y
int sD = 80; // z
float s = 1;
float sb = 0.8;
bool bff = false;
bool bbb = false;
bool bt = false;
bool verbal = false;
int type=0;
int t=0;
std::string
    pI = "",
    pL="colorMap.txt",
    pO="output",
    extension = ".ply",
    textureFolderName = "./textures";

std::vector<void*> vVolumes;
std::vector<float> vPoints;
std::vector<int> vTriangels;
parser *par;

#define hasOccupancy
#include "../Utils/tools.hpp"

void parseCommandLine (int argc, char* argv[]) {
    par->addOption(pkgname(&type), "0: ply, 1: 3mf, >=2: both");
    par->addOption(pkgname(&pI), "The path of input data.",true);
    par->addOption(pkgname(&pO), "The path of output file.");
    par->addOption(pkgname(&sN), "The number of input data.");
    par->addOption(pkgname(&sW), "The width of input data.");
    par->addOption(pkgname(&sH), "The height of input data.");
    par->addOption(pkgname(&sD), "The depth of input data.");
    par->addOption(pkgname(&s), "The scale of output file. Default is a unit volume.");
    par->addOption(pkgname(&sb), "The space between each cube.");
    par->addOption(pkgname(&t), "The number of thread to run. (0: auto detect, >0: the number you want.)");
    par->addSwitch(pkgname(&bff), "Flip the surface direction");
    par->addSwitch(pkgname(&bbb), "In enable, add 8 vetices on the each cornor of the volume.");
    par->addSwitch(pkgname(&bt), "In enable, using texture mapping instead of color.");
    par->addSwitch(pkgname(&verbal), "verbal");
    if(par->showMsg() < 1) {
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]){
    printf("---Reading Parameters---\n");
    par = new parser(argc, argv);
    parseCommandLine(argc, argv);
    printf("------------------------\n\n");

    if(pI == "") {
        printf("[ERROR] No given pthIn.\n");
        exit(EXIT_FAILURE);
    }
    if(t==0){
        t = std::thread::hardware_concurrency();
        if(sN < t) t=sN;
        printf("using auto detection threads mode: %d threads will be used.\n", t);
    }
    
    tools::TaskThreadPool pool(t);

    printf("-Loading Volume...");
    loadVolume<float>();
    printf("Success.\n");

    printf("-Converting & Saving...");
    auto kernel = [](size_t i){
        processRaw<float>(i, vVolumes[i]);
    };
    for(size_t i=0; i < sN; ++i)
        pool.runTaskWithID(std::bind(kernel, i));
    //        kernel(i);
    pool.waitWorkCompleteandShow(sN);
    pool.waitWorkComplete();


    printf("Success.\n");
    
    saveLog();
    
    delete par;
    return EXIT_SUCCESS;
}
