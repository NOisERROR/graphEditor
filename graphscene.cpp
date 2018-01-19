#include "GraphScene.h"

/*!����������� �����
*/
GraphScene::GraphScene()
{
	setItemIndexMethod(NoIndex);
	vertNameCount = 1;			//��������� �������� ����� �������
	edgeNameCount = 1;			//��������� �������� ����� ����
								//��������� ������ �����
	setSceneRect(QRectF(0, 0, 800, 1000));
	startForEdge = NULL;		//���������� �� ����������� ����
	addingEdge = NULL;
	addingVertex = NULL;		//���������� �� ����������� �������
	sourceOfSearching = NULL;	//��� ��������� ������ ������
	mode = MovingMode;			//����� �����������
}

/*!������� ��������� �������
* \param pos - ���������� �������
* \param name - ��� �������
* \return - ���������� ��������
*/
bool GraphScene::addVertex(QPointF pos, const QString &name)
{
	if( vertByName.find(name) != vertByName.end() )
	//���� ��� ������
		return false;
	VertexItem *newVertex = addVertexItem(pos, name);	//�������� ����� ������� � �����

	if( newVertex == NULL )
	//���� �� ������� ������
		return false;
	uint newId = graph.addVertex();	//������������� ������� � �����
	newVertex->setId(newId);		//��������� �������������
	vertexes[newId] = newVertex;	//������������� ����� 
	vertByName[name] = newVertex;
	return true;
}

/*!������� ������� �������
* \param name - ��� �������
* \return - ���������� ��������
*/
bool GraphScene::removeVertex(const QString &name)
{
	//�������� ��� �������
	QMap<QString, VertexItem*>::iterator i;
	//���� �� ��������
	QVector<EdgeItem *> edgesForRemoving;
	i = vertByName.find(name);

	if( i == vertByName.end() )
	//���� ������� ���
		return false;

	while( i.value()->getInEdges().count() != 0 )
	{//������� ��� �������� ����
		removeEdge(i.value()->getInEdges().first()->getText());
	}
	while(i.value()->getOutEdges().count() != 0 )
	{//������� ��� ��������� ����
		removeEdge(i.value()->getOutEdges().first()->getText());
	}
	graph.removeVertex(i.value()->getId());	//������� �� �����
	int ids = i.value()->getId();			
	vertexes.remove(i.value()->getId());	//������� �� ���� �������
	removeVertexItem(i.value());
	vertByName.remove(i.key());
	return true;
}

/*!������� ������� ����
* \param name - ��� ����
* \return - ���������� ��������
*/
bool GraphScene::removeEdge(const QString &name)
{
	//�������� ��� ����
	QMap<QString, EdgeItem* >::iterator i;
	i = edgeByName.find(name);

	if( i == edgeByName.end() )
	//���� ���� �� �������
		return false;
	graph.removeEdge(i.value()->getId());	//������� �� �����
	edges.remove(i.value()->getId());		//������� �� ���� �������
	removeEdgeItem(i.value());	
	edgeByName.remove(i.key());
	return true;
}

/*!������� ��������� � ����� ����
* \param from - ��������� �������
* \param to - �������� �������
* \param name - ��� ����
* \return - ��������� �� ����� ����
*/ 
EdgeItem* GraphScene::addEdgeItem(VertexItem *from, VertexItem *to, const QString &name)
{
	EdgeItem *newItem = new EdgeItem(from, to, name);

	if( newItem != NULL )
		addItem(newItem);
	return newItem;
}

/*!������� ��������� � ����� �������
* \param pos - ���������� �������
* \param name - ��� �������
* \param timeVert - ���� ��������� ������� 
*/
VertexItem* GraphScene::addVertexItem(QPointF pos, const QString & name, bool timeVert)
{
	VertexItem *newItem = new VertexItem(name, timeVert);
	
	if( newItem != NULL )
	{
		newItem->setPos(pos);
		addItem(newItem);
	}
	return newItem;
}

/*������� ��������� ����
* \param from - ��� ��������� �������
* \param to - ��� �������� �������
* \param name - ��� �������
* \return - ���������� ����������
*/
bool GraphScene::addEdge(const QString &from, const QString &to, const QString &name)
{
	QMap<QString, VertexItem*>::iterator i1;	//�������� ��� ��������� �������
	QMap<QString, VertexItem*>::iterator i2;	//�������� ��� �������� �������
	i1 = vertByName.find(from);					//����� ������
	i2 = vertByName.find(to);

	if( i1 == vertByName.end() || i2 == vertByName.end() 
		|| edgeByName.find(name) != edgeByName.end() )
		//���� ����� ������ �� ���������� ��� ���� ��� ���� � �������� ������
		return false;
	//������ ����
	EdgeItem *newEdge = addEdgeItem(i1.value(), i2.value(), name);

	if( newEdge == NULL )
		return false;
	//������������ �� ���� �����
	uint newId = graph.addEdge(i1.value()->getId(), i2.value()->getId());
	newEdge->setId(newId);
	//������������� �����
	edges[newId] = newEdge;
	edgeByName[name] = newEdge;
	return true;
}

