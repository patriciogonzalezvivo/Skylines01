//
//  Prototype01.h
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once

#include "UI2DProject.h"

#include "ofxCv.h"

struct Line{
    ofPoint a,b;
};

class Prototype01 : public UI2DProject {
public:
    
    string getSystemName(){ return "Prototype01";}
    
    void selfSetupGuis();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    void selfUpdate();
    
    void selfDraw();
    void selfPostDraw();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    
    ofVideoGrabber      video;
    ofImage             image,canny;
    
    //  Line Tracer
    //
    void                removeIslands(ofPixels& img);
    vector<ofPolyline>  getPaths(ofPixels& img, float minGapLength = 2, int minPathLength = 0);
    vector<ofPolyline>  contourLines;
    float               fCannyThreshold1, fCannyThreshold2;
    float               minPathLength;
    float               minGapLength;
    float               contourSpace;
    float               contourSmooth;
    bool                bContour;
    bool                bTrace;
    
    vector<Line>        houghLines;
    float               houghtRho,houghtTheta,houghtThreshold,houghtMinLinLenght,houghtMaxLineGap;
};