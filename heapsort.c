#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>

#define LEFT 0
#define RIGHT 1

typedef struct bheap bheap;

int maxlevel;


void printfullheap();
struct bheap
{
	int a;
	bheap* left;
	bheap* right;
	bheap* parent;
};

bheap *root;//always the smallest number
int globnum;//number of elements

void mybalancedown(bheap *);

/*count the level in the binary tree where to add new element*/
int level()
{	
	return (int)floor(log2(globnum));
	
}

void balance(bheap *child)
{

#ifdef DEBUG
printf("balance: child: %d\n",child->a);
#endif

//	if(child == NULL)
//		return;

	int tmp;
	bheap* parent;
	parent = child->parent;

	if(parent == NULL)
		return;

	if(child->a >= parent->a)
	{
		tmp = child->a;
		child->a = parent->a;
		parent->a = tmp;
	}
	balance(parent);
	
}
int myinsertrec(bheap *r,bheap* ne, int l)
{
#ifdef DEBUG
printf("myinserrec: lev = %d,elem: %d\n",l+1,ne->a);
#endif
int tmp;
int d=0;
	if(l == 0)
	{
		if(r->left == NULL){
			r->left = ne;
			ne->parent = r;
			balance(ne);
			return 1;
		}
		else if(r->right == NULL)
		{
			r->right = ne;
			ne->parent = r;
			balance(ne);
			return 1;
		}
		else 
			return 0;
	}else{
		d = myinsertrec(r->left,ne, l-1);
		if(d)
		{
			return 1;
		}
		
		d = myinsertrec(r->right,ne,l-1);
		if(d)
		{
			return 1;
		}
	}
	return 0;
}

//<sys/ioctl.h> needed
static inline int gettermwidth()
{
struct winsize w;
ioctl(0,TIOCGWINSZ, &w);
return w.ws_col;
}

void insertheap(int a)
{
#ifdef DEBUG
printf("insertheap: %d\n",a);
#endif	
	int lev = level();
	bheap *tmp = (bheap*)malloc(sizeof(bheap));
	tmp->a = a;
	tmp->left = NULL;
	tmp->right = NULL;
	tmp->parent = NULL;

		if(root == NULL)
		{
			root = tmp;
		}
		else
		{
			myinsertrec(root,tmp,lev-1);
		}
	
}



//maxlevel defines max depth 
inline int getposinlev(int lev)
{
int num;
int maxlevel = level();

if(lev < maxlevel)
	num = (int)pow(2,lev);
else
	num = globnum - (int)pow(2,lev-1)-1;

return num;
}



int g;




/*Garantie: jedes parent ist kleiner  als child*/
void myprintheap(bheap *p, int l, int termsize)
{
#ifdef DEBUG1
printf("myprintheap: lev=%d\n",l);
#endif
	int i=0;

	if(l==0)
	{
		if(p != NULL)
		{	
			printf("%d",p->a);
			for(i=0;i<((l*2)+4);i++)
			{//start from the center of terminal
				printf(" ");
			}
		}
	}
	else
	{
		if(p->left != NULL)
		{
			myprintheap(p->left,l-1,termsize);
		}
		
		if(p->right != NULL)
		{
			myprintheap(p->right,l-1,termsize);
		}
	}
}

bheap *lastelement;
bheap* getlastelem(bheap *root,int level, int posinlevel)
{
#ifdef DEBUG
printf("root: %d, level: %d, posinlevel: %d\n",root->a,level,posinlevel);
#endif

	if(level==0)
	{
		lastelement = root;
	}
	else
	{	
		if(root->left != NULL)
			getlastelem(root->left,level-1,posinlevel);
		if(root->right != NULL)
			getlastelem(root->right,level-1,posinlevel);
	}
}


int myremove()
{
	int lev = level();
	int ans = root->a;//save heap
	bheap *oldroot = root;
	int posinlev = globnum - (2<<lev-1) + 1;//??

	getlastelem(root,lev,posinlev);

#ifdef DEBUG
if(lastelement != NULL)
	printf("==>lastel: %d\n", lastelement->a);
	
#endif

	root = lastelement;

		
	if(root->parent != NULL && root->parent != oldroot)
	{	
		if(root->parent->left == root)
			root->parent->left = NULL;
		else
			root->parent->right = NULL;
	}
	
	if(root == oldroot->left)
		root->left = NULL;
	else
		root->left = oldroot->left;
		
	if(oldroot->left != NULL)	
		oldroot->left->parent = root;
	if(oldroot->right != NULL)
		oldroot->right->parent = root;
	
	if(root == oldroot->right)
		root->right = NULL;
	else
		root->right = oldroot->right;	
	root->parent = NULL;
	
	mybalancedown(root);//check the balance after remove of max element
	globnum--;

//	free(oldroot);
	return ans;
	
}

