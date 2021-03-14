#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10


#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1


static void backtrace_main(int* op_matrix, int col_size, char* str1, char* str2, int n, int m, int level, char align_str[][8]);
void print_matrix(int* op_matrix, int col_size, int n, int m);
int min_editdistance(char* str1, char* str2);
static int __GetMin4(int a, int b, int c, int d);
static int __GetMin3(int a, int b, int c);
void print_alignment(char align_str[][8], int level);
void backtrace(int* op_matrix, int col_size, char* str1, char* str2, int n, int m);
char* Delete(char* str, int i, char* str_1)
{
	int stridx = 0;
	int str_1idx = 0;

	while (stridx < strlen(str))
	{
		if (stridx != i)
		{
			str_1[str_1idx] = str[stridx];
			str_1idx++;
			stridx++;
		}
		else if (stridx == i)
		{
			stridx++;
		}
	}
	if (str_1idx < strlen(str))
		str_1[strlen(str)-1] = '\0';

	return str_1;


}
int onestep_bfr(int i, int j, int col_size, int* op_matrix, int* d)
{
	int op = op_matrix[i * col_size + j];

	int binary[5] = { 0, }; // binary[0]: I, [1]: D, [2] S [3] M [4] T
	int pos = 0;
	int dec = op;
	while (1)
	{
		binary[pos] = dec % 2;
		dec = dec / 2;
		pos++;
		if (dec == 0)
			break;
	}
	
	if (binary[0] == 1) // I
		return d[i * col_size + (j - 1)];
	else if (binary[1] == 1) // D
		return d[(i - 1) * col_size + j];
	else if (binary[2] == 1) // S
		return d[(i - 1) * col_size + (j - 1)];
	else if (binary[3] == 1) // M
		return d[(i - 1) * col_size + (j - 1)];

}

char* DelLast(char* str, char *newstr)
{
	
	for (int i = 0; i < strlen(str) - 1; i++)
	{
		newstr[i] = str[i];
	}
	newstr[strlen(str) - 1] = '\0';
	return newstr;
}




// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!) -> 이 연산자 행렬이 완성된 상태에서 쓰이는 함수!
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이 (맨 뒤의 /0 제외) != 배열의 크기(sizeof str1)
// m : 문자열 2의 길이 (맨 뒤의 /0 제외)
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
static void backtrace_main(int* op_matrix, int col_size, char* str1, char* str2, int n, int m, int level, char align_str[][8])
{

	if (n == 0 && m == 0)
	{
		
		printf("--------------------------\n");
		print_alignment(align_str, level - 1);

	}

	else
	{

		//현재 cell의 연산자 정보
		int op = op_matrix[n * col_size + m];

		//연산자정보 (_OP) 확인을 위해 이진수로 바꾸기
		// binary[0]: I, [1]: D, [2] S [3] M [4] T
		int binary[5] = { 0, };
		int pos = 0;
		int dec = op;
		while (1)
		{
			binary[pos] = dec % 2;
			dec = dec / 2;
			pos++;
			if (dec == 0)
				break;
		}

		
		//왼쪽 거의 연산자 정보
		int left_op = op_matrix[n * (col_size - 1) + (m - 1)];

		//왼쪽 열까지의 op_matrix
		int op_matrix_left[(n + 1) * m];

		for (int i = 0; i <= n; i++)
		{

			for (int j = 0; j < m; j++)
			{
				op_matrix_left[i * (col_size - 1) + j] = op_matrix[i * col_size + j];
			}
		}

		//아래 거의 연산자 정보
		int lower_op = op_matrix[(n - 1) * col_size + m];
		//아래 열까지의 op_matrix
		int op_matrix_lower[n * (m + 1)];
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j <= m; j++)
			{
				op_matrix_lower[i * col_size + j] = op_matrix[i * col_size + j];
			}
		}

		//대각선 왼쪽 아래 거의 연산자 정보
		int lowerleft_op = op_matrix[(n - 1) * (col_size - 1) + (m - 1)];
		//대각선 왼쪽 아래까지의 op_matrix
		int op_matrix_lowerleft[n * m];
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				op_matrix_lowerleft[i * (col_size - 1) + j] = op_matrix[i * col_size + j];

			}
		}

		

		//현재 연산자 정보 (각 연산의 여부 확인)
		//S
		if (binary[2] == 1)
		{
			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';

			char str1_1[n];//n
			char str2_1[m];//m
			DelLast(str1, str1_1);
			DelLast(str2, str2_1);

			backtrace_main(op_matrix_lowerleft, col_size - 1, str1_1, str2_1, n - 1, m - 1, level + 1, align_str);
			
		}

		if (binary[3] == 1) // M
		{

			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';
	
			char str1_1[n];//n
			char str2_1[m];//m
			DelLast(str1, str1_1);
			DelLast(str2, str2_1);

			backtrace_main(op_matrix_lowerleft, col_size - 1, str1_1, str2_1, n - 1, m - 1, level + 1, align_str);

		}


		if (binary[0] == 1) // I
		{

			align_str[level][0] = '*';
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = str2[m-1];
			align_str[level][5] = '\0';
			
			char str2_1[m];//m
			DelLast(str2, str2_1);

			backtrace_main(op_matrix_left, col_size - 1, str1, str2_1, n, m - 1, level + 1, align_str);

		}

		if (binary[1] == 1) //D
		{

			align_str[level][0] = str1[n - 1];
			align_str[level][1] = ' ';
			align_str[level][2] = '-';
			align_str[level][3] = ' ';
			align_str[level][4] = '*';
			align_str[level][5] = '\0';


			char str1_1[n]; //n
			DelLast(str1, str1_1);

			backtrace_main(op_matrix_lower, col_size, str1_1, str2, n - 1, m , level + 1, align_str);


		}


		if (binary[4] == 1) // T
		{

			align_str[level][0] = str2[m - 1];
			align_str[level][1] = str1[n - 1];
			align_str[level][2] = '-';
			align_str[level][3] = str1[n - 1];
			align_str[level][4] = str2[m - 1];
			align_str[level][5] = '\0';


			//str1, str2 각각 한 칸씩 줄이고 서로 바꾼 거 하나씩 더 빼서 넣어줘야 함
			char str1_1[n]; //n
			char str2_1[m];//m

			int i = 0;
			int j = 0;

			//연산자정보
			int bfrtranspose_op;
			int op_matrix_bfrtranspose[(n - 1) * (m - 1)];


			for (int i = 0; i < n - 1; i++)
			{
				if (str1[i] == str2[m - 1])
				{
					for (int j = 0; j < m - 1; j++)
					{
						if (str2[j] == str1[n - 1] && (i - n + 1 == j - m + 1) && str1[i] != str2[j])
						{
							//str 줄이기 (recursion에 넣을 것)
							Delete(str1, i, str1_1);
							Delete(str2, j, str2_1);

							//matrix 줄이기 (recursion에 넣을 것)
							int k = 0;
							int r = 0;
							int t = 0;


							for (k; k < n;)
							{
								
								if (k != (i + 1))
								{
									for (t=0; t < m ;)
									{
										
										if (t != (j + 1))
										{
											
											op_matrix_bfrtranspose[r] = op_matrix[k * col_size + t];
											r++;
											t++;
										}
										else
											t++;

									}
									k++;
								}
								else
									k++;
							}

						
						

						}
					}
				}

			}
			char str1_2[n-1]; // n-2
			char str2_2[m-1]; // m-2
			DelLast(str1_1, str1_2);
			DelLast(str2_1, str2_2);


			backtrace_main(op_matrix_bfrtranspose, col_size - 2, str1_1, str2_1, n - 2, m - 2, level + 1, align_str);
			

		}


	}
}




// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를  괄호 안의 기호로 표시한다. 삽입(I), 삭제(D), 교체(S), 일치(M), 전위(T)
void print_matrix(int* op_matrix, int col_size, int n, int m)
{
	for (int i = n; i > 0; i--)
	{
		for (int j = 1; j < m+1; j++)
		{
			int binary[7] = { 0, };
			int pos = 0;
			int dec = op_matrix[i * col_size + j];
			while (1)
			{
				binary[pos] = dec % 2;
				dec = dec / 2;
				pos++;
				if (dec == 0)
					break;
			}

			if (binary[2] == 1)
				printf("S");
			if (binary[3] == 1)
				printf("M");
			if (binary[0] == 1)
				printf("I");
			if (binary[1] == 1)
				printf("D");
			if (binary[4] == 1)
				printf("T");

			printf("\t");
		}
		printf("\n");
	}
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance(char* str1, char* str2)
{

	int n = strlen(str1);
	int m = strlen(str2);

	int col_size = m+1;
	int op_matrix[(n+1) *(m+1)];
	int d[(n+1)* (m+1)];

	
	// op_matrix 배열 0으로 초기화
	for (int i = 0; i < ((n+1) * (m+1)) ; i++)
		op_matrix[i] = 0;

	// d 행렬을 0으로 초기화
	for (int i = 0; i < ((n + 1) * (m + 1)); i++)
		d[i] = 0;


	//첫줄 초기화
	op_matrix[0] = 0;
	for (int i = 1; i < n+1; i++)
	{
		op_matrix[i * col_size] = DELETE_OP;
		d[i * col_size] = i;
		
	}
	for (int j = 1; j < m+1; j++)
	{
		op_matrix[j] = INSERT_OP;
		d[j] = j;
		
	}

	for (int i = 1; i < n + 1; i++)
	{
		for (int j = 1; j < m + 1; j++)
		{
			int D, I, S, T;
			D = d[(i - 1) * col_size + j] + DELETE_COST;
			I = d[i * col_size + (j - 1)] + INSERT_COST;

			if (str1[i - 1] == str2[j - 1])
				S = d[(i - 1) * col_size + (j - 1)];
			else if (str1[i - 1] != str2[j - 1])
				S = d[(i - 1) * col_size + (j - 1)] + SUBSTITUTE_COST;

			T = -1;
			for (int k = 0; k + 1 < i; k++)
			{
				if (str1[k] == str2[j - 1] && k < j - 1 && i != 1 && j != 1)
				{
					
					for (int t = 0; t + 1 < j; t++)
					{
						if (str2[t] == str1[i - 1] && (i - 1 - k == j - 1 - t) && str1[k] != str2[t])
						{
							int bfr;
							if (__GetMin3(D, I, S) == D)
								bfr = d[(i - 1) * col_size + j];
							else if (__GetMin3(D, I, S) == I)
								bfr = d[(i - 1) * col_size + j];
							else if (__GetMin3(D, I, S) == S)
								bfr = d[(i - 1) * col_size + (j - 1)];
							T = bfr+TRANSPOSE_COST- (d[(k + 1) * col_size + (t + 1)]-onestep_bfr(k+1, t+1, col_size, op_matrix, d)); 
							
						}
						else
							continue;
					}
				}
				else
					continue;

			}


			if (T == -1)
			{
				d[i * col_size + j] = __GetMin3(D, I, S);
			}
			else
				d[i * col_size + j] = __GetMin4(D, I, S, T);


			if (S == d[i * col_size + j])
			{

				if (S == d[(i - 1) * col_size + (j - 1)])
					op_matrix[i * col_size + j] = MATCH_OP;
				else
					op_matrix[i * col_size + j] = SUBSTITUTE_OP;

			}

			if (I == d[i * col_size + j])
			{
				int origin = op_matrix[i * col_size + j];
				op_matrix[i * col_size + j] = origin + INSERT_OP;
			}

			if (D == d[i * col_size + j])
			{
				int origin = op_matrix[i * col_size + j];
				op_matrix[i * col_size + j] = origin + DELETE_OP;
			}

			if (T == d[i * col_size + j])
			{
				int origin = op_matrix[i * col_size + j];
				op_matrix[i * col_size + j] = origin + TRANSPOSE_OP;
			}
			


		}
		

		
	}
	
	print_matrix(op_matrix, col_size, n, m);
	backtrace(op_matrix, col_size, str1, str2, n, m);

	return d[n * col_size + m];
}


////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings


	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str);
	
}

////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	fprintf( stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf( stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf( stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf( stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);

	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

