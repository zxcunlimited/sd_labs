#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <Windows.h>

void menu(); //the definition of function to it work in every other function

typedef struct Date {
	int hour;
	int minute;
	int day;
	int month;
	int year;
	char weekday[15]; 
}Date;

typedef struct event{
	Date date;
	char description[100];
	char place[50];
	unsigned short importance;
}event;

//binary tree realization
typedef struct B_tree{
	event note;
	struct B_tree* left; //pointer on left subtree
	struct B_tree* right; //pointer on right subtree
	int height; 
} B_tree;

B_tree* importance_tree = NULL;
B_tree* date_tree = NULL;


//function to convert char in int and check if date input was incorrect
int foolproof(Date* date, char symbol_date[20]) { //date in format: HH:MM DD.MM.YYYY
	int hour, minute, day, month, year;
	short correct;
	const short month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (sscanf(symbol_date, "%d:%d %d.%d.%d", &hour, &minute, &day, &month, &year) != 5) {
		printf("Parsing Error\n");
		exit(0);
	}
	if (!((hour >= 0 && hour < 24) && (minute >= 0 && minute < 60))) {
		printf("����������, ������� ���� ���������!\n");
		return 1;
		//call date input again
	}
	if ((month == 2) && ((year % 4 == 0) && (year % 100 == 0) || (year % 400 == 0))) {
		correct = 29;
	}
	else correct = month_days[month - 1];
	if (!((day >= 1 && day <= correct) && (month >= 1 && month < 13) && year >= 0)) {
		printf("����������, ������� ���� ���������!\n");
		return 1;
	}   //call date input again
	date->hour = hour;
	date->minute = minute;
	date->day = day;
	date->month = month;
	date->year = year;
	return 0;
}

/*
in this function we will count weekday depending on the date
there is a formula that let us do this: weekday = (day + month code + year code) % 7
year code = (6 + year % 100 + (year % 100 / 4)) % 7
also if we are counting not our sentury we need to replace 6 by different number: 16s = 6, 17s = 4, 18s = 2, 19s = 0 ,20s = 6, etc.
month code dependiong on a year:
			leap year is the one's that (year % 4 == 0 && year / 100 != 0) || (year / 400 != 0)
		   | Jan | Feb | Mar | Apr | May | Jun | Jul | Aug | Sep | Oct | Nov | Dec |
!leap year |  1  |  4  |  4  |  0  |  2  |  5  |  0  |  3  |  6  |  1  |  4  |  6  |
leap year  |  0  |  3  |  4  |  0  |  2  |  5  |  0  |  3  |  6  |  1  |  4  |  6  |
in this formula weekday counting is on israel system so 0 is saturday and not the monday

also there is a problem with cyrillic - we cant use enum with it so we wiil use other ways to say that weekday is it
*/
void weekday_counter(Date* date) {
	short l_year[12] = { 0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 };
	short nl_year[12] = { 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 };
	short y_code, m_code, weekday;
	y_code = ((6 - 2 * ((date->year / 100) % 4)) + (date->year % 100) + ((date->year % 100) / 4)) % 7;
	if ((date->year % 4 == 0) && (date->year / 100 != 0) || (date->year / 400 != 0)) m_code = l_year[date->month - 1];
	else m_code = nl_year[date->month - 1];
	weekday = (date->day + m_code + y_code) % 7;
	switch (weekday) {
	case 0: strncpy(date->weekday, "�������", 12); break;
	case 1: strncpy(date->weekday, "�����������", 12); break;
	case 2:	strncpy(date->weekday, "�����������", 12); break;
	case 3: strncpy(date->weekday, "�������", 12); break;
	case 4: strncpy(date->weekday, "�����", 12); break;
	case 5: strncpy(date->weekday, "�������", 12); break;
	case 6: strncpy(date->weekday, "�������", 12); break;
	default: printf("����������� ����\n");
	}
}

//if node is NULL( = 0), its height is 0
int check_height(B_tree* n) {
	return n ? n->height : 0;
}