/*!������� ������� ���� �� �����
* \param edge - ��������� �� ����
*/
void GraphScene::removeEdgeItem(EdgeItem* edge)
{
	if( edge != NULL )
	{
		removeItem(edge);
		delete edge;
	}
}

/*!������� ������� ������� �� �����
* \param vert - ��������� �� �������
*/
void GraphScene::removeVertexItem(VertexItem* vert)
{
	if( vert != NULL )
	{
		removeItem(vert);
		delete vert;
	}
}


/*!���������� ������� ������ ����
*/
void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if( mouseEvent->button() == Qt::LeftButton )
	{//���� ������ ����� ������ ����
		if( mode == AddVertexMode )
		{//���� ����� ���������� ������
			if( mouseEvent->scenePos().x() >= width() - 100 ||  
							mouseEvent->scenePos().y() >= height() - 100 )
			{//���� ����� �� ������� �����
				 incSceneRect();
			}
			//��������� ������� � �����
			addVertex(mouseEvent->lastScenePos(), QString::number(vertNameCount));
			//�������� ������ ���������� �������
			emit vertexAdding(QString::number(vertNameCount));
			vertNameCount++;	//����������� �������� �����
		}
		else if( mode == ShowingOneShortWay || mode == ShowingShortWays )
		{//���� ����� ����������� �����
			setMode(MovingMode);
		}
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}

/*!���������� ������� ���������� ������ ����
*/
void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	//���������� ��������
	QList<QGraphicsItem *> selects = selectedItems();
	bool vertexPressed = false;	//�������� �������
	bool edgePressed = false;	//�������� ����

	if( mode == AddEdgeMode )
	{//���� ����� ���������� ���
		for( int i = 0; i < selects.count() && !vertexPressed; i++ )
		//��������� ���� �� ���������� �������
			vertexPressed = selects[i]->type() == VertexItem::Type;

		if( !vertexPressed )
		{//���� ������� �� ��������, �� ������
			edgesManager(Cancel);
		}
	}
	else if( mouseEvent->button() == Qt::LeftButton )
	{//���� �������� ����� ������ ����
		if( mode == DeleteMode )
		{//���� ����� �������� ���������
			for( int i = 0; i < selects.count(); i++ )
			{//��������� ���� �� ������� �����-���� �� ���������
				vertexPressed = selects[i]->type() == VertexItem::Type;
				edgePressed = selects[i]->type() == EdgeItem::Type;

				//������� ��������
				if( vertexPressed )
					removeVertex(qgraphicsitem_cast<VertexItem*>(selects[i])->getText());
				else if( edgePressed )
					removeEdge(qgraphicsitem_cast<EdgeItem*>(selects[i])->getText());
			}
			emit nothingSelected();
		}
		else if( mode == MovingMode )
		{//���� ����� ����������� ������
			for( int i = 0; i < selects.count() && !vertexPressed; i++ )
			{//���� ������� ������ �� ������� �����
				while( selects[i]->pos().x() > width() - 100 || selects[i]->pos().y() > height() - 100 )
				//����������� �����
					 incSceneRect();
			}
		}
	}
	if(  mode != DeleteMode && selects.count() > 0 )
	{//���� ���� ������� ��������
		if( selects[0]->type() == VertexItem::Type )
			emit vertexSelected(qgraphicsitem_cast<VertexItem*>(selects[0])->getText());
		else if( selects[0]->type() == EdgeItem::Type )
			emit edgeSelected(qgraphicsitem_cast<EdgeItem*>(selects[0])->getText());
	}
	else if( mode == DeleteMode || selects.count() == 0 )
		emit nothingSelected();
	return QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

/*!���������� �������� ����
*/
void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if( mouseEvent->scenePos().x() < 10 || mouseEvent->scenePos().y() < 10 )
		//����� �� ��������� ������� �� ���� �� ��������� �����
			return;
	if( mode == AddVertexMode && addingVertex != NULL )
	{//���� ����� ���������� ������
		addingVertex->setVisible(true);
		addingVertex->setPos(mouseEvent->lastScenePos());
	}
	else
	{
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
}

