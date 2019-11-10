#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

enum CType {
	Atom = 0,
	And,
	Or,
	Not
};

struct _CTree {
	enum CType type;
	int n;
	int children_cnt;
	struct _CTree *children;
};

struct _Module {
	int* arr;
	int size;
};

typedef struct _Module Module;
typedef Module *pModule;

struct _ModuleList {
	pModule modules;
	int size;
};

typedef struct _ModuleList ModuleList;
typedef ModuleList *pModuleList;

typedef struct _CTree CTree;
typedef CTree *pCTree;

bool isRightForm(char* str);
pCTree asCTree(char* str);
void printCTree(pCTree ctree);

void asCNF(pCTree ctree);
void printCNF(pCTree ctree);

void downLevel_not(pCTree ctree);

CTree not(CTree ctree) ;
void CNF(pCTree ctree) ;
CTree _multi(CTree a, CTree b);

void m_mult(pCTree ctree);

CTree m_or(CTree a, CTree b);
CTree m_and(CTree a, CTree b);

char* _toBuffer(pCTree ctree);

void clone(pCTree new,pCTree tree);

int main() {

	char str[1024];
	gets(str);

	if(isRightForm(str)) {
//		printf("It is CNF\n");

		pCTree ctree = asCTree(str);

		printf("Input: ");	
		printf("%s\n",str);
		printf("Tree : ");	
		printCTree(ctree);
		printf("\n\n");


		downLevel_not(ctree);
		printf("Down Not: ");	
		printCTree(ctree);
		printf("\n");

		asCNF(ctree);

		printf("CNF: ");	
		printCTree(ctree);
		printf("\n");

		printCNF(ctree);

	}
	else printf("Syntax error\n");

	



	return 0;
}

bool isRightForm(char* str) {
	enum CType type;

	//printf("Right recur: '%s'\n",str);

	if(str[0] == 'a') {
		int n = atoi(str+1);
		type = Atom;
		return n > 0;
	} else {

		char buf[8];
		sscanf(str,"%s",buf);

		if(strcmp("(and",buf) == 0) {
			type = And;
		}
		else if(strcmp("(or",buf) == 0) { 
			type = Or;
		}
		else if(strcmp("(not",buf) == 0) { 
			type = Not;
		}
		else {
			return false;
		}

		int d = 0;
		for(int i = 0; i < strlen(str); i++) {
			if(str[i] == '(') {
				d++;
			}
			else if(str[i] == ')') {
				d--;
				if(d<0) return false;
			}
		}

		if(d != 0) return false;
		
		int child_cnt = 0;
		char strbuf[2048];
		int strbuf_c = 0;
		int depth = 1;

		switch(type) {

		case And:

			for(int i = 5; i < strlen(str); i++) {

				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {
						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';
						if(!isRightForm(strbuf)) return false;
						strcpy(strbuf,"");
						strbuf_c = 0;
						child_cnt++;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';
							if(!isRightForm(strbuf)) return false;
							strcpy(strbuf,"");
							strbuf_c = 0;
							child_cnt++;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {

						strbuf[strbuf_c] = '\0';
						if(!isRightForm(strbuf)) return false;
						strcpy(strbuf,"");
						strbuf_c = 0;
						child_cnt++;


					}
					strcpy(strbuf,"");
					continue;
				}
				strbuf[strbuf_c] = str[i];
				strbuf_c++;
			}
			
			if(child_cnt < 1)
				return false;

			break;


		case Or:

			for(int i = 4; i < strlen(str); i++) {

				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {
						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';
						if(!isRightForm(strbuf)) return false;
						strcpy(strbuf,"");
						strbuf_c = 0;
						child_cnt++;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';
							if(!isRightForm(strbuf)) return false;
							strcpy(strbuf,"");
							strbuf_c = 0;
							child_cnt++;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {
			
						strbuf[strbuf_c] = '\0';
						if(!isRightForm(strbuf)) return false;
						strcpy(strbuf,"");
						strbuf_c = 0;
						child_cnt++;

					}
					strcpy(strbuf,"");
					continue;
				}
				strbuf[strbuf_c] = str[i];
				strbuf_c++;
			}
			if(child_cnt < 1)
				return false;

			break;

		case Not:

			child_cnt = 1;

			char* pr = str+5;
			char temp[256];
			strcpy(temp,pr);
			temp[strlen(pr)-1] = '\0';

			int d = 0;
			int cnt = 0;

			for(int j = 0; j < strlen(temp); j++) {

				if(temp[j] == '(') {
					if(d==0) cnt++;
					d++;
				}
				else if(temp[j] == 'a' && d == 0) {
					cnt++;
				}
				else if(temp[j] == ')') {
					d--;
				}
			}

			if(cnt > 1) return false;

			return isRightForm(temp);
		}
	}

	return true;
}