int compare_dates(Date* a, Date* b) {
	if (a->year != b->year) return (a->year > b->year) ? 1 : -1;
    if (a->month != b->month) return (a->month > b->month) ? 1 : -1;
    if (a->day != b->day) return (a->day > b->day) ? 1 : -1;
    if (a->hour != b->hour) return (a->hour > b->hour) ? 1 : -1;
    if (a->minute != b->minute) return (a->minute > b->minute) ? 1 : -1;
    return 0; //if dates are equal (that can be true only when we delete node) we need to return smth
}

int BF(B_tree* t) {
	return t ? (check_height(t->left) - check_height(t->right)) : 0;
}


//we will not have initialization function because will do this in the main
B_tree* create_node(event n) {
	B_tree* node = (B_tree*)malloc(sizeof(B_tree));
	if (node == NULL) {
		printf("Memory allocation error!\n");
		exit(1);
	}
	node->note = n;
	node->left = NULL;
	node->right = NULL;
	node->height = 1;
	return node;
}

/*
      a            b
       \          /
        b   ->   a  
       /          \
	  val          val
*/
B_tree* L_rotate(B_tree* a) {
	B_tree* b = a->right;

	a->right = b->left;
	b->left = a;

	a->height = max(check_height(a->left), check_height(a->right)) + 1;
	b->height = max(check_height(b->left), check_height(b->right)) + 1;

	return b;
}

/*
	 a        b
    /          \
   b     ->     a 
    \          /
	val      val  
*/
B_tree* R_rotate(B_tree* a) {
	B_tree* b = a->left;

	a->left = b->right;
	b->right = a;

	a->height = max(check_height(a->left), check_height(a->right)) + 1;
	b->height = max(check_height(b->left), check_height(b->right)) + 1;

	return b;
}
/*
B_tree* insert(B_tree* t, int val, event n) {
	if (t == NULL) return create_node(val, n);
	if (val < t->value) {
		printf("left\n");
		t->left = insert(t->left, val, n);
	}
	else if (val > t->value) {
		printf("right\n");
		t->right = insert(t->right, val, n);
	}
	else {
		return t;
	}

	t->height = max(check_height(t->left), check_height(t->right)) + 1;
	
	int balance = BF(t);
	//LL
	if (balance > 1 && val < t->left->value) {
		return R_rotate(t);
	}
	//LR
	if (balance > 1 && val > t->left->value) {
		t->left = L_rotate(t->left);
		return R_rotate(t);
	}
	//RR
	if (balance < -1 && val > t->right->value) {
		return L_rotate(t);
	}
	//RL
	if (balance < -1 && val < t->right->value) {
		t->right = R_rotate(t->right);
		return L_rotate(t);
	}

	return t;
}
*/

B_tree* insert_importance(B_tree* t, event n) {
	if (t == NULL) return create_node(n);
	if (n.importance < t->note.importance) {
		printf("left\n");
		t->left = insert_importance(t->left, n);
	}
	else if (n.importance > t->note.importance) {
		printf("right\n");
		t->right = insert_importance(t->right, n);
	}
	else if (n.importance == t->note.importance) { //if importance is equal we need to compare by date
		short compare = compare_dates(&n.date, &t->note.date);
		if (compare == -1) {
			printf("left\n");
			t->left = insert_importance(t->left, n);
		}
		else if (compare == 1) {
			printf("right\n");
			t->right = insert_importance(t->right, n);
		}
	}
	else { //no duplicates
		return t;
	}

	t->height = max(check_height(t->left), check_height(t->right)) + 1;

	int balance = BF(t);
	//LL
	if (balance > 1 && n.importance < t->note.importance) { //val < t->left->value
		return R_rotate(t);
	}
	//LR
	if (balance > 1 && n.importance > t->note.importance) { //val > t->left->value
		t->left = L_rotate(t->left);
		return R_rotate(t);
	}
	//RR
	if (balance < -1 && n.importance > t->note.importance) { //val > t->right->value
		return L_rotate(t);
	}
	//RL
	if (balance < -1 && n.importance < t->note.importance) { //val < t->right->value
		t->right = R_rotate(t->right);
		return L_rotate(t);
	}

	return t;
}