void myexchange2(bheap *par, bheap *son, int l)
{
	bheap *tmpparent, *tmpleft, *tmpright, *sonleftdown = NULL;
	bheap* sonrightdown = NULL;

	tmpparent = par->parent;
	tmpright = par->right;
	tmpleft = par->left;
	

	
	
		if(son->left != NULL)
		{
			//leftsonchildup = son->left;
			sonleftdown = son->left;
			
		}

		if(son->right != NULL)
		{
			//rightsonchildup = son->right;
			sonrightdown = son->right;
		}		


//		par->parent = son;
		
		if(l == LEFT)
		{
			if(par->left != NULL)
				par->left->parent = tmpparent;
			if(par->right != NULL)
				par->right->parent = son;
			
			son->left = par;
			son->right = tmpright;
		
			if(par->parent != NULL && par->parent->left == par)	
				par->parent->left = son;
			if(par->parent != NULL && par->parent->right == par)
				par->parent->right = son;
			
			par->parent = son;
			par->left = sonleftdown;
			par->right = sonrightdown;
			if(sonrightdown != NULL)
				sonrightdown->parent = par;
			if(sonleftdown != NULL)
				sonleftdown->parent = par;
			
		}
		else
		{
			if(par->left != NULL)
				par->left->parent = son;
			if(par->right != NULL)
				par->right->parent = tmpparent;
			son->right = par;
			son->left = tmpleft;
			
			if(par->parent != NULL && par->parent->left == par)
				par->parent->left = son;
			if(par->parent != NULL  && par->parent->right == par)
				par->parent->right = son;


			par->parent = son;
			par->left = sonleftdown;
			par->right = sonrightdown;
			if(sonleftdown != NULL)
				sonleftdown->parent = par;
			if(sonrightdown != NULL)
				sonrightdown->parent = par;
			
		}

		
	
	//son->parent = tmpparent;

	
			
}

#ifdef DEBUG
void printnode(bheap* n)//debug
{
	if(n != NULL)
		printf("node: %d, ",n->a);
	else 
	{
		printf(" node: 0x0");
		return;
	}
	if(n->parent == NULL)
		printf(" parent: 0x0");
	else
		printf(" parent: %d",n->parent->a);

	if(n->left == NULL)
		printf(" left: 0x0");
	else
		printf(" left: %d", n->left->a);
	
	if(n->right == NULL)
		printf("right: 0x0");
	else
		printf("right: %d\n", n->right->a);
}
#endif

//build max heap
void mybalancedown(bheap* cv)//current value
{
	if(cv == NULL)
		return;

	if(cv->left != NULL && cv->right != NULL)
	{
		if(cv->left->a > cv->right->a) //left > right
		{
			if(cv->left->a > cv->a)//left > cv
			{
				if(cv == root)
					root = cv->left;
				myexchange2(cv,cv->left,LEFT);
				mybalancedown(cv);
			}
		}
		else//right > left
		{
			if(cv->right->a > cv->a)
			{
				if(cv == root)
					root = cv->right;
				myexchange2(cv,cv->right,RIGHT);
				mybalancedown(cv);
			}
		}
	}else if(cv->left != NULL)
	{
		if(cv->left->a > cv->a)
		{
			if(cv == root)
				root =  cv->left;
			myexchange2(cv,cv->left,LEFT);
			mybalancedown(cv);
		}
	}else if(cv->right != NULL)
	{
		if(cv->right->a > cv->a)
		{
			if(cv == root)
				root = cv->right;
			myexchange2(cv,cv->right,RIGHT);
			mybalancedown(cv);
		}
	}
#ifdef DEBUG
	puts(">----------------------------\n");
	printfullheap();//test
	puts("----------------------------<\n");
#endif
}


void printfullheap()
{
int lev = level();
int termsize = gettermwidth();
int j,k,i;

for(i=0;i<=lev;i++){
//printf("\n termsize: %d,  pos: %d, g: %d\n",termsize,pos,g);

	for(j=0;j<(termsize/2)-(i*(int)pow(2,i*0.5))-(2*i);j++)
		printf(" ");
	
	

	myprintheap(root,i,termsize);
	printf("\n");
}

}

int main(int argc, char **argv)
{

	if(argc < 2)
		exit(0);

	int i;
	int args;
	
	if(argc == 2)
		 args = atoi(argv[1]);
	else
	{
		puts("Please put the number of elements to be inserted\n");
		exit(0);
	}
	srand(0);
	int rnum;

#ifdef DEBUG
printf("argc: %d\n", argc);
#endif

int m;
printf("Enter 0 random numbers or 1 for 0..k\n");
scanf("%d",&m);
if(m != 0 && m != 1)
	m = 1;

for(i=0;i<args;i++)
{	
	globnum++;
	if(!m)
	{
		rnum = rand() % 97;
		insertheap(rnum);
	}
	else
	{
		insertheap(i);
	}
}

printfullheap();

while(globnum)
{
	printf("<remove: %d>\n", myremove());
	printfullheap();
}

return 0;
}
