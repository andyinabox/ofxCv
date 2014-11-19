#include "ofApp.h"

// editorial: not a big fan of the mixed namespacing here,
// it makes it difficult to differentate between methods
// that are specific to openFrameworks, openCv, and ofxCv
// 
// using namespace ofxCv
// using namespace cv;

void ofApp::setup() {
	ofSetVerticalSync(true);
	
	// create an ofDirectory object so we can traverse the file system
	// http://www.openframeworks.cc/documentation/utils/ofDirectory.html
	ofDirectory dir;
	// we want to go through ".png" files when we call `listDir`
	dir.allowExt("png");
	// open the default path, which is "bin/data/"
	dir.open(".");
	// build a list of ".png" files in the "bin/data/" directory
	dir.listDir();

	// iterate through the images
	for(int i = 0; i < dir.size(); i++) {

		// for each image file, make an ofImage
		ofImage cur;

		// load the image from the filesystem
		cur.loadImage(dir.getName(i));

		// set the image type, we don't need/want color here
		cur.setImageType(OF_IMAGE_GRAYSCALE);

		// add the current image to the
		// input/output/canny `ofImage` arrays
		input.push_back(cur);
		output.push_back(cur);
		canny.push_back(cur);
	}
	
	// add all the gui controls
	gui.setup();
	gui.addPanel("Settings");
	
	// do image smoothing using a "Difference of Gaussians" algorithm?
	// http://en.wikipedia.org/wiki/Difference_of_Gaussians 
	gui.addToggle("doFDoG", true);
	//
	// below are arguments used in `ofxCv::CLD`
	// 
	// "coherent line drawing: good values for halfw are between 1 and 8"
	gui.addSlider("halfw", 4, 1, 8, true);
	//
	// "smoothPasses 1, and 4"
	gui.addSlider("smoothPasses", 2, 1, 4, true);
	//
	// "sigma1 between .01 and 2"
	gui.addSlider("sigma1", 0.68, 0.01, 2.0, false);
	//
	// "sigma2 between .01 and 10"
	gui.addSlider("sigma2", 6.0, 0.01, 10.0, false);
	//
	// "tau between .8 and 1.0"
	gui.addSlider("tau", 0.974, 0.8, 1.0, false);
	//
	// black point?
	gui.addSlider("black", -8, -255, 255, true);

	// do we want to apply thresholding?
	// http://en.wikipedia.org/wiki/Thresholding_(image_processing)
	gui.addToggle("doThresh", true);
	//
	// where to set the threshold? 
	gui.addSlider("thresh", 150, 0, 255, false);

	// do we want tothin the resulting contours
	// i.e. contract the lines to a single pixel?
	// http://homepages.inf.ed.ac.uk/rbf/HIPR2/thin.htm
	gui.addToggle("doThin", true);

	// do we want to perform Canny edge detection?
	// http://en.wikipedia.org/wiki/Canny_edge_detector
	gui.addToggle("doCanny", true);
	// first and second thresholds for edge detection
	gui.addSlider("cannyParam1", 400, 0, 1024, true);
	gui.addSlider("cannyParam2", 600, 0, 1024, true);

	gui.loadSettings("settings.xml");
}

void ofApp::update(){

	// load gui settings into app vars
	doFDoG = gui.getValueB("doFDoG");
	doCanny = gui.getValueB("doCanny");

	// load the rest into this context
	bool doThin = gui.getValueB("doThin");
	bool doThresh = gui.getValueB("doThresh");
	int black = gui.getValueI("black");
	float sigma1 = gui.getValueF("sigma1");
	float sigma2 = gui.getValueF("sigma2");
	float tau = gui.getValueF("tau");
	float thresh = gui.getValueF("thresh");
	int halfw = gui.getValueI("halfw");
	int smoothPasses = gui.getValueI("smoothPasses");
	int cannyParam1 = gui.getValueI("cannyParam1");
	int cannyParam2 = gui.getValueI("cannyParam2");
	
	for(int i = 0; i < input.size(); i++) {
		if(doFDoG) {
			// CLD = Coherent Line Drawing
			// This method is defined in `Wrappers.h`, although it utilizes
			// the `CLD` lib (see "ofxCv/libs/CLD")
			ofxCv::CLD(input[i], output[i], halfw, smoothPasses, sigma1, sigma2, tau, black);

			// invert the resulting image
			// defined in 'Wrappers.h'
			ofxCv::invert(output[i]);

			if(doThresh) {
				// defined in 'Wrappers.h'
				// http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
				ofxCv::threshold(output[i], thresh);
			}

			if(doThin) {
				// defined in 'Helpers.h'
				ofxCv::thin(output[i]);
			}

			// upate the output `ofImage`
			output[i].update();

			if(doCanny) {
				// perform Canny edge detection
				// defined in 'Wrappers.h'
				// http://docs.opencv.org/modules/imgproc/doc/feature_detection.html?highlight=canny%20edge%20detection#canny
				ofxCv::Canny(input[i], canny[i], cannyParam1 * 2, cannyParam2 * 2, 5);

				// update the canny `ofImage`
				canny[i].update();
			}
		}
	}
}

void ofApp::draw(){
	// set our stage
	ofBackground(255);
	ofTranslate(300, 0);

	// handle each of the three images
	for(int i = 0; i < input.size(); i++) {

		// enable alpha blending (this is on by default after ofx 0.8.0)
		// this allows us to overlay the edges on top of the original img
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);

		// draw the input image
		input[i].draw(i * 256, 0);

		// only draw the output image if we've enabled FDoG
		if(doFDoG) {
			// use additive blending to draw overlay
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			// draw the output image (contours) over the input image
			output[i].draw(i * 256, 0);
		}

		// this repeats the above for a second row
		// of images using canny edge detection (for comparison)
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		input[i].draw(i * 256, 256);
		// only draw the canny image if we've enabled it
		if(doCanny && doFDoG) {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			canny[i].draw(i * 256, 256);
		}
	}
	
	// label the CLD and Canny image sets
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofDrawBitmapStringHighlight("Coherent line drawing", 10, 20);
	ofDrawBitmapStringHighlight("Canny edge detection", 10, 256 + 20);
}
