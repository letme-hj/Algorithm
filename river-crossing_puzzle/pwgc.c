#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

static void print_statename(FILE *fp, int state);
static void get_pwgc(int state, int* p, int* w, int* g, int* c);
static int is_dead_end(int state);
static int is_possible_transition(int state1, int state2);
static int changeP(int state);
static int changePW(int state);
static int changePG(int state);
static int changePC(int state);
static int is_visited(int visited[], int level, int state);
static void print_states(int visited[], int count);
static void dfs_main(int state, int goal_state, int level, int visited[]);
void make_adjacency_matrix(int graph[][16]);
void print_graph(int graph[][16], int num);
void save_graph(char* filename, int graph[][16], int num);





// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename(FILE *fp, int state)
{
	int binary[4] = { 0, };
	
	for (int i = 0; i < 4; i++)
	{
		binary[i] = state % 2;
		state = state / 2;
	}
	fprintf(fp, "\"<");
	for (int i = 3; i >= 0; i--)
	{
		fprintf(fp, "%d", binary[i]);
	}
	fprintf(fp, ">\"");
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc(int state, int* p, int* w, int* g, int* c)
{
	int forc = state % 2;
	*c = forc;
	state /= 2;


	int forg = state % 2;
	*g = forg;
	state /= 2;


	int forw = state % 2;
	*w = forw;
	state /= 2;


	int forp = state % 2;
	*p = forp;

}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end(int state)
{
	int arr[4] = { 0, };
	for (int i = 3; i >= 0; i--)
	{
		arr[i] = state % 2;
		state = state / 2;
		
	}
	if ((arr[0] != arr[1] && arr[1] == arr[2]) || (arr[0] != arr[2] && arr[2] == arr[3]))
		return 1;
	else
		return 0;
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용 (같이 이동하려면 같은 쪽에 있어야 함)
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition(int state1, int state2)
{
	int p1, w1, g1, c1;
	int p2, w2, g2, c2;

	get_pwgc(state1, &p1, &w1, &g1, &c1);
	get_pwgc(state2, &p2, &w2, &g2, &c2);


	if (p1 == p2)
	{
		return 0;
	}
		
	else if (p1 != w1 && w1 != w2)
	{
		return 0;
	}
	else if (p1 != g1 && g1 != g2)
	{
		return 0;
	}
	else if (p1 != c1 && c1 != c2)
	{
		return 0;
	}
	else if (is_dead_end(state2) == 1 || is_dead_end(state1) ==1)
	{
		return 0;
	}
	else if (p1 != p2 && w1 == w2 && g1 == g2 && c1 == c2)
	{
		return 1;
	}
	else if (p1 != p2 && w1 != w2 && g1 == g2 && c1 == c2)
	{
		return 1;
	}
	else if (p1 != p2 && w1 == w2 && g1 != g2 && c1 == c2)
	{
		return 1;
	}
	else if (p1 != p2 && w1 == w2 && g1 == g2 && c1 != c2)
	{
		return 1;
	}
	else
	{

		return 0;
	}
		

	
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP(int state)
{
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == 1)
		state = state - 8;
	else if (p == 0)
		state = state + 8;
	return state;

}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state)
{
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != w)
		return -1;
	else {
		if (p == 1)
			state = state - 12;
		else if (p == 0)
			state = state + 12;
		return state;
	}
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG(int state)
{
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != g )
		return -1;
	else {
		if (p == 1)
			state = state - 10;
		else if (p == 0)
			state = state + 10;
		return state;
	}
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC(int state)
{
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != c )
		return -1;
	else {
		if (p == 1)
			state = state - 9;
		else if (p == 0)
			state = state + 9;
		return state;
	}
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited(int visited[], int level, int state)
{
	for (int i = 0; i <= level; i++)
	{
		if (visited[i] == state)
			return 1;
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
static void print_states(int visited[], int count)
{
	fprintf(stderr, "지금까지의 방문\n");

	for (int i = 0; i <= count; i++)
	{
		fprintf(stderr, "\tlevel %d: %d", i, visited[i]);
		print_statename(stderr, visited[i]);
		fprintf(stderr, "\n\n");
	}
	
}

void print_bin(FILE * fp, int dec_state)
{
	int binary[4] = { 0, };
	for (int i = 3; i >= 0; i--)
	{
		binary[i] = dec_state % 2;
		dec_state = dec_state / 2;
	}
	fprintf(fp, "<");
	for (int i = 0; i < 4; i++)
	{
		fprintf(fp, "%d", binary[i]);
	}
	fprintf(fp, ">");
}

// recursive function
static void dfs_main(int state, int goal_state, int level, int visited[])
{
	if (level != 0)
		fprintf(stderr, "이어서 탐색 -> level %d (state: %d)부터 탐색\n", level, state);
	if (state == goal_state)
	{
		visited[level] = state;
		fprintf(stderr, "\n\n\n>>>>>>>>>>>>>>>>>>>>경로 발견<<<<<<<<<<<<<<<<<<<<<\n");
		print_states(visited, level);
		fprintf(stderr, "\n\n");
		
	}
	else
	{
		fprintf(stderr, "\nlevel %d(state: %d", level, state);
		print_bin(stderr, state);
		fprintf(stderr, "부터 탐색 시작!\n\n");
		int nextstep[4] = { changeP(state), changePW(state), changePG(state), changePC(state) };
		for (int i = 0; i < 4; i++)
		{
			int next = nextstep[i];
			if (is_possible_transition(state, next) && !(is_visited(visited, level, next)) && next <= 15 && next >= 0)
			{
				visited[level] = state;
				fprintf(stderr, ">>>>>>>>>>다음 경로(state: %d ", next);
				print_bin(stderr, next);
				fprintf(stderr, ")가 있어요! visited 경로에 level %d - state %d 추가\n\n", level, state);
				print_states(visited, level);
				dfs_main(next, goal_state, level + 1, visited);
			}
			else
			{
				if (0<= next && next <= 15)
				{
					fprintf(stderr, "\n>>state: %d ", next);
					print_bin(stderr, next);
					fprintf(stderr, "로는 못 가요!\n\n");
				}
				
			}
		}
		
	}


}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16])
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = i+1; j < 16; j++)
		{

				if (is_possible_transition(i, j) == 1)
				{
					graph[i][j] = 1;
					graph[j][i] = 1;
				}

			
		}
	}

}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num)
{
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			fprintf(stderr, "   %d   ", graph[i][j]);
		}
		fprintf(stderr, "\n");
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char* filename, int graph[][16], int num)
{

	FILE* fp;
	fp = fopen(filename, "w");
	fprintf(fp, "*Vertices %d\n", 16);
	for (int i = 0; i < 16; i++)
	{
		fprintf(fp, "%d ", i+1);
		print_statename(fp, i);
		fprintf(fp, "\n");
	}

	fprintf(fp, "*Edges\n");
	for (int i = 0; i < 16; i++)
	{
		for (int j = i + 1; j < 16; j++)
		{
			if (graph[i][j] == 1)
				fprintf(fp, " %d %d\n", i+1, j+1);
		}
	}
	fclose(fp);


}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int level = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( init_state, goal_state, level, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging)
	print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}


