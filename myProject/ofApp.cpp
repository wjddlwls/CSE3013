
#include "ofApp.h"
#include <iostream>
#include <math.h>
using namespace std;

void addEdge(int start, int end) {

	node_pointer newnode = (node_pointer)malloc(sizeof(node));
	newnode->visited = 0;
	newnode->vertex = end;
	newnode->link = graph[start];//ó���� null, ���ĺ��ʹ� �����ֱ�
	graph[start] = newnode;//graph[start]�� newnode�� ����Ű�� ��(graph[n]�� ��带 ����Ű�� ������)

	newnode = (node_pointer)malloc(sizeof(node));
	newnode->vertex = start;
	newnode->link = graph[end];
	graph[end] = newnode;
}

void ofApp::setup() {//�ʱ� ����

	ofSetWindowTitle("Maze Example"); 
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isplay = false;
	isOpen = 0;
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	myFont.loadFont("verdana.ttf", 12, true, true);

	hWnd = WindowFromDC(wglGetCurrentDC());

	ofSetEscapeQuitsApp(false);

	menu = new ofxWinMenu(this, hWnd);

	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	HMENU hMenu = menu->CreateWindowMenu();

	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = false;  // screen info display on
	menu->AddPopupItem(hPopup, "Play Mode", false, true);
	bShowInfo = false;  // screen info display on////////////���� true
	menu->AddPopupItem(hPopup, "Show DFS", false, true); // Checked ///////���� f, f
	//ù��° false�� üũ���� ���� ���·� �����Ѵٴ°�, �ι�° true�� �ڵ����� üũ���°� ��������� ����, �� �׸��� ���õǸ� �ڵ����� üũ���� �����
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, true); // Not checked (default)
	//�Ⱦ��� �ڵ����� false, false��
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

	countdownTime = 0;    // Ÿ�̸� �ʱ�ȭ
	lastUpdateTime = 0.0; // ������ ������Ʈ �ð� �ʱ�ȭ
	startTime = 0.0;      // Ÿ�̸� ���� �ð� �ʱ�ȭ

} // end Setup


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
	}

	if(title == "Show BFS") {
		//doTopmost(bChecked); // Use the checked value directly//�ּ�ó�� ���ص� ��� �޶����°� ����. �׳� üũ�ǰ� �ϴ� �ڵ�? 
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		isbfs = bChecked;
		if (isOpen && isbfs)
		{
			BFS();
			//bShowInfo = bChecked;
		}
	}

	//////////////////////////////
	if (title == "Play Mode") {
		//doTopmost(bChecked);
		currentRoom = 0;
		playroute.top = 0;
		playroute.arr[playroute.top] = 0;//�ʱ�ȭ
		calcTime2escape();//draw���� ȣ���ϸ� ��� �� �Լ� ���� dfs()�� ���� ȣ��ż� �޸� ���������� �Ź� �ʱ�ȭ�ؾߵ�. �÷��̸�� �ϸ� �ѹ��� ȣ���ؼ� �� �����صα�. 
		isplay = bChecked;
		isSuccess = 0;
		isFail = 0;
	}
	//////////////////////////

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
void ofApp::update() {//�����Ӻ� ������Ʈ �ڵ�

	if (isplay) {
		float currentTime = ofGetElapsedTimef();
		if (startTime == 0.0) {
			// Ÿ�̸� ���� �� �ʱ�ȭ
			startTime = currentTime;
			countdownTime = time_limit; 
			lastUpdateTime = currentTime;
		}
		if (currentTime - lastUpdateTime >= 1.0) { // 1�ʰ� �������� Ȯ��
			if (countdownTime > 0) {
				countdownTime--; // Ÿ�̸Ӹ� 1�� ����
				lastUpdateTime = currentTime; // ������ ������Ʈ�� �ð� ����
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw() {//�̷��� �β�, ��, ��� �� �߰� ����

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(166, 166, 166);//grey
	ofSetLineWidth(7);
	int i, j;
	
	// TO DO : DRAW MAZE; ///////////////////////////////////////////////////////
	// ����� �ڷᱸ���� �̿��� �̷θ� �׸���.
	// add code here

	int startrow = 10;
	int startcol = 10;
	int endrow = startrow + HEIGHT * SIZE;
	int endcol = startcol + WIDTH * SIZE;

	if (isOpen) {//���� ���� ��
		ofDrawLine(startcol, startrow, endcol, startrow);// ��� ���μ�
		ofDrawLine(startcol, startrow + SIZE, startcol, endrow);// ���� ���μ�
		ofDrawLine(startcol, endrow, endcol, endrow);// �ϴ� ���μ�
		ofDrawLine(endcol, startrow, endcol, endrow - SIZE);// ���� ���μ�
		//ofDrawLine�Լ� : col, row ��, �� x, y������ �Է��ؾ���
	}

	ofSetLineWidth(5);
	int interval[4] = { -1, 1, -1 * WIDTH, WIDTH };
	for (i = 0; i < WIDTH * HEIGHT; i++) {
		for (j = 0; j < 4; j++) {
			node_pointer tmp = graph[i];
			int isEdge = 0;
			int nearVertax = i + interval[j];
			if (nearVertax >= 0 && nearVertax < WIDTH * HEIGHT) {
				while (tmp != NULL) {
					if (tmp->vertex == nearVertax) {
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
	
	if (isplay) {
		if (isOpen) {
			ofSetColor(0, 130, 153);//����
			ofSetLineWidth(5);
			playdraw();
			if (currentRoom == HEIGHT * WIDTH - 1) {
				isSuccess = 1;
				sprintf(str, "You Escaped!");
				myFont.drawString(str, 20, (HEIGHT + 1) * SIZE + 20);
			}
			else {
				if (countdownTime > 0) {
					ofSetLineWidth(1);
					ofDrawLine(10, (HEIGHT + 1) * SIZE, 10, (HEIGHT + 1) * SIZE + 30);
					ofDrawLine(10, (HEIGHT + 1) * SIZE, 210, (HEIGHT + 1) * SIZE);
					ofDrawLine(210, (HEIGHT + 1) * SIZE, 210, (HEIGHT + 1) * SIZE + 30);
					ofDrawLine(10, (HEIGHT + 1) * SIZE + 30, 210, (HEIGHT + 1) * SIZE + 30);
					sprintf(str, "Escape in %d seconds!", time_limit);
					myFont.drawString(str, 20, (HEIGHT + 1) * SIZE + 20);
					sprintf(str, "time remaining : %d seconds", countdownTime);
					myFont.drawString(str, 10, (HEIGHT + 1) * SIZE + 50);
				}
				else {
					ofSetColor(200, 0, 0);//red
					sprintf(str, "Time's up!!!");
					myFont.drawString(str, 20, (HEIGHT + 1) * SIZE + 20);
					isFail = 1;
				}
			}
		}
		else {
			cout << "You must open file first" << endl;
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
void ofApp::keyPressed(int key) {//Ű�Է� ó�� �ڵ�

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

	//Ű �Է¿� ���� �̵� ��� �߰� ����
	if (isplay && !isSuccess && !isFail) {
		int tmpRoom = 0;
		if (key == OF_KEY_RIGHT) {
			tmpRoom = currentRoom + 1;
		}
		if (key == OF_KEY_LEFT) {
			tmpRoom = currentRoom - 1;
		}
		if (key == OF_KEY_DOWN) {
			tmpRoom = currentRoom + WIDTH;
		}
		if (key == OF_KEY_UP) {
			tmpRoom = currentRoom - WIDTH;
		}
		if (isMovable(currentRoom, tmpRoom)) {
			targetRoom = tmpRoom;
			if (targetRoom == playroute.arr[playroute.top - 1]) {
				playroute.top--;
				currentRoom = targetRoom;
			}
			else {
				playroute.arr[++playroute.top] = targetRoom;
				currentRoom = targetRoom;
			}
		}
		//printStack(&playroute);
	}
}// end keyPressed

bool ofApp::isMovable(int startRoom, int endRoom){

	if (endRoom < 0 || endRoom > HEIGHT * WIDTH) {
		return 0;
	}
	node_pointer ptr = graph[startRoom];
	while (ptr != NULL) {
		if (ptr->vertex == endRoom) {
			return 1;
		}
		ptr = ptr->link;
	}
	return 0;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){//Ű ������ ó���ڵ�

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
			if (count != 0) {//���� �бⰡ ó���� �ƴ� ���(�ʱ�ȭ) ���õ� �ʱ�ȭ�ؾ���
				freeMemory();
				row1 = 0;
				row2 = 0;
				col1 = 0;
				col2 = 0;
				isdfs = 0;
				DFSans.top = -1;
				for (int j = 0; j < 100; j++) {
					DFSsubroute[j].top = -1;
				}
				isbfs = 0;
				BFSans.front = 0;
				for (int j = 0; j < 100; j++) {
					BFSsubroute[j].front = 0;
					BFSsubroute[j].rear = 0;
				}
				isplay = 0;
				currentRoom = 0;
				time_limit = 0;
				countdownTime = 0;
				lastUpdateTime = 0.0;
				startTime = 0.0;
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
}////////////////////////////////////////////////////////////////
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
	return 0;
}

///////////////////������
void ofApp::printStack(Stack* stack){
	for (int i = 0; i <= stack->top; i++) {
		printf("%d ", stack->arr[i]);
	}
	printf("\n");
}///////////////////

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
void ofApp::BFS() {

	int dest = HEIGHT * WIDTH - 1;//������
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

void ofApp::playdraw() {

	row1 = 10 + SIZE / 2;
	col1 = 10 + SIZE / 2;
	row2 = row1;
	col2 = col1;
	int i = 0;
	while (i < playroute.top) {
		if (abs(playroute.arr[i + 1] - playroute.arr[i]) != WIDTH) {//����
			col2 += SIZE * (playroute.arr[i + 1] - playroute.arr[i]);
		}
		else {//����
			row2 += SIZE * ((playroute.arr[i + 1] - playroute.arr[i]) / WIDTH);
		}
		ofDrawLine(col1, row1, col2, row2);
		row1 = row2;
		col1 = col2;
		i++;
	}
	ofDrawCircle(col1, row1, SIZE / 5);//������ġ ��Ÿ���� �� ǥ��
}

//����Ұ�
//1)�̷λ���� Ŭ����, 2)Ż���ΰ� ���̴� �κ��� ��������. �� Ż������ ����/ť�� ������ ������ ���� ����.
void ofApp::calcTime2escape() {

	time_limit = 1;
	time_limit *= sqrt(pow(HEIGHT, 2) + pow(WIDTH, 2));//�밢�� ���̶� ���

	DFS();
	time_limit += DFSans.top;//2�� ����Ѱ�����, 1�� ���� �߿� ����� ��. ����� Ŭ���� ������ ���ҵ� ���� �� �ۿ�..

}