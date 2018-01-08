#include <iostream>
#include <process.h>

#include "TuioClient.h"
#include "TuioListener.h"
#include "../Recognizer/GeometricRecognizer.h"
#include "../Cube/Wuerfel.h"
#include <list>


#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glut.h>

using namespace TUIO;
using namespace std;
using namespace DollarRecognizer;



TUIO::TuioClient *tuioClient; // global tuioClient for testing
float lineWidth = 6.5;			//define width of the lines
bool cleanWindow = true;		//clean the window if it true in the draw method
bool drawCircle = false;
bool drawQuad = true;
float quadRotateAngle = 0;
float scaleQuadValue = 0;
float quadMoveX = 0;
float quadMoveY = 0;
float fingerDistanceByQuad;
vector<TuioCursor> detectedFingersInQuad;


vector<Point2D> quadPoints{ { -0.3, -0.3 },// bottom left corner
							{ -0.3, 0.3 },// top left corner
							{ 0.3, 0.3 }, // top right corner
							{ 0.3, -0.3 } };// bottom right corner


GLfloat quadVertices[] = {  -0.5, -0.5, 0, // bottom left corner
							-0.5,  0.5, 0, // top left corner
							 0.5,  0.5, 0, // top right corner
							 0.5, -0.5, 0 }; // bottom right corner



GeometricRecognizer Gr;

//translate the X coordinats from Tracking to Client
float translateXcoords(float x) {
	if (x > 0.5) {
		x = (x - 0.5) * 2;
	}
	else if (x < 0.5) {
		x = (x * 2) - 1;
	}
	else {
		x = 0;
	}
	return x;
}

//translate the Y coordinats from Tracking to Client
float translateYcoords(float y) {
	if (y > 0.5) {
		y = (y - 0.5) * -2;
	}
	else if (y < 0.5) {
		y = ((y * 2) - 1)* -1;
	}
	else {
		y = 0;
	}
	return y;
}


class Client : public TuioListener {
	// these methods need to be implemented here since they're virtual methods
	// these methods will be called whenever a new package is received
	


	


	void Client::addTuioObject(TuioObject *tobj){};
	void Client::updateTuioObject(TuioObject *tobj){};
	void Client::removeTuioObject(TuioObject *tobj){};
	
	void Client::addTuioCursor(TuioCursor *tcur)
	{

		if (quadDetection(translateXcoords((tcur)->getX()), translateYcoords((tcur)->getY()))) {
			detectedFingersInQuad.push_back(*tcur);
		}
		
	
		
		//std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor *tcur){
		//std::cout << "update finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
		
		if (drawQuad && detectedFingersInQuad.size() > 0 && detectedFingersInQuad.size() < 3) {
			fingerDistanceByQuad = 0;
			bool fingerDetectedInQuad = false;
			int indexOfTheDetectedFinger;


			

			for (int i = 0; i < detectedFingersInQuad.size(); i++) {//Check was the finger in the quad
				if (detectedFingersInQuad.at(i).getSessionID() == (tcur)->getSessionID()) {
					fingerDetectedInQuad = true;
					indexOfTheDetectedFinger = i;
				}
			}

			if (fingerDetectedInQuad) {

				if (fingerDetectedInQuad && detectedFingersInQuad.size() == 1) { //Move the quad
						//cout << "finger is detected and updated\n";
					moveQuad(translateXcoords((tcur)->getX()) - translateXcoords(detectedFingersInQuad.at(indexOfTheDetectedFinger).getX()), translateYcoords((tcur)->getY()) - translateYcoords(detectedFingersInQuad.at(indexOfTheDetectedFinger).getY()));
				}



				if (detectedFingersInQuad.size() == 2) {//Scale the Quad
					cout << "2 Finger Detected";
					float newDistance = 0;
					float oldDistance = detectedFingersInQuad.at(0).getDistance(detectedFingersInQuad.at(1).getX(), detectedFingersInQuad.at(1).getY());

					if (indexOfTheDetectedFinger == 0) {
						newDistance = detectedFingersInQuad.at(1).getDistance((tcur)->getX(), (tcur)->getY());
					}
					else {
						newDistance = detectedFingersInQuad.at(0).getDistance((tcur)->getX(), (tcur)->getY());
					}

					if (newDistance - oldDistance != 0) {
						scaleQuad(newDistance - oldDistance);
						//scaleQuadValue = newDistance - oldDistance;
					}
					
				}


				if (detectedFingersInQuad.size() == 2) {//rotate the Quad NOt READY AT THE MOMENT
					/*Funktioniert noch nicht
					quadRotateAngle = detectedFingersInQuad.at(0).getAngleDegrees(detectedFingersInQuad.at(1).getX(), detectedFingersInQuad.at(1).getY());
					cout << quadRotateAngle << "\n";
					RotateQuad(90);*/
				}











				detectedFingersInQuad.at(indexOfTheDetectedFinger) = (tcur);



			}

		
				
		}
			
			
		
	


			//list<TuioPoint>::iterator it = (tcur)->getPath().begin();		<<<<< Fehler !!!!!!
			//++it;
			//cout << (*it).getX() << " New: " << (tcur)->getX() << "\n";

		
	};

