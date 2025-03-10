#include "tetris.h"

static struct sigaction act, oact;
int B,count;

typedef struct _Node {
	char name[NAMELEN+1];
	int score;
	struct _Node *next;
} Node;
Node *head = NULL;
int rank_cnt;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		createRankList();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMMEND: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i=0;i<BLOCK_NUM;i++){
		nextBlock[i]=rand()%7;
	}
	blockRotate=0;
	blockY=0;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	
	recRoot = (struct RecNode*)malloc(sizeof(struct RecNode));
	recRoot->accumulatedScore=0;
	recRoot->lv=0;//현재블록
	recRoot->curBlockID=nextBlock[recRoot->lv];
	recRoot->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
	for (int i = 0; i < CHILDREN_MAX; i++) {
        recRoot->child[i] = NULL; // 자식 노드 배열 초기화
    }
	for(int j=0;j<HEIGHT;j++){
		for(int i=0;i<WIDTH;i++){
			recRoot->recField[j][i]=field[j][i];//기본 필드 상태를 복사함
		}
	}
	int t=modified_recommend(recRoot);//트리 생성됨

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 테두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	move(2,WIDTH+10);
	printw("NEXT BLOCK");

	// next block을 보여주는 공간의 테두리를 그린다.
	for(int x=0;x<BLOCK_NUM-1;x++){
		DrawBox(3+6*x,WIDTH+10,4,8);
	}
	move(3+6*(BLOCK_NUM-1),WIDTH+10);
	printw("SCORE");

	// score를 보여주는 공간의 태두리를 그린다.
	DrawBox(3+6*(BLOCK_NUM-1)+1,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/*space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(3+6*(BLOCK_NUM-1)+2,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;

	for(int x=0;x<BLOCK_NUM-1;x++){
		for(i=0;i<4;i++){
			move(4+6*x+i,WIDTH+13);
			for(j=0;j<4;j++){
				if(block[nextBlock[x+1]][0][i][j]==1){//회전 0으로
					attron(A_REVERSE);//색 반전
					printw(" ");
					attroff(A_REVERSE);
				}
				else printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);

}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}


/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){

	int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (block[currentBlock][blockRotate][i][j] != 0) { // 블럭 모양에 따라.. 블럭이 깔릴 좌표는 1로 표시됨
                // 필드의 배열 경계를 벗어나는지 확인
                if (blockY + i < 0 || blockY + i >= HEIGHT || blockX + j < 0 || blockX + j >= WIDTH) {//blockX + j > WIDTH?
                    return 0;
                }
                // 필드에 블럭이 이미 있는지 체크
                if (f[blockY + i][blockX + j] != 0) {
                    return 0;
                }
            }
        }
    }
    return 1;
	
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
//이전그림자정보 찾고 지우기, 이전 블록정보 찾고지우기, 그림자와 블럭 새 좌표 찾기, 그림자랑 블럭 새로 그리기

	//1. 이전 블록 정보(x,y)를 찾는다. ProcessCommand의 switch문을 참조할 것
	int i,j;
	int blk=currentBlock, rot=blockRotate, y=blockY, x=blockX;
	int oldShadowY;
	int max=0;

	switch(command){
		case KEY_UP:
			rot=(rot+3)%4;
			break;
        case KEY_DOWN:
			y--;
            break;
        case KEY_LEFT:
			x++;
            break;
        case KEY_RIGHT:
			x--;
            break;
		default:
			break;
}

	//1-2. 이전 그림자 정보 찾기

	int yy=y;
	while(CheckToMove(f, blk, rot, yy, x)==1){
		yy++;
	}
	yy--;

	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[blk][rot][i][j]==1){
				if(i+y>=0){
					move(i+y+1, j+x+1);
					printw("%c", '.');
				}
			}
		}
	}

	//2-2. 이전 그림자 정보 지우기
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[blk][rot][i][j]==1){
				if(i+yy>=0){
					move(i+yy+1, j+x+1);
					printw("%c", '.');
				}
			}
		}
	}

	//3. 새로운 블록 정보를 그린다. +그림자
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){
	// user code

	//강의자료 p26-27의 플로우차트를 참고한다.
	int drawFlag=0;
	int i;
	if((drawFlag=CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX))){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);

	}
	else{
		if(blockY==-1){
			gameOver=1;
		}//블럭이 더 내려갈 수 없고 새 블럭도 놓을 수 없는 경우
		//블럭이 더 내려갈 수 없고 새 블록을 추가할 수 있는 경우
		for(int i=0;i<4;i++){//이전 추천 블록 지우기
			for(int j=0;j<4;j++){
				if(block[nextBlock[recRoot->lv]][recommendR][i][j]==1){
					if(i+recommendY>=0){
						move(i+recommendY+1, j+recommendX+1);
						printw("%c", '.');
					}
				}
			}
		}
		score+=AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);//블럭이 더 내려갈 수 없고 새 블록을 추가할 수 있는 경우
		score+=DeleteLine(field);
		blockY=-1;
		blockX=(WIDTH/2)-2;
		blockRotate=0;
		for(i=0;i<BLOCK_NUM-1;++i){
			nextBlock[i]=nextBlock[i+1];
		}
		nextBlock[BLOCK_NUM-1]=rand()%7;

		recRoot = (struct RecNode*)malloc(sizeof(struct RecNode));
		recRoot->accumulatedScore=score;
		recRoot->lv=0;
		recRoot->curBlockID=nextBlock[recRoot->lv];////////////////////////////////////////////////////////////////
		recRoot->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
		for (int i = 0; i < CHILDREN_MAX; i++) {
    	    recRoot->child[i] = NULL; // 자식 노드 배열 초기화
    	}
		for(int j=0;j<HEIGHT;j++){
			for(int i=0;i<WIDTH;i++){
				recRoot->recField[j][i]=field[j][i];//기본 필드 상태를 복사함
			}
		}
		int index=modified_recommend(recRoot);//트리 생성됨
		/*for (int i = 0; i < CHILDREN_MAX; i++) {
    	    if (recRoot->child[i] != NULL) {
    	        free(recRoot->child[i]);
    	    }
    	}
    	free(recRoot->child);
    	free(recRoot);*/

		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[recRoot->lv], blockRotate);
	}
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	//점수 계산 후 필드값 바꿔야
	int i,j;

	//1주차 과제 점수 계산 부분 추가, int형 함수로 바꿔줌
	int touched=0;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if((blockY+i+1)==HEIGHT){//블럭과 맞닿는 면이 필드 바닥인 경우
					touched+=1;
				}
				else{//블럭이 쌓여있던 블럭과 맞닿는 경우
					if(f[blockY+i+1][blockX+j]==1){
						touched+=1;
					}
				}
			}
		}
	}

	//Block이 추가된 영역의 필드값을 바꾼다.

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				f[blockY+i][blockX+j]=1;
			}
		}
	}

	return touched*10;


}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	int i,j,k;
	int fillFlag;
	int count=0;
	for(j=1;j<HEIGHT;j++){
		fillFlag=1;
		for(i=0;i<WIDTH;i++){
			if(f[j][i]!=1){//한번이라도 안 차 있으면 즉시 종료
				fillFlag=0;
				break;
			}
		}
		if(fillFlag==1){
			count++;
			for(int y=j;y>=1;y--){
				for(int x=0;x<WIDTH;x++){
                    f[y][x]=f[y-1][x];//한칸씩 내리기
             	}
			}
			for(int x=0;x<WIDTH;x++){
				f[0][x]=0;//한줄씩 내려가서 맨 윗줄의 값 사라졌으므로 0으로 초기화
			}

		}
	
	}
	return 100*count*count;
}


