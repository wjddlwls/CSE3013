/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

	#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#define SIZE 36//�̷� �� ������ ����
typedef struct Stack {
	int top;
	int arr[100];
};
typedef struct Queue {
	int front;
	int rear;
	int arr[100];
};
class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void printGraph();
		void printStack(Stack* stack);
		void dfssubdraw();
		void BFS();
		void enqueue(Queue* q, int n);
		int dequeue(Queue* q);
		void bfsdraw();
		void bfssubdraw();
		void bfs2queue(int parent[], int end);

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool readFile();
		void freeMemory();
		bool DFS();
		void dfsdraw();
		int HEIGHT;//�̷��� ����
		int WIDTH;//�̷��� �ʺ�
		char** input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭�̴�.
		int** visited;//�湮���θ� ������ ����
		int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int k;
		int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
		int isDFS;//DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
		int isBFS;//BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
		// Menu
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
        ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		// Example menu variables
		bool bShowInfo;
		bool bFullscreen;
		bool bTopmost;
		bool isdfs;
		bool isbfs;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);
		Stack DFSans;
		Stack DFSsubroute[100];//���ٰ� ���� ��� ������ ����
		Queue BFSans;
		Queue BFSsubroute[100];
		
		// ��θ� ������ ���� ����
		//int path[100][100]; // Ž�� ��� ����
		//int path_length[100];     // �� �������� ��� ���� ����

		int d_index = 0;//subdfs ����
		int b_index = 0;//subbfs ����
		int row1 = 0;
		int row2 = 0;
		int col1 = 0;
		int col2 = 0;
};
