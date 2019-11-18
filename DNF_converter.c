#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_SIZE 4096

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
	struct _CTree **children;
};

typedef struct _CTree CTree;
typedef CTree *pCTree;

bool isRightForm(char* str);
pCTree asCTree(char* str);
void _printCTree(pCTree ctree);
void printCTree(pCTree ctree);

void CNF(pCTree ctree);
void printCNF(pCTree ctree);
void NNF(pCTree ctree);

pCTree not(pCTree ctree) ;
void m_and(pCTree tree,pCTree a, pCTree b); 
void m_or(pCTree tree,pCTree a, pCTree b); 
void m_distr(pCTree tree,pCTree a, pCTree b);
char *replaceAll(char *s, const char *olds, const char *news);
void _toCNF(char* result, pCTree ctree);

char* _toBuffer(pCTree ctree);
int main() {

	char str[MAX_SIZE];

	fgets(str,MAX_SIZE,stdin);

	bool option = true;

	if(isRightForm(str)) {
//		printf("It is CNF\n");

		pCTree ctree = asCTree(str);
		if(option) {
			printf("Input: ");	
			printf("%s\n",str);
			printf("Tree : ");	
			printCTree(ctree);
		}

		NNF(ctree);
		if(option) {
			printf("NNF: ");	
			printCTree(ctree);
		}

		CNF(ctree);
		if(option) {
			printf("CNF: ");	
			printCTree(ctree);
		}

//		printCNF(ctree);
/*
*/
	}
	else printf("ERROR: Not right propositional logic\n");

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
			
			if(child_cnt < 2)
				return false;

			break;


		case Or:

			for(int i = 4; i < len; i++) {

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
			if(child_cnt < 2)
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
/*
*/
	//printf("asCTree: '%s'\n",str);

	if(str[0] == 'a') {

		int n = atoi(str+1);
		type = Atom;

		pCTree pc = malloc(sizeof(CTree));
		pc->type = type;
		pc->n = n;
		pc->children_cnt = 0;
		pc->children = malloc(sizeof(CTree*)*2048);

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

		CTree** children;
		children = malloc(sizeof(CTree*)*1024);

//		char s_children[1024][1024];
		char** s_children= malloc(sizeof(char*)*1024);		
		for(int i = 0; i < 1024; i++) s_children[i] = malloc(sizeof(char)*1024);


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
						
						strcpy(s_children[child_cnt],strbuf);
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

						strcpy(s_children[child_cnt],strbuf);
						child_cnt++;
							strcpy(strbuf,"");
							strbuf_c = 0;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {

						strbuf[strbuf_c] = '\0';

						strcpy(s_children[child_cnt],strbuf);
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

			for(int i = 4; i < len; i++) {
				if(str[i] == '(') {
					depth++;
				} else if(str[i] == ')') {
					depth--;
					if(depth == 1) {



						strbuf[strbuf_c] = str[i];
						strbuf[strbuf_c+1] = '\0';

						strcpy(s_children[child_cnt],strbuf);
						child_cnt++;

						strcpy(strbuf,"");
						strbuf_c = 0;
						continue;

					} else if(depth == 0) {
						if(strlen(strbuf) > 0) {
				
							strbuf[strbuf_c] = '\0';

						strcpy(s_children[child_cnt],strbuf);
						child_cnt++;

							strcpy(strbuf,"");
							strbuf_c = 0;

						}
						continue;
					}
				} else if(str[i] == ' ' && depth == 1) {
					if(strlen(strbuf) > 0) {

						strbuf[strbuf_c] = '\0';

						strcpy(s_children[child_cnt],strbuf);
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

			pCTree* pchilds = malloc(sizeof(pCTree*)*1024);

			pCTree pchild = asCTree(temp);

			pchilds[0] = pchild;


			pCTree pc = malloc(sizeof(CTree));

			pc->type = Not;
			pc->n = 0;
			pc->children_cnt = 1;
			pc->children = malloc(sizeof(pCTree));
			pc->children = pchilds;


			return pc;
			// break;
		}


		pCTree pc = malloc(sizeof(CTree));
		pc->type = type;
		pc->n = 0;
		pc->children_cnt = child_cnt;
		pc->children = malloc(sizeof(CTree*)*2048);


		for(int i = 0; i < child_cnt; i++) {
			pCTree t = asCTree(s_children[i]);
			pc->children[i] = t;
			
		}
		
		return pc;

	}

	return 0x0;

}

int dep = 0;

char* _toBuffer(pCTree ctree) {
	char* buf = malloc(sizeof(char)*2048);
	enum CType type = ctree->type;

	if(type == Atom) {
		sprintf(buf,"a%d",ctree->n);
	}

	if(type != Atom) {

		if(ctree->children_cnt < 1) {
			printf("Hello! %d t: %d\n",ctree->n,type);
			exit(1);
		}

		sprintf(buf,"%s", type == And? "(and " : type == Or? "(or ": "(not ");
		for(int i = 0; i < ctree->children_cnt; i++)  {
	

			sprintf(buf,"%s%s ",buf,_toBuffer(ctree->children[i]));
			strcat(buf," ");

		}
		
		int len = strlen(buf);
		buf[len-1] = ')';
		buf[len] = '\0';
		
	}
	return buf;

}
void printCTree(pCTree ctree) {

	_printCTree(ctree);
	printf("\n");
}
void _printCTree(pCTree ctree) {

	enum CType type = ctree->type;

	if(type == Atom) {
		printf("a%d",ctree->n);
	}

	if(type != Atom) {
		printf("%s", type == And? "(and " : type == Or? "(or ": "(not ");
		for(int i = 0; i < ctree->children_cnt; i++)  {
			_printCTree(ctree->children[i]);
			printf(" ");
		}
		printf("\b)");
	}

}
void CNF(pCTree ctree) {

	pCTree t = asCTree(_toBuffer(ctree));

	ctree->type = t->type;
	ctree->n = t->n;
	ctree->children_cnt = t->children_cnt;
	ctree->children = t->children;




	pCTree* children = malloc(sizeof(pCTree)*1024);

	int children_cnt = ctree->children_cnt;
	if(!(ctree->type == Atom))
	for(int i = 0; i < ctree->children_cnt; i++) {

		char* buf = _toBuffer(ctree->children[i]);
		children[i] = asCTree(buf);
	}

	switch(ctree->type) {

	case Atom:
		return;
	case Not:
		return;

	//case And:
	case Or:

		for(int i = 0; i < children_cnt; i++) {
			CNF(children[i]);

		}

		//m_and(ctree,children[0],children[1]);
		m_or(ctree,children[0],children[1]);

		for(int i = 2; i < children_cnt; i++) {
			//m_and(ctree,ctree,children[i]);
			m_or(ctree,ctree,children[i]);
		}
		return;

	case And:
	//case Or:

		for(int i = 0; i < children_cnt; i++) {
			CNF(children[i]);
		}

		m_distr(ctree,children[0],children[1]);


		for(int i = 2; i < children_cnt; i++) {
			m_distr(ctree,ctree,children[i]);
		}
		return;
	}
}

void m_and(pCTree tree,pCTree a, pCTree b) { 
	pCTree a1 = asCTree(_toBuffer(a));
	pCTree b1 = asCTree(_toBuffer(b));

	pCTree* children = malloc(sizeof(pCTree)*2);
	children[0] = a1;
	children[1] = b1;
	
	tree->type = And;
	tree->n = 0;
	tree->children_cnt = 2;
	tree->children = children;
	
}
void m_or(pCTree tree,pCTree a, pCTree b) { 
	pCTree a1 = asCTree(_toBuffer(a));
	pCTree b1 = asCTree(_toBuffer(b));

	pCTree* children = malloc(sizeof(pCTree)*2);
	children[0] = a1;
	children[1] = b1;
	
	tree->type = Or;
	tree->n = 0;
	tree->children_cnt = 2;
	tree->children = children;
}

int dis_dep = 0;
	
void m_distr(pCTree tree,pCTree a, pCTree b) {
	
	a = asCTree(_toBuffer(a));
	b = asCTree(_toBuffer(b));

	if(a->type == And) {
		for(int i = 0; i < a->children_cnt; i++) {
			m_distr(a->children[i],a->children[i],b);
		}

		m_and(tree,a->children[0],a->children[1]);
		for(int i = 2; i < a->children_cnt; i++) {
			//m_and(tree,tree,a->children[i]);
			m_or(tree,tree,a->children[i]);
		}

	} else if(b->type == And) {
		for(int i = 0; i < b->children_cnt; i++) {
			m_distr(b->children[i],b->children[i],a);
		}

		m_and(tree,b->children[0],b->children[1]);
		for(int i = 2; i < b->children_cnt; i++) {
			m_or(tree,tree,b->children[i]);
			//m_and(tree,tree,b->children[i]);
		}
	} else {
		//m_or(tree,a,b);
		m_and(tree,a,b);
	}


}
void NNF(pCTree ctree) {

	pCTree child;

	switch(ctree->type) {

	case Atom:
		return;

	case And:
	case Or:

		
		for(int i = 0; i < ctree->children_cnt; i++) {
			NNF(ctree->children[i]);
		}
		return;

	case Not:

		switch((ctree->children[0])->type) {

		case Atom:
			return;


		case Not:

			child = (ctree->children[0])->children[0];

			ctree->type = child->type;
			ctree->n = child->n;
			ctree->children_cnt = child->children_cnt;
			ctree->children = malloc(sizeof(pCTree)*child->children_cnt);

			//printf("Hello! %d\n",child->children_cnt);

			for(int i = 0; i < child->children_cnt; i++) {
				ctree->children[i] = child->children[i];
			}


			NNF(ctree);

			return;

		case And:
			ctree->type = Or; // inverse

			pCTree* children = (ctree->children[0])->children;
			int children_cnt = (ctree->children[0])->children_cnt;

			ctree->children_cnt = children_cnt;
			ctree->children = malloc(sizeof(pCTree)*children_cnt);

			for(int i = 0; i < children_cnt; i++) {

				pCTree new_ctree;
				new_ctree = not(children[i]);
				
				(ctree->children)[i] = new_ctree;

			}

			NNF(ctree);

			break;
		
		case Or:
			ctree->type = And; // inverse

			children = (ctree->children[0])->children;
			children_cnt = (ctree->children[0])->children_cnt;

			ctree->children_cnt = children_cnt;
			ctree->children = malloc(sizeof(pCTree)*children_cnt);

			for(int i = 0; i < children_cnt; i++) {

				pCTree new_ctree;
				new_ctree = not(children[i]);
				
				(ctree->children)[i] = new_ctree;

			}

			NNF(ctree);

			break;
		
		}
	}

}
pCTree not(pCTree ctree) {
	
	pCTree new_ctree = malloc(sizeof(CTree));

	new_ctree->type = Not;
	new_ctree->n = 0;
	new_ctree->children_cnt = 1;
	new_ctree->children = malloc(sizeof(pCTree));
	
	new_ctree->children[0] = ctree;

	return new_ctree;
}

void printCNF(pCTree ctree) {
	char* str = malloc(sizeof(char)*2048);

	_toCNF(str,ctree);
	int len = strlen(str);

	str[len] = '\n';
	str[len+1] = '\0';

	char* str2 = replaceAll(str,"\n\n","\n");

	while(strlen(str) != strlen(str2)) {
		str = str2;
		str2 = replaceAll(str2,"\n\n","\n");
	}

	printf("%s",str);
}

void _toCNF(char* result, pCTree ctree) {
	char buf[2048] = "";

	switch(ctree->type) {

	case Atom:
		sprintf(buf,"%d",ctree->n);
		strcpy(result,buf);
		return;

	case Not:
		sprintf(buf,"-%d",ctree->children[0]->n);
		strcpy(result,buf);
		return;

	case Or:
		for(int i = 0; i < ctree->children_cnt; i++) {
			char* temp = malloc(sizeof(char)*2048);
			_toCNF(temp,ctree->children[i]);
			sprintf(buf,"%s%s ",buf,temp);
		}
		buf[strlen(buf)-1] = '\0';
		strcpy(result,buf);
		return;

	case And:
		
		for(int i = 0; i < ctree->children_cnt; i++) {
			char* temp = malloc(sizeof(char)*2048);
			_toCNF(temp,ctree->children[i]);
			sprintf(buf,"%s%s\n",buf,temp);
		}
		strcpy(result,buf);
		return;
	}
}


char *replaceAll(char *s, const char *olds, const char *news) {
  char *result, *sr;
  size_t i, count = 0;
  size_t oldlen = strlen(olds); if (oldlen < 1) return s;
  size_t newlen = strlen(news);


  if (newlen != oldlen) {
    for (i = 0; s[i] != '\0';) {
      if (memcmp(&s[i], olds, oldlen) == 0) count++, i += oldlen;
      else i++;
    }
  } else i = strlen(s);


  result = (char *) malloc(i + 1 + count * (newlen - oldlen));
  if (result == NULL) return NULL;


  sr = result;
  while (*s) {
    if (memcmp(s, olds, oldlen) == 0) {
      memcpy(sr, news, newlen);
      sr += newlen;
      s  += oldlen;
    } else *sr++ = *s++;
  }
  *sr = '\0';

  return result;
}
