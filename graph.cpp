#include "graph.h"

/*����������� �� ���������
*/
Graph::Graph()
{
	clear();
}

/*!����������� �����(�������� ������ �����)
* 
*/
Graph::Graph(const Graph &gr)
{
	QMap<uint, uint> ids;		//������ �������������� ���������������
	QVector<uint> copyVertexes;	//������ �������������� ���������� ������
	QVector<uint> copyEdges;	//������ ��������� ���
	copyVertexes = gr.getVertexIds();
	copyEdges = gr.getEdgesIds();

	clear();

	for( int i = 0; i < copyVertexes.count(); i++ )
	{//��������� ������� � ����
		ids[copyVertexes[i]] = addVertex();
	}
	for( int i = 0; i < copyEdges.count(); i++ )
	{//��������� ����
		addEdge(ids[gr.getEdgeSource(copyEdges[i])], ids[gr.getEdgeDest(copyEdges[i])]);
	}
}

/*!����������� �� ������� ���������
*\param adjMatrix - ������� ���������, ���������� ���� ���(infinity ��� ��������������)
*/
Graph::Graph(const QVector<QVector<double>> & adjMatrix)
{
	int i = 0;	//������ ������, ����������� ����
	int j = 0;	//������ �������, ����������� ����
	//������ ��� ��������������� ����������� ������
	QVector<uint> vertIds(adjMatrix.count());
	clear();	//������� ������ � ����������

	for( i; i < adjMatrix.count(); i++ )
	{//��������� ��� �������������� ������
		vertIds[i] = addVertex();
	}
	for( i = 0; i < adjMatrix.count(); i++ )
	{//���� �� ��������� ���� ����� ����� ���������
		for( j = 0; j < adjMatrix[i].count(); j++ )
		{
			if( adjMatrix[i][j] != INF ) 
			{//���� ���� ����������
				//��������� ����
				addEdge(vertIds[i], vertIds[j], adjMatrix[i][j]);
			}
		}
	}
}
/*!����� ��������� ������� � �����
*\return - ������������� �������
*/
uint Graph::addVertex()
{
	Vertex *newVertex;					//����� �������
	newVertex = new Vertex;		
	newVertex->changesCount = 0;		//������������� ����������
	newVertex->cost = INF;
	newVertex->inQueue = false;
	newVertex->last = 0;
	newVertex->id = (++maxVertId);		//��������� ������� �������������
	allVertexes[maxVertId] = newVertex;	//���������� ������� � ������ ������
	return maxVertId;					//���������� ������������� �������
}

/*!����� ��������� ���� � �����
*\param from - ������������� ������� � ������� ���������� ����
*\param to - ������������� ������� ������� ������������� ����
*\param weight - ��� ����
*\return - ������������� ������������� ���� ��� ������� �������� � ������ ������
*/
uint Graph::addEdge(uint from, uint to, double weight)
{
	//��������� ��� ������, ����� �������� ����������� ����
	QMap<uint, Vertex*>::iterator vert1 = allVertexes.find(from);
	QMap<uint, Vertex*>::iterator vert2 = allVertexes.find(to);
	Edge *newEdge;			//����� ����

	if( vert1 == allVertexes.end() || vert2 == allVertexes.end() )
	{//���� �����-�� �� ������ �� ����������
		return 0;
	}
	newEdge = new Edge;
	newEdge->from = vert1.value();	//������������� ��������� ������ ����
	newEdge->to = vert2.value();	//������������� ��������� ������ ����
	newEdge->weight = weight;		//������������� ��� ����	
	newEdge->id = (++maxEdgeId);	//��������� ���� �������������
	allEdges[maxEdgeId] = newEdge;	//��������� ���� � ������ ������
	vert1.value()->adjEdges.append(newEdge);		//��������� ������������� ���� � �������������� �������
	edgesByEnd.insert(to, maxEdgeId);				//��������� ������������� ���� � �������������� �� ���� ������
	haveNegWeights = haveNegWeights || weight < 0;	//�������� ���� �� ����
	return maxEdgeId;				//���������� ������������� ����
}

