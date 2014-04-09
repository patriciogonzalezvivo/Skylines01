//
//  Prototype01.cpp
//  PrintMaking
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "Prototype01.h"

using namespace ofxCv;
using namespace cv;

void Prototype01::selfSetup(){
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    video.initGrabber(640, 480);
    fbo.allocate(640,480);
    pixels.allocate(640, 480, 3);
    
    edgeShader.loadFrag(getDataPath()+"/shaders/edge.frag");
}

void Prototype01::selfSetupGuis(){
    guiAdd(edgeShader);
}

void Prototype01::selfGuiEvent(ofxUIEventArgs &e){
    
}

void Prototype01::selfSetupSystemGui(){
    sysGui->addSlider("Canny_Threshold1", 0.0, 1.0, &fCannyThreshold1);
    sysGui->addSlider("Canny_Threshold2",0.0,1.0,&fCannyThreshold2);
    sysGui->addSlider("MinGapLenght", 2.0, 12.0, &minGapLength);
    sysGui->addSlider("MinPathLenght", 0.0, 50.0, &minPathLength);
    sysGui->addSlider("pathSpace", 0.0, 10.0, &contourSpace);
    sysGui->addSlider("pathSmooth", 0.0, 10.0, &contourSmooth);
    sysGui->addSpacer();
    sysGui->addButton("TRACE", &bTrace);
}

//---------------------------------------------------

void Prototype01::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name == "Canny_Threshold1" || name == "Canny_Threshold2" ||
       name == "MinGapLenght" || name == "MinPathLenght" ||
       name == "pathSpace" || name == "pathSmooth"){
        bTrace = true;
    }
}

//--------------------------------------------------- Tracer
void Prototype01::removeIslands(ofPixels& img) {
	int w = img.getWidth(), h = img.getHeight();
	int ia1=-w-1,ia2=-w-0,ia3=-w+1,ib1=-0-1,ib3=-0+1,ic1=+w-1,ic2=+w-0,ic3=+w+1;
	unsigned char* p = img.getPixels();
	for(int y = 1; y + 1 < h; y++) {
		for(int x = 1; x + 1 < w; x++) {
			int i = y * w + x;
			if(p[i]) {
				if(!p[i+ia1]&&!p[i+ia2]&&!p[i+ia3]&&!p[i+ib1]&&!p[i+ib3]&&!p[i+ic1]&&!p[i+ic2]&&!p[i+ic3]) {
					p[i] = 0;
				}
			}
		}
	}
}

typedef std::pair<int, int> intPair;
vector<ofPolyline> Prototype01::getPaths(ofPixels& img, float minGapLength, int minPathLength) {
	float minGapSquared = minGapLength * minGapLength;
	
	list<intPair> remaining;
	int w = img.getWidth(), h = img.getHeight();
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			if(img.getColor(x, y).getBrightness() > 128) {
				remaining.push_back(intPair(x, y));
			}
		}
	}
	
	vector<ofPolyline> paths;
	if(!remaining.empty()) {
		int x = remaining.back().first, y = remaining.back().second;
		while(!remaining.empty()) {
			int nearDistance = 0;
			list<intPair>::iterator nearIt, it;
			for(it = remaining.begin(); it != remaining.end(); it++) {
				intPair& cur = *it;
				int xd = x - cur.first, yd = y - cur.second;
				int distance = xd * xd + yd * yd;
				if(it == remaining.begin() || distance < nearDistance) {
					nearIt = it, nearDistance = distance;
					// break for immediate neighbors
					if(nearDistance < 4) {
						break;
					}
				}
			}
			intPair& next = *nearIt;
			x = next.first, y = next.second;
			if(paths.empty()) {
				paths.push_back(ofPolyline());
			} else if(nearDistance >= minGapSquared) {
				if(paths.back().size() < minPathLength) {
					paths.back().clear();
				} else {
					paths.push_back(ofPolyline());
				}
			}
			paths.back().addVertex(ofVec2f(x, y));
			remaining.erase(nearIt);
		}
	}
	
	return paths;
}

void Prototype01::selfUpdate(){
    video.update();
    
    fbo.begin();
    ofClear(0);
    edgeShader.begin();
    video.draw(0, 0);
    edgeShader.end();
    fbo.end();
    
    //  CONTOUR
    //
    if(bTrace){
        getRenderTarget().getTextureReference().readToPixels(pixels);
        ofPixels gray;
        convertColor(pixels, gray, CV_RGB2GRAY);
        
		Canny(gray, pixels, fCannyThreshold1*255., fCannyThreshold2*255., 3);
        
        contourLines.clear();
        vector<ofPolyline> tmp = getPaths(pixels, minGapLength, minPathLength);
        for (int i = 0; i < tmp.size(); i++) {
            ofPolyline contornoSpaced;
            ofPolyline contornoSmooth;
            
            if (contourSpace>0.0){
                contornoSpaced = tmp[i].getResampledBySpacing(contourSpace);
            } else {
                contornoSpaced = tmp[i];
            }
            
            if (contourSmooth>0.0){
                contornoSmooth = contornoSpaced.getSmoothed(contourSmooth);
            } else {
                contornoSmooth = contornoSpaced;
            }
            
            contourLines.push_back(contornoSmooth);
        }
        
        bTrace = false;
    }
}

void Prototype01::selfDraw(){
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5-fbo.getWidth()*0.5, ofGetHeight()*0.5-fbo.getHeight()*0.5);
    
    for (int i = 0; i < contourLines.size(); i++) {
        contourLines[i].draw();
    }
    
    fbo.draw(0,0);
    ofPopMatrix();
}

void Prototype01::selfPostDraw(){
    UI2DProject::selfPostDraw();
}

void Prototype01::selfEnd(){
}

void Prototype01::selfExit(){
}

void Prototype01::selfKeyPressed(ofKeyEventArgs & args){

}

void Prototype01::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Prototype01::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Prototype01::selfMousePressed(ofMouseEventArgs& data){

}

void Prototype01::selfMouseDragged(ofMouseEventArgs& data){

}

void Prototype01::selfMouseReleased(ofMouseEventArgs& data){

}