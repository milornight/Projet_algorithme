#include<cstdio>
#include<iostream>
#include<vector>
#include<map>
#include<cstring>
#include<time.h>


#define N 5     //Le nombre de sommets dans le graphe d'origine
#define INF 100


using namespace std;


// classe arete
class Edge {
public:
    int src, dest, poids; //source, destination et poids pour chaque arete

    // classer les arêtes par les poids en ordre décroissant 
    friend bool operator < (Edge e1, Edge e2){
        return e1.poids < e2.poids;
    }
  Edge(){};
  Edge(const Edge& e);  //constructeur par copie
};

Edge::Edge(const Edge& e):src(e.src),dest(e.dest),poids(e.poids){}


// classe de l'arbre
class Arbre {
public:
    int sommets[N]; // tableau du sommet
    int g[N][N];    // matrice d'adjacence
    map <string, Edge> edges;   //liste d'arête
    // map: source et destinataire comme key et edges comme la valeur
    int m[N];   // tableau de la limite déjà utilisé
    int dp[N];
    vector <Edge> dp1[N];    //tableau pour stocker les arêtes de poids maximum 
    Edge dp2;    //l'arête qui lie le point indiqué
    int poids;  
    Arbre();
    Arbre(const Arbre& a);  //constructeur par copie
};

// constructeur de l'arbre, initialisation
Arbre::Arbre() {
    
    memset(this->sommets, 0, N*sizeof(int));
    memset(this->g, 0, N * N*sizeof(int));
    memset(this->m, 0, N * sizeof(int));
    memset(this->dp,-1, N*sizeof(int));
    this->poids = 0;
    
}

Arbre::Arbre(const Arbre& a){
  for(int i=0;i<N;i++){
    this->sommets[i] = a.sommets[i];
    this->m[i] = a.m[i];
    for(int j=0;j<N;j++){
      this->g[i][j] = a.g[i][j];
    }
  }
  this->poids = a.poids;
  for(auto iterE : a.edges){
    this->edges.insert(iterE);
  }
}

// classe pour le graphe
class Graph {
public:
  int V,E;    //nombre de sommets et d'aretes
  int sommets[N]; //liste de sommets
  vector <class Edge> edges;  // liste d'aretes
  int g[N][N];  // liste d'adjacence (valeur)
  //int father[N]; // parent pour les sommets 
  int limite[N]; //limite de degre pour chaque sommet
  int mini;
  //vector<class Graph> sousG; //les sous-ensembles de graphe
  vector<Edge> nonEdges;    // liste des arêtes qui ne peuvent plus utilisées
  vector<Arbre> Tmini; //les arbres couvrants
  Graph();
  Graph(int V,int E,int lim[N],int g[N][N]);
};

Graph::Graph(){
    // initialisation
    this->mini = 0;
     
    for(int i=0;i<N;i++){
      sommets[i] = 0;
      for(int j=0;j<N;j++)
	    g[i][j] = INF;
    }
}

Graph::Graph(int V,int E,int lim[N],int g[N][N]):V(V),E(E)
{
    // initialisation
    this->mini = 0;
    
    for(int i=0;i<N;i++){
        this->sommets[i]=1;
	this->limite[i] = lim[i];
	    for(int j=0;j<N;j++){
	      this->g[i][j] = g[i][j];
	    }
    }
    
}

// créé une liste d'arête à partir de la matrice d'adjacence du graphe G 
//et retourne le nombre total d'arêtes
int creatEdgesG(Graph& G) {
    int nbE=0;  // nombre d'arêtes
    Edge* p;
    for (int i = 0; i < N; i++)
      for (int j = i + 1; j < N; j++){
	      if (G.g[i][j] != INF) {
          nbE += 1;
          p = new Edge;
          p->src = i;
          p->dest = j;
          p->poids = G.g[i][j];
          G.edges.push_back(*p);
          delete p;
	      }
      }
    return nbE;
  }

// Ajouter une arête e dans un arbre a
void plusEdge(Arbre& a, Edge& e){
  a.g[e.src][e.dest] = a.g[e.dest][e.src] = e.poids;
  a.poids += e.poids;
  a.m[e.src]++;
  a.m[e.dest]++;
  a.sommets[e.src] = a.sommets[e.dest] = 1;
  string str = to_string(e.src) + "-" + to_string(e.dest);
  a.edges[str] = e;
}


// supprimer une arête e de l'arbre a
void minusEdge(Arbre& a, Edge& e) {
    a.poids -= e.poids;
    a.m[e.src]--;
    a.m[e.dest]--;
    a.g[e.src][e.dest] = a.g[e.dest][e.src] = 0;
    string str = to_string(e.src) + "-" + to_string(e.dest);
    map<string,Edge>::iterator iter = a.edges.find(str);
    a.edges.erase(iter);
}


// trouver des arêtes qui sont de poids maximum parmi les arêtes allant du point x au point s
int dfs1(Arbre& a, int x, int s, int pre){
    int i;
    for (i = 0; i <= N; i++){
        if (i != pre && a.g[x][i]){
        if (a.dp[i] == -1){
                // comparer le poids de l'arête du point précédent du x à x et l'arête allant de x à i
                if (a.dp[x] > a.g[x][i]){
                    a.dp1[i].clear();
                    for (auto itE : a.dp1[x]) {
                        a.dp1[i].push_back(itE);
                    }
                    a.dp[i] = a.dp[x];
                }
                else if(a.dp[x] == a.g[x][i]){
                    for(auto itE: a.dp1[x]){
                    a.dp1[i].push_back(itE);
                    }
                    Edge* pe = new Edge;
                    pe->poids = a.g[x][i];
                    pe->src = x < i ? x : i;   // enregistre cette arête
                    pe->dest = x > i ? x : i;
                    a.dp1[i].push_back(*pe);
                    delete pe;
                    a.dp[i] = a.dp[x];
                }
                else {  //a.dp1[x].poids < a.g[x][i]
                    a.dp1[i].clear();
                    Edge* pe = new Edge;
                    pe->poids = a.g[x][i];
                    pe->src = x < i ? x : i;   
                    pe->dest =x > i ? x : i;
                    a.dp1[i].push_back(*pe);
                    a.dp[i] = a.g[x][i];
                    delete pe;
                }
            }
            dfs1(a, i, s, x);   // récursive pour parcourir tous les sommets de x à s
        }
    }
    return a.dp[s];
}