/*!����� ������� ������� �� �����
*\param id - ������������� ��������� �������
*\return - false � ������ �������, ����� true
*/
bool Graph::removeVertex(uint id)
{
	//�������� ��� ��������� �������
	QMap<uint, Vertex*>::iterator i = allVertexes.find(id);
	
	if( i == allVertexes.end() )
	{//���� ��� ����� �������
		return false;
	}
	//�������� ��� ��������� �������� ����
	QMultiMap<uint, uint>::iterator j;	
	//������������� �������, ����������� ����, �������� � ��������� �������
	uint fromId;
	//������ ����, ��������� � ��������� ������
	int delEdge;

	for( int k = 0; k < i.value()->adjEdges.count(); k++ )
	{//���� �� ������ ��� ����, ��������� �� �������
		removeEdge(i.value()->adjEdges[k]->id);
	}
	j = edgesByEnd.find(i.value()->id);	//������� �������� � ������� ����
	
	while( j != edgesByEnd.end() )
	{//���� �� ������ ��� �������� ����
		//�������  ��������� �������� ���� �� ������� ���������	
		removeEdge(j.value());	
		j = edgesByEnd.find(i.value()->id);	//������� �������� � ������� ����
	}
	delete i.value();
	allVertexes.remove(id);	//������� �������
	return true;			//�������� ��������
}

/*!����� ������� ���� �� �����
*\param id - ������������� ��������� ����
*\return - true � ������ ������, ����� false
*/
bool Graph::removeEdge(uint id)
{
	//�������� ��� ��������� ����
	QMap<uint, Edge*>::iterator i = allEdges.find(id);
	//�������� ��� �������, ������� ����
	QMap<uint, Vertex*>::iterator j;	

	if( i ==  allEdges.end() )
	{//���� ���� �����������
		return false;
	}
	//������� ����� ����� �������� � �����
	i.value()->from->adjEdges.remove(i.value()->from->adjEdges.indexOf(i.value()));								
	edgesByEnd.remove(i.value()->to->id);		//������� ���� �� ���� �������
	delete i.value();
	allEdges.remove(id);				
	return true;								//�������� ��������
}

/*!����� ������� ��������� ������ �����
*/
void Graph::clear()
{
	//��������� ��� ��������� ���������
	QMap<uint, Vertex*>::iterator i;
	QMap<uint, Edge*>::iterator j;

	for( i = allVertexes.begin(); i != allVertexes.end(); i++ )
	{//����������� ������ �� ��� ���� ������
		delete i.value();
	}
	for( j = allEdges.begin(); j != allEdges.end(); j++ )
	{//����������� ������ �� ��� ���� ���
		delete j.value();
	}
	sourceVert = 0;		//��� ��������� ������ �����
	maxVertId = 0;		//��������� ��������� ���������������
	maxEdgeId = 0;
	haveNegWeights = false;	//��� ��� � ������������� �����
	haveNegCycle = false;	//��� ������������� ������
	acyclic = true;			//���������
	allVertexes.clear();	//������� �������� ������
	allEdges.clear();
	edgesByEnd.clear();
	negCycle.clear();
	time = 0;				//����� � ������� �� �������������
}

/*!����� ���������� ������� ��������� �����
*\param vertIds - ������ ��� ������ ���������������,
*�������������� ������� � �������� ������� ���������
*/
QVector<QVector<double>> Graph::getAdjMatrix(QVector<uint> *vertIds) const
{
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	//������������ ������� ���������
	QVector<QVector<double>> adjMatrix(allVertexes.count());
	//�������������� ������
	QVector<uint> ids;	
	//������� ��������������� � ������� ���������
	QMap<uint, uint> indexes;	
	//������ ��� ������
	int k = 0;	
	//������������� �������, ����������� ����
	uint vertId;
	//������ ������ ������� ���������
	int j = 0;
	//������������� ������ ������� � ����������������
	i = allVertexes.constBegin();
	ids = getVertexIds();		

	for( k = 0; k < ids.count(); k++ )
	{//��������� ������ ���������������� � ��������� �� ���������
		indexes[ids[k]] = k;	//��������� ������
	}
	
	i = allVertexes.constBegin();

	for( i; i != allVertexes.constEnd(); i++ )
	{//���� �� �������� ��� ��������
		//������������ ���������� ��������
		adjMatrix[j].fill(INF, allVertexes.count());

		for( k = 0; k < i.value()->adjEdges.count(); k++ )
		{//������������� ���� ���
			vertId = i.value()->adjEdges[k]->to->id;//������� ������������� �������, ����������� ����
													//���������� � ������� ��� ����	
			adjMatrix[j][indexes[vertId]] = i.value()->adjEdges[k]->weight;
		}
		j++;	//����������� ������ ������
	}
	if( vertIds != NULL )
	{//���� ����� ��������� ������� ���������������
		(*vertIds) = ids;
	}
	return adjMatrix;	//���������� ������� ���������
}