pCTree asCTree(char* str) {
	enum CType type;

	//printf("recur: '%s'\n",str);

	if(str[0] == 'a') {

		int n = atoi(str+1);
		type = Atom;

		pCTree pc = malloc(sizeof(CTree));
		pc->type = type;
		pc->n = n;
		pc->children_cnt = n;
		pc->children = 0x0;

		//CTree ctree = {type, n, 0, 0x0};

		return pc;
	} else {

		char buf[8];
		sscanf(str,"%s",buf);

		if(strcmp("(and",buf) == 0) {
			type = And;
		}
		else if(strcmp("(or",buf) == 0) { 
			type = Or;
		}
		else if(strcmp("(not",buf) == 0) { 
			type = Not;
		}
		else {
			return false;
		}

		
		char strbuf[2048];
		int strbuf_c = 0;
		int depth = 1;

		CTree children[1024];
		int child_cnt = 0;

				int len = strlen(str);

		switch(type) {

		case And:

			for(int i = 5; i < len; i++) {
				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {
						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';
						pCTree pchild = asCTree(strbuf);
						//children[child_cnt] = *pchild;
						children[child_cnt] = *asCTree(_toBuffer(pchild));
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

							pCTree pchild = asCTree(strbuf);
							//children[child_cnt] = *pchild;
							children[child_cnt] = *asCTree(_toBuffer(pchild));
							child_cnt++;

							strcpy(strbuf,"");
							strbuf_c = 0;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {

						strbuf[strbuf_c] = '\0';

						pCTree pchild = asCTree(strbuf);
						children[child_cnt] = *asCTree(_toBuffer(pchild));
						//children[child_cnt] = *pchild;
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;


					}
					strcpy(strbuf,"");
					continue;
				}
				strbuf[strbuf_c] = str[i];
				strbuf_c++;
			}
			break;

		case Or:

			printf("\n\nHello\n\n");
			for(int i = 4; i < len; i++) {
				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {



						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';
						pCTree pchild = asCTree(strbuf);
						//children[child_cnt] = *pchild;



						if(child_cnt>1) {
							printCTree(&children[0]);
							printf("\n");
						}
						children[child_cnt] = *asCTree(_toBuffer(pchild));
							printCTree(&children[child_cnt]);
							printf("\n");
						child_cnt++;



						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

							pCTree pchild = asCTree(strbuf);
							//children[child_cnt] = *pchild;
							children[child_cnt] = *asCTree(_toBuffer(pchild));
							child_cnt++;

							strcpy(strbuf,"");
							strbuf_c = 0;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {

						strbuf[strbuf_c] = '\0';

						pCTree pchild = asCTree(strbuf);
						children[child_cnt] = *asCTree(_toBuffer(pchild));
						//children[child_cnt] = *pchild;
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;


					}
					strcpy(strbuf,"");
					continue;
				}
				strbuf[strbuf_c] = str[i];
				strbuf_c++;
			}
			break;

		case Not:
			type = Not;

			char* pr = str+5;
			char temp[1024];
			strcpy(temp,pr);
			temp[strlen(pr)-1] = '\0';

			CTree childs[1024];

			pCTree pchild = asCTree(temp);

			childs[0] = *pchild;



			pCTree pc = malloc(sizeof(CTree));

			pc->type = Not;
			pc->n = 0;
			pc->children_cnt = 1;
			pc->children = childs;

			printf("pc:");
			printCTree(pc);
			printf("\n");

			return pc;
			// break;
		}

		CTree ctree = {type,0, child_cnt,0x0};

		ctree.children = malloc(sizeof(CTree)*child_cnt);
		
		for(int j = 0 ; j < ctree.children_cnt; j++) ctree.children[j] = children[j];

		return &ctree;

	}

	return 0x0;;

}
/*
char* toBuffer(pCTree ctree) {

	char str[2048];
	return _toBuffer(ctree,str);
}
*/

