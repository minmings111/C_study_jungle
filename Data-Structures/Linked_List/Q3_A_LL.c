//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 3
한글 번역:
목적: 문제 3에서 요구하는 함수를 구현하는 것 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode
					// 한글 번역: ListNode의 정의는 변경하면 안 됩니다.

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList
					// 한글 번역: LinkedList의 정의는 변경하면 안 됩니다.


//////////////////////// function prototypes /////////////////////////////////////

// You should not change the prototype of this function
// 한글 번역: 이 함수의 프로토타입은 변경하면 안 됩니다.
void moveOddItemsToBack(LinkedList *ll);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode * findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);

//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;
	//Initialize the linked list 1 as an empty linked list
	// 한글 번역: 연결 리스트를 빈 연결 리스트로 초기화
	ll.head = NULL;
	ll.size = 0;


	printf("1: Insert an integer to the linked list:\n");
	printf("2: Move all odd integers to the back of the linked list:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			moveOddItemsToBack(&ll); // You need to code this function
								   // 한글 번역: 이 함수는 직접 구현해야 합니다.
			printf("The resulting linked list after moving odd integers to the back of the linked list is: ");
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

void moveOddItemsToBack(LinkedList *ll)
{
	/* add your code here */
	/* 한글 번역: 여기에 코드를 작성하세요 */
	// when ll is Null or ll's node is null or ll's size is 1
	if(ll == NULL || ll->head == NULL || ll->size == 1){
		return;
	}
	
	// we need end node!
	ListNode *tailNode = ll->head;
	while (tailNode->next != NULL){
		tailNode = tailNode->next;
	}
	
	ListNode *currNode = ll->head; // check the current
	ListNode *prevNode = NULL; // when curr is head, prev is NULL!!
	
	for(int i = 0; i < ll->size; i++){
		if(currNode == NULL){break;} // maybe... something wrong... excape

		if(((currNode->item)%2 == 1) && (currNode != tailNode)){ // is odd? - yes!
			ListNode *tempNode = currNode->next; // don't lose next node idx
			// move back
			tailNode->next = currNode;
			tailNode = currNode;
			tailNode->next = NULL;
			
			// new curr check with tempNode
			currNode = tempNode; 

			if(prevNode == NULL){ // first node is odd...
				ll->head = currNode; // connected ll.head with new curr
			}
			else{
				prevNode->next = currNode; // connected prev with new curr
			}
			
		}
		// not odd!
		else{
			prevNode = currNode;
			currNode = currNode->next;
		}
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
	// 한글 번역: 빈 리스트이거나 첫 번째 노드를 삽입하는 경우 head 포인터를 갱신해야 함
	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}


	// Find the nodes before and at the target position
	// 한글 번역: 목표 위치의 이전 노드와 해당 위치의 노드를 찾음
	// Create a new node and reconnect the links
	// 한글 번역: 새 노드를 만들고 링크를 다시 연결함
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
	// 한글 번역: 삭제할 수 있는 가장 큰 인덱스는 size-1임
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	// 한글 번역: 첫 번째 노드를 삭제하는 경우 head 포인터를 갱신해야 함
	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// 한글 번역: 목표 위치의 이전 노드와 이후 노드 관계를 확인함
	// Free the target node and reconnect the links
	// 한글 번역: 목표 노드를 해제하고 링크를 다시 연결함
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