/*!����� ���������� ������ ��������������� ���� ������
* \return - ������ ���������������
*/
QVector<uint> Graph::getVertexIds() const
{
	//�������� ��� ������ �����
	QMap<uint, Vertex*>::const_iterator i = allVertexes.constBegin();
	//������ ��������������� ���� ������
	QVector<uint> vertIds(allVertexes.count());
	//������ ��������� ������� ���������������
	int k = 0;

	for( i; i != allVertexes.constEnd(); i++ )
	{//���� �� ������ �� ���� ��������
		vertIds[k] = i.key();	//���������� �������� ��������������
		k++;					//����������� ������ �������� �������
	}
	return vertIds;	//���������� ������ ���������������
}

/*!����� ���������� ������ ��������������� ���
* \return - ������ ��������������� ���
*/
QVector<uint> Graph::getEdgesIds() const
{
	//�������� ��� ���
	QMap<uint, Edge*>::const_iterator i = allEdges.constBegin();
	//������ ��������������� ���
	QVector<uint> edgeIds(allEdges.count());
	//������ �������� ������� ���������������
	int k = 0;

	for( i; i != allEdges.constEnd(); i++ )
	{//���� �� ������ �� ���� �����
		edgeIds[k] = i.key();	//���������� �������� ��������������
		k++;					//����������� ������ �������� �������
	}
	return edgeIds;	//���������� ������ ��������������
}

/*!����� ������������� ��� ����
* \param id - ������������� ����
* \param weight - ���
* \return - ������������� ����
*/
uint Graph::setEdgeWeight(uint id, double weight)
{
	//�������� ��� ���
	QMap<uint, Edge*>::iterator i;
	i = allEdges.find(id);

	if( i == allEdges.end() )
	//���� ���� �� ����������
		return 0;
	i.value()->weight = weight;
	haveNegWeights = haveNegWeights || weight < 0;
	return i.value()->id;
}

/*!������� ��������� ����� ����������� ���� ��
* ������ ��������� �� ��� ���������
* \param source - ��������
* \return - true � ������ ������ ������ ����������� ����
*/
bool Graph::findShortWays(uint source)
{
	if( allVertexes.find(source) == allVertexes.constEnd() )
	//���� ������� ���
		return false;

	resetWays();			//����� ���������� ������
	if( !haveNegWeights )
	{//���� ��� ������������� ���
		dijkstra(source);	//����� �� ��������� ��������
	}
	//����� �� ��������� ��������-�����
	else if( !bellmanFord(source) )
	{//���� ���� ������������� ����
		sourceVert = 0;		//������������� ���������� ���������
		return false;
	}
	sourceVert = source;	//������������� ��������
	return true;
}

