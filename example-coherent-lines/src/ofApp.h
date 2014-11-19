#pragma once

#include "ofMain.h"
// make sure that you're using kyle's fork of ofxControlPanel
// https://github.com/kylemcdonald/ofxControlPanel
// also, make sure you've included ofControlPanel in your build!
#include "ofxAutoControlPanel.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	// we are actually creating vectors (arrays) of `ofImage` instances,
	// 1 for each phase of image processing, since we are applying it to
	// 3 different ".png" files (find the source images in "bin/data/")
	// 
	// input - image from the original file
	// canny - separate image to perform canny edge detection on
	// output - output image with coherent lines
	vector<ofImage> input, output, canny;
	
	// and a gui for image processing controls
	ofxAutoControlPanel gui;

	// from gui settings, enable/disable main features
	bool doFDoG;
	bool doCanny;
};
