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
#define SIZE 36//미로 방 사이의 간격
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
		int HEIGHT;//미로의 높이
		int WIDTH;//미로의 너비
		char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		int** visited;//방문여부를 저장할 포인
		int maze_col;//미로칸의 열의 인덱스를 가리킨다.
		int maze_row;//미로칸의 행의 인덱스를 가리킨다.
		int k;
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int isBFS;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
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
		Stack DFSsubroute[100];//가다가 막힌 길들 저장할 스택
		Queue BFSans;
		Queue BFSsubroute[100];
		
		// 경로를 저장할 전역 변수
		//int path[100][100]; // 탐색 경로 저장
		//int path_length[100];     // 각 노드까지의 경로 길이 저장

		int d_index = 0;//subdfs 개수
		int b_index = 0;//subbfs 개수
		int row1 = 0;
		int row2 = 0;
		int col1 = 0;
		int col2 = 0;
};