/*!������� ��������� ����� ����������� ���� ��
* ����� ������� � ������. ������������: 
* findShortWays(source); getShortWay(dest);
* \param source - ������������� ��������� 
* \param dest - ������������� ����
* \param vertIds - ������ ��� ������ ����
* \return - ���������� ����� ���������
*/
double Graph::findShortWays(uint source, uint dest, QVector<uint> *vertIds)
{
	if( allVertexes.find(source) == allVertexes.constEnd() 
		|| allVertexes.find(dest) == allVertexes.constEnd() )
		//���� ���� �� ������ �����������
		return false;

	if( findShortWays(source) )
	{//���� ��� ������������� ������
		if( vertIds != NULL )
		{//���� ���������� ��������� ����
			(*vertIds) = getShortWay(dest);
		}
		return getDistFor(dest);
	}
	else
	{//���� ���� ������
		resetWays();
		return INF;
	}

}
/*!������� ��������� �������� �������� ��� ������ ����������� ����
* �� ������ ��������� �� ��� ��������� �������
* \param src - ������������� ���������
*/
void Graph::dijkstra(uint src)
{
	//������ ������ - ���������� �� �������� ���
	QMultiMap<double, Vertex*> candForVisit;
	//�������� ��� ������
	QMap<uint, Vertex*>::iterator i = allVertexes.begin();
	//������� �������
	Vertex *adjVert;
	//������� ����
	Edge *curEdge;
	//�������, ��� ������� ������ ���������� ����
	Vertex *min;
	//�������� ��� ����������� �������
	QMultiMap<double, Vertex*>::iterator updVert;
	//������ ������� ����
	int k;
	//������� ���������� �� �������
	double curCost;
	//���������� �� ���������� �������
	double prevCost;

	for( i; i != allVertexes.end(); i++ )
	{//���� �� ������ � ������ ��� �������
		if( i.value()->id == src )
		{//���� ��������� �������
			i.value()->cost = 0;
		}
		candForVisit.insert(i.value()->cost, i.value());
	}
	while( candForVisit.count() > 1 )
	{//���� �� ������� ��� �������
		min = candForVisit.begin().value();	//��������� ������� � ���������� ����
		prevCost = min->cost;				//���������� �� ������ �������
											//������� ������� �� ���������� �� �������� ���
		candForVisit.erase(candForVisit.begin());	
		
		for( k = 0; k < min->adjEdges.count(); k++ )
		{//���� �� �������� ��� ����
			curEdge = min->adjEdges[k];
			//������� ������� ����������
			curCost = prevCost + curEdge->weight;
			//������� ������� �������
			adjVert = curEdge->to;

			if( curCost < adjVert->cost )
			{//���� ������� ���������� ������, ��� ��� ����������
				//���� ����������� �������
				updVert = candForVisit.find(adjVert->cost, adjVert);
				candForVisit.erase(updVert);			//������� ������� �� ����������
				adjVert->cost = curCost;				//������������� ����� ����������
				adjVert->last = min;					//������������� ���������� �������
				adjVert->lastShortEdge = curEdge;		//������������� ������ ���������� ����
				candForVisit.insert(curCost, adjVert);	//���������� ���������� ������� � ������ ����������
			}
		}
	}
}

/*!������� ��������� �������� ��������-�����
* \param src - ������������� ���������
* \return - ���������� ����������� ���������(true)
*/
bool Graph::bellmanFord(uint src)
{
	//������� ������ - ���������� �� �������� ���
	QQueue<Vertex *> candForVisit;	
	//��������� �� �������, � ������� ��������������� ����
	Vertex *curVert;
	//��������� �� ������� �������
	Vertex *adjVert;
	//�������� ��� ������
	QMap<uint, Vertex*>::iterator i = allVertexes.begin();
	//�������� ��� ������� ���
	Edge *adjEdge;
	//��������� � �������� ������� �������������� �����
	Vertex *cycleVert;
	//������ ����
	int k;
	//������� ���������� �� �������
	double curCost;

	i = allVertexes.find(src);			//������� ��������
	i.value()->cost = 0;
	candForVisit.enqueue(i.value());	//��������� ������ ������� � �������

	while( !candForVisit.isEmpty() )
	{//���� ������� �� �����
		//������� ������� ��� ��������� ���
		curVert = candForVisit.dequeue();
		curVert->inQueue = false;		//�������� ��� ���������

		if( haveNegCycle && curVert == cycleVert  )
		{//���� ����������� ������������� ����
			negCycle << cycleVert->id;

			do
			{
				curVert = curVert->last;
				negCycle << curVert->id;
			}//���� �� �������� ���� ����
			while( curVert != cycleVert );
			return false;
		}
		if( curVert->changesCount == allVertexes.count() )
		{//���� ��������� ������������� ����
			haveNegCycle = true;
			cycleVert = curVert;
		}
		
		for( k = 0; k < curVert->adjEdges.count(); k++ )
		{//���� �� �������� ��� ����
			//������� ������� ����
			adjEdge = curVert->adjEdges[k];
			//���������� ����������
			curCost = curVert->cost + adjEdge->weight;
			//���������� ������� �������
			adjVert = adjEdge->to;

			if( adjVert->cost > curCost )
			{//���� ������ ����� �������� ����
				adjVert->cost = curCost;
				adjVert->last = curVert;
				adjVert->lastShortEdge = adjEdge;

				if( !adjVert->inQueue )
				{//���� ������� ��� �������
					adjVert->inQueue = true;	
					adjVert->changesCount++;
					candForVisit.enqueue(adjVert);
				}
				else
				{//����� ��������� � �������
					adjVert->changesCount++;
				}
			}
		}
	}
	return true;
}

