#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setup() {
    ofDisableDataPath();
    ofSetFrameRate(15);
    
#ifdef TARGET_RASPBERRY_PI
    cam.setup(width, height,false);
#else
	cam.initGrabber(width, height);
#endif
}

void testApp::update() {
#ifdef TARGET_RASPBERRY_PI
    frame = cam.grab();
    if(!frame.empty()){
#else
	cam.update();
	if(cam.isFrameNew()){
        cv::Mat vcCam = toCv(cam);
		convertColor(vcCam, frame, CV_RGB2GRAY);
#endif
		Canny(frame, edge, CannyThreshold1*2.0, CannyThreshold2*2.0, 5);
        toOf(edge, pixels);
        
        vector<ofPolyline> tmp;
        
        if(bHough){
            vector<Vec4i> cvlines;
            HoughLinesP(edge, cvlines, 1,(PI/180),1,CannyMinGapLength,CannyMinPathLength);
            for( size_t i = 0; i < cvlines.size(); i++ ){
                ofPolyline line;
                line.addVertex(ofPoint(cvlines[i][0],cvlines[i][1]));
                line.addVertex(ofPoint(cvlines[i][2],cvlines[i][3]));
                tmp.push_back(line);
            }
        } else {
            tmp = getPaths(pixels, HoughtMinLinLenght, HoughtMaxLineGap);
        }
        
        lines.clear();
        for (int i = 0; i < tmp.size(); i++) {
            ofPolyline tmpSpaced;
            ofPolyline tmpSmooth;
            
            if (space>0.0){
                tmpSpaced = tmp[i].getResampledBySpacing(space);
            } else {
                tmpSpaced = tmp[i];
            }
            
            if (smooth>0.0){
                tmpSmooth = tmpSpaced.getSmoothed(smooth);
            } else {
                tmpSmooth = tmpSpaced;
            }
            
            lines.push_back(tmpSmooth);
        }
        
        
        {
            ofxXmlSettings XML;
            XML.addTag("svg");
            XML.addAttribute("svg", "version", "1.2",0);
            XML.addAttribute("svg", "baseProfile", "tiny",0);
            XML.addAttribute("svg", "id", "Layer_1",0);
            XML.addAttribute("svg", "xmlns", "http://www.w3.org/2000/svg",0);
            XML.addAttribute("svg", "xmlns:xlink", "http://www.w3.org/1999/xlink",0);
            XML.addAttribute("svg", "x", "0px",0);
            XML.addAttribute("svg", "y", "0px",0);
            XML.addAttribute("svg", "width", ofToString(width)+"px",0);
            XML.addAttribute("svg", "height", ofToString(height)+"px",0);
            XML.addAttribute("svg", "viewBox", "0 0 "+ofToString(width)+ " " +ofToString(height), 0);
            XML.addAttribute("svg", "xml:space", "preserve",0);
            
            XML.pushTag("svg");
            for (int j = 0; j < lines.size(); j++) {
                XML.addTag("path");
                string path = "";
                for (int i = 0; i < lines[j].size(); i++) {
                    if(i==0){
                        path+="M";
                    } else {
                        path+="L";
                    }
                    path+=ofToString(lines[j][i].x)+","+ofToString(lines[j][i].y);
                }
                XML.addAttribute("path","d",path,j);
                XML.addAttribute("path","fill","none",j);
                XML.addAttribute("path","stroke","black",j);
                XML.addAttribute("path","stroke-width","1",j);
            }
            
            if (XML.save("test.svg")){
                ofExit();
            }
        }
	}
}

typedef std::pair<int, int> intPair;
vector<ofPolyline> testApp::getPaths(ofPixels& img, float minGapLength, int minPathLength) {
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

void testApp::draw() {
    ofBackground(0);
    
    for (int i = 0; i < lines.size(); i++) {
        lines[i].draw();
    }
}
