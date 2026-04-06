//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
한국어 번역: CE1007/CZ1007 자료구조
Lab Test: Section A - Linked List Questions
한국어 번역: 랩 테스트: 섹션 A - 연결 리스트 문제
Purpose: Implementing the required functions for Question 1
한국어 번역: 목적: 문제 1에서 요구하는 함수들을 구현하기 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode
				// 한국어 번역: ListNode의 정의를 변경하면 안 됩니다.

typedef struct _linkedlist{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList
				// 한국어 번역: LinkedList의 정의를 변경하면 안 됩니다.


///////////////////////// function prototypes ////////////////////////////////////

//You should not change the prototype of this function
//한국어 번역: 이 함수의 원형은 변경하면 안 됩니다.
int insertSortedLL(LinkedList *ll, int item);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);


//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;

	//Initialize the linked list 1 as an empty linked list
	//한국어 번역: 연결 리스트 1을 빈 연결 리스트로 초기화합니다.
	ll.head = NULL;
	ll.size = 0;

	printf("1: Insert an integer to the sorted linked list:\n");
	printf("2: Print the index of the most recent input value:\n");
	printf("3: Print sorted linked list:\n");
	printf("0: Quit:");

	while (c != 0)
	{
		printf("\nPlease input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertSortedLL(&ll, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			printf("The value %d was added at index %d\n", i, j);
			break;
		case 3:
			printf("The resulting sorted linked list is: ");
			printList(&ll);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}


	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item)
{
	/* add your code here */
	/* 한국어 번역: 여기에 코드를 작성하세요. */
	ListNode *curr = ll->head;
	int idx = 0;

	// head가 없거나, 처음 값이 item보다 작을 때
	if(ll->head == NULL || (curr->item) > item){
		insertNode(ll, idx, item);
		return idx;
	}
	else{
		// Null이 아니면서 item이 현재값보다 클 때
		while(curr != NULL && (curr->item) < item){
			curr = curr->next;
			idx++;
		}
		// Null이 아니면서 item의 값이 현재값과 같을 때
		if(curr != NULL && curr->item == item){
			return -1;
		}
		insertNode(ll, idx, item);
		return idx;
	}
}

///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll){

	ListNode *cur;
	if (ll == NULL)
		return;
	cur = ll->head;

	if (cur == NULL)
		printf("Empty");
	while (cur != NULL)
	{
		printf("%d ", cur->item);
		cur = cur->next;
	}
	printf("\n");
}


void removeAllItems(LinkedList *ll)
{
	ListNode *cur = ll->head;
	ListNode *tmp;

	while (cur != NULL){
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	ll->head = NULL;
	ll->size = 0;
}


ListNode *findNode(LinkedList *ll, int index){

	ListNode *temp;

	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head;

	if (temp == NULL || index < 0)
		return NULL;

	while (index > 0){
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		index--;
	}

	return temp;
}

int insertNode(LinkedList *ll, int index, int value){

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	// 한국어 번역: 빈 리스트이거나 첫 번째 노드를 삽입하는 경우 head 포인터를 갱신해야 합니다.
	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}


	// Find the nodes before and at the target position
	// 한국어 번역: 목표 위치의 이전 노드와 해당 위치의 노드를 찾습니다.
	// Create a new node and reconnect the links
	// 한국어 번역: 새 노드를 만들고 링크를 다시 연결합니다.
	if ((pre = findNode(ll, index - 1)) != NULL){
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}


int removeNode(LinkedList *ll, int index){

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	// 한국어 번역: 삭제할 수 있는 가장 큰 인덱스는 size-1입니다.
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	// 한국어 번역: 첫 번째 노드를 삭제하는 경우 head 포인터를 갱신해야 합니다.
	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// 한국어 번역: 목표 위치의 이전 노드와 이후 노드를 찾습니다.
	// Free the target node and reconnect the links
	// 한국어 번역: 대상 노드를 해제하고 링크를 다시 연결합니다.
	if ((pre = findNode(ll, index - 1)) != NULL){

		if (pre->next == NULL)
			return -1;

		cur = pre->next;
		pre->next = cur->next;
		free(cur);
		ll->size--;
		return 0;
	}

	return -1;
}
