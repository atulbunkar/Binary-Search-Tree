//DSA ASSIGNMENT 1: THREADED BST. 
//ROLL NO: 214101011 , 
//SEM 1 ||Mtech|| CSE.

#include<iostream>
#include<queue>
#include <algorithm>
#include <fstream>
#include<vector>
using namespace std;


struct llist{                        // linked list, needed in some functions.
    int data;
    llist* next;
    llist(){}                         //default constructor
    llist(int _data){              // For most of the functions , I have used a naming convention for variables where parameter names that are very common 
        data = _data;                      //starts with an underscore( _ ) to easily differentiate from local variables.
        next = NULL;
    }
    llist(int _data ,llist* _next ){ //another parameterised constructor
        data = _data;
        next = _next;
    }      
};

static int node_id = 0; // used to assign tree nodes unique IDs.
struct treeNode{         // A single Binary search tree node.
    int val;                 // value stored at the node
    treeNode* left ;        // left pointer
    treeNode* right ;       //right pointer
    int Rcount;   // this will be needed in kthLargest function. Stores the num of nodes in right subtree for every node.
    //This is updated after each insert and delete operation accordingly.

    bool leftThread ; 
    bool rightThread ; 
    // If left/rightThread is false, pointer of that treeNode is pointing to left/right child node respectively.
    // AND if it is true ,pointer is pointing to inorder predecessor/successor of that treenode respectively.

    int nodeID ; // Used while printing tree.

    treeNode(int _val){
        val = _val ;
        Rcount = 0;
        left = NULL;
        right = NULL;
        nodeID = node_id++;  // This will be used to get graph .DOT file generated.
        //Whenever a new treenode is created, node_id is assigned to nodeID of that new node, then node_id is incremented by 1 for next node.
        //So it is unique for every node created. When deleted , no need to change any nodeID.
    }
    
};

class BST{
    private:        // I set all utility functions in private and all functions to call via main in public.
        treeNode* pred(treeNode* ptr);                         // Utility function for predecessor, will be used in delete, Reverse InOrder operation.
        treeNode* succ(treeNode* ptr);                                          // For a given ptr , returns its successor ptr.
        treeNode* delete0(treeNode* _root ,treeNode* parent , treeNode* ptr) ; // to delete node with 0 children , i.e leaf node
        treeNode* delete1(treeNode* _root , treeNode* parent ,  treeNode* ptr) ; // to delete node with 1 child either left or right
        treeNode* delete2(treeNode* _root ,treeNode* parent , treeNode* ptr); // to delete node with 2 children
        vector<treeNode*> split_utility(treeNode* _root , int k, int* visitd); // Finds 2 nodes which are <=k  ans >k resp in O(h) time.
        

    public:
        BST(){};      // Default Contructor
        // ALL 9 functions are written as per their task number.
        treeNode* insert(treeNode* _root, int _val);            // All operation modifies threads in insert and delete.
        treeNode* search(treeNode* _root , int _val);
        treeNode* deleteNode(treeNode* _root , int _val);     // This call all 3 delete functions accordingly!!
        llist* reverseInorder(treeNode* _root);              // Uses pred function to get predecessor.
        int successor(treeNode* ptr);                       // Just to return int value of successor, calls succ.
        llist* allElementsBetween(treeNode* _root , int k1, int k2);
        int kthLargest(treeNode* _root, int k);
        vector<treeNode*> split(treeNode* _root , int k); // calls split_utility then Removes any connected threads in O(h) time 
                                                         // Then calls print_inorder to print 2 trees in O(n) time.
        void printTree(treeNode* _root, const string& name,const string&  label);
        void print_inorder(treeNode* _root);        //To print inorder of split nodes.

};

//-------------------------------------------------------------------------------------------------------------------------------------//

