#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define BALLOT_SIZE 1000000
#define MAX_TREE_SIZE 500
#define NUM_VOTERS 3

#define NO_COLOR -1
#define BLACK 0
#define RED 1

#define LEFT 0
#define RIGHT 1

typedef struct red_black_tree_node * NodeP; //node pointer
typedef struct red_black_tree_node Node; //node

struct red_black_tree_node {
	short color;
	int value;
	NodeP left, right, parent;
};

NodeP Plant_Tree() {
	//sets all pointers to NULL as well
	NodeP head = calloc(1, sizeof(Node));
	
	if (head == NULL) {
		fprintf(stderr, "Unable to allocate enough space.\n");
		return NULL;
	}
	head->value = -1;
	head->color = NO_COLOR;
	return head;
}

void Free_Recursive(NodeP Tree) {
	if (Tree->left != NULL) {
		Free_Recursive(Tree->left);
	}
	if (Tree->right != NULL) {
		Free_Recursive(Tree->right);
	}
	free(Tree);
}

void Deforestation(NodeP Tree) {
	//if the tree has been initialized
	if (Tree != NULL) {
		//and it currently contains values
		if (Tree->left != NULL) {
			//start freeing from the root
			Free_Recursive(Tree->left); //left and right point to same thing at root
		}
		free(Tree);
	}
}

NodeP Create_New_Node(NodeP Parent, int Val) {
	NodeP newNode = calloc(1, sizeof(Node));
	
	if (newNode == NULL) {
		fprintf(stderr, "Unable to allocate space for a new node\n");
		exit(-1);
	}
	
	newNode->value = Val;
	newNode->color = RED;
	newNode->parent = Parent;
	return newNode;
}


NodeP Retrieve(NodeP Tree, int Val) {
	if (Tree == NULL) {
		fprintf(stderr, "Tree hasn't been initialized\n");
		return NULL;
	}
	
	if (Tree->left == NULL || Tree->right == NULL) {
		fprintf(stderr, "Tree contains no values\n");
		return NULL;
	}
	
	NodeP Current = NULL;
	//checking if we've been passed a subtree or the full tree
	//if it's a full tree, then case 1 will access the root of the tree to start
	//the search
	if ((Tree->left == Tree->right) && (Tree->left != NULL)) {
		Current = Tree->left;
	}
	else {
		Current = Tree;
	}
	
	while (1) { 
		if (Current == NULL) {
		//value not found
			return NULL;
		}
		
		if (Current->value > Val) {
			Current = Current->left;
		}
		else if (Current->value < Val) {
			Current = Current->right;
		}
		else if (Current->value == Val) {
		//value found
			return Current;
		}
	}
}
	
short Exists(NodeP Tree, int Val) {
	NodeP result = Retrieve(Tree, Val);
	if (result == NULL) {
		return 0;
	}
	return 1;
}

void Left_Rotate(NodeP in) {

	NodeP Root = in;
	
	//Root is an initialized node
	assert(Root != NULL);
	//Root has a defined right child
	assert(Root->right != NULL);
	//Root is not the pointer to the tree
	assert(Root->color != NO_COLOR);

	NodeP Rchild = Root->right;
	
	//Turn the Root's right subtree to the left subtree of Root's right child
	//then set the parent of the new subtree
	Root->right = Rchild->left;
	
	if (Root->right != NULL) {
		Root->right->parent = Root;
	}
	
	//set the parent of right child to be the pivot's parent
	//fix any now broken left and right pointers one level up
	Rchild->parent = Root->parent;
	if (Root->parent->right == Root) {
		Root->parent->right = Rchild;
	}
	if (Root->parent->left == Root) {
		Root->parent->left = Rchild;
	}
	
	//complete the rotation placing Root underneath Rchild
	Rchild->left = Root;
	Root->parent = Rchild;
}

