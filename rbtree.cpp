#include<bits/stdc++.h>
using namespace std;

struct RBTreeNode {
	bool black;
	int val;
	RBTreeNode *left,*right,*parent;
	RBTreeNode(int v = 0) {
		val = v;
		black = false;
		left = right = parent = NULL;
	}
};
struct RBTree {
private:
	RBTreeNode *root;

	void leftRoate(RBTreeNode*node) {
		RBTreeNode*tmp = node->left;//tmp可能为NULL
		RBTreeNode*parent = node->parent;
		RBTreeNode*grandpa = parent->parent;

		node->left = parent;
		parent->right = tmp;

		if(tmp) tmp->parent = parent;
		parent->parent = node;
		node->parent = grandpa;
		if(!grandpa) { 
			root = node;
			return;
		}
		if(grandpa->left == parent) grandpa->left = node;
		else grandpa->right = node;
	}
	void rightRoate(RBTreeNode*node) { //《不要随便传引用！！！》-- RBTreeNode*&node 在旋转时传入node->parent->left
		RBTreeNode*tmp = node->right;		 //然后第六行赋值parent->left = tmp(tmp == node) 直接导致node<-NULL,段错误找了好久
		RBTreeNode*parent = node->parent;
		RBTreeNode*grandpa = parent->parent;

		node->right= parent;
		parent->left= tmp;

		if(tmp) tmp->parent = parent;//tmp可能为NULL,这里段错误找了好久
		parent->parent = node;
		node->parent = grandpa;
		if(!grandpa) { 	//如果改变了根要把树的根给改变了，不然会出现只剩一个根的情况*********这里出错找了好久
			root = node;
			return;
		}
		if(grandpa->left == parent) grandpa->left = node;
		else grandpa->right = node;
	}

	void insertAdjust(RBTreeNode *root) {
		if(!root->parent) { //当前插入节点为根，变为黑
			root->black = true;
			return;
		}
		RBTreeNode* parent = root->parent;
		if(parent->black) {	//红连黑，什么都不用做
			return;
		}
		RBTreeNode* &grandpa = parent->parent;	//父节点为红，必不为根
		//assert(grandpa && grandpa->black);
		if(grandpa->left && grandpa->right && !grandpa->left->black && !grandpa->right->black) { 
			//祖父节点为黑,其两个儿子节点为红,祖父节点变红，儿子节点变黑，往上调整
			grandpa->black = false;
			grandpa->left->black = grandpa->right->black = true;
			insertAdjust(grandpa);
			return;
		}
		if(grandpa->right == parent){
			if(parent->left == root) {
				rightRoate(root);
			} else {
				root = root->parent;
			}
			leftRoate(root);
			swap(root->black,root->left->black);
			return;
		}
		if(grandpa->left == parent) {
			if(parent->right == root) {
				leftRoate(root);
			} else {
				root = root->parent;
			}
			rightRoate(root);
			swap(root->black,root->right->black);
			return;
		}
		//assert(0);
	}

	void insert(RBTreeNode *parent,RBTreeNode *&root,int val) {
		if(!root) {
			root = new RBTreeNode(val);
			root->parent = parent;
			insertAdjust(root);
			return;
		}
		//printf("root:%p left:%p right:%p parent:%p val:%d black:%d\n",root,root->left,root->right,root->parent,root->val,root->black);
		if(val > root->val) {
			insert(root,root->right,val);
		}
		else {
			insert(root,root->left,val);
		}
	}

	RBTreeNode* begin(RBTreeNode *root) {
		while(root && root->left) root = root->left;
		return root;
	}

	RBTreeNode* next(RBTreeNode *root) {
		if(root->right) {
			root = root->right;
			while(root->left) root = root->left;
			return root;
		}
		while(root->parent && root->parent->right == root) root = root->parent;
		return root->parent;
	}

	int check(RBTreeNode *root) {
		if(!root) return 0;
		if(root->left) {
			assert(!(!root->black && !root->left->black));
			assert(root->left->parent == root);
			assert(root->left->val <= root->val);
		}
		if(root->right) {
			assert(root->right->parent == root);
			assert(root->right->val >= root->val);
		}
		int a = check(root->left);
		int b = check(root->right);
		assert(a == b);
		return a + root->black;
	}

	bool erase(RBTreeNode *&root,RBTreeNode *node) {
		if(!node) return false;
		if(node->left && node->right) { //两个子节点，拿左儿子的最右儿子或右儿子的最左儿子复制到要删除的节点
			RBTreeNode *tmp = node->left;
			while(tmp->right) tmp = tmp->right;//取左儿子的最右儿子
			node->val = tmp->val; //copy
			node = tmp;
		}
		if(!node->black) { //被删除节点为红->这个节点不可能为根,直接删除即可
			RBTreeNode*child = node->left;
			RBTreeNode*parent = node->parent;
			if(child) child->parent = parent;
			assert(parent);
			if(parent->left == node) parent->left = child;
			else parent->right = child;
			delete node;
			return true;
		}
		RBTreeNode *child=NULL;
		if(node->left) {
			child = node->left;
		} else if(node->right) {
			child = node->right;
		}
		RBTreeNode *parent = node->parent;
		if(child) child->parent = parent;

		if(!parent) root = child;
		else if(parent->left == node) parent->left = child;
		else parent->right = child;
		eraseAdjust(child,parent);
		delete node;
		return true;
	}

