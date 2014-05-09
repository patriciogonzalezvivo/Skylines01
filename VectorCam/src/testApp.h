#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include "ofxCv.h"

#ifdef TARGET_RASPBERRY_PI
#include "ofxCvPiCam.h"
#endif

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
#ifdef TARGET_RASPBERRY_PI
    ofxCvPiCam cam;
#else
	ofVideoGrabber cam;
#endif
    cv::Mat frame, edge;
    ofPixels pixels;
    
    vector<ofPolyline>  getPaths(ofPixels& img, float minGapLength = 2, int minPathLength = 1);
    vector<ofPolyline>  lines;
    
    string  fileName;
    
    int     width,height;
    float   CannyThreshold1, CannyThreshold2;
    float   CannyMinPathLength, CannyMinGapLength;
    float   HoughtMinLinLenght, HoughtMaxLineGap;
    float   space, smooth;
    
    bool    bHough;
    
    int     counter, timer;
};
