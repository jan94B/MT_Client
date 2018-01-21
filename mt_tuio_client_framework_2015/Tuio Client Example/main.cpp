 #include <iostream>
#include <process.h>

#include "TuioClient.h"
#include "TuioListener.h"
#include "../Recognizer/GeometricRecognizer.h"
#include "../Cube/Wuerfel.h"
#include <list>
#include <math.h>




#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glut.h>

using namespace TUIO;
using namespace std;
using namespace DollarRecognizer;



TUIO::TuioClient *tuioClient; // global tuioClient for testing
float lineWidth = 6.5;			//define width of the lines
bool cleanWindow = true;		//clean the window if it true in the draw method

bool drawQuad = false;
float quadRotateAngle = -1;
float scaleQuadValue = 0;
float quadMoveX = 0;
float quadMoveY = 0;
float fingerDistanceByQuad;

bool drawCircle = true;
float circleRadius = 0.3;
Point2D circlePosition { 0, 0 };

vector<TuioCursor> detectedFingersInQuad;
vector<TuioCursor> detectedFingersInCircle;


vector<Point2D> quadPoints{ { -0.3, -0.6 },// bottom left corner
							{ -0.3, 0.6 },// top left corner //TODO
							{ 0.3, 0.6 }, // top right corner
							{ 0.3, -0.6 } };// bottom right corner


vector<float> colorVectorQuad{ 0.0,1.0,0.0 };//red, green, blue
vector<float> colorVectorCircle{ 0.0,1.0,0.0 };//red, green, blue


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

