#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFullscreen(true);
    ofEnableSmoothing();
	ofSetCircleResolution(32);

	setupGui();
    tuioClient.start(3333);

	tuioChanged = false;

	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);

    ofAddListener(tuioClient.cursorAdded,this,&ofApp::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved,this,&ofApp::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated,this,&ofApp::tuioUpdated);
}

//--------------------------------------------------------------
void ofApp::update()
{
    tuioClient.getMessage();

	if (tuioChanged && tuioContacts.size() == contactCount->getValue())
	{
		pukContacts.clear();
		for (auto tuio : tuioContacts)
		{
			pukContacts.push_back(pukContact());
			pukContacts.back().contact.sid = tuio.sid;
			pukContacts.back().contact.position = tuio.position;
		}
	}

	tuioChanged = false;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(0, 0);
    tuioClient.drawCursors();

	for (auto contact : pukContacts)
	{
		contact.draw(contact.intersects(ofPoint(ofGetMouseX(), ofGetMouseY())));
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	delete gui;
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	gui = new ofxUISuperCanvas("TUIO Puk Editor", 0, 0, 200, 110);

	gui->addSpacer();
	pukFileName = gui->addTextInput("Puk file name: ", "puk.xml");
	pukFileName->setAutoUnfocus(false);

	contactCount = gui->addIntSlider("Contact points count: ", 2, 10, 3);

	gui->addLabelButton("Write file", false);

	gui->setTheme(ofxUIThemeType::OFX_UI_THEME_BLUEBLUE);
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if (kind == OFX_UI_WIDGET_LABELBUTTON)
	{
		ofxUIButton *button = (ofxUIButton *)e.widget;
		if (button->getValue() > 0) {
			cout << name << ": " << pukFileName->getTextString() << endl;
		}
	}
}

void ofApp::tuioAdded(ofxTuioCursor &tuioCursor)
{
	tuioContacts.push_back(tuioContact());
	tuioContacts.back().sid = tuioCursor.getSessionId();
	tuioContacts.back().position.set(tuioCursor.getX() * ofGetWidth(), tuioCursor.getY() * ofGetHeight());

	tuioChanged = true;
}

void ofApp::tuioUpdated(ofxTuioCursor &tuioCursor)
{
	int cursorIndex = getTuioPointIndex(tuioCursor.getSessionId());
	if (cursorIndex < 0)
	{
		return;
	}
	tuioContacts.at(cursorIndex).position.set(tuioCursor.getX() * ofGetWidth(), tuioCursor.getY() * ofGetHeight());

	tuioChanged = true;
}

void ofApp::tuioRemoved(ofxTuioCursor &tuioCursor)
{
	int cursorIndex = getTuioPointIndex(tuioCursor.getSessionId());
	if (cursorIndex >= 0)
	{
		tuioContacts.erase(tuioContacts.begin() + cursorIndex);
	}

	tuioChanged = true;
}

int ofApp::getTuioPointIndex(int sid)
{
	int cursorIndex = -1;
	for (int i = 0; i < tuioContacts.size(); ++i)
	{
		if (tuioContacts.at(i).sid == sid)
		{
			cursorIndex = i;
			break;
		}
	}
	return cursorIndex;
}

int ofApp::getPukContactIndex(int sid)
{
	int cursorIndex = -1;
	for (int i = 0; i < pukContacts.size(); ++i)
	{
		if (pukContacts.at(i).contact.sid == sid)
		{
			cursorIndex = i;
			break;
		}
	}
	return cursorIndex;
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	ofPoint currentMousePosition(x, y);
	for (auto& contact : pukContacts)
	{
		if (contact.clicked)
		{
			contact.contact.position.set(contact.contact.position + (currentMousePosition - lastMousePosition));
		}
	}

	lastMousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	for (auto& contact : pukContacts)
	{
		if (contact.intersects(ofPoint(x, y)))
		{
			contact.clicked = true;
		}
	}
	lastMousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	for (auto& contact : pukContacts)
	{
		contact.clicked = false;
	}
}
