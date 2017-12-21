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
bool cleanWindow = false;		//clean the window if it true in the draw method
GeometricRecognizer Gr;


class Client : public TuioListener {
	// these methods need to be implemented here since they're virtual methods
	// these methods will be called whenever a new package is received
	





	void Client::addTuioObject(TuioObject *tobj){};
	void Client::updateTuioObject(TuioObject *tobj){
	
	};
	void Client::removeTuioObject(TuioObject *tobj){};
	
	void Client::addTuioCursor(TuioCursor *tcur)
	{

		//std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor *tcur){
		//std::cout << "update finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
		
	};


	


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
						cleanWindow = true;
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

	cleanWindow = false;
	}
	glLineWidth(lineWidth);
	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter) {
	
		
		tuioList = (*cursorListIter)->getPath();
		
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0f, 0.0f, 1.0f);
		for (int i = 0; !tuioList.empty(); i++) {
			float x;
			if (tuioList.front().getX() > 0.5) {
				x = (tuioList.front().getX() - 0.5) * 2;
			}
			else if(tuioList.front().getX() < 0.5 ) {
				x = (tuioList.front().getX() * 2) - 1;
			}
			else {
				x = 0;
			}
			float y;
			if (tuioList.front().getY() > 0.5) {
				y = (tuioList.front().getY() - 0.5) * -2;
			}
			else if (tuioList.front().getY() < 0.5) {
				y = ((tuioList.front().getY() * 2) -1)* -1;
			}
			else {
				y = 0;
			}
			glVertex2f(x,y);    // x, y

			//glVertex2f( tuioList.front().getX() * -1, - tuioList.front().getY());    // x, y

			tuioList.pop_front();
			
		}
		glEnd();

		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		glLineWidth(1);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(-0.4, -1);
		glVertex2f(-0.4, 1);

		//glVertex2f(-0.6, -0.4);
		//glVertex2f(-0.6, -1);

		glEnd();
		glPopMatrix();

		glFlush();  // Render now*/



	}
	

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