/*!���������� ������ ����������
*/
void GraphScene::keyPressEvent(QKeyEvent *event)
{	
	if( event->key() == Qt::Key_Delete )
	{//���� ������ ������ Delete
		QList<QGraphicsItem *> selects = selectedItems();

		for( int i = 0; i < selects.count(); i++ )
		{//���� ���� ������� ��������
			//������� ��
			if( selects[i]->type() == VertexItem::Type )
				removeVertex(qgraphicsitem_cast<VertexItem*>(selects[i])->getText());
			else if( selects[i]->type() == EdgeItem::Type )
				removeEdge(qgraphicsitem_cast<EdgeItem*>(selects[i])->getText());
			emit nothingSelected();
		}
		if( mode == AddEdgeMode )
		//���� ��� ����� ���������� ���
			edgesManager(Cancel);
		
		if( selects.count()  && mode == ShowingOneShortWay || mode == ShowingShortWays )
		{//���� ��� ����� ����������� �����
			setMode(MovingMode);
		}
	}
	QGraphicsScene::keyPressEvent(event);
}

/*!������� ����������� �����
*/
void GraphScene::incSceneRect()
{
	//����������� ������ �����
	setSceneRect(0, 0, width() + 100, height() + 100);
}

/*�������� ���������� ���
* \param state - ��������� ����������
* \param vert - �������, ���������� ���������
*/
void GraphScene::edgesManager(EdgesVertState state,VertexItem *vert)
{
	if( mode != AddEdgeMode )
	//���� ���������������� �����
		return;

	if( state == Selected )
	{//���� ������� ���� �������
		if( startForEdge == NULL )
		//���� ��� �� �������� ��������� �������
			startForEdge = vert;
		else
		{//���� ����� �������� ����
			edgesManager(HoverOut);
			QString from = startForEdge->getText();
			QString to = vert->getText();
			addEdge(from, to, QString::number(edgeNameCount));
			emit edgeAdding(QString::number(edgeNameCount));
			edgeNameCount++;
			startForEdge = vert;
		}
	}
	else if( state == HoverOn )
	{//���� �� ������� ������ ������
		if( startForEdge != NULL )
		{//���� ����� ������� ������� ����
			addingEdge = addEdgeItem(startForEdge, vert, "");

			if( addingEdge == NULL )
				return;
			addingEdge->setColor(QColor(0, 0, 0, 100));
			addingEdge->setAcceptHoverEvents(false);
		}
	}
	else if( state == HoverOut )
	{//���� � ������� ����� ������
		if( startForEdge != NULL && addingEdge != NULL )
		//���� ������� ���� �� �������
		{
			removeEdgeItem(addingEdge);
			addingEdge = NULL;
		}
			
	}
	else if( state == Cancel )
	{//���� �������� ���� ����������
		startForEdge = NULL;
		addingEdge = NULL;
	}
}


/*!������� ������������� ����� �����
* \param m - �����
*/
void GraphScene::setMode(Mods m)
{
	if( mode == AddVertexMode )
	{//���� ����� ������� ��������� �������
		removeVertexItem(addingVertex);
	}
	else if( mode == AddEdgeMode )
	{//���� ����� �������� ����� ���������� ���
		edgesManager(Cancel);
	}
	else if( mode == ShowingOneShortWay || mode == ShowingShortWays )
	{//���� ����� �������� ���������� ��������
		resetShortWays();
		sourceOfSearching = NULL;
	}
	mode = m;
	
	if( mode == AddVertexMode )
	{//���� ���������� ����� ���������� ������
		addingVertex = addVertexItem(QPointF(0, 0), "", true);
		addingVertex->setTransp(100);
		addingVertex->setAcceptHoverEvents(false);
		addingVertex->setVisible(false);
	}
}

/*!������� ���������� �����
* \return - �����
*/
int GraphScene::getMode()
{
	return mode;
}

/*!������� ���������� ���������� � �������
* \return - ��������� � �����������
*/
VertexInfo GraphScene::getVertexInfo(const QString &name)
{
	//�������������� ���������
	VertexInfo inf;
	QMap<QString, VertexItem*>::iterator i;
	i = vertByName.find(name);

	if( i == vertByName.end() )
	//���� ��� ������� � ������ ������
		return inf;
	inf.name = i.value()->getText();
	inf.dist = graph.getDistFor(i.value()->getId());
	return inf;
}

