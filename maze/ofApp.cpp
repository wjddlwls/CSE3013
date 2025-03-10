/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;

#define MAX_VERTICES 5000
//--------------------------------------------------------------
typedef struct node* node_pointer;
typedef struct node {
	int vertex;
	node_pointer link;
	bool visited;
};
node_pointer graph[MAX_VERTICES] = { NULL };//���� n�� ��������Ʈ�� ������ ����Ŵ. ���������� ���� ������ ����Ű�� ����Ʈ����
void addEdge(int start, int end) {

	//
	node_pointer newnode = (node_pointer)malloc(sizeof(node));
	newnode->visited = 0;
	newnode->vertex = end;
	newnode->link = graph[start];//ó���� null, ���ĺ��ʹ� �����ֱ�
	graph[start] = newnode;//graph[start]�� newnode�� ����Ű�� ��(graph[n]�� ��带 ����Ű�� ������)

	newnode = (node_pointer)malloc(sizeof(node));
	//newnode->visited = 0;
	newnode->vertex = start;
	newnode->link = graph[end];
	graph[end] = newnode;
}

void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = false;  // screen info display on////////////���� true
	menu->AddPopupItem(hPopup, "Show DFS",false,true); // Checked ///////���� f, f
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS",false,true); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//�޴� ���ý� üũ �� �ǰ� �׿� ���� �׸� ���൵ �� �ǵ��� �� ������
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		isdfs = bChecked;
		if (isOpen&&isdfs)
		{
			DFS();
			//bShowInfo = bChecked;
		}
		/*else
			cout << "you must open file first" << endl;
		*/
	}

	if(title == "Show BFS") {
		//doTopmost(bChecked); // Use the checked value directly
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		isbfs = bChecked;
		if (isOpen && isbfs)
		{
			BFS();
			//bShowInfo = bChecked;
		}
		/*else
			cout << "you must open file first" << endl;
		*/
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {//�̷��� �β�, ��, ��� �� �߰� ���� 

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE; ///////////////////////////////////////////////////////
	// ����� �ڷᱸ���� �̿��� �̷θ� �׸���.
	// add code here

	int startrow = 10;
	int startcol = 10;
	int endrow = startrow + HEIGHT * SIZE;
	int endcol = startcol + WIDTH * SIZE;
	ofDrawLine(startcol, startrow, endcol, startrow);// ��� ���μ�
	ofDrawLine(startcol, startrow, startcol, endrow);// ���� ���μ�
	ofDrawLine(startcol, endrow, endcol, endrow);// �ϴ� ���μ�
	ofDrawLine(endcol, startrow, endcol, endrow);// ���� ���μ�
	//ofDrawLine�Լ� : col, row ��, �� x, y������ �Է��ؾ���

	int row1, row2, col1, col2;
	int interval[4] = { -1, 1, -1 * WIDTH, WIDTH };
	for (i = 0; i < WIDTH * HEIGHT; i++) {
		for (j = 0; j < 4; j++) {
			node_pointer tmp = graph[i];
			int isEdge = 0;
			int nearVertex = i + interval[j];
			if (nearVertex >= 0 && nearVertex < WIDTH * HEIGHT) {
				while (tmp != NULL) {
					if (tmp->vertex == nearVertex) {
						isEdge = 1;
						break;
					}
					tmp = tmp->link;
				}
				if (isEdge) {
					continue;//�� �ȱ׸��� ���� j������ �Ѿ
				}
				if (j == 0 || j == 1) {//���� ���� ���� �ܿ�, ���κ� �׸���
					row1 = startrow + SIZE * (i / WIDTH);
					row2 = startrow + SIZE * (i / WIDTH + 1);
					col1 = startcol + SIZE * ((i % WIDTH+j));
					ofDrawLine(col1, row1, col1, row2);
				}
				else if (j == 2 || j == 3) {//���� �ٸ� ���� ���, ���κ� �׸���
					row1 = startrow + SIZE * (i / WIDTH+j-2);
					col1 = startrow + SIZE * (i % WIDTH);
					col2 = startcol + SIZE * (i % WIDTH + 1);
					ofDrawLine(col1, row1, col2, row1);
				}
			}
		}
	}
	
	if (isdfs)
	{
		if (isOpen) {
			ofSetColor(200);//grey
			ofSetLineWidth(5);
			dfssubdraw();

			ofSetColor(255, 0, 0);//red
			ofSetLineWidth(5);
			dfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if (isbfs)
	{
		if (isOpen) {
			ofSetColor(200);//grey
			ofSetLineWidth(5);
			bfssubdraw();

			ofSetColor(255, 0, 0);//red
			ofSetLineWidth(5);
			bfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
//���� �о ������ ���·� ������ �ڷᱸ���� �����ϴ� �Լ�
//2���� ����
bool ofApp::readFile()
{
	static int count = 0;//�� �Լ��� �ٽ� ȣ��Ǿ ���α׷��� ����Ǵ� ������ �ٽ� 0���� �ʱ�ȭ���� ����
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");

		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {//���� �б� ���� ��
	
			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int i = 0;

			// Read file line by line
			int cnt = 0;

			ofBuffer::Line it = buffer.getLines().begin();
			ofBuffer::Line end = buffer.getLines().end();

			string line = *it;
			
			WIDTH = (line.length())/2;

			if (it != end) {//�� ����, �̷��� �ֻ���� �н�
				++it;
			}
			
			// TO DO
			// .maz ������ input���� �޾Ƽ� ������ �ڷᱸ���� �ִ´�
			//line by line���� ��ȸ, �ȿ��� ��� ���� ���پ� �ݺ��ǹǷ� 
			// ���� �о���̸� ���κ� Ȯ���� ���� �߰��ϸ鼭 ���������� �̵�
			// (����)���� �о���̸� ���� �߰��ϸ鼭 ���������� ����
			// ������� 0���� �ѹ���
			//���̿� ���� ���� ��� addEdge ȣ��
			if (count!=0) {//���� �бⰡ ó���� �ƴ� ���(�ʱ�ȭ) ���õ� �ʱ�ȭ�ؾ���
				freeMemory();
				row1 = 0;
				row2 = 0;
				col1 = 0;
				col2 = 0;
				isdfs = 0;
				isbfs = 0;
				DFSans.top = -1;
				for (int j = 0; j < 100; j++) {
					DFSsubroute[j].top = -1;
				}
				BFSans.front = 0;
				for (int j = 0; j < 100; j++) {
					BFSsubroute[j].front = 0;
					BFSsubroute[j].rear = 0;
				}
			}
			for (; it != end; ++it) {//�ι�° �ٺ��� ������ �ٱ��� ��ȸ
				line = *it;
				if (line[0] == '|') {//���� ���... |  |  |    | ......
					for (i = 2; i < line.length()-1; i += 2) {//¦���ε����� Ȯ���ϸ� ��, �������� �̷��� �׵θ��� Ȯ�ξ��ϵ��� -1
						if (line[i] != '|') {
							addEdge(cnt * WIDTH + i/2-1, cnt * WIDTH + i/2);
						}
					}
					cnt++;
				}
				else {//���� ���... +-+  +-+- .....
					for (i = 1; i < line.length()-1; i += 2) {
						if (line[i] != '-') {
							addEdge((cnt -1)* WIDTH + i/2, cnt * WIDTH + i/2);
						}
					}
				}
			}
			HEIGHT = cnt;
			//printGraph();///////////////////////////////////////������
			//printf("height: %d width: %d\n", HEIGHT, WIDTH);//////////////////////////////////
			count++;//�ѹ� ���� ���� ���� ����ÿ� �ʱ�ȭ�Ǿ�� �ϹǷ� ���� ������Ʈ
			return true;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
///////////////////////////////////////////////////////////������
void ofApp::printGraph() {
	for (int i = 0; i < MAX_VERTICES; i++) {
		if (graph[i] != NULL) {
			printf("Adjacency list of vertex %d:\n head ", i);
			node_pointer temp = graph[i];
			while (temp) {
				printf("-> %d ", temp->vertex);
				temp = temp->link;
			}
			printf("\n");
		}
	}
}///////////////////////////////////////////////////////

void ofApp::freeMemory() {

	//TO DO
	// malloc�� memory�� free���ִ� �Լ�
	//�� �Լ� ���ο��� �ڽ��� ���� �ڷᱸ���� �Ҵ��� �޸𸮵��� ���� �� �־�߸�, ���� ������ �ҷ��� �� �޸� ���� �������� �߻����� �ʴ´�. �޸� ���� ����
	for (int i = 0; i < MAX_VERTICES; ++i) {
		node_pointer current = graph[i];
		while (current != NULL) {
			node_pointer toDelete = current;
			current = current->link;
			free(toDelete); // �Ҵ�� �޸� ����
		}
		graph[i] = NULL; // �׷��� �迭�� �ش� �׸��� NULL�� ����
	}
}

//1,1���� �����ؼ� �� ���鼭 ������ÿ� ������� ����, 
//������ ���ο� ���ø��� ��� ���ÿ� �ִ��� �����ϰ�,  ���粨 pop�ؼ� �� ���ÿ� ���� �� �������� ���ư�
// ->���� ��ġ�� ��� ������ ž ���� ��
//������ �׷����̹Ƿ�, �̹� ������ ���� �������ٰ�(visited) ǥ���ؾߵ�->node�� visited ���� �߰�, ó���� 0���� �ʱ�ȭ
bool ofApp::DFS()//DFSŽ���� �ϴ� �Լ�
{
	//TO DO
	//DFSŽ���� �ϴ� �Լ� ( 3����)
	int size = HEIGHT * WIDTH;

	// Stack �ʱ�ȭ
	DFSans.top = -1;
	
	int i = 0;
	while (graph[i] != NULL) {
		graph[i]->visited = 0;
		i++;
	}

	node_pointer ptr = graph[0];//graph[n]�� vertax�� �ٷ� ������
	//ex) 0->1, 1->0->2->6, 2->1, 3->4->8�̸�
	//graph[1]->vertax=0, graph[1]->link->vertax=2, graph[1]->link->link->vertax=6..
	DFSans.arr[++DFSans.top] = 0;
	graph[0]->visited = 1;

	int index = 0;
	int roomnum = 0;
	while (roomnum<HEIGHT*WIDTH-1) {
		if (ptr != NULL) {//������ ���� ���
			if (graph[ptr->vertex]->visited == 0) {//�湮�ߴ� ���� �ƴϾ����
				DFSans.arr[++DFSans.top] = ptr->vertex;
				graph[ptr->vertex]->visited = 1;
				roomnum = ptr->vertex;
				ptr = graph[roomnum];
			}
			else {//�湮�ߴ� ���� ���, graph[]->link->link...��
				ptr = ptr->link;
			}
		}
		else {//���� ���, �� ���ÿ� ���� ��� ���� �� �������� ���ư�
			for (int i = 0; i <= DFSans.top; i++) {
				DFSsubroute[d_index].arr[i] = DFSans.arr[i];
			}
			//subroute[v_index]->arr[ans.top + 1] = ptr->vertax;
			DFSsubroute[d_index].top = DFSans.top--;
			d_index++;

			//����(top), �� ����(top-1),...�� ���� graph[����]->link�� �湮�Ǿ�����, �湮�Ǿ��ٸ� graph[����]->link->link�� �湮�Ǿ�����..
			//�� �湮�Ǿ��ٸ� graph[top-1]�� link���� �湮 �ȵ� �� ã�������� ����
			//ã���� �� ���� roomnum���� �ϰ� graph[roomnum]���� �ٽ� Ž�� ����
			
			ptr = graph[DFSans.arr[DFSans.top]];

			while (1) {
				if (graph[ptr->vertex]->visited == 0) {
					break;
				}
				if (ptr->link != NULL) {
					ptr = ptr->link;
				}
				else {
					ptr = graph[DFSans.arr[--DFSans.top]];
				}
			}
		}
	}
	DFSans.top--;
	//printStack(&ans);
	isdfs = 1;
	return 0;
}

///////////////////������
void ofApp::printStack(Stack* stack){
	for (int i = 0; i <= stack->top; i++) {
		printf("%d ", stack->arr[i]);
	}
	printf("\n");
}//////////////////////////////////

void ofApp::dfsdraw()
{
	//TO DO 
	//DFS�� ������ ����� �׸���. (3���� ����)
	row1 = 10 + SIZE / 2;
	col1 = 10 + SIZE / 2;
	row2 = row1;
	col2 = col1;
	int i = 0;
	while (i < DFSans.top) {
		if (abs(DFSans.arr[i + 1] - DFSans.arr[i]) != WIDTH) {//����
			col2 += SIZE * (DFSans.arr[i + 1] - DFSans.arr[i]);
		}
		else {//����
			row2 += SIZE * ((DFSans.arr[i + 1] - DFSans.arr[i]) / WIDTH);
		}
		ofDrawLine(col1, row1, col2, row2);
		row1 = row2;
		col1 = col2;
		i++;
	}
	if ((HEIGHT * WIDTH - 1) - DFSans.arr[DFSans.top] != WIDTH) {
		col2 += SIZE;
	}
	else {
		row2 += SIZE;
	}
	ofDrawLine(col1, row1, col2, row2);
}

void ofApp::dfssubdraw() {
	for (int k = 0; k < d_index; k++) {
		row1 = 10 + SIZE / 2;
		col1 = 10 + SIZE / 2;
		row2 = row1;
		col2 = col1;
		int i = 0;
		while (i < DFSsubroute[k].top) {
			if (abs(DFSsubroute[k].arr[i + 1] - DFSsubroute[k].arr[i]) != WIDTH) {//����
				col2 += SIZE * (DFSsubroute[k].arr[i + 1] - DFSsubroute[k].arr[i]);
			}
			else {//����
				row2 += SIZE * ((DFSsubroute[k].arr[i + 1] - DFSsubroute[k].arr[i]) / WIDTH);
			}
			ofDrawLine(col1, row1, col2, row2);
			row1 = row2;
			col1 = col2;
			i++;
		}
	}
}

void ofApp::enqueue(Queue* q, int n) {
	q->arr[q->rear++] = n;
}
int ofApp::dequeue(Queue* q) {
	return q->arr[q->front++];
}

void ofApp::BFS(){

	int dest = HEIGHT * WIDTH-1;//������
	//int* parent = (int*)malloc(dest * sizeof(int));//�� ���� �θ� �����ϴ� �迭
	int parent[5000];
	for (int i = 0; i < dest; ++i) {
		parent[i] = -1;
	}

	BFSans.front = 0;
	BFSans.rear = 0;

	Queue tmp;
	tmp.front = 0;
	tmp.rear = 0;

	int i = 0;
	while (graph[i] != NULL) {
		graph[i]->visited = 0;
		i++;
	}
	
	int current = 0;//���� ��(���)
	node_pointer ptr = graph[current];
	ptr->visited = 1;

	while (current != dest) {//��� �湮 �ݺ�, ����������
		int cnt = 0;
		while (1) {//�ش� ���� ������(link)�� �湮, null����
			if (graph[ptr->vertex]->visited == 0) {
				cnt++;
				enqueue(&tmp, ptr->vertex);
				parent[ptr->vertex] = current;
				graph[ptr->vertex]->visited = 1;
			}
			if (ptr->link != NULL) {
				ptr = ptr->link;
			}
			else {
				break;
			}
		}
		if (cnt == 0) {//���Ӱ� �߰��� �� �ϳ��� ����, �� ���� ����->��� ���� �ʿ�
			bfs2queue(parent, current);
		}
		/*printf("Queue contents: ");///////////////////////������
		for (int i = tmp.front; i < tmp.rear; i++) {
			printf("%d ", tmp.arr[i]);
		}
		printf("\n");*/

		//���� ��� �غ�
		current = dequeue(&tmp);
		ptr = graph[current];

	}

	BFSans.front = 0;
	BFSans.rear = 0;//�ʱ�ȭ

	while (dest != -1) {
		enqueue(&BFSans, dest);
		dest = parent[dest];
	}
	//free(parent);//�����ϸ� �������� �����Ҵ����� �ٲ�, �ֿ���?��׶��忡�� ��ο�� ��� ����Ǵµ� �޸� �����ع�����..?
}

void ofApp::bfs2queue(int parent[], int end) {

	BFSsubroute[b_index].front = 0;
	BFSsubroute[b_index].rear = 0;//�ʱ�ȭ

	while (end != -1) {
		enqueue(&BFSsubroute[b_index], end);
		end = parent[end];
	}
	b_index++;

}

void ofApp::bfsdraw()
{
	//TO DO 
	row1 = 10 + SIZE / 2;
	col1 = 10 + SIZE / 2;
	row2 = row1;
	col2 = col1;
	int i = BFSans.rear - 1;
	while (i > BFSans.front) {
		if (abs(BFSans.arr[i - 1] - BFSans.arr[i]) != WIDTH) {//����
			col2 += SIZE * (BFSans.arr[i - 1] - BFSans.arr[i]);
		}
		else {//����
			row2 += SIZE * ((BFSans.arr[i - 1] - BFSans.arr[i]) / WIDTH);
		}
		ofDrawLine(col1, row1, col2, row2);
		row1 = row2;
		col1 = col2;
		i--;
	}
}

void ofApp::bfssubdraw() {
	for (int k = 0; k < b_index; k++) {
		row1 = 10 + SIZE / 2;
		col1 = 10 + SIZE / 2;
		row2 = row1;
		col2 = col1;
		int i = BFSsubroute[k].rear - 1;
		while (i > BFSsubroute[k].front) {
			if (abs(BFSsubroute[k].arr[i - 1] - BFSsubroute[k].arr[i]) != WIDTH) {//����
				col2 += SIZE * (BFSsubroute[k].arr[i - 1] - BFSsubroute[k].arr[i]);
			}
			else {//����
				row2 += SIZE * ((BFSsubroute[k].arr[i - 1] - BFSsubroute[k].arr[i]) / WIDTH);
			}
			ofDrawLine(col1, row1, col2, row2);
			row1 = row2;
			col1 = col2;
			i--;
		}
	}
}