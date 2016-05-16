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

		const void draw(bool highlight)
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

		const bool intersects(const ofPoint location)
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

	struct puk
	{
	public:
		vector<pukContact> pukContacts;

		const void drawCenter()
		{
			if (pukContacts.size() < 2)
			{
				return;
			}
			ofPushStyle();
			ofVec2f center = getCenter();
			ofFill();
			ofSetColor(CENTER_COLOR);
			ofCircle(center, CENTER_RADIUS);
			ofNoFill();
			ofSetColor(CIRCLE_COLOR);
			ofCircle(center, getCenter().distance(pukContacts.at(0).contact.position));
			ofPopStyle();
		}

		const ofVec2f getCenter()
		{
			if (pukContacts.size() < 3)
			{
				if (pukContacts.size() < 2)
				{
					if (pukContacts.empty())
					{
						return ofVec2f(0);
					}
					return pukContacts.at(0).contact.position;
				}
				return (pukContacts.at(0).contact.position + pukContacts.at(1).contact.position) / 2;
			}

			// https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates_2

			ofVec2f A = pukContacts.at(0).contact.position;
			ofVec2f aB = pukContacts.at(1).contact.position - A;
			ofVec2f aC = pukContacts.at(2).contact.position - A;
			float D = 2 * (aB.x * aC.y - aB.y * aC.x);

			ofVec2f circumcenter(0);
			circumcenter.x = (aC.y * (pow(aB.x, 2) + pow(aB.y, 2)) - aB.y * (pow(aC.x, 2) + pow(aC.y, 2))) / D;
			circumcenter.y = (aB.x * (pow(aC.x, 2) + pow(aC.y, 2)) - aC.x * (pow(aB.x, 2) + pow(aB.y, 2))) / D;
			return circumcenter + A;
		}

	private:
		const ofColor CENTER_COLOR = ofColor(255, 255, 255, 255);
		const ofColor CIRCLE_COLOR = ofColor(255, 255, 255, 64);
		const float CENTER_RADIUS = 10;
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

	const int getTuioPointIndex(const int sid);
	const int getPukContactIndex(const int sid);

	ofxUISuperCanvas *gui;
	ofxUITextInput *pukFileName;
	ofxUIIntSlider *contactCount;
    ofxTuioClient tuioClient;
	ofPoint lastMousePosition;
	vector<tuioContact> tuioContacts;
	puk activePuk;
	bool tuioChanged;
};