/*!������� ���������� ��������� ������������� ���� �����
* \return - ������ ���������������
*/
const QVector<uint> & Graph::getNegCycle() const
{
	return negCycle;

}
/*!������� ���������� ������ ��������������� ������,
* ������� �� ���������� ���� �� ��������� � ����
* \param dest - ������������� ����
* \return - ������ ��������������� ������  � ���(������� � �������)
*/
QVector<uint> Graph::getShortWay(uint dest) const
{
	uint prevId = dest;	//������������� �������������� �������
	QVector<uint> path;	//���� �� ��������� � ����
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.find(dest);
	
	if( !sourceVert || i == allVertexes.constEnd() || i.value()->cost == INF )
	{//���� ������� �� ���������� ��� ����� �� ��� ���������
		return QVector<uint>();
	}
	do
	{//���� �� ��������� ��������� �������
		path.prepend(prevId);
		i = allVertexes.constFind(prevId);
		prevId = i.value()->last == NULL ? 0 :  i.value()->last->id;
	}
	while( prevId != 0 );
	return path;
}

/*!������� ���������� ���������� �� ��������� �� ����
* \param dest - ������������� ����
* \return - ���������� �� ����
*/
double Graph::getDistFor(uint dest) const
{
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(dest);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//���� ����� �� ��� ��������� ������� �����������
		return INF;
	return i.value()->cost;
}

/*!������� ���������� ������������� ���������� �������
* �� ���������� ����
* \param id - ������������� �������
* \return - ������������� ���������� �������, 0 � ������ ������
*/
uint Graph::getLast(uint id) const
{
	//�������� ��� �������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//���� ������� ��� ��� �������� �� ����������
		return 0;
	return i.value()->last->id;
}

/*!������� ���������� ������������� ����, ��������
*  � �������, �� ������� �������� ���������� ����
* \param id - ������������� �������
* \return - ������������� ����
*/
uint Graph::getLastShortEdge(uint id)
{
	//�������� ��� �������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//���� ������� ��� ��� �������� �� ����������
		return 0;
	return i.value()->lastShortEdge->id;
}

/*!������� ���������� ������������� �������, ����������� ����
* \param id - ������������� ����
* \return - ������������� �������
*/
uint Graph::getEdgeSource(uint id) const
{
	//�������� ��� ���
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//���� ���� ���
		return 0;
	return i.value()->from->id;
}
										
/*!������� ���������� ������������� �������, ����������� ����
* \param id - ������������� ����
* \return - ������������� �������
*/
uint Graph::getEdgeDest(uint id) const
{
	//�������� ��� �������
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//���� ���� ���
		return 0;
	return i.value()->to->id;
}

/*!������� ���������� ��� ����
* \param id  - ������������� ����
* \return - ��� ����
*/
double Graph::getEdgeWeight(uint id) const
{
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//���� ���� ���
		return 0;
	return i.value()->weight;
}

/*!������� ���������� ��������� ������ ���������� �����
*/
void Graph::resetWays()
{
	//�������� ��� ������
	QMap<uint, Vertex*>::iterator i;
	i = allVertexes.begin();

	for( i; i != allVertexes.end(); i++ )
	{//����� ���������� ������
		i.value()->last = 0;
		i.value()->changesCount = 0;
		i.value()->cost = INF;
		i.value()->lastShortEdge = NULL;
	}
	//������� ���������� �������������� �����
	haveNegCycle = false;
	negCycle.clear();
}

/*!������� ������������ ����� � �������
*/
void Graph::depthFirstSearch()
{
	//�������� ��� ������
	QMap<uint, Vertex*>::iterator i;
	i = allVertexes.begin();
	
	for( i; i != allVertexes.end(); i++ )
	{//���� �� ���������� ��� �������
		i.value()->dfsColor = 0;	//������� �� ��������
		i.value()->dsfLast = NULL;	//���������� ������
	}	
	i = allVertexes.begin();
	
	for( i; i != allVertexes.end(); i++ )
	{//���� �� ���������� ��� �������
		if( i.value()->dfsColor == 0 )
		{//���� ������� ��� �� ��������
		//	time = 0;
			//�������� �
			depthFirstSearchVisiter(i.value());
		}
	}	
}