char* _toBuffer(pCTree ctree) {

	char buf[2048];
	enum CType type = ctree->type;


	if(type == Atom) {

		sprintf(buf,"a%d",ctree->n);


		return buf;
	}

	if(type != Atom) {

		sprintf(buf,"%s", type == And? "(and " : type == Or? "(or ": "(not ");
		for(int i = 0; i < ctree->children_cnt; i++)  {

			char* temp = _toBuffer(&(ctree->children[i]));

			//strcat(buf,temp);
			sprintf(buf,"%s%s",buf,temp);
			strcat(buf," ");

		}
		buf[strlen(buf)-1] = ')';
	//	strcat(buf,"\b)");
		return buf;
	}

}
void printCTree(pCTree ctree) {

	enum CType type = ctree->type;

	if(type == Atom) {
		printf("a%d",ctree->n);
	}

	if(type != Atom) {
		printf("%s", type == And? "(and " : type == Or? "(or ": "(not ");
		for(int i = 0; i < ctree->children_cnt; i++)  {
			printCTree(&(ctree->children[i]));
			printf(" ");
		}
		printf("\b)");
	}

}
void asCNF(pCTree ctree) {


	CNF(ctree);

}

void CNF(pCTree ctree) {
/*
	printf("CNF: ");
	printCTree(ctree);
	printf("\n");
*/

	switch(ctree->type) {

	case Atom:
		return;
	case Not:
		return;

	case And:
		for(int i = 0; i < ctree->children_cnt; i++) {

			CTree child = ctree->children[i];
			CNF(&child);
		}
		return;

	case Or:
		for(int i = 0; i < ctree->children_cnt; i++) {

			CTree child = ctree->children[i];
			CNF(&child);
		}
		
		m_mult(ctree);

		return;
	}
}
void m_mult(pCTree ctree) {
	

	CTree arr1[1024];
	CTree arr2[1024];
	CTree arr3[2048];
	int size1 = 0, size2 = 0;

	for(int i = 0; i < ctree->children_cnt; i++) {
		CTree ch = ctree->children[i];
		switch(ch.type) {
		
		case Atom:
		case Not:
			arr1[size1] = ch;
			size1++;
			break;
			
		case Or:
			for(int j = 0; j < ch.children_cnt; j++) {
				arr1[size1] = ch.children[j];
				size1++;
			}
			break;
		
		case And:
//			printCTree(&ch);
			arr2[size2] = ch;
			size2++;
		}

	}

	if(size2<1) return;


//	printf("size1: %d, size2: %d\n",size1,size2);

	for(int i = 0; i < size2-1; i++) {

		// arr2[i];
		// arr2[i+1];

		arr2[i+1] = _multi(arr2[i],arr2[i+1]);
	}

	CTree sum = arr1[0];


	for(int i = 1; i < size1; i++) {
	
		sum = m_or(sum,arr1[i]);
	}

	printCTree(&sum);
	printf("\n");


	CTree l1 = arr2[size2-1];


	printCTree(&l1);
	printf("\n");

	ctree->type = l1.type;
	ctree->n = l1.n;
	ctree->children_cnt = l1.children_cnt;
	ctree->children = malloc(sizeof(CTree)*l1.children_cnt);
	
	


	CTree n_children[2048];
//	n_children = malloc(l1.children_cnt*sizeof(pCTree));
	

	for(int i = 0; i < l1.children_cnt; i++) {

		CTree tr = m_or(l1.children[i],sum);

		printf("tr: ");
		printCTree(&tr);
		printf("\n");
		char* buf =_toBuffer(&tr);

		
		printf("buf: %s\n",buf);
		pCTree ct = asCTree(buf);

		
		printf("ct: ");
		printCTree(ct);
		printf("\n");
/*
*/

		n_children[i] = tr;



/*
		printf("i: %d\n",i);
		printf("%ld\n",*ct);
		printCTree(ct);
		printf("\n");
*/
	}

/*
	printf("i: %d\n",0);
	printf("%ld\n",&n_children[0]);
	printCTree(&n_children[0]);
	printf("\n");
*/	


	for(int i = 0; i < l1.children_cnt; i++) {
		
		ctree->children[i] = n_children[i];
		
		
		printf("child: ");
		printCTree(&ctree->children[i]);
		printf("\n");

/*
		printCTree(&(ctree->children[i]));
		printf("\n");
*/
	}
/*
	printf("CTree:\n");
	printf("type: %d\n",ctree->type);
	printf("n: %d\n",ctree->n);
	printf("cnt: %d\n",ctree->children_cnt);

	printCTree(&(ctree->children[0]));
	printf("\n");
	printCTree(&(ctree->children[1]));
	printf("\n");

	printCTree(ctree);
	printf("\n");
*/
}

