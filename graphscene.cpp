#include "GraphScene.h"

/*!Конструктор сцена
*/
GraphScene::GraphScene()
{
	setItemIndexMethod(NoIndex);
	vertNameCount = 1;			//Начальное значение имени вершины
	edgeNameCount = 1;			//начальное значение имени дуги
								//Начальный размер сцены
	setSceneRect(QRectF(0, 0, 800, 1000));
	startForEdge = NULL;		//Изначально не добавляются дуги
	addingEdge = NULL;
	addingVertex = NULL;		//Изначально не добавляются вершины
	sourceOfSearching = NULL;	//Нет начальных вершин поиска
	mode = MovingMode;			//Режим перемещения
}

/*!Функция добавляет вершину
* \param pos - координата вершины
* \param name - имя вершины
* \return - успешность удаления
*/
bool GraphScene::addVertex(QPointF pos, const QString &name)
{
	if( vertByName.find(name) != vertByName.end() )
	//Если имя занято
		return false;
	VertexItem *newVertex = addVertexItem(pos, name);	//Создание новой вершины в сцене

	if( newVertex == NULL )
	//Если не хватает памяти
		return false;
	uint newId = graph.addVertex();	//Идентификатор вершины в графе
	newVertex->setId(newId);		//Сохраняем идентификатор
	vertexes[newId] = newVertex;	//Устанавливаем связи 
	vertByName[name] = newVertex;
	return true;
}

/*!Функция удаляет вершину
* \param name - имя вершины
* \return - успешность удаления
*/
bool GraphScene::removeVertex(const QString &name)
{
	//Итератор для вершины
	QMap<QString, VertexItem*>::iterator i;
	//Дуги на удаление
	QVector<EdgeItem *> edgesForRemoving;
	i = vertByName.find(name);

	if( i == vertByName.end() )
	//Если вершины нет
		return false;

	while( i.value()->getInEdges().count() != 0 )
	{//Удаляем все входящие дуги
		removeEdge(i.value()->getInEdges().first()->getText());
	}
	while(i.value()->getOutEdges().count() != 0 )
	{//Удаляем все выходящие дуги
		removeEdge(i.value()->getOutEdges().first()->getText());
	}
	graph.removeVertex(i.value()->getId());	//Удаляем из графа
	int ids = i.value()->getId();			
	vertexes.remove(i.value()->getId());	//Удаляем из всех списков
	removeVertexItem(i.value());
	vertByName.remove(i.key());
	return true;
}

/*!Функция удаляет дугу
* \param name - имя дуги
* \return - успешность удаления
*/
bool GraphScene::removeEdge(const QString &name)
{
	//Итератор для дуги
	QMap<QString, EdgeItem* >::iterator i;
	i = edgeByName.find(name);

	if( i == edgeByName.end() )
	//Если дуги не нашлось
		return false;
	graph.removeEdge(i.value()->getId());	//Удаляем из графа
	edges.remove(i.value()->getId());		//Удаляем из всех списков
	removeEdgeItem(i.value());	
	edgeByName.remove(i.key());
	return true;
}

/*!Функция добавляет в сцену дугу
* \param from - начальная вершина
* \param to - конечная вершина
* \param name - имя дуги
* \return - указатель на новую дугу
*/ 
EdgeItem* GraphScene::addEdgeItem(VertexItem *from, VertexItem *to, const QString &name)
{
	EdgeItem *newItem = new EdgeItem(from, to, name);

	if( newItem != NULL )
		addItem(newItem);
	return newItem;
}

/*!Функция добавляет в сцену вершину
* \param pos - координаты вершины
* \param name - имя вершины
* \param timeVert - флаг временной вершины 
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

/*Функция добавляет дугу
* \param from - имя начальной вершины
* \param to - имя конечной вершины
* \param name - имя вершины
* \return - успешность добавления
*/
bool GraphScene::addEdge(const QString &from, const QString &to, const QString &name)
{
	QMap<QString, VertexItem*>::iterator i1;	//Итератор для начальной вершины
	QMap<QString, VertexItem*>::iterator i2;	//Итератор для конечной вершины
	i1 = vertByName.find(from);					//Поиск вершин
	i2 = vertByName.find(to);

	if( i1 == vertByName.end() || i2 == vertByName.end() 
		|| edgeByName.find(name) != edgeByName.end() )
		//Если таких вершин не существует или дуга уже есть с заданным именем
		return false;
	//Создаём дугу
	EdgeItem *newEdge = addEdgeItem(i1.value(), i2.value(), name);

	if( newEdge == NULL )
		return false;
	//Сопоставляем ей дугу графа
	uint newId = graph.addEdge(i1.value()->getId(), i2.value()->getId());
	newEdge->setId(newId);
	//Устанавливаем связи
	edges[newId] = newEdge;
	edgeByName[name] = newEdge;
	return true;
}