void Right_Rotate(NodeP in) {
	NodeP Root = in;
	
	//Root is an initialized node
	assert(Root != NULL);
	//Root has a defined left child
	assert(Root->left != NULL);
	//Root is not the pointer to the tree
	assert(Root->color != NO_COLOR);


	NodeP Lchild = Root->left;
	
	//Turn the Root's right subtree to the left subtree of Root's right child
	//then set the parent of the new subtree
	Root->left = Lchild->right;
	
	if (Root->left != NULL) {
		Root->left->parent = Root;
	}
	
	//set the parent of right child to be the pivot's parent
	//fix any now broken left and right pointers one level up
	Lchild->parent = Root->parent;
	if (Root->parent->right == Root) {
		Root->parent->right = Lchild;
	}
	if (Root->parent->left == Root) {
		Root->parent->left = Lchild;
	}
	
	//complete the rotation placing Root underneath Rchild
	Lchild->right = Root;
	Root->parent = Lchild;
}

int Insert(NodeP Tree, int Val) {
	//printf("Insert %d\n", Val);
	if (Tree == NULL) {
		fprintf(stderr, "Tree hasn't been initialized\n");
		return -1;
	}
	
	//Case 1
	if (Tree->left == NULL && Tree->right == NULL) {
		NodeP root = Create_New_Node(Tree, Val);
		Tree->left = Tree->right = root;
		root->color = BLACK; //root must be black
		return 0;
	}
	
	NodeP Current = Tree->left; //start of the actual values in the tree
	NodeP Parent = NULL;
	int side = -1;

	while (1) { 
		if (Current == NULL) {
			//Insert value
			break;
		}
		
		if (Current->value > Val) {
			Parent = Current;
			Current = Current->left;
			side = LEFT;
		}
		else if (Current->value < Val) {
			Parent = Current;
			Current = Current->right;
			side = RIGHT;
		}
		else if (Current->value == Val) {
		//value already in tree
			return 0;
		}

	}
	
	assert(Parent != NULL);
	NodeP newInsertion = Create_New_Node(Parent, Val);
	
	if (side == RIGHT) {
		assert(Parent->right == NULL);
		Parent->right = newInsertion;
	}
	
	else {
		assert(Parent->left == NULL);
		Parent->left = newInsertion;
	}
	
	//Case 2
	if (Parent->color == BLACK) {
		return 0;
	}
	
	
	Current = newInsertion;
	NodeP GrandParent = NULL;
	NodeP Uncle = NULL;
	
	while (1) {
		Parent = Current->parent;
		
		//Case 1
		if (Parent->color == NO_COLOR) {
			Current->color = BLACK;
			break;
		}
		
		//Case 2
		if (Parent->color == BLACK) {

			break;
		}
		
		GrandParent = Parent->parent;
		if (GrandParent->left == Parent) {
			Uncle = GrandParent->right;
		}
		else {
			Uncle = GrandParent->left;
		}
		
		//Case 3
		if ((Uncle != NULL) && (Uncle->color == RED)) {
			Parent->color = BLACK;
			Current->color = BLACK;
			GrandParent->color = RED;
			Current = GrandParent;
			continue;
		}
		
		//Case 4
		if ((Current == Parent->right) && (Parent == GrandParent->left)) {
			Left_Rotate(Parent);
			Parent = GrandParent->left;
			Current = Parent->left;

			
		}
		else if ((Current == Parent->left) && (Parent == GrandParent->right)) {
			Right_Rotate(Parent);
			Parent = GrandParent->right;
			Current = Parent->right;
			
		}
		
		//Case 5
		Parent->color = BLACK;
		GrandParent->color = RED;
		
		if (Current == Parent->left) {
			Right_Rotate(GrandParent);
		}
		else {
			Left_Rotate(GrandParent);
		}
		break;
	}
	
	return 0;
	
}

NodeP findMin(NodeP Tree) {
	if (Tree == NULL) {
		fprintf(stderr, "Tree was empty in findMin call\n");
		return NULL;
	}
	
	NodeP Current = NULL;
	//checking if we've been passed a subtree or the full tree
	//if it's a full tree, then case 1 will access the root of the tree to start
	//the search
	if ((Tree->left == Tree->right) && (Tree->left != NULL)) {
		Current = Tree->left;
	}
	else {
		Current = Tree;
	}
	
	//while there is another left node, keep traversing to that left node
	while (Current->left != NULL) {
		Current = Current->left;
	}
	
	return Current;
}

