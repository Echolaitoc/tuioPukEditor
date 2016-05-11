#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    
    tuioClient.start(3333);
    
    ofAddListener(tuioClient.cursorAdded,this,&ofApp::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved,this,&ofApp::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated,this,&ofApp::tuioUpdated);
}

//--------------------------------------------------------------
void ofApp::update(){
    tuioClient.getMessage();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0, 0);
    tuioClient.drawCursors();
}

void ofApp::tuioAdded(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " add at " << loc << endl;
}

void ofApp::tuioUpdated(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " updated at " << loc << endl;
}

void ofApp::tuioRemoved(ofxTuioCursor &tuioCursor){
	ofPoint loc = ofPoint(tuioCursor.getX()*ofGetWidth(),tuioCursor.getY()*ofGetHeight());
	cout << "Point n" << tuioCursor.getSessionId() << " remove at " << loc << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}