///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	//현재 블럭 좌표과 x값은 같게, y값은 +해서 for문으로 찾기(점점 늘리다가 그 좌표의 field가 1인 순간 직전) 

	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",'/');
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, ' ');

	int yy=y;

	while(CheckToMove(field, blockID, blockRotate, yy, x)==1){
		yy++;
	}
	yy--;

	DrawShadow(yy, x, blockID, blockRotate);
	DrawRecommend(recommendY, recommendX, blockID, recommendR);
	//메모리해제
}


void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	char username[NAMELEN];
	int i;
	rank_cnt=0;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	fscanf(fp, "%d", &rank_cnt);

	// 2. 정보읽어오기
	Node* iter = NULL;

    while(fscanf(fp, "%s %d", username, &i) != EOF){
        Node *newnode=(Node*)malloc(sizeof(Node));
        strcpy(newnode->name, username);
        newnode->score=i;
        newnode->next=NULL;
		if(iter==NULL){
            head=newnode;
            iter=head;
        }
        else{
            iter->next=newnode;
            iter=newnode;
        }
		iter->next=NULL;
    }
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=rank_cnt, ch, i, j;
	clear();
	Node* temp;

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		printw("X: ");
		echo();
        	scanw("%d", &X);
        	printw("Y: ");
        	scanw("%d", &Y);
		noecho();

		printw("		name		|		score		\n");
		printw("------------------------------------------------------\n");
		if(X>Y||rank_cnt==0||X>rank_cnt||Y>rank_cnt||X<=0||Y<=0){
			mvprintw(8,0,"search failure: no rank in the list\n");
		}
		else{
			temp = head;
			for(int i=0;i<X-1;i++){
				temp=temp->next;
			}
			for(i=X-1;i<Y;i++){
				printw("		%s		|		%d		\n", temp->name, temp->score);
				temp=temp->next;
			}
		}
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		printw("input the name: ");
		echo();
		scanw("%s", &str);
		noecho();
		printw("		name		|		score		\n");
		printw("------------------------------------------------------\n");
		temp = head;
		if(rank_cnt==0){
			printw("\nsearch failure: no name in the list\n");
			getch();
			return;
		}
		while(temp!=NULL){
			if(!strcmp(temp->name, str)){
				check++;
				printw("		%s		|		%d		\n", temp->name, temp->score);
			}
			temp=temp->next;
		}
		if(check==0)
			printw("\nsearch failure: no name in the list\n");

	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num;

		printw("input the rank: ");
		echo();
		scanw("%d", &num);
		noecho();

		if(num>rank_cnt){
			printw("\nsearch failure: the rank not in the list\n");
		}
		else{
			temp=head;
			if(num==1){
				//맨처음 원소 삭제
				head=head->next;
				free(temp);
			}
			else{
				Node* prev=head;
				for(int i=1;i<num-1;i++){
					prev=prev->next;
				}
				temp=prev->next;
				prev->next=temp->next;
				free(temp);
			}
			rank_cnt--;
			printw("\nresult: the rank deleted\n");
			writeRankFile();
		}
	}
	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "wt");
	Node *x=head;

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	if(rank_cnt!=0){
		fprintf(fp, "%d\n", rank_cnt);
	}
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	if(!rank_cnt){
		fclose(fp);
		return;
	}
	while(x!=NULL){
		fprintf(fp, "%s %d\n", x->name, x->score);
		x=x->next;
	}
	fclose(fp);
	return;
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	clear();

	//1. 사용자 이름을 입력받음
	printw("your name: ");
	echo();
	scanw("%s", &str);
	noecho();

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가

	//Node* p;
	Node *p=(Node*)malloc(sizeof(Node));
	strcpy(p->name, str);
	p->score=score;
	p->next=NULL;
	
