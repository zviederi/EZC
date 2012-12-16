#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graph.h"

int main()
{
    int generateVerts = 10;
    int generateEdgesPerVert = 1;
    
    GraphData graph;
    generateRandomGraph(&graph, generateVerts, generateEdgesPerVert);
    
    printf("== Graph ==\n");
    for(int i = 0; i < graph.edgeCount; i++)
    {
        printf("%i -> %i (%f) \n", graph.vertexArray[i],graph.edgeArray[i],graph.weightArray[i]);
    }    
}