int Delete_By_Reference(NodeP Tree, NodeP toDelete) {

	//if the node has no children, delete it and delete the reference in the parent node.
	//covers the case where the root node is being deleted and the tree will be empty
	//as it performs separate checks for left and right to set both back to NULL which
	//indicates that a tree is empty
	if (toDelete->right == NULL && toDelete->left == NULL) {
		
		NodeP Parent = toDelete->parent;
		if (Parent->right == toDelete) {
			Parent->right = NULL;
		}
		if (Parent->left == toDelete) {
			Parent->left = NULL;
		}
		
		free(toDelete);
		return 0;
	}
	
	//if exactly one child is NULL
	else if ((toDelete->right == NULL) != (toDelete->left == NULL)) {
		NodeP next;
		NodeP Parent = toDelete->parent;
		assert(Parent != NULL);

		if (toDelete->right != NULL) {
			next = toDelete->right;
		}
		else {
			next = toDelete->left;
		}
		
		if (Parent->right == toDelete) {
			Parent->right = next;
		}
		if (Parent->left == toDelete) {
			Parent->left = next;
		}
		
		next->parent = Parent;

		free(toDelete);
		return 0;
	}
	
	//if neither children are NULL
	else if (toDelete->right != NULL && toDelete->left != NULL) {
		NodeP Replace = findMin(toDelete->right);
		int RepVal = Replace->value;
		
		//will trigger case 1 or 2 exclusively as it is a minimum, so it has no left child
		//also, minimum will not be the root of the tree as case 3 only triggers for a node
		//that has two children.  Therefore if the root is the one requested for deletion
		//and it reaches this case, by definition it has a left child which will be smaller.
		Delete_By_Reference(Tree, Replace);
		
		toDelete->value = RepVal;
		return 0;
	}
	
	return -1;
}


	
int Delete(NodeP Tree, int Val) {
	NodeP toDelete = Retrieve(Tree, Val);
	if (toDelete == NULL) {
		fprintf(stderr, "Didn't find %d in tree\n", Val);
		return -1;
	}
	return Delete_By_Reference(Tree, toDelete);
	
}


void shuffle(int *array) {    
    if (BALLOT_SIZE > 1) {
        int currPos;
        for (currPos = BALLOT_SIZE - 1; currPos > 0; currPos--) {
            int swapPos = (drand48()*(currPos+1));
            int toSwap = array[swapPos];
            array[swapPos] = array[currPos];
            array[currPos] = toSwap;
        }
    }
}

int main() {
	if (BALLOT_SIZE > 1000) {
		
		int numTrees = ceil((float)BALLOT_SIZE/MAX_TREE_SIZE);
		
		NodeP * IndexTrees = malloc(numTrees*sizeof(NodeP));
		
		if (IndexTrees == NULL) {
			fprintf(stderr, "Unable to initialize IndexTrees\n");
			exit(-1);
		}
		
		for (int i=0; i < numTrees; i++) {
			IndexTrees[i] = Plant_Tree();
			if (IndexTrees[i] == NULL) {
				fprintf(stderr, "Failed to initialize Tree\n");
				exit(1);
			}
		}
		
		for (int i = 0; i < BALLOT_SIZE; i++) {
			Insert(IndexTrees[(int)floor((float)i/MAX_TREE_SIZE)], i);
		}
		
		printf("FINISHED INSERTIONS\n");
		
		int *randOrd = malloc(BALLOT_SIZE*sizeof(int));
		if (randOrd == NULL) {
			fprintf(stderr, "Unable to allocate enough space for random ordering\n");
			exit(-1);
		}
		
		for (int i = 0; i<BALLOT_SIZE; i++) {
			randOrd[i] = i;
		}
		
		printf("Shuffling the order for deletions\n");
		shuffle(randOrd);
		
		
		printf("Deletions\n");
		for (int i = 0; i< BALLOT_SIZE; i++) {
			Delete(IndexTrees[(int)floor((float)randOrd[i]/MAX_TREE_SIZE)], randOrd[i]);
		}
		
		printf("FINISHED DELETIONS\n");
		
		free(randOrd);
		
		for (int i=0; i < numTrees; i++) {
			Deforestation(IndexTrees[i]);
		}
		
	}
	return 0;
}