//TASK 1: Insert an element in BST !
treeNode* BST::insert(treeNode* _root, int _val) {

    treeNode* cur = _root;        // For iterating through the tree.
    treeNode* parent = NULL;      // This will give the position where treeNode(_val) element will be inserted.

    while(cur){        
        if(cur->val == _val){
            cout<< "ELEMENT "<< _val <<" ALREADY EXIST IN BST\n";
            return _root;
        }

        parent = cur;              // updating parent at every loop along with cur as this will be parent to treeNode(_val);
        if(_val < parent->val){              
            if(parent->leftThread == false){    // If parent has a left child keep iterating to left subtree else break out, we found the parent node.
                cur = cur->left;
            }
            else 
                break ;
        }
        else if(_val > parent->val){
            parent->Rcount++;                  // Whenever a value is inserted to right of any node , that node's rcount is incremented.
            if(parent->rightThread == false){  // If parent has a right child keep iterating to right subtree else break out, we found the parent node.
                cur = cur->right;
            }
            else 
                break ;
        }
    }
    // Now we have parent pointer just above where node(_val) can be inserted.

    treeNode* node = new treeNode(_val) ;                 // This node holds new value, will be linked to tree with ptr operations.
    node->leftThread = true ;                              // Node will point to its predecessor/successor after ptr operations.
    node->rightThread = true ;                             // So , I set them true.

    if(parent == NULL){  // If tree is empty , just make root point to node. 
        _root = node;
        node->left = NULL;
        node->right = NULL;
    }
    else{
        if(_val < parent->val) {                 // If value is lower than parent, it is put left of parent.
                node->left = parent->left;      // First ,we hold parent's left ptr as this will change next. This is now pointed by node's left ptr.
                parent->left = node;             //  Now we can change parent's left ptr to point to node. 
                node->right = parent;             // Finally , Node's successor is parent is set.
                parent->leftThread = false;        // tells us that left thread of parent is no longer pointing to its predecessor.
            }
            
        else if(_val > parent->val){            // If value is higher it is put right of parent.
                node->right = parent->right;    // First we hold parent's right ptr to node's right ptr i.e its successor.
                parent->right = node;           // Then , we change parent's right ptr to node.
                node->left = parent;            // node's left is parent coz that is node's predecessor. 
                parent->rightThread = false;     // tells us that right thread of parent is no longer pointing to its successor.      
            }
    }
    return _root;
}
//---------------------------------------------------------------------------------------------------------------------//

// utility function to return predecessor of ptr.
treeNode* BST::pred(treeNode* ptr){
    if (ptr->leftThread == true) // If leftThread is true, left  child is predecessor
        return ptr->left;
   // Else return rightmost child of left subtree.
   ptr = ptr->left; 
   while (ptr->rightThread == false) // this means there is right child to ptr
       ptr = ptr->right;
   return ptr;
}

// utility function to return successor of ptr.
treeNode* BST::succ(treeNode* ptr){
   // If rightThread is true , just return the right child , as that is its successor
   if (ptr->rightThread == true)
       return ptr->right;

   // Else return leftmost child of right subtree.
   ptr = ptr->right;
   while(ptr->leftThread == false)
       ptr = ptr->left;
   return ptr;

}

//---------------------------------------------------------------------------------------------------------------------------------//