/*!������� ������������ ��������� ������ ��� ������ � �������
* \param - greyVert - ���������� �������
*/
void Graph::depthFirstSearchVisiter(Vertex* greyVert)
{
	greyVert->dfsColor = 1;		//������� ��������
	time++;						//����� ��������
	greyVert->dfsStart = time;	//���������� �������� ������ ���������

	for( int i = 0; i < greyVert->adjEdges.count(); i++ )
	{//���� �� ���������� ��� ������� �������
		if( greyVert->adjEdges[i]->to->dfsColor == 0 )
		{//���� ������� ������� �� ���� ��������
			greyVert->adjEdges[i]->to->dsfLast = greyVert;
			depthFirstSearchVisiter(greyVert->adjEdges[i]->to);
		}
		else 
			//���� ��������� ���������� �������
			acyclic = greyVert->adjEdges[i]->to->dfsColor != 1;
	}
	greyVert->dfsColor = 2;		//������� ���������� ���������
	greyVert->dfsFinish = time;	//���������� �������� ���������� ���������
	time++;						//����� ��������
}

/*!������� ���������� �������������� ������
* � ������� �������������� ����������
* \return - ����� ��������������� � ���������� �������
* � �������� �����
*/
QMultiMap<uint, uint> Graph::getTopSortOrder()
{
	QMap<uint, Vertex*>::iterator i;	//�������� ��� ������
										//�������, ��������������� �� ��������� time
	QMultiMap<uint, uint> topSortOrderByTime;	
	QMultiMap<uint, uint> topSortOrder;
	QMultiMap<uint, uint>::iterator j;	//�������� ��� ������� ������ ������
	uint lastTime;						//���������� �������� ��������� time
	uint pos;							//������ ������� � �������
	depthFirstSearch();					//����� � �������

	if( acyclic )
	{//���� ���� ������������
		i = allVertexes.begin();

		for( i; i != allVertexes.end(); i++ )
		{//���� �� ���������� ��� �������
			//������������� ��������������
			topSortOrderByTime.insert(-i.value()->dfsFinish, i.value()->id);
		}
		j = topSortOrderByTime.begin();
		pos = 1;

		if( j != topSortOrderByTime.end() )
			lastTime = j.key();

		for( j; j != topSortOrderByTime.end(); j++ )
		{//���� �� �������� ��� �������
			if( lastTime < j.key() )
				pos++;

			topSortOrder.insert(pos, j.value());
			lastTime = j.key();
		}

	}
	return topSortOrder;
}

/*!������� ���������� �������� �� ���� ������������
* 'return - true ��� false
*/
bool Graph::isAcyclic() const
{
	return acyclic;
}
/*!������� ���������� ����, ������� �������
* \param id - ������������ �������
* \return - ������������� ���
*/
QVector<uint> Graph::getAdjEdges(uint id) const
{
	//������������ ������
	QVector<uint> adjEdges;
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);

	if( i == allVertexes.constEnd() )
	//���� ������� ���
		return adjEdges;
	adjEdges.resize(i.value()->adjEdges.count());

	for( int j = 0; j < i.value()->adjEdges.count(); j++ )
	//���� �� ��������� �������������� ���� ������
		adjEdges[j] = i.value()->adjEdges[j]->id;
	return adjEdges;
}

/*!������� ���������� ������� �������
* \param id - ������������� �������
* \return - �������
*/
uint Graph::getVertexDegree(uint id) const
{
	return getVertexHalfDegreeIn(id) + getVertexHalfDegreeOut(id);
}

/*!������� ���������� ����������� ������ �������
* \param id - ������������� �������
* \return - �����������
*/
uint Graph::getVertexHalfDegreeIn(uint id) const
{	
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	//�������� ��� ��������� ���
	QMultiMap<uint, uint>::const_iterator j;
	//������������ �����������
	uint degree = 0;
	i = allVertexes.constFind(id);
	j = edgesByEnd.constFind(i.value()->id);
	
	for( j; j != edgesByEnd.constEnd(); j++ )
		degree++;

	return degree;
}

/*!������� ���������� ����������� ������ �������
* \param id - ������������� �������
* \return - �����������
*/
uint Graph::getVertexHalfDegreeOut(uint id) const
{
	//�������� ��� ������
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	return i.value()->adjEdges.count();
}