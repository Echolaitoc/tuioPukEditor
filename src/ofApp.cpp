#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFullscreen(true);
    ofEnableSmoothing();
	ofSetCircleResolution(128);

	setupGui();
    tuioClient.start(3333);

	tuioChanged = false;

	for (int i = 0; i < contactCount->getValue(); ++i)
	{
		activePuk.pukContacts.push_back(pukContact());
		activePuk.pukContacts.back().contact.sid = -1 - i;
		activePuk.pukContacts.back().contact.position = ofVec2f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
		activePuk.pukContacts.back().contact.position.x += sin(((float)i / contactCount->getValue()) * 2 * PI) * 300;
		activePuk.pukContacts.back().contact.position.y += cos(((float)i / contactCount->getValue()) * 2 * PI) * 300;
	}

	activePuk.recalculate = true;

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
		activePuk.pukContacts.clear();
		for (auto tuio : tuioContacts)
		{
			activePuk.pukContacts.push_back(pukContact());
			activePuk.pukContacts.back().contact.sid = tuio.sid;
			activePuk.pukContacts.back().contact.position = tuio.position;
		}
	}

	tuioChanged = false;

	if (activePuk.recalculate)
	{
		activePuk.recalculateOrder();
		activePuk.recalculate = false;
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(0, 0);
    tuioClient.drawCursors();

	activePuk.draw();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	delete gui;
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	gui = new ofxUISuperCanvas("TUIO Puk Editor", 0, 0, 200, 150);

	gui->addSpacer();
	pukFileName = gui->addTextInput("Puk file name: ", "puk.xml");
	pukFileName->setAutoUnfocus(false);

	contactCount = gui->addIntSlider("Contact points count: ", 2, 10, 3);

	tolerance = gui->addIntSlider("Tolerance: ", 1, 100, 10);
	gui->addSpacer();

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
	activePuk.recalculate = true;
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
	activePuk.recalculate = true;
}

const int ofApp::getTuioPointIndex(const int sid)
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

const int ofApp::getPukContactIndex(const int sid)
{
	int cursorIndex = -1;
	for (int i = 0; i < activePuk.pukContacts.size(); ++i)
	{
		if (activePuk.pukContacts.at(i).contact.sid == sid)
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
	ofVec2f currentMousePosition(x, y);
	for (auto& contact : activePuk.pukContacts)
	{
		if (contact.clicked)
		{
			contact.contact.position.set(contact.contact.position + (currentMousePosition - lastMousePosition));
			activePuk.recalculate = true;
		}
	}

	lastMousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	for (auto& contact : activePuk.pukContacts)
	{
		if (contact.intersects(ofVec2f(x, y)))
		{
			contact.clicked = true;
		}
	}
	lastMousePosition.set(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	for (auto& contact : activePuk.pukContacts)
	{
		contact.clicked = false;
	}
}
