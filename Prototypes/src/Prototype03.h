//
//  Prototype03.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "UIProject.h"
#include "UIMapBackground.h"

#include "UIvPlotter.h"

class Prototype03 : public UI2DProject {
public:
    
    string getSystemName(){return "Prototype03";}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    void selfUpdate();
    void selfDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void loadFile(string _file);
    
protected:
    
    //  Input
    //
    ofxUIRadio      *srcFiles;
    ofImage         srcImage;
    
    ofPoint         pointAt(int _x, const ofPolyline &_line);
    ofPolyline      skyline;
    ofPolyline      dstSkyline;
    
    //  Profile extractor
    //
    double          ColourDistance(ofColor e1, ofColor e2);
    vector<ofPoint> offsetPts;
    float           threshold;
    float           thresholdSmoothing;
    float           offSetY,scale,simplify, scaleSpace;
    int             nIterations;
    vector<ofPolyline> lines;
    bool            bFlat;
    
    UIvPlotter      plotter;
    bool            bPrint;
};