//3 삽입
	if(rank_cnt==0) head=p;//리스트가 비어있는경우
	else if(score>head->score){//가장 큰 값을 삽입하는 경우
		p->next=head;
		head=p;
	}
	else{
		Node* prev=head;
		Node* iter=prev->next;
		while(iter!=NULL&&(iter->score)>(p->score)){
			prev=iter;
			iter=iter->next;
		}
		prev->next=p;
		p->next=iter;
	}
	rank_cnt++;
//4
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate){
	// user code
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",'R');
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}
//next블록은 나중에, 현재 블록만 고려해서(재귀x) 먼저 구현한다음 next블록 추가하기(재귀써야함)

//그림자 구현할때 방식으로 놓일 위치 찾기
//8-9개의 놓일 위치마다 노드 하나씩 생성해야함
//x좌표를 0부터 10까지 늘려가며 checktomove로 놓을 수 있으면 점수 계산해서 노드 생성 및 값 초기화
//블록아이디는 nextBlock[0], [1], [2]...
//rotation 0부터 3까지 반복해야함.
//recommand 함수에서는 트리 생성 후 최대인 경우 찾아서 최대점수 max에 저장, max반환
//drawblockwithfeatures에서 자식노드들 중 점수가 max인거 찾아서 그때의 좌표로 drawrecommand 호출??
//index 반환해야하지않나?