// trouver une arête qui est de poids maximum et reliée avec point x parmi les arêtes allant de x au point s
static int flag = 0;
int dfs2(Arbre& a, int x, int xc,  int s, int pre){

    for (int i = 0; i < N; i++){
        if (i != pre && a.g[x][i]){
            if(i==s){
                flag = 1;
            }else{
                dfs2(a, i, xc, s, x);
            }
            if(x==xc && flag){
                a.dp2.src = x < i ? x : i;
                a.dp2.dest = x > i ? x : i;
                a.dp2.poids = a.g[x][i];
                return a.dp2.poids;
            }
        }
    }
    return -1;
}

// trouver de l'arbre couvrant en tester s'il satisfait la limitation
void trouver(Graph& G){
  Arbre* pa = new Arbre();
  G.nonEdges.clear();
  for(auto iterE:G.edges){
    if(pa->sommets[iterE.src] && pa->sommets[iterE.dest]) {
      G.nonEdges.push_back(iterE);
      continue;
    }
    
    if(pa->m[iterE.src]==G.limite[iterE.src] || pa->m[iterE.dest]==G.limite[iterE.dest]) {
      G.nonEdges.push_back(iterE);
      continue;
    }

    plusEdge(*pa,iterE);
  }
  G.Tmini.push_back(*pa);
  delete pa;
}

vector<Arbre> tmp;

// optimiser l'arbre couvrant pour trouver les autres arbres couvrants de poids minimum
void optimizer(Graph& G){
    for(auto iterE:G.nonEdges){
      tmp.clear();
        for(auto iterA:G.Tmini){
            int max = 0;    //poids d'arête à supprimer
            
            //tous les deux sommets sont full (valuation)
            if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
                continue;
            }

            //tous les deux sommets ne sont pas full
            if(iterA.m[iterE.src]<G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){

                memset(iterA.dp,-1,N*sizeof(int));
                iterA.dp[iterE.src] = -INF;

                max = dfs1(iterA,iterE.src, iterE.dest, -1); 
                if (iterE.poids < max) continue;
                if (iterE.poids == max) {
                    for (auto itE : iterA.dp1[iterE.dest]) {
                        Arbre* pa = new Arbre(iterA);
                        minusEdge(*pa, itE);
                        plusEdge(*pa, iterE);
                        tmp.push_back(*pa);
                        delete pa;
                    }
                }else{   //iterE.poids > max
                    G.Tmini.clear();
                    for (auto itE : iterA.dp1[iterE.dest]) {
                        Arbre* pa = new Arbre(iterA);
                        minusEdge(*pa, itE);
                        plusEdge(*pa, iterE);
                        tmp.push_back(*pa);
                        delete pa;
                    }
                }
                continue;
            }
        
            flag = 0;
            //src est full et dest n'est pas full
            if(iterA.m[iterE.src]==G.limite[iterE.src] && iterA.m[iterE.dest]<G.limite[iterE.dest]){
                max = dfs2(iterA,iterE.src, iterE.src,iterE.dest, -1); 
            }
            //src est full et dest n'est pas full
            if(iterA.m[iterE.src]<G.limite[iterE.src] && iterA.m[iterE.dest]==G.limite[iterE.dest]){
                max = dfs2(iterA,iterE.dest, iterE.dest,iterE.src, -1); 
            }

            if (iterE.poids > max) continue;
            if (iterE.poids == max) {
                Arbre* pa = new Arbre(iterA);
                minusEdge(*pa, iterA.dp2);
                plusEdge(*pa, iterE);
                tmp.push_back(*pa);
                delete pa;
                continue;
            }
            else{    //iterE.poids < max
                G.Tmini.clear();
                G.mini = G.mini + iterE.poids - max;
                Arbre* pa = new Arbre(iterA);
                minusEdge(*pa, iterA.dp2);
                plusEdge(*pa, iterE);
                tmp.push_back(*pa);
                delete pa;
            }
        }
        for(auto it:tmp){
          G.Tmini.push_back(it);
        }
    }
}


int main(){
    float temps;
    clock_t t1, t2;


    //generer le graphe
    int lim[N] = {1,3,1,2,2};
    int a[N][N] = {{INF,1,INF,INF,1},
            {1,INF,1,1,1},
            {INF,1,INF,1,INF},
            {INF,1,1,INF,1},
            {1,1,INF,1,INF}};
    Graph G1(5,7,lim,a);

    t1 = clock();

    creatEdgesG(G1);
    //trouver le premier arbre couvrant
    trouver(G1);
      
    optimizer(G1);

    t2 = clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    cout << "temps:" << temps << endl;

    //afficher le resultat
    int nbA = 1;    //nombre d'arbre
    for(auto iterA:G1.Tmini){
        cout<<"Arbre "<<nbA<<": Le poids est "<<iterA.poids<<endl;
        cout<<"Les aretes sont:"<<endl;
        for(auto iterE:iterA.edges){
            cout<<iterE.first<<endl;
        }
        cout<<" "<<endl;
        nbA++;
    }

  return EXIT_SUCCESS;
}
