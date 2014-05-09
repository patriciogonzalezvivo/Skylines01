//
//  Prototype03.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype03.h"

void Prototype03::selfSetup(){
    ofSetVerticalSync(true);
    
    bPrint = false;
    bFlat = true;
}

void Prototype03::loadFile(string _file){
    srcImage.loadImage(_file);
}

void Prototype03::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(plotter);
}

void Prototype03::selfSetupSystemGui(){
    sysGui->addLabel("Scrap");
    sysGui->addSlider("Threshold", 0.0, 255.0, &threshold);
    sysGui->addSlider("Threshold_Smoothing", 0.0, 0.2, &thresholdSmoothing);
    
    sysGui->addLabel("Adjust");
    sysGui->addSlider("Y_Offset",0.0,1.0,&offSetY);
    sysGui->addSlider("Scale", 0.0, 1.0, &scale);
    sysGui->addSlider("Simplify", 0.0, 3.0, &simplify);
    
    sysGui->addLabel("Iterate");
    sysGui->addIntSlider("number", 0, 50, &nIterations);
    sysGui->addSlider("Scale_space", 0, 1, &scaleSpace);
    
    sysGui->addButton("Print", &bPrint);
}

void Prototype03::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if( name == "Print"){
        if(bPrint){
            ofRectangle plotterArea = plotter.getPrintingArea();
            vector<ofPolyline> paths;
            for (int i = 0; i < lines.size(); i++) {
                ofPolyline newP;
                for (int j = 0; j < lines[i].size(); j++) {
                    ofPoint pos = lines[i][j]*ofPoint(.82,1.0)+ofPoint(plotterArea.width*0.18*0.5);
                    if(plotterArea.inside(pos)){
                        newP.addVertex(pos);
                    }
                }
                newP = newP.getResampledBySpacing(1.0);
                paths.push_back(newP);
            }
            plotter.print(paths);
            
        }
    } else {
        bFlat = true;
    }
}

void Prototype03::selfSetupRenderGui(){
    vector<string> filesList;
    string path = "landscapes";
    ofDirectory filesDir(path);
    if(filesDir.exists()){
		filesDir.listDir();
        int total = filesDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            filesList.push_back( filesDir.getName(i) );
        }
        
        if(srcFiles == NULL){
            srcFiles = rdrGui->addRadio("Src_Images", filesList);
        }
    }
}

void Prototype03::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                loadFile("landscapes/"+name);
            }
        }
    }
    
    bFlat = true;
}

//  Take from http://www.compuphase.com/cmetric.htm
//
double Prototype03::ColourDistance(ofColor e1, ofColor e2){
    long rmean = ( (long)e1.r + (long)e2.r ) / 2;
    long r = (long)e1.r - (long)e2.r;
    long g = (long)e1.g - (long)e2.g;
    long b = (long)e1.b - (long)e2.b;
    
    return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}

bool sortColor( const ofPoint & a, const ofPoint & b){
    return a.y > b.y;
}

ofPoint Prototype03::pointAt(int _x, const ofPolyline &_line){
    for(int i = 0;i<_line.size();i++){
        if((int)_line[i].x==_x){
            return _line[i];
        }
    }
    
    return ofPoint(_x,0);
}

void Prototype03::selfUpdate(){
    
    if(bFlat){
        ofPixels srcPixels;
        srcPixels = srcImage.getPixelsRef();
        
        //  Extract the skyline
        //
        offsetPts.clear();
        float width = srcPixels.getWidth();
        float height = srcPixels.getHeight();
        for (int x = 0; x < width; x++){
            
            ofColor a = srcPixels.getColor(x, 0);
            
            offsetPts.push_back(ofPoint(x, 0));
            for(int y = 0; y < height; y++){
                
                ofFloatColor b = srcPixels.getColor(x, y);
                
                if (ColourDistance(a,b) > threshold){
                    offsetPts[x].y = y;
                    break;
                } else {
                    a.lerp(b,thresholdSmoothing);
                }
            }
        }
        
        //  Adjust the skyline
        //
        skyline.clear();
        for (int i = 0; i < offsetPts.size(); i++) {
            ofPoint pos = offsetPts[i]-offsetPts[0];
            pos *= scale;
            skyline.addVertex(pos);
        }
        if(simplify>0) skyline.simplify(simplify);
        skyline = skyline.getResampledBySpacing(1.0);
        
        //  Populate lines iterations
        //
        lines.clear();
        lines.push_back(skyline);
        float factor = 1.0/(float)nIterations;
        for(int i = 0; i < nIterations; i++){
            ofPolyline iterLine;
            for(int j = 0; j < skyline.size(); j++){
                float pct = 1.0-(float)i/(float)nIterations;
                ofPoint pos = skyline[j];
                ofPoint destPos = pointAt(pos.x,dstSkyline);
                iterLine.addVertex(pos*pct+destPos*(1.0-pct));
            }
            lines.push_back(iterLine);
        }
        
        ofRectangle area = plotter.getPrintingArea();
        
        //  Place the lines
        //
        int lastY=0;
        for(int i = 0; i < lines.size(); i++){
            for(int j = 0; j < lines[i].size(); j++){
                lines[i][j].y += offSetY*area.height+lastY;
            }
            lastY += lines[i].getBoundingBox().height*scaleSpace;
        }
        
        bFlat = false;
    }
    
    plotter.update();
}

void Prototype03::selfDraw(){
    ofRectangle area = plotter.getPrintingArea();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    ofTranslate(area.width*-0.5,area.height*-0.5);
    
    ofSetColor(background->getUIBrightness()*255);

    ofPushStyle();
    for(int i = 0; i < lines.size(); i++) {
        ofSetColor(255,255,0);
        lines[i].draw();
        if(bDebug){
            if(i + 1 < lines.size()) {
                ofVec2f endPoint = lines[i].getVertices()[lines[i].size() - 1];
                ofVec2f startPoint = lines[i + 1].getVertices()[0];
                ofSetColor(255,0,255, 128);
                ofLine(endPoint, startPoint);
            }
        }
    }
    ofPopStyle();
    
    plotter.draw();
    ofPopMatrix();
    
    if(bDebug){
        ofPushMatrix();
        ofTranslate(ofGetWidth()-srcImage.getWidth()*0.5, 0);
        ofScale(0.5, 0.5);
        
        srcImage.draw(0, 0);
        
        ofSetColor(255,0,0);
        ofPolyline line;
        line.addVertices(offsetPts);
        line.draw();
        
        ofSetColor(255,0,255);
        ofPolyline mediuanline;
        mediuanline.addVertices(offsetPts);
        mediuanline.draw();
        ofPopMatrix();
    }
}

void Prototype03::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        dstSkyline = skyline;
    } else if(args.key == 'c'){
        dstSkyline.clear();
    }
}

void Prototype03::selfKeyReleased(ofKeyEventArgs & args){

}

void Prototype03::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype03::selfMousePressed(ofMouseEventArgs& data){

}

void Prototype03::selfMouseDragged(ofMouseEventArgs& data){
}

void Prototype03::selfMouseReleased(ofMouseEventArgs& data){

}