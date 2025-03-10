#pragma once
#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#define SIZE 36//미로 방 사이의 간격, 조절 가능
#define MAX_VERTICES 5000///////////나중에 수정
typedef struct node* node_pointer;
typedef struct node {
	int vertex;
	node_pointer link;
	bool visited;
};
node_pointer graph[MAX_VERTICES] = { NULL };//정점 n의 인접리스트의 시작을 가리킴. 순차적으로 인접 노드들을 가리키는 리스트가됨
typedef struct Stack {
	int top;
	int arr[5000];//나중에 조절하기
};
typedef struct Queue {
	int front;
	int rear;
	int arr[5000];
};
class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void printGraph();
		void printStack(Stack* stack);

		bool DFS();
		void dfsdraw();
		void dfssubdraw();

		void BFS();
		void enqueue(Queue* q, int n);
		int dequeue(Queue* q);
		void bfsdraw();
		void bfssubdraw();
		void bfs2queue(int parent[], int end);

		void playdraw();
		bool isMovable(int startRoom, int targetRoom);
		void calcTime2escape();//미로마다 탈출하는데 걸릴 적절한 시간을 계산해주는 함수

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

		int HEIGHT;//미로의 높이
		int WIDTH;//미로의 너비
		char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
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
		bool isplay;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);
		Stack DFSans;
		Stack DFSsubroute[1000];//가다가 막힌 길들 저장할 스택
		Queue BFSans;
		Queue BFSsubroute[1000];

		int d_index = 0;//subdfs 개수
		int b_index = 0;//subbfs 개수
		int row1 = 0;
		int row2 = 0;
		int col1 = 0;
		int col2 = 0;


		//프로젝트용
		int currentRoom;
		int targetRoom;//매 키 입력에 맞게 바뀜
		Stack playroute;
		int time_limit;
		int isSuccess;
		int isFail;
		int countdownTime;// 타이머의 총 시간 (초 단위)
		float lastUpdateTime; // 마지막 업데이트된 시간
		float startTime;// 타이머가 시작된 시간

};