//level=첫번째0,두번째1, 세번째2
//[0] 현재블럭, [1] 다음블럭, [2]다다음블럭
/*char fieldOri[HEIGHT][WIDTH],int lv*/
int recommend(RecNode *root){
	
	//char this_field[HEIGHT][WIDTH];

	int rotation=0;//회전가능횟수
	int max=0;
	int index=0;//자식 노드 개수

	if (root->lv >= BLOCK_NUM) {
        return root->accumulatedScore;
    }

	for(rotation=0;rotation<4;rotation++){//가능한 모든 회전 경우에 대해
		//int blockX=WIDTH/2-2;//초기x좌표
		for(int blockX=0;blockX<WIDTH;blockX++){//x좌표가 0일때부터 10일때까지
			int blockY=-1;//초기 y좌표
			int tempscore=root->accumulatedScore;//현재 누적 점수
			if(CheckToMove(root->recField, nextBlock[root->lv], rotation, blockY, blockX)==1){//0이나 10에서 애초에 놓일 수 없으면 배제
				while(CheckToMove(root->recField, nextBlock[root->lv], rotation, blockY, blockX)==1){//y좌표 설정하는 부분
					blockY++;
				}
				blockY--;//놓일 수 있는 위치로 blockY, blockX의 값 설정해줌
				
				//블럭 놓고 점수계산해서 노드 만들기
				RecNode* newnode=(RecNode*)malloc(sizeof(RecNode));
				newnode->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
				for (int i = 0; i < CHILDREN_MAX; i++) {
    			    newnode->child[i] = NULL; // 자식 노드 배열 초기화
    			}
				for(int j=0;j<HEIGHT;j++){
					for(int i=0;i<WIDTH;i++){
						newnode->recField[j][i]=root->recField[j][i];//필드 상태 복사함
					}
				}
				tempscore+=AddBlockToField(newnode->recField, nextBlock[root->lv], rotation, blockY, blockX);//임시 필드에 블록이 놓인 상태로 필드값 바꿔줌
				tempscore+=DeleteLine(newnode->recField); // 점수 계산
				
				newnode->accumulatedScore=tempscore;
				newnode->recBlockY=blockY;
				newnode->recBlockX=blockX;
				newnode->recBlockRotate=rotation;
				newnode->lv = root->lv + 1;
				newnode->curBlockID=nextBlock[root->lv];

				root->child[index++]=newnode;

				int childScore = recommend(newnode);
                if (max<childScore) {
                    max=childScore;
                    if (root->lv == 0) {
						recommendY=blockY;
						recommendX=blockX;
						recommendR=rotation;
					}
				}
			}
		}
	}
	return max;
	//return index;

}


//과제 modified_recommend() 함수 구현
//레벨1의 노드중에서 점수가 높은 것 한개만 남겨서 자식 노드 만들기
//재귀 호출 전에 최고점인 노드 찾아야함
//애초에 노드 만들고 max보다 큰 경우만 남기기, 작으면 바로 해제(만들어야 점수계산 할수 잇나???)
//max보다 크면 만들고 기존의 max에 해당하는 노드 메모리 해제
//맨끝 블럭 잘 계산 안됨. 맨끝일때 쳌투무브 좀 이상한듯->해결
//맨 처음도 잘 계산 안됨.. ->blockX -1부터 시작하게 해서 해결
//맨 처음에 블럭 공중에 떠다니는 문제 해결 :어케햇더라/..