B_tree* insert_date(B_tree* t, event n) {
	if (t == NULL) return create_node(n);
	short compare = compare_dates(&n.date, &t->note.date);
	if (compare == -1) {
		printf("left\n");
		t->left = insert_date(t->left, n);
	}
	else if (compare == 1) {
		printf("right\n");
		t->right = insert_date(t->right, n);
	}
	else {
		return t;
	}

	t->height = max(check_height(t->left), check_height(t->right)) + 1;

	int balance = BF(t);
	//LL
	if (balance > 1 && compare_dates(&n.date, &t->left->note.date) == -1) { //val < t->left->value
		return R_rotate(t);
	}
	//LR
	if (balance > 1 && compare_dates(&n.date, &t->left->note.date) == 1){ //val > t->left->value
		t->left = L_rotate(t->left);
		return R_rotate(t);
	}
	//RR
	if (balance < -1 && compare_dates(&n.date, &t->right->note.date) == 1) { //val > t->right->value
		return L_rotate(t);
	}
	//RL
	if (balance < -1 && compare_dates(&n.date, &t->right->note.date) == -1) { //val < t->right->value
		t->right = R_rotate(t->right);
		return L_rotate(t);
	}

	return t;
}

void add() {
	event temp = { 0 };
	int flag = 1;
	while (flag) {
		char temp_date[20];
		printf("������� ���� ������� � ������� ��:�� ��.��.���� -  ");
		fgets(temp_date, sizeof(temp_date), stdin);
		flag = foolproof(&temp.date, temp_date);
	}
	weekday_counter(&temp.date);
	printf("������� �������� �������: ");
	fgets(temp.description, sizeof(temp.description), stdin);
	printf("������� ����� �������: ");
	fgets(temp.place, sizeof(temp.place), stdin);
	flag = 0;
	while (!flag) {
		printf("������� �������� ������� �� 0 �� 10: ");
		scanf("%hu", &temp.importance);
		flag = (temp.importance >= 0 && temp.importance <= 10);
	}
	date_tree = insert_date(date_tree, temp);
	importance_tree = insert_importance(importance_tree, temp);
	printf("\n����� ������� ���� ��������� � �������� �����\n");
}

void print_event(B_tree* t) {
	printf("---------------------------------------------\n");
	printf("���� �������: %d:%d %d.%d.%d\n", t->note.date.hour, t->note.date.minute, t->note.date.day, t->note.date.month, t->note.date.year);
	printf("���� ������: %s\n", t->note.date.weekday);
	printf("�����: %s", t->note.place);
	printf("��������: %hu\n", t->note.importance);
	printf("��������: %s", t->note.description);
	printf("---------------------------------------------\n");
}

B_tree* find_replacement(B_tree* t) {
	while (t && t->left != NULL)
		t = t->left;
	return t;
}

B_tree* rebalance(B_tree* t) {
	if (t == NULL) return NULL;
	t->height = max(check_height(t->left), check_height(t->right)) + 1;
	int balance = BF(t);

	//LL
	if (balance > 1 && BF(t->left) >= 0)
		return R_rotate(t);
	//LR
	if (balance > 1 && BF(t->left) < 0) {
		t->left = L_rotate(t->left);
		return R_rotate(t);
	}
	//RR
	if (balance < -1 && BF(t->right) <= 0)
		return L_rotate(t);
	//RL
	if (balance < -1 && BF(t->right) > 0) {
		t->right = R_rotate(t->right);
		return L_rotate(t);
	}
	return t;
}

unsigned short delete_importance;
B_tree* delete_node_date(B_tree* t, Date date) {
	int cmp_res = compare_dates(&date, &t->note.date);
	if (t == NULL)
		return t;
	if (cmp_res == -1)  //val < t->val
		t->left = delete_node_date(t->left, date);
	else if (cmp_res == 1) //val > t->val
		t->right = delete_node_date(t->right, date);
	else {
		printf("�������, ������� ����� �������:\n");
		print_event(t);
		//node was found, now we are regard three events: node have only left subtree, node have only right subtree, and if node have them all, we need to find a replacement
		delete_importance = t->note.importance;
		if (t->left == NULL) {
			B_tree* temp = t->right;
			free(t);
			return temp;
		}
		else if (t->right == NULL) {
			B_tree* temp = t->left;
			free(t);
			return temp;
		}
		//event when node have 2 subtrees
		B_tree* temp = find_replacement(t->right); //we are trying to find min in right subtree
		t->note = temp->note;
		t->right = delete_node_date(t->right, temp->note.date);
	}
	//now when we deleted right node, we need to check if our balance is still correct
	return rebalance(t);
}