	void eraseAdjust(RBTreeNode *node,RBTreeNode *parent) {
		if(!parent) {	
			//1.1 子节点为根，染为黑
			if(node) node->black = true;
			return;
		}
		if(node && !node->black) { 
			//1.2子节点为红，直接染为黑
			node->black = true;
			return;
		}

		RBTreeNode *brother = (parent->left == node ? parent->right : parent->left);
		if(!parent->black) {	//2.父节点为红,兄弟节点必为黑
			assert((!brother||brother->black));
			if((!brother->left || brother->left->black) && (!brother->right || brother->right->black)) {
				//2.1 兄弟节点两个儿子为 black,black
				swap(brother->black,parent->black);
				return;
			}
			if(parent->left == node) {
				if(brother && brother->left && !brother->left->black) {	
					//2.2 兄弟节点两个儿子为 red,？
					parent->black = true;
					rightRoate(brother->left);
					leftRoate(parent->right);
					return;
				}
				//2.3 兄弟节点两个儿子为 black,red
				leftRoate(brother);
				return;
			} else {
				if(brother && brother->right && !brother->right->black) {	
					//2.2 兄弟节点两个儿子为 ?,red
					parent->black = true;
					leftRoate(brother->right);
					rightRoate(parent->left);
					return;
				}
				//2.3 兄弟节点两个儿子为 red,black
				rightRoate(brother);
				return;
			}
		}
		//3.父节点为黑
		if(brother && !brother->black) { 
			//3.1 兄弟节点为红,两个儿子节点必为黑
			swap(parent->black,brother->black);
			if(parent->left == node) leftRoate(brother);
			else rightRoate(brother);
			eraseAdjust(node,parent);	//处理后node路径上还是少一个黑色节点，重新调整
			return;
		}

		//3.2 兄弟节点为黑
		assert(brother);	//brother必不为nil,否则两边黑节点个数不一样
		if((!brother->left||brother->left->black) && (!brother->right||brother->right->black)) {
			//3.2.1 两个儿子节点为黑
			brother->black = false;
			eraseAdjust(parent,parent->parent);
			return;
		}
		if(parent->left == node) {
			if(brother->left && !brother->left->black) { 
				//3.2.2 兄弟节点左儿子为红
				brother->left->black = true;
				rightRoate(brother->left);
				leftRoate(parent->right);
				return;
			}
			//3.2.3 兄弟节点儿子为 black,red
			brother->right->black = true;
			leftRoate(brother);
			return;
		} else {
			if(brother->right && !brother->right->black) { 
				//3.2.2 兄弟节点左儿子为红
				brother->right->black = true;
				leftRoate(brother->right);
				rightRoate(parent->left);
				return;
			}
			//3.2.3 兄弟节点儿子为 black,red
			brother->left->black = true;
			rightRoate(brother);
			return;
		}
	}

	RBTreeNode* find(RBTreeNode *root,int val) {
		while(root && root->val != val) {
			if(root->val > val) root = root->left;
			else root = root->right;
		}
		return root;
	}
public:
	RBTree() { root = NULL; }
	void insert(int x) {
		insert(NULL,root,x);
	}
	void print() {
		for(RBTreeNode *it = begin(root);it;) {
			printf("%d",it->val);
			it = next(it);
			if(!it) printf("\n");
			else printf(" ");
		}
		/*for(RBTreeNode *it = begin(root);it;it=next(it)) {
			printf("self:%p parent:%p left:%p right:%p val:%d black:%d\n",it,it->parent,it->left,it->right,it->val,it->black);
		}*/
	}
	RBTreeNode* begin() {
		return begin(root);
	}
	RBTreeNode* next() {
		return next(root);
	}
	int check() {
		check(root);
	}
	RBTreeNode* find(int val) {
		return find(root,val);
	}
	bool erase(RBTreeNode *node) {
		return erase(root,node);
	}
};


int main() {
	int n;
	while(scanf("%d",&n)==1) {
		RBTree a;
		vector<int>arr;
		for(int i=0,x;i<n;i++) {
			//scanf("%d",&x);
			x = rand();
			arr.push_back(x);
			a.insert(x);
			a.check();
		}
		a.print();
		printf("--------------------------\n");
		for(int i=0,x;i<n;i++) {
			//scanf("%d",&x);
			RBTreeNode *tmp = a.find(arr[i]);
			printf("erase:%d\n",arr[i]);
			a.erase(tmp);
			//a.print();
			a.check();
		}
		break;
	}
	return 0;
}