CTree _multi(CTree a, CTree b) {

	int size = 0;

	CTree* a_child = a.children;
	CTree* b_child = a.children;


	CTree children[2048];

	for(int i = 0; i < a.children_cnt; i++) {

		for(int j = 0; j < b.children_cnt; j++) {

			CTree ntree = m_or(a_child[i],b_child[j]);
			children[size] = ntree;
			size++;
		}
	}

	CTree tree = {And,0,size,children};
	return tree;
}

CTree m_or(CTree a, CTree b) {


	if((a.type == Or) && b.type == a.type) {

		CTree a1 = *asCTree(_toBuffer(&a));
		CTree b1 = *asCTree(_toBuffer(&b));
		
		for(int i = 0; i < a1.children_cnt; i++)
			a1.children[i] = a.children[i];

		for(int i = 0; i < b1.children_cnt; i++)
			b1.children[i] = b.children[i];

		CTree children[1024];
		int cnt = a.children_cnt + b.children_cnt;
		int i;
		for(i = 0; i < b.children_cnt; i++) {

			children[i] = b1.children[i];
		}
		for(int j = 0; j < a.children_cnt; j++) {
			
			children[i+j] = a1.children[j];
		}

		CTree ans = {Or,0,cnt,children};
		
		
/*
		for(int i = 0; i < cnt; i++) {
			printf("i: %d\n",i);
			printCTree(&children[i]);
			printf("\n");
		}
*/
		return ans;
		
	} 
	else if(a.type == Or && (b.type == Atom || b.type == Not)) {

		a.children[a.children_cnt] = b;
		a.children_cnt++;
	
		return a;
	}
	else if(b.type == Or && (a.type == Atom || a.type == Not)) {

		b.children[b.children_cnt] = a;
		b.children_cnt++;

		return b;
	}
	else {
		CTree children[2] = {a,b};
		CTree ans = {Or,0,2,children};
		return ans;
	}
	
}
CTree m_and(CTree a, CTree b) {
	if(a.type == b.type == And) {
		
		CTree children[1024];
		int cnt = a.children_cnt + b.children_cnt;
		int i;
		for(i = 0; i < a.children_cnt; i++) {

			children[i] = a.children[i];
		}
		for(int j = 0; j < b.children_cnt; j++) {
			
			children[i+j] = b.children[j];
		}

		CTree ans = {And,0,cnt,children};
		return ans;
		
	} else {
		CTree children[2] = {a,b};
		CTree ans = {And,0,2,children};
		return ans;
	}
}