double Distance(double dX0, double dY0, double dX1, double dY1)
{
	return sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0));
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
		else if (circleDetection(translateXcoords((tcur)->getX()), translateYcoords((tcur)->getY()))) {
			cout << "circle Detected";
			detectedFingersInCircle.push_back(*tcur);
		}

	
		
		//std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};



	void Client::updateTuioCursor(TuioCursor *tcur){
		//std::cout << "update finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
		


		if (drawCircle && detectedFingersInCircle.size() > 0) {
			bool fingerDetectedInCircle = false;
			int indexOfTheDetectedFingerCircle;

			for (int i = 0; i < detectedFingersInCircle.size(); i++) {
				if (detectedFingersInCircle.at(i).getSessionID() == (tcur)->getSessionID()) {
					fingerDetectedInCircle = true;
					indexOfTheDetectedFingerCircle = i;
				}
			}


			if (fingerDetectedInCircle) {
				if (fingerDetectedInCircle && detectedFingersInCircle.size() == 1) { //Move the Circle
					circlePosition.x = translateXcoords((tcur)->getX());
					circlePosition.y = translateYcoords((tcur)->getY());
					//cout << "translation" << endl;
				}





				if (detectedFingersInCircle.size() == 2) {//Scale circle
					//cout << "2 Finger Detected";
					float newDistance = 0;
					float oldDistance = detectedFingersInCircle.at(0).getDistance(detectedFingersInCircle.at(1).getX(), detectedFingersInCircle.at(1).getY());

					if (indexOfTheDetectedFingerCircle == 0) {
						newDistance = detectedFingersInCircle.at(1).getDistance((tcur)->getX(), (tcur)->getY());
					}
					else {
						newDistance = detectedFingersInCircle.at(0).getDistance((tcur)->getX(), (tcur)->getY());
					} 
					if (newDistance - oldDistance != 0) {
						circleRadius += (newDistance - oldDistance) * 5;
					}
				}
				detectedFingersInCircle.at(indexOfTheDetectedFingerCircle) = (tcur);
			}
		}
		


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
					moveQuad(translateXcoords((tcur)->getX()) - translateXcoords(detectedFingersInQuad.at(indexOfTheDetectedFinger).getX()), translateYcoords((tcur)->getY()) - translateYcoords(detectedFingersInQuad.at(indexOfTheDetectedFinger).getY()));
					//cout << "translation" << endl;
				}

				if (fingerDetectedInQuad && detectedFingersInQuad.size() == 2) { //Move the quad (two fingers)
					// get X and Y of both and interpolate
					float oldMidX = (translateXcoords(detectedFingersInQuad.at(0).getX())+ translateXcoords(detectedFingersInQuad.at(1).getX())) /2 ;
					float oldMidY = (translateYcoords(detectedFingersInQuad.at(0).getY()) + translateYcoords(detectedFingersInQuad.at(1).getY())) / 2;
					float newMidX = 0;
					float newMidY = 0;


					if (indexOfTheDetectedFinger == 0) {
						newMidX = (translateXcoords((tcur)->getX()) + translateXcoords(detectedFingersInQuad.at(1).getX())) /2;
						newMidY = (translateYcoords((tcur)->getY()) + translateYcoords(detectedFingersInQuad.at(1).getY())) / 2;
					}
					else {
						newMidX = (translateXcoords((tcur)->getX()) + translateXcoords(detectedFingersInQuad.at(0).getX())) /2;
						newMidY = (translateYcoords((tcur)->getY()) + translateYcoords(detectedFingersInQuad.at(0).getY())) /2;
					}
					moveQuad(newMidX - oldMidX, newMidY- oldMidY);
					cout << "translation two fingers" << endl;
				}

				
				
				if (detectedFingersInQuad.size() == 2) {//Scale the Quad
					//cout << "2 Finger Detected";
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
					}
					
				}


			

				if (detectedFingersInQuad.size() == 2) {//rotate the Quad
					double newQuadRotateAngle = 0;

					if (quadRotateAngle == -1) {
						quadRotateAngle = detectedFingersInQuad.at(0).getAngleDegrees(detectedFingersInQuad.at(1).getX(), detectedFingersInQuad.at(1).getY());
					}
					else {
						newQuadRotateAngle = detectedFingersInQuad.at(0).getAngleDegrees(detectedFingersInQuad.at(1).getX(), detectedFingersInQuad.at(1).getY());

						if (newQuadRotateAngle - quadRotateAngle > 2 || newQuadRotateAngle - quadRotateAngle < -2) {
							cout << "rotate :" << newQuadRotateAngle - quadRotateAngle << "\n";
							RotateQuad((newQuadRotateAngle - quadRotateAngle) * 0.1);
							quadRotateAngle = newQuadRotateAngle;
						}
					}

				}

				detectedFingersInQuad.at(indexOfTheDetectedFinger) = (tcur);//update the vector detectedFingersInQuad with the new Finger

			}	
		}
			
			
			//list<TuioPoint>::iterator it = (tcur)->getPath().begin();		<<<<< Fehler !!!!!!
			//++it;
			//cout << (*it).getX() << " New: " << (tcur)->getX() << "\n";

		
	};
	

	void RotateQuad(float quadRotateAngle) {

		// 1. Mittelpunkt berechnen
		// 2. Wie weit ist der Körper vom Mittelpunkt entfernt? -> alle 4 Koord entsprechend verschieben

		double midpointX = 0;
		double midpointY = 0;

		for (int i = 0; i < quadPoints.size(); i++) {
			midpointX += quadPoints.at(i).x;
			midpointY += quadPoints.at(i).y;
		}

		midpointX /= 4;
		midpointY /= 4;

		// Translation aller Eckpunkte um M
		for (int i = 0; i < quadPoints.size(); i++) {
			quadPoints.at(i).x -= midpointX; 
			quadPoints.at(i).y -= midpointY;
		}

		for (int i = 0; i < quadPoints.size(); i++) {
			////neu
			double oldx = quadPoints.at(i).x;
			quadPoints.at(i).x = (quadPoints.at(i).x * cos(quadRotateAngle)) - quadPoints.at(i).y * sin(quadRotateAngle) ;
			quadPoints.at(i).y = oldx * (sin(quadRotateAngle)) + quadPoints.at(i).y * (cos(quadRotateAngle));


		}
		// Translation wieder zurück
		for (int i = 0; i < quadPoints.size(); i++) {
			quadPoints.at(i).x += midpointX;
			quadPoints.at(i).y += midpointY;
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

	bool circleDetection(float x, float y) {

		if (circleRadius >= Distance(circlePosition.x, circlePosition.y, x, y)) {
			return true;
		}
		else {
			return false;
		}
	}

	//Check if the point in the quad
	bool quadDetection(float x, float y) {


		if (drawQuad) {

			double midpointX = 0;
			double midpointY = 0;

			for (int i = 0; i < quadPoints.size(); i++) {
				midpointX += quadPoints.at(i).x;
				midpointY += quadPoints.at(i).y;
			}

			midpointX /= 4;
			midpointY /= 4;

			double pointD;
			double quadDistance;

			pointD = Distance(midpointX, midpointY, x, y);
			quadDistance = Distance(midpointX, midpointY, quadPoints.at(0).x, quadPoints.at(0).y);
			
			if (pointD < quadDistance) {
				//cout << "in quad";
				return true;
			}
			else {
				//cout << " not in quad";
				return false;
			}
		}

		return false;
	}
	

	int colorSwitchQuad = 0;
	int colorSwitchCircle = 0;

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
				quadRotateAngle = -1;//Reset the old rotate angle
			}
		}

		for (int i = 0; i < detectedFingersInCircle.size(); i++) {//Erase the detectetCursor in the quad vector
			if (detectedFingersInCircle.at(i).getSessionID() == (tcur)->getSessionID()) {
				detectedFingersInCircle.erase(detectedFingersInCircle.begin() + i);
			}
		}


		//tranform the TUIO points to Point2D
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

					cout << "Result Name: " << rResult.name << " score" << rResult.score;

					if (rResult.name == "Circle") {
						if (drawCircle) {
							drawCircle = false;
							detectedFingersInCircle.clear();
						}
						else {
							drawCircle = true;
						}
					}

					if (rResult.name == "Delete") {
						cleanWindow = true;

					}
					if (rResult.name == "Rectangle") {
						if (drawQuad) {
							drawQuad = false;
							detectedFingersInQuad.clear();
						}
						else {
							drawQuad = true;
						}
					}
					

					if (rResult.name == "V" && detectedFingersInQuad.size() > 0) {

						switch (colorSwitchQuad) {
						case 0:
							colorVectorQuad.at(0) = 1;
							colorVectorQuad.at(1) = 0;
							colorVectorQuad.at(2) = 0;
							colorSwitchQuad++;
							break;
						case 1:
							colorVectorQuad.at(0) = 0;
							colorVectorQuad.at(1) = 0;
							colorVectorQuad.at(2) = 1;
							colorSwitchQuad++;
							break;
						case 2:
							colorVectorQuad.at(0) = 0;
							colorVectorQuad.at(1) = 1;
							colorVectorQuad.at(2) = 0;
							colorSwitchQuad = 0;
							break;
						}

						colorVectorQuad[0] = 1;
					}


						if (rResult.name == "V" && detectedFingersInCircle.size() > 0) {

							switch (colorSwitchCircle) {
							case 0:
								colorVectorCircle.at(0) = 1;
								colorVectorCircle.at(1) = 0;
								colorVectorCircle.at(2) = 0;
								colorSwitchCircle++;
								break;
							case 1:
								colorVectorCircle.at(0) = 0;
								colorVectorCircle.at(1) = 0;
								colorVectorCircle.at(2) = 1;
								colorSwitchCircle++;
								break;
							case 2:
								colorVectorCircle.at(0) = 0;
								colorVectorCircle.at(1) = 1;
								colorVectorCircle.at(2) = 0;
								colorSwitchCircle = 0;
								break;
							}
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

	/*//Zeichnen Auskommentiert, da immer wieder Fehler bei getPath kommt
	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter) {
	
		
		tuioList = (*cursorListIter)->getPath();
		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.0f, 1.0f);
		for (int i = 0; !tuioList.empty(); i++) {	
			glVertex2f((float)translateXcoords(tuioList.front().getX()),(float) translateYcoords(tuioList.front().getY()));    // x, y
			tuioList.pop_front();
		}
		glEnd();
		glPopMatrix();

	}*/

	
		if (drawQuad) {
			glPushMatrix();			
			glColor3f(colorVectorQuad.at(0), colorVectorQuad.at(1), colorVectorQuad.at(2));

			glBegin(GL_QUADS);
			glVertex2d(quadPoints.at(0).x, quadPoints.at(0).y);
			glVertex2d(quadPoints.at(1).x, quadPoints.at(1).y);
			glVertex2d(quadPoints.at(2).x, quadPoints.at(2).y);
			glVertex2d(quadPoints.at(3).x, quadPoints.at(3).y);

			glEnd();

			glPopMatrix();

			
		}


		if (drawCircle) {
			glPushMatrix();
			glColor3f(colorVectorCircle.at(0), colorVectorCircle.at(1), colorVectorCircle.at(2));
			glBegin(GL_TRIANGLE_FAN);
			for (int i = 0; i <= 300; i++) {
				double angle = 2 * M_PI * i / 300;
				double x = cos(angle) * circleRadius + circlePosition.x;
				double y = sin(angle) * circleRadius + circlePosition.y;
				glVertex2d(x, y);
			}
			glEnd();
			glPopMatrix();

		}


		/*
		//Koord system
		glBegin(GL_LINES);   //Vorderseite
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f); //ROT x
		glVertex3f(-100.0, 0.0, 0.0);
		glVertex3f(100.0, 0.0, 0.0);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f); //GRUEN y
		glVertex3f(0.0, -100.0, 0.0);
		glVertex3f(0.0, 100.0, 0.0);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f); //BLAU z
		glVertex3f(0.0, 0.0, -100.0);
		glVertex3f(0.0, 0.0, 100.0);
		glEnd();
		// Koord system
		*/



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
	

	return 0;
}