//TASK 2: Search in BST
treeNode* BST::search(treeNode* _root, int _val){
    treeNode* temp = _root;                 // create a temporary root node to iterate.
    while(temp){
        if(temp->val == _val)return temp;       // if key is found return the node

        else if(_val < temp->val)temp = pred(temp); // else search according to value in left or right subtree
        else temp = succ(temp);
    }
    return NULL;  // If key is not found null is returned.

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
// 3 Utility Functions for delete !

//If the node to delete has no child, then delete0 is called.
treeNode* BST::delete0(treeNode* _root ,treeNode* parent , treeNode* ptr){
    // If Node to be deleted is root
   if (parent == NULL)
       _root = NULL;

   // If Node to be deleted is left of its parent
   else if (ptr == parent->left) {
       parent->leftThread = true;
       parent->left = ptr->left;
   }
   else {
       parent->rightThread = true;
       parent->right = ptr->right;
   }

   // Free memory and return new root
   free(ptr);
   return _root;
}

//If the node to delete has one child, then delete1 is called.
treeNode* BST::delete1(treeNode* _root ,treeNode* parent , treeNode* ptr){
   treeNode* child;           // Initialize child Node to store child of node to be deleted.


   if (ptr->leftThread == false) // if node to delete has left child.
       child = ptr->left;
   // if node to be deleted has right child.
   else
       child = ptr->right;

     // Node to be deleted is root Node than its parent is null. The child now becomes the root.
    if(parent == NULL)
       _root = child;

   else if(ptr == parent->left)  //Node to delete is left child of its parent. So, the child of node becomes parents left node.
       parent->left = child;
   else
       parent->right = child;     //Node to delete is right child of its parent. So, the child of node becomes parents right node.

   // Find successor and predecessor of deleted node
   treeNode* suc = succ(ptr);
   treeNode* pre = pred(ptr);

   // If ptr has left subtree.
   if(ptr->leftThread == false)
       pre->right = suc;

   // If ptr has right subtree.
   else {
       if(ptr->rightThread == false)
           suc->left = pre;
   }

   free(ptr);     // free pointer memory!
   return _root;

}

//If the node to delete has 2 child, then delete2 is called.
// We find inorder successor of Node to be deleted and then we copy the information of this inorder successor into current Node. 
//After this inorder successor Node is deleted using either delete0 or delete1 explained above.
treeNode* BST::delete2(treeNode* _root ,treeNode* parent , treeNode* ptr){
    
   treeNode* parent_suc = ptr;     // This stores inorder successor and its parent which is ptr itself.
   treeNode* suc = ptr->right;     // suc is sucessor of ptr.

   // Find leftmost child of successor
   while (suc->leftThread==false) {
       parent_suc = suc;
       suc = suc->left;
   }
   //suc is now the inorder successor to ptr.
   ptr->val = suc->val;  // we replace that node's value in current ptr and delete the successor node.
   ptr->Rcount = suc->Rcount; // This makes current ptr to store successors right subtree nodes count.

   if(suc->leftThread == true && suc->rightThread == true)   // checks if successor has no child so call delete0 to delete suc node only.
       _root = delete0(_root, parent_suc, suc);
   else                                           //this is executed when successor has one child , so delete1 is called to delete suc node accordingly
       _root = delete1(_root, parent_suc, suc);      

    return _root;
}


//-----------------------------------------------------------------------------------------------------------------------------------------//
//TASK 3: Delete a Node from BST. This calls all 3 delete functions
treeNode* BST::deleteNode(treeNode* _root, int _key){
    
   treeNode *parent = NULL; //Parent is stored separately so if child i
   treeNode*ptr = _root;

   // Set true if key is found !
   bool present = false;

   // Search key in BST : find Node and its parent.
   while (ptr != NULL) {
       if(_key == ptr->val) {                  // If key value is greater than current , move right
           present = true;
           break;
       }
       parent = ptr;
       if(_key < ptr->val) {                    //if key value is less than current , move left
           if(ptr->leftThread == false)
               ptr = ptr->left;
           else
               break;
       }
       else if(_key > ptr->val){
           ptr->Rcount--;          //All node's rcount is decremented if any node is deleted from right subtree of that node.
           if(ptr->rightThread == false)
               ptr = ptr->right;
           else
               break;
        }
   }

   if (!present)                                                 //If key is not found
       cout << "key Value: "<< _key<< " not present in BST\n" ;

   //If ptr has 2 children, Call delete2.
   else if(ptr->leftThread == false && ptr->rightThread == false)
       _root = delete2(_root, parent, ptr);

   //If ptr has one child left or right , Call delete1.
   else if(ptr->leftThread == false || ptr->rightThread == false)
       _root = delete1(_root, parent, ptr);

   //If ptr has no child, call delete0.
   else
       _root = delete0(_root, parent, ptr);

   return _root;
    
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//TASK 4 : Return linkedlist of reverse Inorder traversal of BST
llist* BST::reverseInorder(treeNode* _root){
    if(_root == NULL)return NULL;
    treeNode* node = _root;

    while(node->right){          // To get the rightmost child of BST.
        node = node->right;
    }
    //node now holds the max element treenode!
    int _val = node->val;

    llist* head = new llist(_val);        // create a linked list node and store max node in head.
    llist* cur = head;                   // to iterate through lnkd list , set cur ptr to head.
    treeNode* prev = pred(node);        //get predecessor of node and store in prev.
    // The function pred uses left threads to get predecessors of tree node.

    while(prev){

        _val = prev->val;                // Get value of element from prev treenode
        llist* temp = new llist(_val);  // create new llist node and store that value.
        cur->next = temp;               // set temp as next of cur ptr.
         
        prev = pred(prev);            // get predecessor of prev node now. i.e the next predecessor
        cur = cur->next;               // move one node ahead in llist , i.e this will now point to temp and so after
    }                               // every loop , this will keep moving ahead with temp. 
    return head;                   //Finally,  Return the head of linkedlist which stores the max element.
}

//-----------------------------------------------------------------------------------------------------------------------------------------//

// TASK 5: Return successor element's value for a given node pointer.
int BST::successor(treeNode *ptr){
    treeNode* s = succ(ptr);
    return s->val ;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//task 6: Split the BST based on k

//Utility function for split to get 2 root nodes with <=k and >k elements
vector<treeNode*> BST::split_utility(treeNode* _root, int k ,int* visitd){ // This takes O(h) to get the 2 roots.

    vector<treeNode*> trees(2,NULL);         // To store 2 roots, 1st is node which is just <=k and second one just >k.
     if(_root == NULL)return trees;

     if(visitd[_root->nodeID] == 1){ //if visited , then return.
         return trees;
     }
     visitd[_root->nodeID] = 1; // This node is visited

     int index_a , index_b ;        //ind to decide which tree should hold the node.
     treeNode* temp; 

     if(_root->val > k){          // if root value if greater , seacrh in left subtree and store current node in 2nd tree. i.e index =1.
        index_a = 1;                //so root will be root of bigger tree
        index_b = 0;
        temp = pred(_root);       // we use pred to get predecessor and use this as root for next recursion
     }
     else {                 // if root value if smaller , seacrh in right subtree and store current node in 1st tree. i.e index =0 .
        index_a = 0;        // so root will be root of smaller tree.
        index_b = 1;
        temp =  succ(_root);  //same as for pred but we get successor as root for next recursion
     }
     trees[index_a] = _root;
     vector<treeNode*> rem_tree =  split_utility(temp , k, visitd);  // Recurse in left/right subtree to get node which is smaller/greater than k.
     
     trees[index_b] = rem_tree[index_b];  //after coming back from every recursion, the larger element node is made root of big tree.

     return trees; //returns the vector of 2 pointers.
}

//The utility function is called in split along with the print_inorder function , given below.
vector<treeNode*> BST::split(treeNode* _root, int k ){ // O(h) to remove the connected threads from 2 trees.

    int visitd[node_id] = {0} ; //keep track of visited nodes, size is node_id , i,e no of nodes created.
    vector<treeNode*> trees = split_utility(_root,k,visitd); //Get the 2 root ptr from utilty functn

     //Now we have 2 roots , 1st holds root for smaller elements tree ans 2nd index holds root of bigger elements tree.

     //We have to remove any threads that may be connecting the 2 trees.
     // These can be the threads outgoing from the left children in Bigger element tree to other tree.
     // or from right children in smaller element tree to other tree.

     //Removing thread from right subtree of smaller element tree if it connects to the bigger element tree.
     treeNode* parent1 = trees[0];            //Root of smaller tree
     treeNode* rightchild = parent1->right ;
     while(rightchild){
        if(rightchild->val > k){  // We just needs to find the right child which is >k , and remove its parents right ptr.
            parent1->right = NULL;
            break;
        }
        else{
            parent1 = rightchild;
            rightchild = rightchild->right ;
        }
    }
     //Removing thread from left subtree of bigger element tree if it connects to the smaller element trees.
     treeNode* parent2 = trees[1];                 // root of larger element tree
     treeNode* leftchild = parent2->left; 
     while(leftchild){
        if(leftchild->val <= k){
            parent2->left = NULL;
            break;
        }
        else{
            parent2 = leftchild;
            leftchild = leftchild->left ;
        }
    }
    cout<< "Inorder Traversal of the BST with elements<= " << k <<":- \n"  ;
    print_inorder(trees[0]);
    cout<< "Inorder Traversal of the BST with elements > " << k <<" :- \n" ;
    print_inorder(trees[1]);
    cout<< "\n";
    return trees;

}

void BST::print_inorder(treeNode* _root){        // To print the split trees in inorder.
    if(_root == NULL)return ;
    treeNode* node = _root;

    while(node->left){          // To get the leftmost child of BST.
        node = node->left;
    }
    //node now holds the min element treenode!

    int _val;          //store the node->val i.e the element;   
          
    while(node){
        _val = node->val;
        cout<< _val << "  ";              //print the node values  
        node = succ(node) ;        // get successor of node now. i.e the next successor.
    }  
    cout<< endl;                           
    return ;                   
}


//-----------------------------------------------------------------------------------------------------------------------------------------//
//TASK 7: Return Lnkdlist containing all elements between k1 and k2;
llist* BST::allElementsBetween(treeNode* _root, int k1, int k2){ // O(h) to find start and end Pointers using pred and succ functions.
    treeNode* temp = _root;  
    if(_root == NULL)return NULL; 

    treeNode* start = _root; 
    treeNode* end = _root;
    while(succ(start) && start->val <k1){ //if root is <k1 ,move ahead till we find node val >k1
        start = succ(start);
    }

    while(pred(start) && pred(start)->val >= k1){  
        start = pred(start);              //To get lower bound i.e node whose value is just >= k1 , keep getting predecessor in while loop.
    }

    while(pred(end) && end->val >k2){
        end = pred(end);
    }
    while(succ(end) && succ(end)->val <= k2){  //To get upper bound i.e node whose value is just <= k2 , keep getting successor in while loop.
        end = succ(end);   
    }

    int _val = start->val ;            // get value of first element
    llist* head = new llist(_val);    // create a new linkdlist node and store that value in it.
    llist* cur = head;               //For iterating in linkedlist.

    while(start != end){                  // O(n) to store all in linked list.
        start = succ(start);             // get next element from succ function which returns ptr to successor node
        _val = start->val ;                // get its value
        llist* temp = new llist(_val);      // create a new linklist node and store value in it.
        cur->next = temp;                    // set next ptr of cur lnkdlist node to point to temp.
        cur = cur->next;                     // move to  next node in lnkdlist which is temp.
    }
    
    return head;                               //return head of linked list
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//TASK 8 : Return kth largest element
int BST::kthLargest(treeNode* _root, int k){  // runs in O(h) time using morris traversal
    treeNode* temp = _root;
    while(true){
        if(temp == NULL)break;

        int rightNodesCount = temp->Rcount ;     // Count of nodes right of root node.

        if(k == rightNodesCount+1 ){             // If root's rightNodesCount +1 == k , we say root is kth largest in BST !
            return temp->val;
        }
        else if(k < rightNodesCount+1 ){         // If k is less than nodes in right+1 , then we search kth largest in right subtree.
            
            temp = temp->right ;
            continue;
        }
        else{
            temp = temp->left ;                 // If k is greater than nodes in right subtree+ 1 ,                                                       
            k = k- rightNodesCount-1;
            continue;                       //then we search (k- RightNodesCount )th largest in right subtree.
        }
    }                                                   
    return -1;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//TASK 9 : Print Tree! Here I am creating a DOT file to get graph from it . The .png images are attached in PDF file .
// Each Image has a label for clear understanding.

void BST::printTree(treeNode* _root, const std::string&  name ,const std::string&  label ){  //creating a .DOT file
    int nullcount = 0;
    ofstream myfile;
    myfile.open(name); 
    queue<treeNode*> nodes;                 //Needed for Level order traversal , BFS of the BST.
    nodes.push(_root);

    int visited[node_id] = {0};            // To keep track of visited nodes as they are connected to each other ,saves program from going into loop.

    myfile<<"digraph G{\n" ;                 //DOT code.
    myfile<<"node[shape=record];\n" ;
    myfile<<"label = "<< "\" "<< label <<"\"" << endl;

    while(!nodes.empty()){
        treeNode* cur =  nodes.front();  nodes.pop();                     // visited is for checking visited nodes.
        if(visited[cur->nodeID] == 1 )                              //check if already visited so continue;
            continue;
        visited[cur->nodeID] = 1 ; 
       
        myfile<< cur->nodeID<<"[label = \"<f1> Left ptr"<< "|<f2>  "<< cur->val  << "| <f3> Right ptr" << " \"] \n"; //Code for node data

        //code for edge data.
        if(cur->left){
            myfile<< cur->nodeID << ":f1 -> " << cur->left->nodeID << endl; // get edge  for root to its
            nodes.push(cur->left); 
        }
        else{
            myfile<< cur->nodeID << ":f1 -> null" << nullcount++  << endl;  // if left points to nullptr
        }

        if(cur->right){
             myfile<< cur->nodeID << ":f3 -> " <<cur->right->nodeID <<endl;
             nodes.push(cur->right);
         }
         else{
            myfile<< cur->nodeID << ":f3 -> null" << nullcount++ << endl;  //iF right points to null
        }
    }
    myfile<< "}\n";
    myfile.close();
}


//-----------------------------------------------------------------------------------------------------------------------------------------//

int main(){
    //Working of all functions 
    BST bst;
    treeNode *root = NULL;
    cout<< "START OF OUTPUT\n";

    //TASK 1 : insert
    root = bst.insert(root,5);
    root = bst.insert(root,7);
    root = bst.insert(root,-2);
    root = bst.insert(root,6);
    root = bst.insert(root,11);
    root = bst.insert(root,6); // testcase: inserting again 
    root = bst.insert(root,0);
    bst.printTree(root ,"insert1.txt", "After first insertion of all nodes"); //parameters are : root ptr ,filename, graph label.
                                                                    // check images of tree in PDF and output in console.
    cout<< "\nAfter first insertion of all nodes\n";
    bst.print_inorder(root);                                   //prints inorder of bst.

    root = bst.insert(root,4);
    root = bst.insert(root,-5);
    root = bst.insert(root,8);
    bst.printTree(root ,"insert2.txt","After  inserting 3 more nodes "); // Task 9
    cout<< "After  inserting 3 more nodes \n";
    bst.print_inorder(root);
    cout<<endl;
    

    //TASK 2:Search
    cout<< "SEARCH : \n";
    treeNode* result = bst.search(root , 5);
    if( result == NULL){  // if not present
        cout<< "5 not present !\n";
    }
    else{
        cout<< "5 is present !\n";
    }
    
    result = bst.search(root , 100);
    if( result == NULL){  // if not present
        cout<< "100 not present !\n";
    }
    else{
        cout<< "100 is present !\n";
    }
    cout<< endl;

    //Task 8 :kth largest
    cout<< "Kth largest :- \n";
    cout<< "See the inorder traversal:- \n";
    bst.print_inorder(root);
    int k = 2;

    int ans = bst.kthLargest(root, k);
    cout << "kth largest element in BST for k = "<< k << " is : " << ans  << endl;
    
    k = 4;
    ans = bst.kthLargest(root, k);
    cout << "kth largest element in BST for k = "<< k << " is : " << ans << endl;
    cout << "\n\n";


    //TASK 3: Delete
    cout<< "DELETE : \n";
    root = bst.deleteNode(root, 100); //testcase : doesn't exist.
    
    root= bst.deleteNode(root, 5);                                          
    bst.printTree(root ,"delete1.txt","After deleting root node");            
    cout<< "After deleting root node: \n";
    bst.print_inorder(root);

    root = bst.deleteNode(root, 7); 
    bst.printTree(root ,"delete2.txt", "After deleting node 7");
    cout<< "After deleting node 7: \n";
    bst.print_inorder(root);
    cout<<endl;

    //TASK 4:-
    //Printing Reverse inorder of BST

    llist* ri = bst.reverseInorder(root);
    cout<< "REVERSE INORDER ELEMENTS :-\n";
    while(ri){
        cout << ri->data <<  "\t";
        ri = ri->next ;
    }
    cout<<endl;

    //TASK 5: print successor
    int sn = bst.successor(root);
    cout<< "Successor of root node is : " <<  sn << endl;
    cout<<endl;


    //TASK 7: All elements between
    cout<< "Find all elements between k1, k2 :-  \n";
     llist* aeb = bst.allElementsBetween(root, 2,7);  // k1,k2 need not be in BST.
    cout<< "ALL ELEMENT Between 2,7 in BST :-\n";
    while(aeb){
        cout << aeb->data << "\t";
        aeb = aeb->next ;
    }
    cout<< endl;

    //Another Example
    llist* aeb2 = bst.allElementsBetween(root,-100,0);
    cout<< "ALL ELEMENT Between -100,0 in BST :-\n";
    while(aeb2){
        cout << aeb2->data << "\t";
        aeb2 = aeb2->next ;
    }
    cout<< "\n\n";

    //Task 6: Split. 
    cout<< "AFTER SPLIT : \n";
    vector<treeNode*> roots = bst.split(root,5 ); // k is not in tree. 
    //This prints the 2 trees after split and graph image is in PDF.
    treeNode* x = roots[0];
    treeNode* y = roots[1];

    bst.printTree(x ,"smallTree.txt","Smaller elements tree After splitting main tree");
    bst.printTree(y ,"BigTree.txt","Bigger elements tree After splitting main tree");

    cout<< "END OF OUTPUT\n";
    
}

//END OF ASSIGNMENT !! Thank You !!