//수정
//자식 노드 세개만 남기기
//한 줄이 꽉 차서 클리어될 가능성을 1순위로 -> deleteline*100000
//쌓인 후의 최종 높이가 낮게 배치될 수 있을 가능성을 그 다음으로,
//필드에 구멍이 생기지 않을 가능성을 그 다음으로 높게 평가(구현x)
#define CHILD_NUM 3 //만들 자식 노드 개수
int modified_recommend(RecNode *root){
	
	int rotation;//회전가능횟수
	int index=0;//자식 노드 인덱스용으로 쓸거
	int max[CHILD_NUM]={0};
	int Max=0;
	char tmpfield[HEIGHT][WIDTH];

	if (root->lv >= BLOCK_NUM) {
        return root->accumulatedScore;
    }


	for(rotation=0;rotation<4;rotation++){//가능한 모든 회전 경우에 대해
		//int blockX=WIDTH/2-2;//초기x좌표
		for(int blockX=-1;blockX<WIDTH;blockX++){//x좌표가 0일때부터 10일때까지
			int blockY=0;//초기 y좌표, -1 or 0 ???
			int tempscore=root->accumulatedScore;//현재 누적 점수
			if(CheckToMove(root->recField, root->curBlockID, rotation, blockY, blockX)==1){//0이나 10에서 애초에 놓일 수 없으면 배제
				while(CheckToMove(root->recField, root->curBlockID, rotation, blockY, blockX)==1){//y좌표 설정하는 부분
					blockY++;
				}
				blockY--;//놓일 수 있는 위치로 blockY, blockX의 값 설정해줌
				
				//임시 필드에 현재 필드 상태 복사(블럭 안놓인 root의 필드 상태로)
				for(int j=0;j<HEIGHT;j++){
					for(int i=0;i<WIDTH;i++){
						tmpfield[j][i]=root->recField[j][i];//필드 상태 복사함
					}
				}
				tempscore+=AddBlockToField(tmpfield, root->curBlockID, rotation, blockY, blockX);//임시 필드에 블록이 놓인 상태로 필드값 바꿔줌
				tempscore+=10*DeleteLine(tmpfield); // 점수 계산
				tempscore+=5*blockY; //blockY 클수록, 즉 높이가 낮을수록 좋음

				for(int c=0;c<CHILD_NUM;c++){//제일 큰거 세개만 남기도록 세개중에서 현재 케이스의 점수보다 더 낮은거 있으면 교체 후 포문 탈출 시키기(중복으로 바뀌는 것 방지)
					if(max[c]<tempscore){//max의 0,1,2번째랑 root->child의 0,1,2번째가 같아짐
						max[c]=tempscore;

						//새 노드 생성
						RecNode* newnode=(RecNode*)malloc(sizeof(RecNode));
						newnode->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
						for (int i = 0; i < CHILDREN_MAX; i++) {
    	    				newnode->child[i] = NULL; // 자식 노드 배열 초기화
    					}
						for(int j=0;j<HEIGHT;j++){
							for(int i=0;i<WIDTH;i++){
								newnode->recField[j][i]=tmpfield[j][i];//필드 상태 복사함
							}
						}
						newnode->accumulatedScore=tempscore;
						newnode->recBlockY=blockY;
						newnode->recBlockX=blockX;
						newnode->recBlockRotate=rotation;
						newnode->lv = root->lv + 1;
						newnode->curBlockID=nextBlock[newnode->lv];//drawrecom호출할때 그냥 0으로 바꿧더니 잘됨, 여기서 값 제대로 안바뀌는 오류 나는듯

						if (root->child[c] != NULL) {
            				free(root->child[c]);
        				}
						root->child[c]=newnode;

						//재귀 호출
						int childscore=modified_recommend(newnode);
						if(Max<=childscore){
							Max=childscore;
							if(root->lv==0){
								recommendY=newnode->recBlockY;
								recommendX=newnode->recBlockX;
								recommendR=newnode->recBlockRotate;
							}
						}
						break;//여기에 하면 이미 블럭있는 곳이나 라인 벗어나게 블럭 추천해줌 : 필드 반영 오류? 아니면 블럭넘 오류?
					}
				}

				/*if(max<tempscore){//기존 맥스 점수보다 크면 노드 생성 후 기존의 맥스 노드 메모리 해제, max보다 작으면 이프문 안돌고 바로 패스
					max=tempscore;
					RecNode* newnode=(RecNode*)malloc(sizeof(RecNode));
					newnode->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
					for (int i = 0; i < CHILDREN_MAX; i++) {
    	    			newnode->child[i] = NULL; // 자식 노드 배열 초기화
    				}
					for(int j=0;j<HEIGHT;j++){
						for(int i=0;i<WIDTH;i++){
							newnode->recField[j][i]=tmpfield[j][i];//필드 상태 복사함
						}
					}
					newnode->accumulatedScore=tempscore;
					newnode->recBlockY=blockY;
					newnode->recBlockX=blockX;
					newnode->recBlockRotate=rotation;
					newnode->lv = root->lv + 1;
					newnode->curBlockID=nextBlock[newnode->lv];//drawrecom호출할때 그냥 0으로 바꿧더니 잘됨, 여기서 값 제대로 안바뀌는 오류 나는듯
					recommendY=newnode->recBlockY;
					recommendX=newnode->recBlockX;
					recommendR=newnode->recBlockRotate;
					
					if(index>0){//첫번째 경우가 맥스의 경우가 되는 경우 해제할 이전 메모리가 없음
						//index-=1;
						free(root->child[--index]);
					}
					root->child[index++]=newnode;
				}//인덱스는 1인 상태로 끝남(노드가 한개(max의 case)만 생기기 때문)*/
			}
		}
	}
	return Max;
}
//블록다운 대신 다른 핸들러 함수 만들어야
//블럭과 추천 위치가 띄워짐 -> 추천위치에 블럭을 놓음 -> 새로운 블럭과 해당 불럭의 추천 위치 뜨고 반복
void recommendedPlay(){
	clear();
	act.sa_handler = AutoBlock;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();//입력받는 부분 없음, 첫 블럭의 추천 위치랑 그림자 그림 
	do{
		if(timed_out==0){
			alarm(3);//3초
			timed_out=1;
		}
		if(GetCommand()==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);

}
//블록다운이 하는 일 : 한칸 내릴 수 있으면 drawflag 1로 설정 후 drawchande 호출해서 내리기
//내릴 수 없으면 새 블록 추가할 수 없으면 게임오버 1로 변경
//새 블록 추가할 수 있으면 기존 블록 필드에 반영하고 점수 업데이트 후 새 블록으로 초기화 후 
//DrawNextBlock(nextBlock), PrintScore(score), DrawField(), DrawBlockWithFeatures(blockY, blockX, nextBlock[recRoot->lv], blockRotate);

//오토블럭이 할 일 : 추천 위치 계산 한 후 해당 위치에 그린 후 점수 업데이트, 새 블록으로 초기화 , 옮길 수 없으면 게임오버
void AutoBlock(int sig) {

    int drawflag=0;

    if (drawflag=CheckToMove(field, nextBlock[0], recommendR, recommendY, recommendX)) {
        // 블록을 새로운 위치로 이동
		score += AddBlockToField(field, nextBlock[0], recommendR, recommendY, recommendX);
        score += DeleteLine(field);
		blockY = -1;
        blockX = (WIDTH / 2) - 2;
        blockRotate = 0;
        for (int i = 0; i < BLOCK_NUM - 1; ++i) {
            nextBlock[i] = nextBlock[i + 1];
        }
        nextBlock[BLOCK_NUM-1] = rand() % 7;

		//추천 블록 좌표 계산하도록 recommend 호출
		recRoot = (struct RecNode*)malloc(sizeof(struct RecNode));
		for (int j = 0; j < HEIGHT; j++) {
        	for (int i = 0; i < WIDTH; i++) {
            	recRoot->recField[j][i] = field[j][i]; // 필드 상태를 복사
        	}
    	}
    	recRoot->accumulatedScore = score;
    	recRoot->lv = 0;
    	recRoot->curBlockID = nextBlock[recRoot->lv];
    	recRoot->child = (RecNode **)malloc(CHILDREN_MAX * sizeof(RecNode *));
    	for (int i = 0; i < CHILDREN_MAX; i++) {
    	    recRoot->child[i] = NULL; // 자식 노드 배열 초기화
    	}
		//까지 recRoot 초기화 후 호출
		int t = modified_recommend(recRoot);
		DrawNextBlock(nextBlock);
        PrintScore(score);
        DrawField();
		DrawBlock(blockY, blockX, nextBlock[recRoot->lv], blockRotate, ' '); //떠있는 블럭
		DrawRecommend(recommendY, recommendX, nextBlock[recRoot->lv], recommendR);//추천 블럭
    }
	else {
        // 이동 실패 시 (즉, 블록을 더 이상 내릴 수 없을 때)
		gameOver=1;
    }
	timed_out=0;
}
