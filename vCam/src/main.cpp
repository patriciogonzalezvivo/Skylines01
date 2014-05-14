#include "ofMain.h"

#include "testApp.h"
#include "ofAppNoWindow.h"

#include <algorithm>

char* getCmdOption(char ** begin, char ** end, const std::string & option){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option){
    return std::find(begin, end, option) != end;
}

void printVersion(){
    cout << "vCam 0.1" << endl;
    cout << "Copyright (c) 2014 Patricio Gonzalez Vivo ( http://www.patriciogonzalezvivo.com )" << endl;
    cout << "This is free software with ABSOLUTELY NO WARRANTY." << endl;
}

//========================================================================
int main(int argc, char * argv[]){
    
    ofSetupOpenGL(1024,768, OF_WINDOW);
    
    testApp *app = new testApp();
    app->width = 640;
    app->height = 480;
    app->CannyThreshold1 = 585.14;
    app->CannyThreshold2 = 413.64;
    app->CannyMinPathLength = 10;
    app->CannyMinGapLength = 2;
    app->space = 1.0;
    app->smooth = 0.0;
    app->bHough = false;
    app->HoughtMinLinLenght = 17;
    app->HoughtMaxLineGap = 5;
    app->fileName = "test.svg";
    app->timer = 1;
    
    char * seconds = getCmdOption(argv, argv+argc, "-s");
    if(seconds){
        app->timer = ofToFloat(seconds)*10;
    }
    
    char * width = getCmdOption(argv, argv+argc, "--width");
    if(width){
        app->width = ofToInt(width);
    }
    
    char * height = getCmdOption(argv, argv+argc, "--height");
    if(height){
        app->height = ofToInt(height);
    }
    
    char * CannyThreshold1 = getCmdOption(argv, argv+argc, "--canny1");
    if(CannyThreshold1){
        app->CannyThreshold1 = ofToFloat(CannyThreshold1);
    }
    
    char * CannyThreshold2 = getCmdOption(argv, argv+argc, "--canny2");
    if(CannyThreshold2){
        app->CannyThreshold2 = ofToFloat(CannyThreshold2);
    }
    
    char * CannyMinPathLength = getCmdOption(argv, argv+argc, "--CannyMinPathLength");
    if(CannyMinPathLength){
        app->CannyMinPathLength = ofToFloat(CannyMinPathLength);
    }
    
    char * CannyMinGapLength = getCmdOption(argv, argv+argc, "--CannyMinGapLength");
    if(CannyMinGapLength){
        app->CannyMinGapLength = ofToFloat(CannyMinGapLength);
    }
    
    if(cmdOptionExists(argv, argv+argc, "--hough")){
        app->bHough = true;
    }
    
    char * HoughtMinLinLenght = getCmdOption(argv, argv+argc, "--HoughtMinLinLenght");
    if(HoughtMinLinLenght){
        app->HoughtMinLinLenght = ofToFloat(HoughtMinLinLenght);
        app->bHough = true;
    }
    
    char * HoughtMaxLineGap = getCmdOption(argv, argv+argc, "--HoughtMaxLineGap");
    if(HoughtMaxLineGap){
        app->HoughtMaxLineGap = ofToFloat(HoughtMaxLineGap);
        app->bHough = true;
    }
    
    char * space = getCmdOption(argv, argv+argc, "--space");
    if(space){
        app->space = ofToFloat(space);
    }
    
    char * smooth = getCmdOption(argv, argv+argc, "--smooth");
    if(smooth){
        app->smooth = ofToFloat(smooth);
    }
    
    //  HELP
    //
    if(cmdOptionExists(argv, argv+argc, "-v")){
        printVersion();
        return 0;
    }
    
    if(cmdOptionExists(argv, argv+argc, "-h")){
        printVersion();
        cout << endl;
        cout << "Usage: vCam [options] -o file.svg" << endl;
        cout << endl;
        cout << "Commands and default values:" << endl;
        cout << "-s                     " << app->timer << endl;
        cout << "--width                " << app->width << endl;
        cout << "--height               " << app->height << endl;
        cout << "--canny1               " << app->CannyThreshold1 << endl;
        cout << "--canny2               " << app->CannyThreshold2 << endl;
        cout << "--CannyMinPathLength   " << app->CannyMinPathLength << endl;
        cout << "--CannyMinGapLength    " << app->CannyMinGapLength << endl;
        cout << "--hough                " << app->bHough << endl;
        cout << "--HoughtMinLinLenght   " << app->HoughtMinLinLenght << endl;
        cout << "--HoughtMaxLineGap     " << app->HoughtMaxLineGap << endl;
        cout << "--space                " << app->space << endl;
        cout << "--smooth               " << app->smooth << endl;
        return 0;
    }
    
    //  OUTPUT SVG file
    //
    char * filename = getCmdOption(argv, argv+argc, "-o");
    if (filename){
        app->fileName = filename;
    }
    
    ofRunApp( app );
}