//to delete a node from importance tree we need to know what importance have our event, thats why we will remember it while deleting node by date
B_tree* delete_node_importance(B_tree* t, Date date) {
	if (t == NULL) return t;
	else if (delete_importance < t->note.importance) t->left = delete_node_importance(t->left, date);
	else if (delete_importance > t->note.importance) t->right = delete_node_importance(t->right, date);
	else { //if importance !> && !< => we found node that we need to delete
		int cmp_res = compare_dates(&date, &t->note.date);
		if (cmp_res == -1) t->left = delete_node_importance(t->left, date);
		else if (cmp_res == 1) t->right = delete_node_importance(t->right, date);
		else {
			//node was found, now we are regard three events: node have only left subtree, node have only right subtree, and if node have them all, we need to find a replacement
			if (t->left == NULL) {
				B_tree* temp = t->right;
				free(t);
				return temp;
			}
			else if (t->right == NULL) {
				B_tree* temp = t->left;
				free(t);
				return temp;
			}
			//event when node have 2 subtrees
			B_tree* temp = find_replacement(t->right); //we are trying to find min in right subtree
			t->note = temp->note;
			t->right = delete_node_importance(t->right, temp->note.date);
		}
	}
	return rebalance(t);
}

//in importance we will do negative in-order to go from biggest importance to lowest
void importance_output(B_tree* t) {
	if (t != NULL) {
		importance_output(t->right);
		print_event(t);
		importance_output(t->left);
	}
}

//in date we weiil realize default in-order to go from the earliest to the last date
void date_output(B_tree* t) {
	if (t != NULL) {
		date_output(t->left);
		print_event(t);
		date_output(t->right);
	}
}

void free_tree(B_tree* t) {
	if (t != NULL) {
		free_tree(t->right);
		free_tree(t->left);
	}
	free(t);
}

void empty_check(B_tree* t) {
	if (date_tree == NULL) {
		printf("� ������ ��� �������!\n");
		menu();
	}
}

void menu() {
	printf("\t-----------����:-------------------------\n"
		   "\t| 1. �������� ������                    |\n"
		   "\t| 2. ������� ������                     |\n"
		   "\t| 3. ����� ������� �� ��������          |\n"
		   "\t| 4. ����� ������� �� ����              |\n"
		   "\t| 5. ����� ������� �� �����             |\n"
		   "\t| 6. �������� ������� �� �����          |\n"
		   "\t| 7. ��������� ������� � ����           |\n"
		   "\t| 0. �����                              |\n"
		   "\t-----------------------------------------\n"
	       "\t ������� ��������: ");
	unsigned short option;
	scanf("%hu", &option);
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF) {}
	switch (option) {
	case 0: exit(0);
	case 1: add();
		menu();
		break;
	case 2: {
		if (date_tree == NULL) {
			empty_check(date_tree);
			break;
		}
		char temp_date[20];
		Date temp;
		printf("������� ���� �������, ������� ������ �������: ");
		fgets(temp_date, sizeof(temp_date), stdin);
		foolproof(&temp, temp_date);
		date_tree = delete_node_date(date_tree, temp);
		importance_tree = delete_node_importance(importance_tree, temp);
		menu();
		break;
	}
	case 3: {
		if (date_tree == NULL) {
			empty_check(date_tree);
			break;
		}
		importance_output(importance_tree);
		menu();
		break;
	}
	case 4: {
		if (date_tree == NULL) {
			empty_check(date_tree);
			break;
		}
		date_output(date_tree);
		menu();
		break;
	}
	case 5:

	case 6:

	case 7:

	default: 
		printf("������� ��������� ��������!\n");
		menu();
		break;
	}
}

int main() {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	menu();
}