	void RotateQuad(float quadRotateAngle) {
		for (int i = 0; i < quadPoints.size(); i++) {
			quadPoints.at(i).x = quadPoints.at(i).x * cos(quadRotateAngle) + quadPoints.at(i).y * (-sin(quadRotateAngle));
			quadPoints.at(i).y = quadPoints.at(i).y * (sin(quadRotateAngle)) + quadPoints.at(i).y * (cos(quadRotateAngle));
		}


	}


	void scaleQuad(float scaleValue) {
		for (int i = 0; i < quadPoints.size(); i++) {
			quadPoints.at(i).x = quadPoints.at(i).x * (scaleValue + 1);
			quadPoints.at(i).y = quadPoints.at(i).y * (scaleValue + 1);
		}
	}

	void moveQuad(float xDifference, float yDifference) {
		for (int i = 0; i < quadPoints.size(); i++) {
			quadPoints.at(i).x = quadPoints.at(i).x + xDifference;
			quadPoints.at(i).y = quadPoints.at(i).y + yDifference;
		}
	}

	bool quadDetection(float x, float y) {
		if (drawQuad) {
			if (x > quadPoints.at(0).x && x < quadPoints.at(3).x && y > quadPoints.at(0).y && y < quadPoints.at(1).y) {
				cout << "in quad";
				return true;
			}
			else {
				//cout << " not in quad";
				return false;
			}
		}

		return false;
	}
	


	void Client::removeTuioCursor(TuioCursor *tcur){
		Gr.loadTemplates();
		//std::cout << "remove finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
		Path2D path1;
		list<TuioPoint> tuioList;
		vector<Point2D> points;
		Point2D point;



		for (int i = 0; i < detectedFingersInQuad.size(); i++) {//Erase the detectetCursor in the quad vector
			if (detectedFingersInQuad.at(i).getSessionID() == (tcur)->getSessionID()) {
				detectedFingersInQuad.erase(detectedFingersInQuad.begin() + i);
			}
		}



		tuioList = (tcur)->getPath();
		for (int i = 0; !tuioList.empty(); i++) {
			point.x = tuioList.front().getX();
			point.y = tuioList.front().getY();
			tuioList.pop_front();
			path1.push_back(point);
		}



		if (!path1.empty()) {

			if (tcur->getX() < 0.3 && tcur->getY() > 0.3 && tcur->getY() < 0.6) {//chek the Position from the input
				RecognitionResult rResult = Gr.recognize(path1);
				//cout << "Result Name: " << rResult.name << " Score" << rResult.score;

				if (rResult.score > 0.75) { //check if score high enough

					cout << "Result Name: " << rResult.name;

					if (rResult.name == "Circle") {
						drawCircle = true;
					}

					if (rResult.name == "Delete") {
						cleanWindow = true;

					}
					if (rResult.name == "Rectangle") {
						if (drawQuad) {
							drawQuad = false;
						}
						else {
							drawQuad = true;
						}
					}
					

					if (rResult.name == "V" && lineWidth == 6.5) {
						lineWidth = 2;
					}
					else if (rResult.name == "V" && lineWidth == 2) {
						lineWidth = 6.5;
					}

				
				}
				else {
					cout << "Result Name: Not Found";

				}
			}
		}
	};

