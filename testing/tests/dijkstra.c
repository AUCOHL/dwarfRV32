#define MAX 11
#define INFINITY 0x7fffffff

unsigned int next = 1;

void dijkstra(int G[MAX][MAX],int n,int startnode);

int main()
{
	int G[MAX][MAX],i,j,n,u;

	n = 10;

	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			G[i][j] = i*n+j;


	u = 5564891%n;
	dijkstra(G,n,u);

	return 0;
}

void dijkstra(int G[MAX][MAX],int n,int startnode)
{

	int cost[MAX][MAX],distance[MAX];
	int visited[MAX],count,mindistance,nextnode,i,j;



	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(G[i][j]==0)
				cost[i][j]=INFINITY;
			else
				cost[i][j]=G[i][j];


	for(i=0;i<n;i++)
	{
		distance[i]=cost[startnode][i];
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
					distance[i]=mindistance+cost[nextnode][i];

				
		count++;
	}
}
