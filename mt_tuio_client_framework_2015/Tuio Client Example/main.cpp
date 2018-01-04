#include <iostream>
#include <process.h>

#include "TuioClient.h"
#include "TuioListener.h"
#include "../Recognizer/GeometricRecognizer.h"
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
bool drawQuad = false;
vector<long> detectetFingerInQuad;
vector<TuioCursor> detectetCursorInQuad;


vector<Point2D> quadPoints{ { -0.3, -0.3 },// bottom left corner
							{ -0.3, 0.3 },// top left corner
							{ 0.3, 0.3 }, // top right corner
							{ 0.3, -0.3 } };// bottom right corner


GLfloat quadVertices[] = {  -0.5, -0.5, 0, // bottom left corner
							-0.5,  0.5, 0, // top left corner
							 0.5,  0.5, 0, // top right corner
							 0.5, -0.5, 0 }; // bottom right corner



GeometricRecognizer Gr;

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
			detectetFingerInQuad.push_back((tcur)->getSessionID());
			detectetCursorInQuad.push_back(*tcur);
		}
		
	
		
		//std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor *tcur){
		//std::cout << "update finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
		
		if (drawQuad) {
			for (int i = 0; i < detectetCursorInQuad.size(); i++) {
				if (detectetCursorInQuad.at(i).getSessionID() == (tcur)->getSessionID()) {
					//cout << "finger is detected and updated\n";
					moveQuad(translateXcoords((tcur)->getX()) - translateXcoords(detectetCursorInQuad.at(i).getX()), translateYcoords((tcur)->getY()) - translateYcoords(detectetCursorInQuad.at(i).getY()));
					detectetCursorInQuad.at(i) = (tcur);
				}
			}
		}
	


			//list<TuioPoint>::iterator it = (tcur)->getPath().begin();		<<<<< Fehler !!!!!!
			//++it;
			//cout << (*it).getX() << " New: " << (tcur)->getX() << "\n";

		
	};

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
				cout << " not in quad";
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
						drawQuad = true;

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
	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter) {
	
		
		tuioList = (*cursorListIter)->getPath();
		
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.0f, 1.0f);
		for (int i = 0; !tuioList.empty(); i++) {	
			glVertex2f((float)translateXcoords(tuioList.front().getX()),(float) translateYcoords(tuioList.front().getY()));    // x, y
			tuioList.pop_front();
		}
		glEnd();
		
	}

	
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