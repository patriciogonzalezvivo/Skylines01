//
//  Prototype02.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/12/13.
//
//

#include "Prototype02.h"

void Prototype02::selfSetup(){
    ofEnableAlphaBlending();
    ofSetFullscreen(false);
    int distance = 1000;
    {
        ofBuffer buffer = ofBufferFromFile(getDataPath()+"stars.txt");
        while(!buffer.isLastLine()) {
            string temp = buffer.getNextLine();
            
            if(temp.length() != 0) {
                vector<string> values = ofSplitString(temp, ",");
                
                float ra = ofToFloat(values[0])*(360.0/24.0)-180;
                float dec = ofToFloat(values[1]);
                float dist = 1000;//ofToFloat(values[2])*100;
                
                if(abs(dist) > 0){
                    ofQuaternion decRot, raRot;
                    decRot.makeRotate( dec, 1, 0, 0);
                    raRot.makeRotate( ra, 0, 1, 0);
                    
                    Star nS;
                    nS.pos = decRot * raRot * ofPoint(0,0,dist);
                    
                    stars.push_back(nS);
                }
            }
        }
    }

    bRotate = true;
}

void Prototype02::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(plotter);
}

void Prototype02::selfSetupSystemGui(){
    sysGui->addLabel("Skyline Scraper");
    sysGui->addSlider("Threshold", 0.0, 255.0, &threshold);
    sysGui->addSlider("Threshold_Smoothing", 0.0, 0.2, &thresholdSmoothing);
    
    sysGui->addLabel("Line");
    sysGui->addSlider("Y_Offset",0.0,1.0,&offSetY);
    sysGui->addSlider("Scale", 0.0, 1.0, &scale);
    sysGui->addSlider("Simplify", 0.0, 3.0, &simplify);
    sysGui->addSpacer();
    
    sysGui->addButton("PRINT", &bPrint);
}

ofPoint center;
bool sortByDistance (const Star &a, const Star &b){
    float distA = 0;
    if(a.trail.size()>0){
        distA = a.trail[0].distance(center);
    }
    
    float distB = 0;
    if(b.trail.size()>0){
        distB = b.trail[0].distance(center);
    }
    
    return distA < distB;
}

bool sortByLenght (const Star &a, const Star &b){
    return a.trail.size() < b.trail.size();
}

void Prototype02::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if( name == "PRINT"){
        if(bPrint){
            ofRectangle printArea = plotter.getPrintingArea();
            center = camera->getCameraPtn()->worldToScreen(ofPoint(0,1000,0));
            
            sort(stars.begin(), stars.end(), sortByDistance);
            
            ofPolyline tmp;
            for (int i = 0; i < line.size(); i++) {
                tmp.addVertex(line[i]*ofPoint(.82,1.0)+ofPoint(printArea.width*0.18*0.5,0));
            }
            tmp = tmp.getResampledBySpacing(1);
            plotter.print(tmp);
            
            paths.clear();
            for (int i = 0; i < stars.size(); i++) {
                if(stars[i].trail.size()>0){
                    
                    ofPolyline newP;
                    for (int j = 0; j < stars[i].trail.size(); j++) {
                        ofPoint pos = stars[i].trail.getVertices()[j]-plotterArea.getTopLeft();
                        if(printArea.inside(pos)){
                            newP.addVertex(pos*ofPoint(.82,1.0)+ofPoint(printArea.width*0.18*0.5,0));
                        }
                    }
                    
                    newP = newP.getResampledBySpacing(1.0);
                    
                    paths.push_back(newP);
                }
            }
            plotter.print(paths);
        }
    } else {
        ofRectangle printArea = plotter.getPrintingArea();
        
        area.clear();
        area.addVertex(plotterArea.getTopLeft());
        for (int i = 0; i < line.size(); i++) {
            ofPoint pos = line[i]+plotterArea.getTopLeft();
            if(printArea.inside(pos)){
                area.addVertex(pos);
            }
            
        }
        area.addVertex(plotterArea.getTopRight());
        area.close();
        bFlat = true;
    }
}

void Prototype02::selfSetupRenderGui(){
    vector<string> filesList;
    string path = "landscapes";
    ofDirectory filesDir(path);
    if(filesDir.exists()){
		filesDir.listDir();
        int total = filesDir.getFiles().size();
        for (int i = 0; i < total; i++) {
            filesList.push_back( filesDir.getName(i) );
        }
        
        if(sourceFiles == NULL){
            sourceFiles = rdrGui->addRadio("Images", filesList);
        }
    }
}

void Prototype02::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        ofxUIToggle *t = (ofxUIToggle*)e.widget;
        if (t != NULL){
            if (t->getValue()){
                image.loadImage("landscapes/"+name);
            }
        }
    }
    
    bFlat = true;
}

