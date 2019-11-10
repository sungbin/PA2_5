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

void downLevel_not(pCTree ctree);

CTree not(CTree ctree) ;

pModuleList asModules(pCTree pctree);
void printModuleList(pModuleList list);
void printModule(Module mod);
void addAll(pModuleList list1, pModuleList list2);
void pModule_init(pModule pModule);
void addModule(pModuleList list, pModule module);

int main() {

	char str[1024];
	gets(str);

	if(isRightForm(str)) {
//		printf("It is CNF\n");

		pCTree ctree = asCTree(str);

		printCTree(ctree);
		printf("\n\n");


		asCNF(ctree);


	}
	else printf("Syntax error\n");

	



	return 0;
}

bool isRightForm(char* str) {
	enum CType type;

//	printf("recur: '%s'\n",str);

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

		CTree ctree = {type, n, 0, 0x0};

		return &ctree;
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
						
						pCTree pchild = asCTree(strbuf); 
						
						CTree node = {pchild->type,pchild->n,pchild->children_cnt,pchild->children};

						children[child_cnt] = node;
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

							pCTree pchild = asCTree(strbuf);
							children[child_cnt] = pchild[0];
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
						children[child_cnt] = *pchild;
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

			for(int i = 4; i < strlen(str); i++) {

				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {
						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';
						pCTree pchild = asCTree(strbuf);
						children[child_cnt] = *pchild;
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

							pCTree pchild = asCTree(strbuf);
							children[child_cnt] = *pchild;
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
						children[child_cnt] = *pchild;
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
			strncpy(temp,pr,strlen(pr)-1);

			pCTree pchild = asCTree(temp);

			CTree ctree = {type, 0, 1, pchild};

			return &ctree;
			// break;
		}

		CTree ctree = {type,0, child_cnt,0x0};

		ctree.children = malloc(sizeof(CTree)*child_cnt);
		
		for(int j = 0 ; j < ctree.children_cnt; j++) ctree.children[j] = children[j];

		return &ctree;

	}

	return 0x0;;

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

	downLevel_not(ctree);

	printCTree(ctree);
	printf("\n");

	

}

pModuleList asModules(pCTree pctree) {

	pModuleList list = malloc(sizeof(ModuleList));
	list->modules = malloc(sizeof(Module)*1024);
	list->size = 0;

		pModule pMod;
		pModule_init(pMod);

	switch(pctree->type) {

	case Atom:
		
		pMod->arr[0] = pctree->n;
		pMod->size++;
		
		addModule(list,pMod);
		
		break;

	case And:

		for(int i = 0; i < pctree->children_cnt; i++) {

			pModuleList ch = asModules(&(pctree->children[i]));
			addAll(list,ch);
		}

	}

	return list;

}

void printModuleList(pModuleList list) {

	for(int i = 0; i < list->size; i++) {

		Module mod = list->modules[i];
		printModule(mod);
	}
}
void printModule(Module mod) {
	for(int i = 0; i < mod.size; i++) {
		
		int n = mod.arr[i];
		printf("%d ",n);
	}
	printf("\n");
}

void addAll(pModuleList list1, pModuleList list2) {

	for(int i = 0; i < list2->size; i++) {
		addModule(list1,&(list2->modules[i]));
	}
	
}

void pModule_init(pModule pModule) {
	pModule->arr = malloc(sizeof(int)*1024);
	pModule->size = 0;
}

void addModule(pModuleList list, pModule module) {
	list->modules[list->size] = *module;
	list->size++;
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