/*!������� ���������� ���������� � ����
* \return - ��������� � �����������
*/
EdgeInfo GraphScene::getEdgeInfo(const QString &name)
{
	//�������������� ���������
	EdgeInfo inf;
	QMap<QString, EdgeItem*>::iterator i;
	i = edgeByName.find(name);

	if( i == edgeByName.end() )
	//���� ��� ������� � ������ ������
		return inf;
	inf.name = i.value()->getText();
	inf.weight = graph.getEdgeWeight(i.value()->getId());
	return inf;
}

/*!������� ������������� ��� �������
* \param oldName - ������ ��� �������
* \param newName - ����� ��� �������
*/
void GraphScene::setVertexName(const QString &oldName, const QString &newName)
{	
	QMap<QString, VertexItem*>::iterator i;
	VertexItem *v;
	i = vertByName.find(newName);

	if( i != vertByName.end() )
	//���� ������� � ����� ������ ��� ����
		return;
	i = vertByName.find(oldName);

	if( i == vertByName.end() )
	//���� ��� ������� ��� ������������
		return;
	i.value()->setText(newName);
	v = i.value();
	vertByName.remove(i.key());
	vertByName[newName] = v;
}

/*!������� ������������� ��� ����
* \param oldName - ������ ��� �������
* \param newName - ����� ��� �������
*/
void GraphScene::setEdgeName(const QString &oldName, const QString &newName)
{	
	QMap<QString, EdgeItem*>::iterator i;
	EdgeItem *v;
	i = edgeByName.find(newName);

	if( i != edgeByName.end() )
	//���� ���� � ����� ������ ��� ����
		return;
	i = edgeByName.find(oldName);

	if( i == edgeByName.end() )
	//���� ��� ���� ��� ������������
		return;
	i.value()->setText(newName);
	v = i.value();
	edgeByName.remove(i.key());
	edgeByName[newName] = v;
}

/*!������� ������������� ���� ���
* \param name - ��� ����
* \param weight - ��� ����
*/
void GraphScene::setEdgeWeight(const QString &name, double weight)
{
	QMap<QString, EdgeItem*>::iterator i;
	uint id;
	i = edgeByName.find(name);
	
	if( i == edgeByName.end() )
	//���� ��� ����
		return;

	id = i.value()->getId();
	graph.setEdgeWeight(id, weight);
}

/*!������� ������������ ����� ����������� ���� ����� ����� ������
* \param source - ������ �������
* \param dest - ������
* \param way - ������ ��� ������ ������ � ��� �� ���� 
* [��������� �������|����|�������|����|...|�������� �������]
* \return - ���������� ����� ���������
*/
double GraphScene::showShortWay(const QString &source, const QString &dest, 
									QVector<QString> *way)
{

	QMap<QString, VertexItem*>::iterator src;		//�������� ��� ��������� �������
	QMap<QString, VertexItem*>::iterator dst;		//�������� ��� �������� �������
	QVector<uint> ids;								//�������������� ������ �� ���� �� ������
	QMap<uint, VertexItem*>::iterator vertOnPath;	//�������� ��� ������ �� ����
	QMap<uint, EdgeItem*>::iterator edgeOnPath;		//�������� ��� ������ �� ����
	bool fixWay = way != NULL;						//���� ������������ ���� � ways
	double dist;									//���������� �� �������� �������
	src = vertByName.find(source);
	dst = vertByName.find(dest);

	if( src == vertByName.end() || dst == vertByName.end() )
	//���� �����-���� �� ������ ����������
		return INF;
	//��������� ��������� � �������� �������
	//���� ���������� ����
	dist = graph.findShortWays(src.value()->getId(), dst.value()->getId(), &ids);
	specItems.clear();

	for( int i = 1; i < ids.count(); i += 2 )
	{//���� �� ������� ��� ����
		ids.insert(i, graph.getLastShortEdge(ids[i]));
	}
	for( int i = 0; i < ids.count(); i++ )
	{//���� �� ������� ��� ���� � �������
		if( i % 2 == 0 )
		{//���� �������
			vertOnPath = vertexes.find(ids[i]);	//������� �������
			vertOnPath.value()->setSpecial();	//�������� �
			specItems.append(vertOnPath.value());
		}
		else
		{//���� ����
			edgeOnPath = edges.find(ids[i]);	//������� ����
			edgeOnPath.value()->setSpecial();	//�������� �
			specItems.append(edgeOnPath.value());
		}
	}
	if( fixWay )
	{//���� ���������� ��������� ����
		for( int i = 0; i < specItems.count(); i++ )
		{//���� �� �������� ��� ��������
			if( specItems[i]->type() == VertexItem::Type )
			{//���� ��� �������
				way->append(qgraphicsitem_cast<VertexItem*>(specItems[i])->getText());
			}
			else if( specItems[i]->type() == EdgeItem::Type )
			{//���� ��� ����
				way->append(qgraphicsitem_cast<EdgeItem*>(specItems[i])->getText());
			}
		}
	}
	setMode(ShowingOneShortWay);	//������������� �����
	return dist;
}