/*!Функция удаляет дугу из сцены
* \param edge - указатель на дугу
*/
void GraphScene::removeEdgeItem(EdgeItem* edge)
{
	if( edge != NULL )
	{
		removeItem(edge);
		delete edge;
	}
}

/*!Функция удаляет вершину из сцены
* \param vert - указатель на вершину
*/
void GraphScene::removeVertexItem(VertexItem* vert)
{
	if( vert != NULL )
	{
		removeItem(vert);
		delete vert;
	}
}


/*!Обработичк нажатия кнопки мыши
*/
void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if( mouseEvent->button() == Qt::LeftButton )
	{//Если нажали левую кнопку мыши
		if( mode == AddVertexMode )
		{//Если режим добавления вершин
			if( mouseEvent->scenePos().x() >= width() - 100 ||  
							mouseEvent->scenePos().y() >= height() - 100 )
			{//Если вышли за пределы сцены
				 incSceneRect();
			}
			//Добавляем вершину к графу
			addVertex(mouseEvent->lastScenePos(), QString::number(vertNameCount));
			//Посылаем сигнал добавления вершины
			emit vertexAdding(QString::number(vertNameCount));
			vertNameCount++;	//Увеличиваем значение имени
		}
		else if( mode == ShowingOneShortWay || mode == ShowingShortWays )
		{//Если режим отображения путей
			setMode(MovingMode);
		}
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}

/*!Обработчик события отпускания кнопки мыши
*/
void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	//Выделенные элементы
	QList<QGraphicsItem *> selects = selectedItems();
	bool vertexPressed = false;	//Выделена вершина
	bool edgePressed = false;	//Выделена дуга

	if( mode == AddEdgeMode )
	{//Если режим добавления дуг
		for( int i = 0; i < selects.count() && !vertexPressed; i++ )
		//Проверяем есть ли выделенные вершины
			vertexPressed = selects[i]->type() == VertexItem::Type;

		if( !vertexPressed )
		{//Если вершины не выделены, то отмена
			edgesManager(Cancel);
		}
	}
	else if( mouseEvent->button() == Qt::LeftButton )
	{//Если отпущена левая кнопка мыши
		if( mode == DeleteMode )
		{//Если режим удаления элементов
			for( int i = 0; i < selects.count(); i++ )
			{//Проверяем были ли выбраны какие-либо из элементов
				vertexPressed = selects[i]->type() == VertexItem::Type;
				edgePressed = selects[i]->type() == EdgeItem::Type;

				//Удаляем элементы
				if( vertexPressed )
					removeVertex(qgraphicsitem_cast<VertexItem*>(selects[i])->getText());
				else if( edgePressed )
					removeEdge(qgraphicsitem_cast<EdgeItem*>(selects[i])->getText());
			}
			emit nothingSelected();
		}
		else if( mode == MovingMode )
		{//Если режим перемещения вершин
			for( int i = 0; i < selects.count() && !vertexPressed; i++ )
			{//Если вершину вывели за пределы сцены
				while( selects[i]->pos().x() > width() - 100 || selects[i]->pos().y() > height() - 100 )
				//Увеличиваем сцену
					 incSceneRect();
			}
		}
	}
	if(  mode != DeleteMode && selects.count() > 0 )
	{//Если были выбраны элементы
		if( selects[0]->type() == VertexItem::Type )
			emit vertexSelected(qgraphicsitem_cast<VertexItem*>(selects[0])->getText());
		else if( selects[0]->type() == EdgeItem::Type )
			emit edgeSelected(qgraphicsitem_cast<EdgeItem*>(selects[0])->getText());
	}
	else if( mode == DeleteMode || selects.count() == 0 )
		emit nothingSelected();
	return QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

/*!Обработчик движения мыши
*/
void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if( mouseEvent->scenePos().x() < 10 || mouseEvent->scenePos().y() < 10 )
		//Чтобы не принимать сигналы от мыши за пределами сцены
			return;
	if( mode == AddVertexMode && addingVertex != NULL )
	{//Если режим добавления вершин
		addingVertex->setVisible(true);
		addingVertex->setPos(mouseEvent->lastScenePos());
	}
	else
	{
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
}

