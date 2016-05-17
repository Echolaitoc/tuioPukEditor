#pragma once

#define OFX_UI_NO_XML

#include "ofMain.h"
#include "ofxUi.h"
#include "ofxTuio.h"

class ofApp : public ofBaseApp {
private:
	struct tuioContact
	{
	public:
		ofVec2f position;
		long sid;
	};

	struct pukContact
	{
	public:
		tuioContact contact;
		ofVec2f pukCenter;
		bool clicked;

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
			ofDrawCircle(contact.position, RADIUS);
			ofSetColor(INNER_COLOR);
			ofDrawCircle(contact.position, INNER_RADIUS);
			ofPopStyle();
		}

		const bool intersects(const ofVec2f location)
		{
			return contact.position.squareDistance(location) <= pow(RADIUS, 2);
		}

		bool operator < (const pukContact& other) const
		{
			float angle = (contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			angle *= -1;
			angle = angle < 0 ? 360 + angle : angle;
			float otherAngle = (other.contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			otherAngle *= -1;
			otherAngle = otherAngle < 0 ? 360 + otherAngle : otherAngle;
			return (angle < otherAngle);
		}

		bool operator > (const pukContact& other) const
		{
			float angle = (contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			angle *= -1;
			angle = angle < 0 ? 360 + angle : angle;
			float otherAngle = (other.contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			otherAngle *= -1;
			otherAngle = otherAngle < 0 ? 360 + otherAngle : otherAngle;
			return (angle > otherAngle);
		}

		bool operator == (const pukContact& other) const
		{
			float angle = (contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			angle *= -1;
			angle = angle < 0 ? 360 + angle : angle;
			float otherAngle = (other.contact.position - pukCenter).normalized().angle(ofVec2f(0, -1));
			otherAngle *= -1;
			otherAngle = otherAngle < 0 ? 360 + otherAngle : otherAngle;
			return (angle == otherAngle);
		}

		pukContact& operator = (const pukContact& other)
		{
			contact = other.contact;
			pukCenter = other.pukCenter;
			clicked = other.clicked;
			return *this;
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
		bool recalculate;

		void recalculateOrder()
		{
			if (pukContacts.size() < 3)
			{
				return;
			}

			ofVec2f center = getCenter();
			for (auto& contact : pukContacts)
			{
				contact.pukCenter = center;
			}

			std::sort(pukContacts.begin(), pukContacts.end());
		}

		const void draw()
		{
			for (auto contact : pukContacts)
			{
				contact.draw(contact.intersects(ofVec2f(ofGetMouseX(), ofGetMouseY())));
			}
			drawOrder();
			drawLines();
			drawCenter();
		}

		const void drawOrder()
		{
			ofPushStyle();
			for (int i = 0; i < pukContacts.size(); ++i)
			{
				ofSetColor(ORDER_COLOR);
				ofDrawBitmapString((i + 1), pukContacts.at(i).contact.position + ofVec2f(0, ORDER_STRING_OFFSET));

				ofSetColor(ANGLE_COLOR);
				ofVec2f center = getCenter();
				float angle = (pukContacts.at(i).contact.position - center).normalized().angle(ofVec2f(0, -1));
				angle *= -1;
				angle = angle < 0 ? 360 + angle : angle;
				ofDrawBitmapString(angle, pukContacts.at(i).contact.position + ofVec2f(0, ANGLE_STRING_OFFSET));
			}
			ofPopStyle();
		}

		const void drawLines()
		{
			if (pukContacts.size() < 3)
			{
				return;
			}

			ofPushStyle();
			ofSetLineWidth(LINE_WIDTH);
			for (int i = 0; i < pukContacts.size(); ++i)
			{
				ofSetColor(CIRCLE_COLOR);
				ofVec2f A = pukContacts.at(i).contact.position;
				ofVec2f B = pukContacts.at((i + 1) % pukContacts.size()).contact.position;
				ofDrawLine(A, B);
				ofVec2f center = getCenter();
				ofDrawLine(A, center);
				ofVec2f midAB = (A + B) / 2;

				ofSetColor(ofColor::white);
				ofDrawBitmapString(A.distance(B), midAB + (midAB - center).normalized() * LENGTH_STRING_OFFSET);
			}
			ofPopStyle();
		}

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
			ofDrawCircle(center, CENTER_RADIUS);
			ofNoFill();
			ofSetLineWidth(RING_WIDTH);
			ofSetColor(CIRCLE_COLOR);
			ofDrawCircle(center, getCenter().distance(pukContacts.at(0).contact.position));
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
		const ofColor ORDER_COLOR = ofColor(255, 0, 255, 128);
		const ofColor ANGLE_COLOR = ofColor(255, 0, 255, 64);
		const float CENTER_RADIUS = 10;
		const float RING_WIDTH = 40;
		const float LINE_WIDTH = 2;
		const float LENGTH_STRING_OFFSET = 5;
		const float ORDER_STRING_OFFSET = -10;
		const float ANGLE_STRING_OFFSET = 25;
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
	ofxUIIntSlider *tolerance;
	ofxTuioClient tuioClient;
	ofVec2f lastMousePosition;
	vector<tuioContact> tuioContacts;
	puk activePuk;
	bool tuioChanged;
};
