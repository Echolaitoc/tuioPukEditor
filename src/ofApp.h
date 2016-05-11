#pragma once

#define OFX_UI_NO_XML

#include "ofMain.h"
#include "ofxUi.h"
#include "ofxTuio.h"

class ofApp : public ofBaseApp{
private:
	struct tuioContact
	{
	public:
		long sid;
		ofPoint position;
	};

	struct pukContact
	{
	public:
		tuioContact contact;
		bool clicked = false;

		void draw(bool highlight)
		{
			ofPushStyle();
			if (highlight)
			{
				ofSetColor(HIGHLIGHT_COLOR);
			}
			else
			{
				ofSetColor(OUTER_COLOR);
			}
			ofCircle(contact.position, RADIUS);
			ofSetColor(INNER_COLOR);
			ofCircle(contact.position, INNER_RADIUS);
			ofPopStyle();
		}

		bool intersects(ofPoint location)
		{
			return contact.position.distanceSquared(location) <= pow(RADIUS, 2);
		}

	private:
		const ofColor OUTER_COLOR = ofColor(255, 255, 255, 64);
		const ofColor HIGHLIGHT_COLOR = ofColor(0, 150, 210, 128);
		const ofColor INNER_COLOR = ofColor(0, 154, 213, 255);
		const float RADIUS = 50;
		const float INNER_RADIUS = 5;
	};

    void setup();
    void update();
    void draw();
	void exit();

	void setupGui();
	void guiEvent(ofxUIEventArgs &e);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void tuioAdded(ofxTuioCursor & tuioCursor);
	void tuioRemoved(ofxTuioCursor & tuioCursor);
	void tuioUpdated(ofxTuioCursor & tuioCursor);

	int getTuioPointIndex(int sid);
	int getPukContactIndex(int sid);

	ofxUISuperCanvas *gui;
	ofxUITextInput *pukFileName;
	ofxUIIntSlider *contactCount;
    ofxTuioClient tuioClient;
	ofPoint lastMousePosition;
	vector<tuioContact> tuioContacts;
	vector<pukContact> pukContacts;
	bool tuioChanged;
};
