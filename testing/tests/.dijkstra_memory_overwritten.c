#define MAX 10
#define INFINITY 0x7fffffff

unsigned int next = 1;

int rand() { //not so random
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}


void dijkstra(int G[MAX][MAX],int n,int startnode);
 
int main()
{
    int G[MAX][MAX],i,j,n,u;

    n = rand()%MAX+6;
    
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            G[i][j] = rand();
            if ((i*j)&1) 
                next = i+j;
        }
    }

    

    u = rand()%n;
    dijkstra(G,n,u);
    
    return 0;
}
 
void dijkstra(int G[MAX][MAX],int n,int startnode)
{
 
    int cost[MAX][MAX],distance[MAX],pred[MAX];
    int visited[MAX],count,mindistance,nextnode,i,j;
    
    //pred[] stores the predecessor of each node
    //count gives the number of nodes seen so far
    //create the cost matrix
    for(i=0;i<n;i++)
        for(j=0;j<n;j++)
            if(G[i][j]==0)
                cost[i][j]=INFINITY;
            else
                cost[i][j]=G[i][j];
    
    //initialize pred[],distance[] and visited[]
    for(i=0;i<n;i++)
    {
        distance[i]=cost[startnode][i];
        pred[i]=startnode;
        visited[i]=0;
    }
    
    distance[startnode]=0;
    visited[startnode]=1;
    count=1;
    
    while(count<n-1)
    {
        mindistance=INFINITY;
        
        //nextnode gives the node at minimum distance
        for(i=0;i<n;i++)
            if(distance[i]<mindistance&&!visited[i])
            {
                mindistance=distance[i];
                nextnode=i;
            }
            
            //check if a better path exists through nextnode            
            visited[nextnode]=1;
            for(i=0;i<n;i++)
                if(!visited[i])
                    if(mindistance+cost[nextnode][i]<distance[i])
                    {
                        distance[i]=mindistance+cost[nextnode][i];
                        pred[i]=nextnode;
                    }
        count++;
    }
}
