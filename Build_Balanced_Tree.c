#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>


//Much smarter than making so many insertions into a RB tree
//This approach builds a balanced search tree out of a list of values in one step
//it also has multiple trees so searching speed is adjustable as you can limit the depth
//of the search at the cost of more memory
#define BALLOT_SIZE 10000000
#define MAX_SIZE_OF_TREE 500

 
struct Node
{
    int value;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
};
 
struct Node* newNode(struct Node* parent, int value)
{
    struct Node* node = calloc(1, sizeof(struct Node));
    
    if (node == NULL) {
    	fprintf(stderr, "Unable to allocate more space for new nodes\n");
    	exit(-1);
    }
    
    node->value = value;
    node->parent = parent;
    return node;
}

struct Node * findMin(struct Node * Tree) {
	if (Tree == NULL) {
		fprintf(stderr, "Tree was empty in findMin call\n");
		return NULL;
	}
	
	struct Node * Current = NULL;
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

struct Node * Retrieve(struct Node * Tree, int Val) {
	if (Tree == NULL) {
		fprintf(stderr, "Tree hasn't been initialized\n");
		return NULL;
	}
	
	if (Tree->left == NULL || Tree->right == NULL) {
		fprintf(stderr, "Tree contains no values\n");
		return NULL;
	}
	
	struct Node * Current = NULL;
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

int Delete_By_Reference(struct Node * Tree, struct Node * toDelete) {

	//if the node has no children, delete it and delete the reference in the parent node.
	//covers the case where the root node is being deleted and the tree will be empty
	//as it performs separate checks for left and right to set both back to NULL which
	//indicates that a tree is empty
	if (toDelete->right == NULL && toDelete->left == NULL) {
		
		struct Node * Parent = toDelete->parent;
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
		struct Node * next;
		struct Node * Parent = toDelete->parent;
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
		struct Node * Replace = findMin(toDelete->right);
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


	
int Delete(struct Node* Tree, int Val) {
	struct Node * toDelete = Retrieve(Tree, Val);
	if (toDelete == NULL) {
		fprintf(stderr, "Didn't find %d in tree\n", Val);
		return -1;
	}
	return Delete_By_Reference(Tree, toDelete);
	
}

void Free_Recursive(struct Node * Tree) {
	if (Tree->left != NULL) {
		Free_Recursive(Tree->left);
	}
	if (Tree->right != NULL) {
		Free_Recursive(Tree->right);
	}
	free(Tree);
}

void Deforestation(struct Node * Tree) {
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

struct Node* sortedArrayToBST(struct Node * entryPoint, int arr[], int start, int end, 
								int * seen, int currDepth)
{
	// Get the middle element
    int mid = (start + end)/2;
    int midElement = arr[mid];
    
    

    //deny it from making any duplicate nodes
    //seen's length is log base 2 of BALLOT_SIZE
    //do note that the entire seen list is not being traversed, only the segment between 
    //position 0 and current recursion depth
    //binary search cannot be used here as seen is not guaranteed to be in a sorted order
    //depth is also not a good indicator, this case always triggers when the depth is one
    //less than lenSeen, but there are also legitimate cases where depth can be one less
    //than lenSeen
    //nor would there be an advantage to sorting the array every step
    //however, using another balanced tree here for the seen list could be advantageous
	for (int i = 0; i < currDepth; i++) {
		if (midElement == seen[i]) {
			return NULL;
		}
	}
	
	
    struct Node *root = newNode(entryPoint, arr[mid]);
    //printf("Created %d\n", arr[mid]);
    seen[currDepth] = arr[mid];
    
    // base case
    if (start+1 > end) {
      	return root;
 	}
    //printf("%d's left child is %d\n", arr[mid], arr[(start+mid-1)/2]);
    root->left =  sortedArrayToBST(root, arr, start, mid-1, seen, currDepth+1);
    //printf("%d's right child is %d\n", arr[mid], arr[(mid+1+end)/2]);
    root->right = sortedArrayToBST(root, arr, mid+1, end, seen, currDepth+1);
 	
    return root;
}
 
void in_Order(struct Node* node) {
	if (node == NULL) {
	}
	else {
		if (node->left != NULL) {
			in_Order(node->left);
		}
		printf("%d ", node->value);
		if (node->right != NULL) {
			in_Order(node->right);
		}
	}
}

int * In_Order_Non_Recursive(struct Node* entryPoint) {
	if (entryPoint == NULL) {
		return NULL;
	}
	//int stackSize = ceil((float)log(MAX_SIZE_OF_TREE)/log(2));
	//struct Node* stack = malloc(stackSize * sizeof(struct Node));

	return NULL;
}
	
	
int min(int x, int y) {
	if (x < y) {
		return x;
	}
	return y;
}
int max(int x, int y) {
	if (x > y) {
		return x;
	}
	return y;
}

void Depth_First_Heap_Assert(struct Node* node) {
	if (node->left != NULL) {
		assert(node->value > node->left->value);
		Depth_First_Heap_Assert(node->left);
	}
	
	if (node->right != NULL) {
		assert(node->value < node->right->value);
		Depth_First_Heap_Assert(node->right);
	}
}



int main()
{
	int numTrees = (int)ceil((float)BALLOT_SIZE/MAX_SIZE_OF_TREE);

	
	//struct Node * Container[numTrees];
	//int Counts[numTrees]; //keeps track of how many entries are in each tree
	struct Node ** Container = malloc(numTrees * sizeof(struct Node *));

    int lenSeen = ceil(log(MAX_SIZE_OF_TREE)/log(2));
    int *seen = malloc(lenSeen * sizeof(int));
    if (seen == NULL) {
    	fprintf(stderr, "Unable to allocate space for seen array\n");
    	exit(-1);
    }
 	
 	int size, offset;
	int * arr = malloc(MAX_SIZE_OF_TREE * sizeof(int));
 	for (int i = 0; i < numTrees; i++) {
 	
 		offset = i*MAX_SIZE_OF_TREE;
 		size = min(MAX_SIZE_OF_TREE, BALLOT_SIZE - offset);
 		//printf("\nSize is %d and offset is %d\n", size, offset);
 		
 		if (size < MAX_SIZE_OF_TREE) {
 			arr = realloc(arr, size*sizeof(int));
 		}

 		for (int j = 0; j < size; j++) {
    		arr[j] = j+offset;
    	}
 		
    	//new entry point to a tree
    	Container[i] = malloc(sizeof(struct Node));
    	if (Container[i] == NULL) {
    		fprintf(stderr, "Unable to allocate space for a new container\n");
    		exit(-1);
    	}
    
    	struct Node *root = sortedArrayToBST(Container[i], arr, 0, size-1, seen, 0);
    	Container[i]->left = Container[i]->right = root;
		
 		assert(root->parent == Container[i]);
		Depth_First_Heap_Assert(root);
		
		for (int j = 0; j < size; j++) {
			Delete(Container[i], arr[j]);
		}
		
	}
	
	
	free(arr);
	free(seen);
	for (int i = 0; i < numTrees; i++) {
		Deforestation(Container[i]);
	}

	printf("\nPassed assertions\n");
	
    return 0;
    
}
