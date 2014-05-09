//
//  Prototype02.h
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#pragma once

#include "UIProject.h"
#include "UIMapBackground.h"

#include "UIvPlotter.h"

struct Star {
    ofPoint pos;
    ofPolyline trail;
};

class Prototype02 : public UI3DProject {
public:
    
    string getSystemName(){return "Prototype02";}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetup();
    void selfUpdate();
    void selfDraw();
    void selfDrawOverlay();
    void selfPostDraw();

    void selfWindowResized(ofResizeEventArgs & args);
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);

protected:
    
    //  Input
    //
    ofxUIRadio      *sourceFiles;
    ofImage         image;
    
    //  Profile extractor
    //
    double          ColourDistance(ofColor e1, ofColor e2);
    vector<ofPoint> offsetPts;
    float           threshold;
    float           thresholdSmoothing;
    float           offSetY,scale,simplify;
    ofPolyline      line;
    bool            bFlat;
    
    ofPolyline      area;
    
    //  Stars
    //
    vector<Star> stars;
    bool        bRotate;
  
    vector<ofPolyline> paths;
    
    //  Plotter
    //
    UIvPlotter      plotter;
    ofRectangle     plotterArea;
    bool            bPrint;
    bool            bViewMovePaths;
};
