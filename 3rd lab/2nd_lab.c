#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <math.h>

/*
план выполнения работы:
1) реализовать стек и очередь
2) реализовать структуру "токенов" в замену value для стек и очереди
3) реализовать токенизацию - разбиение по токенам входящей строки, чтобы привычное выражение типа 2+2 превратилось для компьютера в токен токен токен
4) реализовать алгоритм сортировочной станции Дейкстры, который бы обрабатывал выражение в токенизированном виде
5) реализовать вычисление выражения в польской нотации
*/

/*
устройство работы сортировочной станции Дейкстры:
сочетает в себе использование двух структур: очереди и стека, очередь будет являться буфером в который будет собираться изначальное выражение в инфиксной нотации в постфиксную
стек же будет стеком операторов, то есть будет содержать ТОЛЬКО операторы и ничего более
как обрабатывается запись: 
1) операнды (числа, переменные) сразу помещаются с очередь
2) функции (sin, cos, tg...) кладутся в стек и ждут своей закрывающей скобки
3) операторы попадают в стек, перед этим каждый оператор перед тем как попасть туда сравнивается с тем что уже находится на вершине стека: 
	- в случае >= приоритета или лево-ассоциативностью и > приоритета, оператор сразу выталкивается в очередь	|	лево-ассоциативность: операции выполняются слева направо, примеры: +, -, *, :
		- кладем текущий оператор в стек																		|	право-ассоциативность: операции выполняются справа налево, примеры: ^
4) открывающая скобка - кладется в стек и отсекает собой все предыдущие операторы								|------------------------------------------------------------------------------	
5) закрывающая скобка - выталкивает все операторы до открывающей скобки в очередь, если после исчезновения		| оператор | приоритет | унарность | ассоциативность | унарность - сколько операндов требуется
закрывающей скобки была функция - так же скидываем ее в очередь, после скобки *испаряются*						|	 !	   |	 4	   |  унарная  |	 правая	     | унарный => один
6) все что осталось в стеке переходит в очередь																	| функция  |	 4	   |  унарная  |	 правая		 | бинарный => два
																												|	 ^	   |	 3	   |  бинарная |     правая      |
																												|	*,/	   |	 2     |  бинарная |	 левая		 |
																												|   +,-    |     1     |  бинарная |     левая       |
*/
/*
как считать выражение в постфиксной нотации: 
1) число или переменная -> кладем в стек
2) оператор бинарный - достаем одно значение из стека, вставляем оператор, вставляем еще одно значение, считаем и засовываем обратно в стек
3) оператор унарный - достаем одно значение из стека, выполняем оператор или функцию и возвращаем в стек
4)если в стеке остался всего один элемент - мы нашли результат
*/

/*
пример - 2 + 2 * 3
1) Дейкстра: 
выражение: + 2 * 3  | выражение: 2 * 3     | выражение: * 3      | выражение: 3            | выражение:             | выражение:
стек операторов:    | стек операторов: +   | стек операторов: +  | стек операторов: *, +   | стек операторов: *, +  | стек операторов:
очередь: 2          | очередь: 2           | очередь: 2, 2       | очередь: 2, 2           | очередь: 2, 2, 3       | очередь: 2, 2, 3, *, +
2)рассчет выражения:
выражение: 2, 2, 3, *, +  | выражение: 2, 3, *, +   | выражение: 3, *, +   | выражение: *, +   | выражение: +  | между делом было: вытащили 3, вставили справа,                  | выражение: 
стек:                     | стек: 2                 | стек: 2, 2           | стек: 2, 2, 3     | стек: 2, 6    | вытащили *, вставили перед 3, вставили 2 и вернули результат: 6 | стек: 8
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
		printf("Стэк пустой!\n");
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
		printf("Очередь пуста!\n");
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
		printf("\t-------------Меню-----------\n"
			   "\t| 1. Ввести выражение      |\n"
			   "\t| 0. Выход из программы    |\n"
			   "\t----------------------------\n");
		scanf("%hu", &option);
		int ch;
		while ((ch = getchar()) != '\n' && ch != EOF) {} //fixing "\n" after choice
	}
}