/*!Обработчик клавиш клавиатуры
*/
void GraphScene::keyPressEvent(QKeyEvent *event)
{	
	if( event->key() == Qt::Key_Delete )
	{//Если нажата кнопка Delete
		QList<QGraphicsItem *> selects = selectedItems();

		for( int i = 0; i < selects.count(); i++ )
		{//Если были выбраны элементы
			//Удаляем их
			if( selects[i]->type() == VertexItem::Type )
				removeVertex(qgraphicsitem_cast<VertexItem*>(selects[i])->getText());
			else if( selects[i]->type() == EdgeItem::Type )
				removeEdge(qgraphicsitem_cast<EdgeItem*>(selects[i])->getText());
			emit nothingSelected();
		}
		if( mode == AddEdgeMode )
		//Если был режим добавления дуг
			edgesManager(Cancel);
		
		if( selects.count()  && mode == ShowingOneShortWay || mode == ShowingShortWays )
		{//Если был режим отображения путей
			setMode(MovingMode);
		}
	}
	QGraphicsScene::keyPressEvent(event);
}

/*!Функция увеличивает сцену
*/
void GraphScene::incSceneRect()
{
	//Увеличиваем размер сцены
	setSceneRect(0, 0, width() + 100, height() + 100);
}

/*Менеджер добавления дуг
* \param state - состояние добавления
* \param vert - вершина, изменившая состояние
*/
void GraphScene::edgesManager(EdgesVertState state,VertexItem *vert)
{
	if( mode != AddEdgeMode )
	//Если необрабатываемый режим
		return;

	if( state == Selected )
	{//Если вершина была выбрана
		if( startForEdge == NULL )
		//Если ещё не отмечена начальная вершина
			startForEdge = vert;
		else
		{//Если нужно добавить дугу
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
	{//Если на вершину наведён курсор
		if( startForEdge != NULL )
		{//Если нужно создать вариант дуги
			addingEdge = addEdgeItem(startForEdge, vert, "");

			if( addingEdge == NULL )
				return;
			addingEdge->setColor(QColor(0, 0, 0, 100));
			addingEdge->setAcceptHoverEvents(false);
		}
	}
	else if( state == HoverOut )
	{//Если с вершины убран курсор
		if( startForEdge != NULL && addingEdge != NULL )
		//Если вариант дуги не подошёл
		{
			removeEdgeItem(addingEdge);
			addingEdge = NULL;
		}
			
	}
	else if( state == Cancel )
	{//Если создание дуги отменяется
		startForEdge = NULL;
		addingEdge = NULL;
	}
}


/*!Функция устанавливает режим сцены
* \param m - режим
*/
void GraphScene::setMode(Mods m)
{
	if( mode == AddVertexMode )
	{//Если нужно удалить временную вершину
		removeVertexItem(addingVertex);
	}
	else if( mode == AddEdgeMode )
	{//Если нужно сбросить режим добавления дуг
		edgesManager(Cancel);
	}
	else if( mode == ShowingOneShortWay || mode == ShowingShortWays )
	{//Если нужно сбросить отмеченные элементы
		resetShortWays();
		sourceOfSearching = NULL;
	}
	mode = m;
	
	if( mode == AddVertexMode )
	{//Если установлен режим добавления вершин
		addingVertex = addVertexItem(QPointF(0, 0), "", true);
		addingVertex->setTransp(100);
		addingVertex->setAcceptHoverEvents(false);
		addingVertex->setVisible(false);
	}
}

/*!Функция возвращает режим
* \return - режим
*/
int GraphScene::getMode()
{
	return mode;
}

/*!Функция возвращает информацию о вершине
* \return - структура с информацией
*/
VertexInfo GraphScene::getVertexInfo(const QString &name)
{
	//Информационная структура
	VertexInfo inf;
	QMap<QString, VertexItem*>::iterator i;
	i = vertByName.find(name);

	if( i == vertByName.end() )
	//Если нет вершины с данным именем
		return inf;
	inf.name = i.value()->getText();
	inf.dist = graph.getDistFor(i.value()->getId());
	return inf;
}

/*!Функция возвращает информацию о дуге
* \return - структура с информацией
*/
EdgeInfo GraphScene::getEdgeInfo(const QString &name)
{
	//Информационная структура
	EdgeInfo inf;
	QMap<QString, EdgeItem*>::iterator i;
	i = edgeByName.find(name);

	if( i == edgeByName.end() )
	//Если нет вершины с данным именем
		return inf;
	inf.name = i.value()->getText();
	inf.weight = graph.getEdgeWeight(i.value()->getId());
	return inf;
}

/*!Функция устанавливает имя вершины
* \param oldName - старое имя вершины
* \param newName - новое имя вершины
*/
void GraphScene::setVertexName(const QString &oldName, const QString &newName)
{	
	QMap<QString, VertexItem*>::iterator i;
	VertexItem *v;
	i = vertByName.find(newName);

	if( i != vertByName.end() )
	//Если вершина с таким именем уже есть
		return;
	i = vertByName.find(oldName);

	if( i == vertByName.end() )
	//Если нет вершины для переименовки
		return;
	i.value()->setText(newName);
	v = i.value();
	vertByName.remove(i.key());
	vertByName[newName] = v;
}

/*!Функция устанавливает имя дуги
* \param oldName - старое имя вершины
* \param newName - новое имя вершины
*/
void GraphScene::setEdgeName(const QString &oldName, const QString &newName)
{	
	QMap<QString, EdgeItem*>::iterator i;
	EdgeItem *v;
	i = edgeByName.find(newName);

	if( i != edgeByName.end() )
	//Если дуга с таким именем уже есть
		return;
	i = edgeByName.find(oldName);

	if( i == edgeByName.end() )
	//Если нет дуги для переименовки
		return;
	i.value()->setText(newName);
	v = i.value();
	edgeByName.remove(i.key());
	edgeByName[newName] = v;
}

/*!Функция устанавливает дуге вес
* \param name - имя дуги
* \param weight - вес дуги
*/
void GraphScene::setEdgeWeight(const QString &name, double weight)
{
	QMap<QString, EdgeItem*>::iterator i;
	uint id;
	i = edgeByName.find(name);
	
	if( i == edgeByName.end() )
	//Если нет дуги
		return;

	id = i.value()->getId();
	graph.setEdgeWeight(id, weight);
}

/*!Функция осуществляет поиск кратчайшего пути между парой вершин
* \param source - первая вершина
* \param dest - вторая
* \param way - массив для записи вершин и дуг на пути 
* [начальная вершина|дуга|вершина|дуга|...|конечная вершина]
* \return - расстояние между вершинами
*/
double GraphScene::showShortWay(const QString &source, const QString &dest, 
									QVector<QString> *way)
{

	QMap<QString, VertexItem*>::iterator src;		//Итератор для начальной вершины
	QMap<QString, VertexItem*>::iterator dst;		//Итератор для конечной вершины
	QVector<uint> ids;								//Идентификаторы вершин на пути от истока
	QMap<uint, VertexItem*>::iterator vertOnPath;	//Итератор для вершин на пути
	QMap<uint, EdgeItem*>::iterator edgeOnPath;		//Итератор для вершин на пути
	bool fixWay = way != NULL;						//Флаг фиксирования пути в ways
	double dist;									//Расстояние до конечной вершины
	src = vertByName.find(source);
	dst = vertByName.find(dest);

	if( src == vertByName.end() || dst == vertByName.end() )
	//Если какая-либо из вершин отсуствует
		return INF;
	//Сохраняем начальные и конечные вершины
	//Ищем кратчайшие пути
	dist = graph.findShortWays(src.value()->getId(), dst.value()->getId(), &ids);
	specItems.clear();

	for( int i = 1; i < ids.count(); i += 2 )
	{//Пока не вставим все дуги
		ids.insert(i, graph.getLastShortEdge(ids[i]));
	}
	for( int i = 0; i < ids.count(); i++ )
	{//Пока не выделим все дуги и вершины
		if( i % 2 == 0 )
		{//Если вершина
			vertOnPath = vertexes.find(ids[i]);	//Находим вершину
			vertOnPath.value()->setSpecial();	//Выделяем её
			specItems.append(vertOnPath.value());
		}
		else
		{//Если дуга
			edgeOnPath = edges.find(ids[i]);	//Находим дугу
			edgeOnPath.value()->setSpecial();	//Выделяем её
			specItems.append(edgeOnPath.value());
		}
	}
	if( fixWay )
	{//Если необходимо сохранить путь
		for( int i = 0; i < specItems.count(); i++ )
		{//Пока не перенесём все элементы
			if( specItems[i]->type() == VertexItem::Type )
			{//Если это вершина
				way->append(qgraphicsitem_cast<VertexItem*>(specItems[i])->getText());
			}
			else if( specItems[i]->type() == EdgeItem::Type )
			{//Если это дуга
				way->append(qgraphicsitem_cast<EdgeItem*>(specItems[i])->getText());
			}
		}
	}
	setMode(ShowingOneShortWay);	//Устанавливаем режим
	return dist;
}

/*!Функция сбрасывает выделения путей
*/
void GraphScene::resetShortWays()
{
	for( int i = 0; i < specItems.count(); i++ )
	{//Пока не снимем отметки со всех вершин
		if( specItems[i]->type() == VertexItem::Type )
		{//Если это вершина
			//Снимаем отметку
			qgraphicsitem_cast<VertexItem*>(specItems[i])->setSpecial(false);
		}
		else if( specItems[i]->type() == EdgeItem::Type )
		{//Если это дуга
			//Снимаем отметку
			qgraphicsitem_cast<EdgeItem*>(specItems[i])->setSpecial(false);
		}
	}
	specItems.clear();//Очищаем список
}

/*!Менеджер путей
* \param state - новое сотояние
* \param distVertex - вершина - назначение
*/
void GraphScene::waysManager(WaysSearchingState state, VertexItem *distVertex)
{
	if( mode != ShowingShortWays || sourceOfSearching == NULL )
	{//Если  не установлен режим вывода всех путей из источника
		return;
	}

	if( state == Show )
	//Если необходимо отобразить путь
	{
		QVector<uint> path;
		path = graph.getShortWay(distVertex->getId());
		resetShortWays();

		for( int i = 1; i < path.count(); i += 2 )
		{//Пока не вставим все дуги
			path.insert(i, graph.getLastShortEdge(path[i]));
		}
		for( int i = 0; i < path.count(); i++ )
		{//Пока не выделим все дуги и вершины
			if( i % 2 == 0 )
			{//Если вершина
				specItems.append(vertexes.find(path[i]).value());
				qgraphicsitem_cast<VertexItem*>(specItems[i])->setSpecial();
			}
			else
			{//Если дуга
				specItems.append(edges.find(path[i]).value());
				qgraphicsitem_cast<EdgeItem*>(specItems[i])->setSpecial();
			}
		}
	}
	else
	//Если необходимо сбросить путь
		resetShortWays();
}

/*!Функция осуществляет поиск путей из одного источника
*
*/
void GraphScene::shortWays(const QString &source)
{
	QMap<QString, VertexItem*>::iterator src;	//Итератор для начальной вершины
	src = vertByName.find(source);

	if( src == vertByName.end() )
	//Если какая-либо из вершин отсуствует
		return;
	graph.findShortWays(src.value()->getId());
	sourceOfSearching = src.value();
	setMode(ShowingShortWays);
}

/*!Функция выполняет топологическую сортировку графа
*/
void GraphScene::topologicalSorting()
{
	//Карта с идентификаторами вершин в порядке сортировки
	QMultiMap<uint, uint> sorted = graph.getTopSortOrder();
	//Итератор по порядку
	QMultiMap<uint, uint>::iterator i;
	//Итератор для обновления дуг
	QMap<uint, EdgeItem*>::iterator l;
	//Предыдущий уровень вершин
	uint level = 1;
	//Индекс вершины на уровне
	int k = 0;
	int side = 1;
	//Массив вершин по уровням
	QVector<QVector<VertexItem*>> order(1);
	i = sorted.begin();

	for( i; i != sorted.end(); i++ )
	{//Пока не обработаем все вершины
		if( level < i.key() )
		{//Если порядок увеличился
			level++;
			k = 0;
			order.append(QVector<VertexItem*>());
		}
		order[level - 1].append(vertexes[i.value()]);
		k++;
	}
	while( height() / 2 / order.count() < 200 )
	//Увеличиваем высоту сцены
			incSceneRect();
	for( int j = 0; j < order.count(); j++ )
	{//Пока не установим позиции всем вершинам
		while( width() / 2 / order[j].count() < 100 )
		//Увеличиваем ширину сцены
			incSceneRect();
		for( k = 0; k < order[j].count(); k++ )
		{//Пока не установим позиции вершинам на уровне
			order[j][k]->setPos(width() / 2 + side * k * 100 + side * 100,
								height() / 2 - (order.count() - 1) * 200 + j * 200);
		}
		side = -side;
	}

	for( l = edges.begin(); l != edges.end(); l++ )
	//Обновляем дуги
		l.value()->updatePos();
	sorted.clear();
}
