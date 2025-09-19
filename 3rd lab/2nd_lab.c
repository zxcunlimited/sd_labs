#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <math.h>

/*
���� ���������� ������:
1) ����������� ���� � �������
2) ����������� ��������� "�������" � ������ value ��� ���� � �������
3) ����������� ����������� - ��������� �� ������� �������� ������, ����� ��������� ��������� ���� 2+2 ������������ ��� ���������� � ����� ����� �����
4) ����������� �������� ������������� ������� ��������, ������� �� ����������� ��������� � ���������������� ����
5) ����������� ���������� ��������� � �������� �������
*/

/*
���������� ������ ������������� ������� ��������:
�������� � ���� ������������� ���� ��������: ������� � �����, ������� ����� �������� ������� � ������� ����� ���������� ����������� ��������� � ��������� ������� � �����������
���� �� ����� ������ ����������, �� ���� ����� ��������� ������ ��������� � ������ �����
��� �������������� ������: 
1) �������� (�����, ����������) ����� ���������� � �������
2) ������� (sin, cos, tg...) �������� � ���� � ���� ����� ����������� ������
3) ��������� �������� � ����, ����� ���� ������ �������� ����� ��� ��� ������� ���� ������������ � ��� ��� ��� ��������� �� ������� �����: 
	- � ������ >= ���������� ��� ����-���������������� � > ����������, �������� ����� ������������� � �������	|	����-���������������: �������� ����������� ����� �������, �������: +, -, *, :
		- ������ ������� �������� � ����																		|	�����-���������������: �������� ����������� ������ ������, �������: ^
4) ����������� ������ - �������� � ���� � �������� ����� ��� ���������� ���������								|------------------------------------------------------------------------------	
5) ����������� ������ - ����������� ��� ��������� �� ����������� ������ � �������, ���� ����� ������������		| �������� | ��������� | ��������� | ��������������� | ��������� - ������� ��������� ���������
����������� ������ ���� ������� - ��� �� ��������� �� � �������, ����� ������ *����������*						|	 !	   |	 4	   |  �������  |	 ������	     | ������� => ����
6) ��� ��� �������� � ����� ��������� � �������																	| �������  |	 4	   |  �������  |	 ������		 | �������� => ���
																												|	 ^	   |	 3	   |  �������� |     ������      |
																												|	*,/	   |	 2     |  �������� |	 �����		 |
																												|   +,-    |     1     |  �������� |     �����       |
*/
/*
��� ������� ��������� � ����������� �������: 
1) ����� ��� ���������� -> ������ � ����
2) �������� �������� - ������� ���� �������� �� �����, ��������� ��������, ��������� ��� ���� ��������, ������� � ���������� ������� � ����
3) �������� ������� - ������� ���� �������� �� �����, ��������� �������� ��� ������� � ���������� � ����
4)���� � ����� ������� ����� ���� ������� - �� ����� ���������
*/

/*
������ - 2 + 2 * 3
1) ��������: 
���������: + 2 * 3  | ���������: 2 * 3     | ���������: * 3      | ���������: 3            | ���������:             | ���������:
���� ����������:    | ���� ����������: +   | ���� ����������: +  | ���� ����������: *, +   | ���� ����������: *, +  | ���� ����������:
�������: 2          | �������: 2           | �������: 2, 2       | �������: 2, 2           | �������: 2, 2, 3       | �������: 2, 2, 3, *, +
2)������� ���������:
���������: 2, 2, 3, *, +  | ���������: 2, 3, *, +   | ���������: 3, *, +   | ���������: *, +   | ���������: +  | ����� ����� ����: �������� 3, �������� ������,                  | ���������: 
����:                     | ����: 2                 | ����: 2, 2           | ����: 2, 2, 3     | ����: 2, 6    | �������� *, �������� ����� 3, �������� 2 � ������� ���������: 6 | ����: 8
*/

typedef enum token_types {
	numbers,
	variables,
	operators,
	functions,
	open_brace,
	close_brace
}token_types;

typedef struct Tokens {
	token_types type;
	char text[20];
	double value;
}Tokens;

typedef struct node {
	int value;
	struct node* next;
}node;

typedef struct stack {
	struct node* top;
}stack;

typedef struct queue {
	struct node* head;
	struct node* tail;
}queue ;

//functions to put a value in the struct (push to stack and en(ter)queue for queue)
void push(stack* s, int val) {
	node* s_node = (node*)malloc(sizeof(node));
	if (!s_node) {
		printf("Memory allocation error!");
		exit(0);
	}
	s_node->next = NULL;
	s_node->value = val;
	s_node->next = s->top;
	s->top = s_node;
}

void enqueue(queue* q, int val) {
	node* q_node = (node*)malloc(sizeof(node));
	if (!q_node) {
		printf("Memory allocation error!");
		exit(0);
	}
	q_node->value = val;
	q_node->next = NULL;
	if (q->head == NULL)	q->head = q_node;
	else q->tail->next = q_node;
	q->tail = q_node;
}

//functions to get a value from struct (because stack work on LIFO and queue FIFO, the value gets from the top of the stack and from the head of the queue)
int pop(stack* s) {
	if (s == NULL) {
		printf("���� ������!\n");
		return 0;
	}
	node* temp = (node*)malloc(sizeof(node));
	temp = s->top;
	int x = temp->value;
	s->top = temp->next;
	free(temp);
	return x;
}

int dequeue(queue* q) {
	if (q == NULL) {
		printf("������� �����!\n");
		return 0;
	}
	node* temp = (node*)malloc(sizeof(node));
	temp = q->head;
	int x = temp->value;
	q->head = temp->next;
	if (q->head = NULL) q->tail = NULL; //with the queue its quite harder, we need to check if after our manipulations the head is empty, if it is we know that the whole queue is empty now and need to null the tail too
	free(temp);
	return x;
}

int main() {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	//initializing all structures
	stack* main_stack = (stack*)malloc(sizeof(stack));
	main_stack->top = NULL;
	queue* main_queue = (queue*)malloc(sizeof(queue));
	main_queue->head = NULL;
	main_queue->tail = NULL;
	unsigned short option;

	while (TRUE) {
		printf("\t-------------����-----------\n"
			   "\t| 1. ������ ���������      |\n"
			   "\t| 0. ����� �� ���������    |\n"
			   "\t----------------------------\n");
		scanf("%hu", &option);
		int ch;
		while ((ch = getchar()) != '\n' && ch != EOF) {} //fixing "\n" after choice
	}
}