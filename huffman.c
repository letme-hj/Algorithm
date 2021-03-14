//#define BINARY_MODE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Node 
{ 
	unsigned char	data;	// 문자	
	int		freq; 			// 빈도
	struct	Node *left;		// 왼쪽 서브트리 포인터
	struct	Node *right; 	// 오른쪽 서브트리 포인터
} tNode;

////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	int	last;		// 힙에 저장된 마지막 element의 index
	int	capacity;	// heapArr의 크기
	tNode **heapArr;
} HEAP;

// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1
HEAP *heapCreate( int capacity);

// 최소힙 유지
static void _reheapUp( HEAP *heap, int index);

// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert( HEAP *heap, tNode *data);

// 최소힙 유지
static void _reheapDown( HEAP *heap, int index);

// 최소값 제거
// _reheapDown 함수 호출
tNode *heapDelete( HEAP *heap);

// 힙 메모리 해제
void heapDestroy( HEAP *heap);

////////////////////////////////////////////////////////////////////////////////

int read_chars(FILE* fp, int* ch_freq)
{
	int count = 0;

	while (1)
	{
		int c = fgetc(fp);
		if (c == EOF)
		{
			break;
		}

		ch_freq[c] += 1;
		count++;
	}


	return count;
}


void free_huffman_code(char* codes[])
{
	for (int i = 0; i < 256; i++)
		free(codes[i]); 
}

tNode* newNode(char data, int freq)
{
	tNode* brandnew = malloc(sizeof(tNode));
	brandnew->data = data;
	brandnew->freq = freq;
	brandnew->left = NULL;
	brandnew->right = NULL;

	
	return brandnew;
}



tNode* make_huffman_tree(int* ch_freq)
{
	// 1. capacity 256짜리 빈 힙 생성
	HEAP* hp = heapCreate(256);

	// 2. 개별 알파벳에 대한 노드 생성
	// 3. 힙에 삽입
	for (int i = 0; i < 256; i++)
	{
		heapInsert(hp, newNode(i, ch_freq[i]));
	}
	
	while (hp->last > 0) // 7. 힙에 한 개의 노드가 남을 때까지 반복
	{
		// 4. 2개의 최소값을 갖는 트리 추출
		tNode* min1 = heapDelete(hp);
		tNode* min2 = heapDelete(hp); 

		// 5. 두 트리 결합 후 새 노드에 추가
		tNode* brandnew = newNode(0, min1->freq + min2->freq);
		brandnew->left = min1;
		brandnew->right = min2;

		// 6. 새 트리를 힙에 삽입
		heapInsert(hp, brandnew);
	
	}
	
	tNode* out = hp->heapArr[0]; 
	return out;

}


void traverse_tree(tNode* root, char* code, int depth, char* codes[]) // 여기 들어온 root는 완성된 허프만 트리!!
{

	if (root->left ==NULL && root->right ==NULL)
	{
		codes[root->data] = code; 
	}
	else 
	{
		
		char* updateleft = strdup(code);
		updateleft[depth] = '0'; // 왼쪽 노드로 가는 code
		

		char *updateright = strdup(code); 
		updateright[depth] = '1';
		
		traverse_tree(root->left, updateleft, depth + 1, codes);
		traverse_tree(root->right, updateright, depth + 1, codes);
		

		//free(updateleft);
		//free(updateright);

	}
	
}


void make_huffman_code(tNode* root, char* codes[])
{
	char *code = ""; 
	traverse_tree(root, code, 0, codes);
	

}


void destroyTree(tNode* root)
{
	if (root->left != NULL)
	{
		free(root->left);
		free(root->right);
		destroyTree(root->left);
		destroyTree(root->right);
	}
	
}



int encoding(char* codes[], FILE* infp, FILE* outfp)
{
	int count = 0;
	while (1)
	{
		int c = fgetc(infp);
		if (c == EOF)
		{
			break;
		}

		fputs(codes[c], outfp);
		count += (int)(sizeof(codes[c]));
	}
	return count / 8;
}

int encoding_binary( char *codes[], FILE *infp, FILE *outfp);


void decoding(tNode* root, FILE* infp, FILE* outfp)
{
	
	tNode* base = root;
	
	while (fgetc(infp)!=EOF)
	{
		
		if (root->left == NULL && root->right == NULL)
		{

			fputc(root->data, outfp);
			root = base;
			
		}

		else
		{
			char c = fgetc(infp);
			if (c == '0')
				root = root->left;
			
			else if (c == '1')
				root = root->right;
		}
		
	}
}
void decoding_binary( tNode *root, FILE *infp, FILE *outfp);

