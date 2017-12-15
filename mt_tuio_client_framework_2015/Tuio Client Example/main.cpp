#include <iostream>
#include <process.h>

#include "TuioClient.h"
#include "TuioListener.h"
#include "../Recognizer/GeometricRecognizer.h"
#include <list>

#include <GL/glut.h>


using namespace TUIO;
using namespace std;
using namespace DollarRecognizer;


TUIO::TuioClient *tuioClient; // global tuioClient for testing

class Client : public TuioListener {
	// these methods need to be implemented here since they're virtual methods
	// these methods will be called whenever a new package is received

	void Client::addTuioObject(TuioObject *tobj){};
	void Client::updateTuioObject(TuioObject *tobj){
	
	};
	void Client::removeTuioObject(TuioObject *tobj){};
	
	void Client::addTuioCursor(TuioCursor *tcur)
	{
		std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor *tcur){
		std::cout << "update finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";

	};
	void Client::removeTuioCursor(TuioCursor *tcur){
		std::cout << "remove finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";

	};

	void  Client::refresh(TuioTime frameTime){};
};

void draw()
{
	// openGL draw function
	std::list<TuioCursor*> cursorList;

	// get the actual cursor list from the tuioClient
	tuioClient->lockCursorList();
	cursorList = tuioClient->getTuioCursors();
	tuioClient->unlockCursorList();
	GeometricRecognizer Gr;


		
	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter)
	{


		//line(original, oldCoords.at(i), oldCoords.at(i + 1), Scalar(255, 255, 255), 2, 8);
		std::cout << "id: " << (*cursorListIter)->getCursorID() << "\n" <<
			"X: " << (*cursorListIter)->getX() << "\n"
			"Y:" << (*cursorListIter)->getY();
			


		/*
		Path2D path1;
		list<TuioPoint> tuioList;
		
		tuioList = (*cursorListIter)->getPath;



		Point2D point2D;
		path1.push_back(point2D);

		RecognitionResult rResult = Gr.recognize(path1);

		cout << "Result Name: " << rResult.name;
		*/
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
}

int main(int argc, char** argv)
{
	// create a second thread for the TUIO listener
	HANDLE hThread_TUIO;
	unsigned threadID;
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &tuioThread, NULL, 0, &threadID );
	hThread_TUIO = (HANDLE)_beginthread( tuioThread, 0, NULL );

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

	return 0;
}