	void  Client::refresh(TuioTime frameTime){};
};



bool firstDraw = true;
void draw() {
	
	if (firstDraw) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black and opaque
		glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
		firstDraw = false;
	}
	// openGL draw function
	std::list<TuioCursor*> cursorList;

	list<TuioPoint> tuioList;
	vector<Point2D> points;
	Point2D point;

	// get the actual cursor list from the tuioClient
	tuioClient->lockCursorList();
	cursorList = tuioClient->getTuioCursors();
	tuioClient->unlockCursorList();




	if (cleanWindow) {
		glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

	//cleanWindow = false;
	}
	glLineWidth(lineWidth);

	/*Zeichnen Auskommentiert, da immer wieder Fehler bei getPath kommt
	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter) {
	
		
		tuioList = (*cursorListIter)->getPath();
		
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.0f, 1.0f);
		for (int i = 0; !tuioList.empty(); i++) {	
			glVertex2f((float)translateXcoords(tuioList.front().getX()),(float) translateYcoords(tuioList.front().getY()));    // x, y
			tuioList.pop_front();
		}
		glEnd();
		
	}*/

	
		if (drawQuad) {
			glPushMatrix();			
			glColor3f(0.0f, 1.0f, 0.0f);

			glBegin(GL_QUADS);
			glVertex2d(quadPoints.at(0).x, quadPoints.at(0).y);
			glVertex2d(quadPoints.at(1).x, quadPoints.at(1).y);
			glVertex2d(quadPoints.at(2).x, quadPoints.at(2).y);
			glVertex2d(quadPoints.at(3).x, quadPoints.at(3).y);

			glEnd();

			glPopMatrix();

			
		}




		//glVertexPointer(3, GL_FLOAT, 0, quadVertices);



		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		glLineWidth(1);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(-0.4, -1);
		glVertex2f(-0.4, 1);
		glEnd();
		glPopMatrix();



		glFlush();  // Render now*/



	
	

}



void tuioThread(void*)
{
	Client *app = new Client();
	tuioClient = new TUIO::TuioClient();
	tuioClient->addTuioListener(app);
	tuioClient->connect(true);
}



void idle(void)
{
	// this might be needed on some systems, otherwise the draw() function will only be called once
	glutPostRedisplay();
}

void glInit()
{	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black and opaque

}

int main(int argc, char** argv)
{
	// create a second thread for the TUIO listener
	HANDLE hThread_TUIO;
	unsigned threadID;
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &tuioThread, NULL, 0, &threadID );
	hThread_TUIO = (HANDLE)_beginthread( tuioThread, 0, NULL );


	
	glutInit(&argc, argv);                 // Initialize GLUT
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutInitWindowSize(1200, 900);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// openGL init
	glInit();


	glutDisplayFunc(draw); // Register display callback handler for window re-paint
	glutIdleFunc(idle);

	glutMainLoop();
	

	/*
	// GLUT Window Initialization (just an example):
	glutInit(&argc, argv);
	glutInitWindowSize(752, 480);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("TUIO Client Example (GLUT Window)");

	// openGL init
	glInit();

	// Register callbacks:
	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutMainLoop();
	*/
	return 0;
}