void downLevel_not(pCTree ctree) {
/*
	printf("Down: ");
	printCTree(ctree);
	printf("\n");
*/
	

	pCTree child;

	switch(ctree->type) {

	case Atom:
		return;

	case And:
	case Or:

		
		for(int i = 0; i < ctree->children_cnt; i++) {
			downLevel_not(&(ctree->children[i]));
		}
		return;

	case Not:

		switch((ctree->children[0]).type) {

		case Atom:
			return;


		case Not:

			child = &((ctree->children[0]).children[0]);

			ctree->type = child->type;
			ctree->n = child->n;
			ctree->children_cnt = child->children_cnt;
			ctree->children = malloc(sizeof(CTree)*child->children_cnt);

			
			//printf("Hello! %d\n",child->children_cnt);

			for(int i = 0; i < child->children_cnt; i++) {
				ctree->children[i] = child->children[i];
			}


			downLevel_not(ctree);

			return;

		case And:
			ctree->type = Or; // inverse

			CTree* children = (ctree->children[0]).children;
			int children_cnt = (ctree->children[0]).children_cnt;

			ctree->children_cnt = children_cnt;
			ctree->children = malloc(sizeof(CTree)*children_cnt);

			for(int i = 0; i < children_cnt; i++) {

				CTree new_ctree;
				new_ctree = not(children[i]);
				
				(ctree->children)[i] = new_ctree;

			}

			downLevel_not(ctree);

			break;
		
		case Or:
			ctree->type = And; // inverse

			children = (ctree->children[0]).children;
			children_cnt = (ctree->children[0]).children_cnt;

			ctree->children_cnt = children_cnt;
			ctree->children = malloc(sizeof(CTree)*children_cnt);

			for(int i = 0; i < children_cnt; i++) {

				CTree new_ctree;
				new_ctree = not(children[i]);
				
				(ctree->children)[i] = new_ctree;

			}

			downLevel_not(ctree);

			break;
		
		}
	}

}
CTree not(CTree ctree) {
	
	CTree new_ctree ;

	new_ctree.type = Not;
	new_ctree.n = 0;
	new_ctree.children_cnt = 1;
	new_ctree.children = malloc(sizeof(CTree));
	
	new_ctree.children[0] = ctree;


	return new_ctree;
}
void clone(pCTree new,pCTree tree) {

	printf("clone: %d\n",tree->children_cnt);
	printCTree(tree);
	printf("\n");


	//new->children = 0x0;
	if(tree->children_cnt>0)
		new->children = malloc(sizeof(CTree)*tree->children_cnt);

	new->type = tree->type;
	new->n = tree->n;
	new->children_cnt = tree->children_cnt;


	for(int i = 0; i < tree->children_cnt; i++) {

		//new->children[i] = tree->children[i];
		clone(&(new->children[i]),&(tree->children[i]));
	}
/*
		printf("type %d, %d\n",new->type,tree->type);
		printf("n %d, %d\n",new->n,tree->n);
		printf("cnt %d, %d\n",new->children_cnt,tree->children_cnt);
		printf("child %d, %d\n",new->children,tree->children);
*/

}
void printCNF(pCTree ctree) {
/*
	printf("%d",ctree->type);
	printf("\nHello!\n");
*/	

	switch(ctree->type) {

	case Atom:
		printf("%d",ctree->n);
		return;

	case Not:
		printf("-%d",ctree->children[0].n);
		return;

	case Or:
		for(int i = 0; i < ctree->children_cnt; i++) {
			printCNF(&(ctree->children[i]));
			printf(" ");
		}
		printf("\b");
		return;

	case And:
		
		for(int i = 0; i < ctree->children_cnt; i++) {
			printCNF(&(ctree->children[i]));
			printf("\n");
		}
	}
}
