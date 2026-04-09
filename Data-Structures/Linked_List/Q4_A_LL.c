//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 4 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList


//////////////////////// function prototypes /////////////////////////////////////

// You should not change the prototype of this function
void moveEvenItemsToBack(LinkedList *ll);

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
	ll.head = NULL;
	ll.size = 0;


	printf("1: Insert an integer to the linked list:\n");
	printf("2: Move all even integers to the back of the linked list:\n");
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
			moveEvenItemsToBack(&ll); // You need to code this function
			printf("The resulting linked list after moving even integers to the back of the linked list is: ");
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

void moveEvenItemsToBack(LinkedList *ll)
{
	/* add your code here */
	/* 빈 리스트면 옮길 노드가 없으므로 바로 종료 */
	if (ll == NULL || ll->head == NULL){
		return;
	}

	ListNode *prevNode = NULL;
	ListNode *currNode = ll->head;
	ListNode *evenNode = NULL;
	ListNode *evenHead = NULL;
	
	while (currNode != NULL){
		/* 홀수 노드는 원래 리스트에 남겨두고 다음 노드로 이동 */
		if (currNode->item % 2 == 1){
			prevNode = currNode;
			currNode = currNode->next;
		}
		else{
			/* 짝수 노드는 먼저 현재 리스트에서 제거한 뒤,
			   별도로 관리하는 짝수 체인 뒤에 붙인다.
			   이렇게 하면 temp 포인터 없이도 head 또는 prevNode->next에서
			   다음에 검사할 노드를 다시 읽어올 수 있다. */
			if (prevNode == NULL){
				ll->head = currNode->next;
			}
			else{
				prevNode->next = currNode->next;
			}

			/* 제거한 짝수 노드를 짝수 체인의 맨 뒤에 연결 */
			if (evenNode == NULL){
				evenHead = currNode;
				evenNode = currNode;
			}
			else{
				evenNode->next = currNode;
				evenNode = currNode;
			}
			evenNode->next = NULL;

			/* currNode는 이미 리스트에서 빠졌으므로,
			   다음에 볼 노드는 새 head 또는 prevNode->next가 된다. */
			if (prevNode == NULL){
				currNode = ll->head;
			}
			else{
				currNode = prevNode->next;
			}
		}
	}

	/* 홀수 리스트 뒤에 짝수 체인을 붙인다.
	   홀수가 하나도 없었다면 짝수 체인이 전체 리스트가 된다. */
	if (prevNode != NULL){
		prevNode->next = evenHead;
	}
	else{
		ll->head = evenHead;
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
	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}


	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
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
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
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
