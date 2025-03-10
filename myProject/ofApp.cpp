
#include "ofApp.h"
#include <iostream>
#include <math.h>
using namespace std;

void addEdge(int start, int end) {

	node_pointer newnode = (node_pointer)malloc(sizeof(node));
	newnode->visited = 0;
	newnode->vertex = end;
	newnode->link = graph[start];//처음에 null, 이후부터는 끼워넣기
	graph[start] = newnode;//graph[start]가 newnode를 가리키게 됨(graph[n]은 노드를 가리키는 포인터)

	newnode = (node_pointer)malloc(sizeof(node));
	newnode->vertex = start;
	newnode->link = graph[end];
	graph[end] = newnode;
}

void ofApp::setup() {//초기 설정

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
	bShowInfo = false;  // screen info display on////////////원래 true
	menu->AddPopupItem(hPopup, "Show DFS", false, true); // Checked ///////원래 f, f
	//첫번째 false는 체크되지 않은 상태로 시작한다는것, 두번째 true는 자동으로 체크상태가 변경될지의 여부, 즉 항목이 선택되면 자동으로 체크상태 변경됨
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, true); // Not checked (default)
	//안쓰면 자동으로 false, false로
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

	countdownTime = 0;    // 타이머 초기화
	lastUpdateTime = 0.0; // 마지막 업데이트 시간 초기화
	startTime = 0.0;      // 타이머 시작 시간 초기화

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
	//메뉴 선택시 체크 잘 되고 그에 따른 그림 실행도 잘 되도록 좀 수정함
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
		//doTopmost(bChecked); // Use the checked value directly//주석처리 안해도 결과 달라지는거 없음. 그냥 체크되게 하는 코드? 
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
		playroute.arr[playroute.top] = 0;//초기화
		calcTime2escape();//draw에서 호출하면 계속 이 함수 내의 dfs()도 같이 호출돼서 메모리 복잡해지고 매번 초기화해야됨. 플레이모드 하면 한번만 호출해서 값 저장해두기. 
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
void ofApp::update() {//프레임별 업데이트 코드

	if (isplay) {
		float currentTime = ofGetElapsedTimef();
		if (startTime == 0.0) {
			// 타이머 시작 시 초기화
			startTime = currentTime;
			countdownTime = time_limit; 
			lastUpdateTime = currentTime;
		}
		if (currentTime - lastUpdateTime >= 1.0) { // 1초가 지났는지 확인
			if (countdownTime > 0) {
				countdownTime--; // 타이머를 1초 감소
				lastUpdateTime = currentTime; // 마지막 업데이트된 시간 갱신
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw() {//미로의 두께, 색, 모양 등 추가 구현

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(166, 166, 166);//grey
	ofSetLineWidth(7);
	int i, j;
	
	// TO DO : DRAW MAZE; ///////////////////////////////////////////////////////
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here

	int startrow = 10;
	int startcol = 10;
	int endrow = startrow + HEIGHT * SIZE;
	int endcol = startcol + WIDTH * SIZE;

	if (isOpen) {//파일 오픈 전
		ofDrawLine(startcol, startrow, endcol, startrow);// 상단 가로선
		ofDrawLine(startcol, startrow + SIZE, startcol, endrow);// 좌측 세로선
		ofDrawLine(startcol, endrow, endcol, endrow);// 하단 가로선
		ofDrawLine(endcol, startrow, endcol, endrow - SIZE);// 우측 세로선
		//ofDrawLine함수 : col, row 순, 즉 x, y순으로 입력해야함
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
					continue;//선 안그리고 다음 j값으로 넘어감
				}
				if (j == 0 || j == 1) {//둘이 같은 줄인 겨우, 세로벽 그리기
					row1 = startrow + SIZE * (i / WIDTH);
					row2 = startrow + SIZE * (i / WIDTH + 1);
					col1 = startcol + SIZE * ((i % WIDTH+j));
					ofDrawLine(col1, row1, col1, row2);
				}
				else if (j == 2 || j == 3) {//둘이 다른 줄인 경우, 가로벽 그리기
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
			ofSetColor(0, 130, 153);//남색
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
void ofApp::keyPressed(int key) {//키입력 처리 코드

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

	//키 입력에 따른 이동 기능 추가 구현
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
void ofApp::keyReleased(int key){//키 릴리스 처리코드

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
//파일 읽어서 적절한 형태로 설계한 자료구조에 저장하는 함수
//2주차 구현
bool ofApp::readFile()
{
	static int count = 0;//이 함수가 다시 호출되어도 프로그램이 실행되는 동안은 다시 0으로 초기화되지 않음
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

		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {//파일 읽기 성공 시
	
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

			if (it != end) {//맨 윗줄, 미로의 최상단은 패스
				++it;
			}
			
			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			//line by line으로 순회, 안에서 방과 벽이 한줄씩 반복되므로 
			// 방을 읽어들이면 세로벽 확인해 엣지 추가하면서 오른쪽으로 이동
			// (가로)벽을 읽어들이면 엣지 추가하면서 오른쪽으로 ㄱㄱ
			// 순서대로 0부터 넘버링
			//사이에 벽이 없는 경우 addEdge 호출
			if (count != 0) {//파일 읽기가 처음이 아닌 경우(초기화) 스택도 초기화해야함
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
			for (; it != end; ++it) {//두번째 줄부터 마지막 줄까지 순회
				line = *it;
				if (line[0] == '|') {//방인 경우... |  |  |    | ......
					for (i = 2; i < line.length()-1; i += 2) {//짝수인덱스만 확인하면 됨, 마지막에 미로의 테두리는 확인안하도록 -1
						if (line[i] != '|') {
							addEdge(cnt * WIDTH + i/2-1, cnt * WIDTH + i/2);
						}
					}
					cnt++;
				}
				else {//벽인 경우... +-+  +-+- .....
					for (i = 1; i < line.length()-1; i += 2) {
						if (line[i] != '-') {
							addEdge((cnt -1)* WIDTH + i/2, cnt * WIDTH + i/2);
						}
					}
				}
			}
			HEIGHT = cnt;
			//printGraph();///////////////////////////////////////디버깅용
			//printf("height: %d width: %d\n", HEIGHT, WIDTH);//////////////////////////////////
			count++;//한번 파일 열면 다음 실행시에 초기화되어야 하므로 변수 업데이트
			return true;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
///////////////////////////////////////////////////////////디버깅용
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
	// malloc한 memory를 free해주는 함수
	//이 함수 내부에서 자신이 만든 자료구조에 할당한 메모리들을 해제 해 주어야만, 여러 파일을 불러올 때 메모리 관련 에러들이 발생하지 않는다. 메모리 유출 주의
	for (int i = 0; i < MAX_VERTICES; ++i) {
		node_pointer current = graph[i];
		while (current != NULL) {
			node_pointer toDelete = current;
			current = current->link;
			free(toDelete); // 할당된 메모리 해제
		}
		graph[i] = NULL; // 그래프 배열의 해당 항목을 NULL로 설정
	}
}

//1,1에서 시작해서 쭉 가면서 결과스택에 순서대로 저장, 
//막히면 새로운 스택만들어서 결과 스택에 있던거 복사하고,  현재꺼 pop해서 새 스택에 넣은 후 직전으로 돌아감
// ->직전 위치는 결과 스택의 탑 보면 됨
//무방향 그래프이므로, 이미 지나간 방은 지나갔다고(visited) 표시해야됨->node에 visited 변수 추가, 처음엔 0으로 초기화
bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	int size = HEIGHT * WIDTH;

	// Stack 초기화
	DFSans.top = -1;
	
	int i = 0;
	while (graph[i] != NULL) {
		graph[i]->visited = 0;
		i++;
	}

	node_pointer ptr = graph[0];//graph[n]의 vertax가 바로 다음거
	//ex) 0->1, 1->0->2->6, 2->1, 3->4->8이면
	//graph[1]->vertax=0, graph[1]->link->vertax=2, graph[1]->link->link->vertax=6..
	DFSans.arr[++DFSans.top] = 0;
	graph[0]->visited = 1;

	int index = 0;
	int roomnum = 0;
	while (roomnum<HEIGHT*WIDTH-1) {
		if (ptr != NULL) {//막히지 않은 경우
			if (graph[ptr->vertex]->visited == 0) {//방문했던 방이 아니어야함
				DFSans.arr[++DFSans.top] = ptr->vertex;
				graph[ptr->vertex]->visited = 1;
				roomnum = ptr->vertex;
				ptr = graph[roomnum];
			}
			else {//방문했던 방인 경우, graph[]->link->link...로
				ptr = ptr->link;
			}
		}
		else {//막힌 경우, 새 스택에 서브 경로 저장 후 직전으로 돌아감
			for (int i = 0; i <= DFSans.top; i++) {
				DFSsubroute[d_index].arr[i] = DFSans.arr[i];
			}
			//subroute[v_index]->arr[ans.top + 1] = ptr->vertax;
			DFSsubroute[d_index].top = DFSans.top--;
			d_index++;

			//직전(top), 그 직전(top-1),...로 가서 graph[직전]->link가 방문되었는지, 방문되었다면 graph[직전]->link->link가 방문되었는지..
			//다 방문되었다면 graph[top-1]의 link돌며 방문 안된 곳 찾을때까지 ㄱㄱ
			//찾으면 그 방을 roomnum으로 하고 graph[roomnum]에서 다시 탐색 시작
			
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

///////////////////디버깅용
void ofApp::printStack(Stack* stack){
	for (int i = 0; i <= stack->top; i++) {
		printf("%d ", stack->arr[i]);
	}
	printf("\n");
}///////////////////

void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)
	row1 = 10 + SIZE / 2;
	col1 = 10 + SIZE / 2;
	row2 = row1;
	col2 = col1;
	int i = 0;
	while (i < DFSans.top) {
		if (abs(DFSans.arr[i + 1] - DFSans.arr[i]) != WIDTH) {//가로
			col2 += SIZE * (DFSans.arr[i + 1] - DFSans.arr[i]);
		}
		else {//세로
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
			if (abs(DFSsubroute[k].arr[i + 1] - DFSsubroute[k].arr[i]) != WIDTH) {//가로
				col2 += SIZE * (DFSsubroute[k].arr[i + 1] - DFSsubroute[k].arr[i]);
			}
			else {//세로
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

	int dest = HEIGHT * WIDTH - 1;//목적지
	//int* parent = (int*)malloc(dest * sizeof(int));//각 방의 부모 저장하는 배열
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

	int current = 0;//현재 방(노드)
	node_pointer ptr = graph[current];
	ptr->visited = 1;

	while (current != dest) {//방들 방문 반복, 목적지까지
		int cnt = 0;
		while (1) {//해당 방의 인접방(link)들 방문, null까지
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
		if (cnt == 0) {//새롭게 추가된 방 하나도 없음, 즉 길이 막힘->경로 저장 필요
			bfs2queue(parent, current);
		}

		//다음 노드 준비
		current = dequeue(&tmp);
		ptr = graph[current];

	}

	BFSans.front = 0;
	BFSans.rear = 0;//초기화

	while (dest != -1) {
		enqueue(&BFSans, dest);
		dest = parent[dest];
	}
	//free(parent);//프리하면 오류나서 정적할당으로 바꿈, 왜오류?백그라운드에서 드로우는 계속 실행되는데 메모리 해제해버랴서..?
}

void ofApp::bfs2queue(int parent[], int end) {

	BFSsubroute[b_index].front = 0;
	BFSsubroute[b_index].rear = 0;//초기화

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
		if (abs(BFSans.arr[i - 1] - BFSans.arr[i]) != WIDTH) {//가로
			col2 += SIZE * (BFSans.arr[i - 1] - BFSans.arr[i]);
		}
		else {//세로
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
			if (abs(BFSsubroute[k].arr[i - 1] - BFSsubroute[k].arr[i]) != WIDTH) {//가로
				col2 += SIZE * (BFSsubroute[k].arr[i - 1] - BFSsubroute[k].arr[i]);
			}
			else {//세로
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
		if (abs(playroute.arr[i + 1] - playroute.arr[i]) != WIDTH) {//가로
			col2 += SIZE * (playroute.arr[i + 1] - playroute.arr[i]);
		}
		else {//세로
			row2 += SIZE * ((playroute.arr[i + 1] - playroute.arr[i]) / WIDTH);
		}
		ofDrawLine(col1, row1, col2, row2);
		row1 = row2;
		col1 = col2;
		i++;
	}
	ofDrawCircle(col1, row1, SIZE / 5);//현재위치 나타내는 원 표시
}

//고려할것
//1)미로사이즈가 클수록, 2)탈출경로가 꺾이는 부분이 많을수록. 즉 탈출경로의 스택/큐의 원소의 개수가 많을 수록.
void ofApp::calcTime2escape() {

	time_limit = 1;
	time_limit *= sqrt(pow(HEIGHT, 2) + pow(WIDTH, 2));//대각선 길이랑 비례

	DFS();
	time_limit += DFSans.top;//2를 고려한거지만, 1도 은연 중에 고려된 것. 사이즈가 클수록 스택의 원소도 많을 수 밖에..

}