////////////////////////////////////////////////////////////////////////////////

void print_char_freq( int *ch_freq)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%d\n", i, ch_freq[i]); // 문자인덱스, 빈도
	}
}

////////////////////////////////////////////////////////////////////////////////

tNode *run_huffman( int *ch_freq, char *codes[])
{
	
	tNode *root;
	root = make_huffman_tree( ch_freq);
	
	make_huffman_code( root, codes);
	
	
	return root;
}

////////////////////////////////////////////////////////////////////////////////

void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv)
{
	FILE *fp;
	FILE *infp, *outfp;
	int ch_freq[256] = {0,}; // 문자별 빈도
	char *codes[256]; // 문자별 허프만 코드 (ragged 배열)
	tNode *huffman_tree; // 허프만 트리
	
	if (argc != 4)
	{
		fprintf( stderr, "%s input-file encoded-file decoded-file\n", argv[0]);
		return 1;
	}

	////////////////////////////////////////
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 1;
	}

	
	int num_bytes = read_chars( fp, ch_freq);

	fclose( fp);

	
	print_char_freq( ch_freq);
	
	
	huffman_tree = run_huffman( ch_freq, codes);
	
	
	print_huffman_code( codes);

	////////////////////////////////////////
	
	infp = fopen( argv[1], "rt");
	
#ifdef BINARY_MODE
	
	outfp = fopen( argv[2], "wb");
#else
	
	outfp = fopen( argv[2], "wt");
#endif

	
#ifdef BINARY_MODE
	//int encoded_bytes = encoding_binary( codes, infp, outfp);
#else
	int encoded_bytes = encoding( codes, infp, outfp);
#endif

	
	free_huffman_code( codes);
	
	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	
#ifdef BINARY_MODE
	infp = fopen( argv[2], "rb");
#else
	infp = fopen( argv[2], "rt");
#endif

	
	outfp = fopen( argv[3], "wt");

	
#ifdef BINARY_MODE
	//decoding_binary( huffman_tree, infp, outfp);
#else

	decoding( huffman_tree, infp, outfp);

#endif

	
	destroyTree( huffman_tree);

	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	printf( "# of bytes of the original text = %d\n", num_bytes);
	printf( "# of bytes of the compressed text = %d\n", encoded_bytes);
	printf( "compression ratio = %.2f\n", ((float)num_bytes - encoded_bytes) / num_bytes * 100);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void heapPrint( HEAP *heap)
{
	int i;
	tNode **p = heap->heapArr;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		printf("[%d]%c(%6d)\n", i, p[i]->data, p[i]->freq);
	}
	printf( "\n");
}

////////////////////////////////////////////////////////////////////////////////

// last = -1
HEAP *heapCreate( int capacity)
{
	HEAP *heap;
	
	heap = (HEAP *)malloc( sizeof(HEAP));
	if (!heap) return NULL;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc( sizeof(tNode *) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf( stderr, "Error : not enough memory!\n");
		free( heap);
		return NULL;
	}
	return heap;
}

////////////////////////////////////////////////////////////////////////////////

static void _reheapUp( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	int parent;
	
	while(1)
	{
		if (index == 0) return; // root node
		
		parent = (index - 1) / 2;
		
		if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;
			
			index = parent;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////

int heapInsert( HEAP *heap, tNode *data)
{
	if (heap->last == heap->capacity - 1)
		return 0;
	
	(heap->last)++;
	(heap->heapArr)[heap->last] = data;
	
	_reheapUp( heap, heap->last);
	
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

static void _reheapDown( HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	tNode *leftData;
	tNode *rightData;
	int noright = 0;
	int largeindex; // index of left or right child with large key
	
	while(1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)
		
		leftData = arr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) rightData = arr[index * 2 + 2];
		else noright = 1;
		
		if (noright || leftData->freq < rightData->freq) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child
		
		if (arr[index]->freq > arr[largeindex]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;
			
			index = largeindex;
			
			noright = 0;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////

tNode *heapDelete( HEAP *heap)
{
	if (heap->last == -1) return NULL; // empty heap
	
	tNode *data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	
	(heap->last)--;
	
	_reheapDown( heap, 0);
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////

void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}