//  Take from http://www.compuphase.com/cmetric.htm
//
double Prototype02::ColourDistance(ofColor e1, ofColor e2){
    long rmean = ( (long)e1.r + (long)e2.r ) / 2;
    long r = (long)e1.r - (long)e2.r;
    long g = (long)e1.g - (long)e2.g;
    long b = (long)e1.b - (long)e2.b;
    
    return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}

void Prototype02::selfUpdate(){
    plotter.update();
    
    plotterArea = plotter.getPrintingArea();
    plotterArea.x = ofGetWidth()*0.5-plotterArea.width*0.5;
    plotterArea.y = ofGetHeight()*0.5-plotterArea.height*0.5;
    
    if(bFlat){
        ofPixels srcPixels;
        srcPixels = image.getPixelsRef();
        
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
        
        line.clear();
        for (int i = 0; i < offsetPts.size(); i++) {
            ofPoint pos = offsetPts[i]-offsetPts[0];
            pos *= scale;
            line.addVertex(pos+ofPoint(0,offSetY*plotter.getPrintingArea().getHeight()));
        }
        if(simplify>0){
            line.simplify(simplify);
        }
        bFlat = false;
    }

    if(bRotate){
        ofQuaternion rotation;
        rotation.makeRotate(1.0/ofGetFrameRate(), 0, 1, 0);
        
        for (int i = 0; i < stars.size(); i++) {
            stars[i].pos = rotation * stars[i].pos;
        
            ofPoint screenPos = camera->getCameraPtn()->worldToScreen(stars[i].pos);
            if(area.inside(screenPos)){
                stars[i].trail.addVertex(screenPos);
            }
        }
    }
}

void Prototype02::selfDraw(){
    materials["MATERIAL 1"]->begin();

    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(255);
    glBegin(GL_POINTS);
    for (int i = 0; i < stars.size(); i++) {
        glVertex3f(stars[i].pos.x,stars[i].pos.y,stars[i].pos.z);
    }
    glEnd();
    
    if(bDebug){
        ofSetColor(255,0,0,100);
        ofLine(ofPoint(0,-1000,0), ofPoint(0,1000,0));
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    materials["MATERIAL 1"]->end();

}

void Prototype02::selfDrawOverlay(){
    for (int i = 0; i < stars.size(); i++) {
        stars[i].trail.draw();
    }
    
    ofPushStyle();
    ofSetColor(255, 0, 0);
    area.draw();
    ofPopStyle();
}

void Prototype02::selfPostDraw(){
    renderTarget.draw(0,0);
    
    ofPushMatrix();
    ofTranslate(plotterArea.x,plotterArea.y);
    
    plotter.draw();
    
    ofPushStyle();
    
    ofSetColor(0,255,0);
    line.draw();
    {
        ofSetColor(255,0,0);
        for(int i = 0; i < paths.size(); i++) {
            ofSetColor(255,255,0);
            paths[i].draw();
            if(i + 1 < paths.size() && bViewMovePaths) {
                ofVec2f endPoint = paths[i].getVertices()[paths[i].size() - 1];
                ofVec2f startPoint = paths[i + 1].getVertices()[0];
                ofSetColor(255,255,0, 128);
                ofLine(endPoint, startPoint);
            }
        }
    }
    ofPopStyle();
    
    ofPopMatrix();
    
    if(bDebug){
        ofPushMatrix();
        ofTranslate(ofGetWidth()-image.getWidth()*0.5, 0);
        ofScale(0.5, 0.5);
        
        image.draw(0, 0);
        
        ofSetColor(255,0,0);
        ofPolyline line;
        line.addVertices(offsetPts);
        line.draw();
        ofPopMatrix();
    }
}

void Prototype02::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        bRotate = !bRotate;
    } else {
        for (int i = 0; i < stars.size(); i++) {
            stars[i].trail.clear();
        }
        area.clear();
        area.addVertex(plotterArea.getTopLeft());
        for (int i = 0; i < line.size(); i++) {
            area.addVertex(line[i]+plotterArea.getTopLeft());
        }
        area.addVertex(plotterArea.getTopRight());
        area.close();
    }
}

void Prototype02::selfKeyReleased(ofKeyEventArgs & args){
}

void Prototype02::selfMouseMoved(ofMouseEventArgs& data){
}

void Prototype02::selfMousePressed(ofMouseEventArgs& data){

}

void Prototype02::selfMouseDragged(ofMouseEventArgs& data){

}

void Prototype02::selfMouseReleased(ofMouseEventArgs& data){
}

void Prototype02::selfWindowResized(ofResizeEventArgs & args){
}