/*!������� ���������� ��������� �����
*/
void GraphScene::resetShortWays()
{
	for( int i = 0; i < specItems.count(); i++ )
	{//���� �� ������ ������� �� ���� ������
		if( specItems[i]->type() == VertexItem::Type )
		{//���� ��� �������
			//������� �������
			qgraphicsitem_cast<VertexItem*>(specItems[i])->setSpecial(false);
		}
		else if( specItems[i]->type() == EdgeItem::Type )
		{//���� ��� ����
			//������� �������
			qgraphicsitem_cast<EdgeItem*>(specItems[i])->setSpecial(false);
		}
	}
	specItems.clear();//������� ������
}

/*!�������� �����
* \param state - ����� ��������
* \param distVertex - ������� - ����������
*/
void GraphScene::waysManager(WaysSearchingState state, VertexItem *distVertex)
{
	if( mode != ShowingShortWays || sourceOfSearching == NULL )
	{//����  �� ���������� ����� ������ ���� ����� �� ���������
		return;
	}

	if( state == Show )
	//���� ���������� ���������� ����
	{
		QVector<uint> path;
		path = graph.getShortWay(distVertex->getId());
		resetShortWays();

		for( int i = 1; i < path.count(); i += 2 )
		{//���� �� ������� ��� ����
			path.insert(i, graph.getLastShortEdge(path[i]));
		}
		for( int i = 0; i < path.count(); i++ )
		{//���� �� ������� ��� ���� � �������
			if( i % 2 == 0 )
			{//���� �������
				specItems.append(vertexes.find(path[i]).value());
				qgraphicsitem_cast<VertexItem*>(specItems[i])->setSpecial();
			}
			else
			{//���� ����
				specItems.append(edges.find(path[i]).value());
				qgraphicsitem_cast<EdgeItem*>(specItems[i])->setSpecial();
			}
		}
	}
	else
	//���� ���������� �������� ����
		resetShortWays();
}

/*!������� ������������ ����� ����� �� ������ ���������
*
*/
void GraphScene::shortWays(const QString &source)
{
	QMap<QString, VertexItem*>::iterator src;	//�������� ��� ��������� �������
	src = vertByName.find(source);

	if( src == vertByName.end() )
	//���� �����-���� �� ������ ����������
		return;
	graph.findShortWays(src.value()->getId());
	sourceOfSearching = src.value();
	setMode(ShowingShortWays);
}

/*!������� ��������� �������������� ���������� �����
*/
void GraphScene::topologicalSorting()
{
	//����� � ���������������� ������ � ������� ����������
	QMultiMap<uint, uint> sorted = graph.getTopSortOrder();
	//�������� �� �������
	QMultiMap<uint, uint>::iterator i;
	//�������� ��� ���������� ���
	QMap<uint, EdgeItem*>::iterator l;
	//���������� ������� ������
	uint level = 1;
	//������ ������� �� ������
	int k = 0;
	int side = 1;
	//������ ������ �� �������
	QVector<QVector<VertexItem*>> order(1);
	i = sorted.begin();

	for( i; i != sorted.end(); i++ )
	{//���� �� ���������� ��� �������
		if( level < i.key() )
		{//���� ������� ����������
			level++;
			k = 0;
			order.append(QVector<VertexItem*>());
		}
		order[level - 1].append(vertexes[i.value()]);
		k++;
	}
	while( height() / 2 / order.count() < 200 )
	//����������� ������ �����
			incSceneRect();
	for( int j = 0; j < order.count(); j++ )
	{//���� �� ��������� ������� ���� ��������
		while( width() / 2 / order[j].count() < 100 )
		//����������� ������ �����
			incSceneRect();
		for( k = 0; k < order[j].count(); k++ )
		{//���� �� ��������� ������� �������� �� ������
			order[j][k]->setPos(width() / 2 + side * k * 100 + side * 100,
								height() / 2 - (order.count() - 1) * 200 + j * 200);
		}
		side = -side;
	}

	for( l = edges.begin(); l != edges.end(); l++ )
	//��������� ����
		l.value()->updatePos();
	sorted.clear();
}
