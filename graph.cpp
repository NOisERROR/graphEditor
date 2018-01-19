#include "graph.h"

/*Конструктор по умолчанию
*/
Graph::Graph()
{
	clear();
}

/*!Конструктор копий(копирует только связи)
* 
*/
Graph::Graph(const Graph &gr)
{
	QMap<uint, uint> ids;		//Список соотвествующих идентификаторов
	QVector<uint> copyVertexes;	//Список идентификаторо копируемых вершин
	QVector<uint> copyEdges;	//Список копируемы дуг
	copyVertexes = gr.getVertexIds();
	copyEdges = gr.getEdgesIds();

	clear();

	for( int i = 0; i < copyVertexes.count(); i++ )
	{//Добавляем вершины в граф
		ids[copyVertexes[i]] = addVertex();
	}
	for( int i = 0; i < copyEdges.count(); i++ )
	{//Добавляем дуги
		addEdge(ids[gr.getEdgeSource(copyEdges[i])], ids[gr.getEdgeDest(copyEdges[i])]);
	}
}

/*!Конструктор из матрицы смежности
*\param adjMatrix - матрица смежности, содержащая весы дуг(infinity для несуществующих)
*/
Graph::Graph(const QVector<QVector<double>> & adjMatrix)
{
	int i = 0;	//Индекс вершиы, испускающей дугу
	int j = 0;	//Индекс вершины, принимающей дугу
	//Массив для идентификаторов добавленных вершин
	QVector<uint> vertIds(adjMatrix.count());
	clear();	//Очистка флагов и параметров

	for( i; i < adjMatrix.count(); i++ )
	{//Сохраняем все идентификаторы вершин
		vertIds[i] = addVertex();
	}
	for( i = 0; i < adjMatrix.count(); i++ )
	{//Пока не установим дуги между всеми вершинами
		for( j = 0; j < adjMatrix[i].count(); j++ )
		{
			if( adjMatrix[i][j] != INF ) 
			{//Если дуга существует
				//Добавляем дугу
				addEdge(vertIds[i], vertIds[j], adjMatrix[i][j]);
			}
		}
	}
}
/*!Метод добавляет вершину к графу
*\return - идентификатор вершины
*/
uint Graph::addVertex()
{
	Vertex *newVertex;					//Новая вершина
	newVertex = new Vertex;		
	newVertex->changesCount = 0;		//Инициализация параметров
	newVertex->cost = INF;
	newVertex->inQueue = false;
	newVertex->last = 0;
	newVertex->id = (++maxVertId);		//Назначаем вершине идентификатор
	allVertexes[maxVertId] = newVertex;	//Добавление вершины к общему списку
	return maxVertId;					//Возвращаем идентификатор вершины
}

/*!Метод добавляет дугу к графу
*\param from - идентификатор вершины с которой начинается дуга
*\param to - идентификатор вершины которой заканчивается дуга
*\param weight - вес дуги
*\return - положительный идентификатор дуги или нулевое значение в случае ошибки
*/
uint Graph::addEdge(uint from, uint to, double weight)
{
	//Итераторы для вершин, между которыми вставляется дуга
	QMap<uint, Vertex*>::iterator vert1 = allVertexes.find(from);
	QMap<uint, Vertex*>::iterator vert2 = allVertexes.find(to);
	Edge *newEdge;			//Новая дуга

	if( vert1 == allVertexes.end() || vert2 == allVertexes.end() )
	{//Если какая-то из вершин не существует
		return 0;
	}
	newEdge = new Edge;
	newEdge->from = vert1.value();	//Устанавливаем указатель исхода дуги
	newEdge->to = vert2.value();	//Устанавливаем указатель захода дуги
	newEdge->weight = weight;		//Устанавливаем вес дуги	
	newEdge->id = (++maxEdgeId);	//Назначаем дуге идентификатор
	allEdges[maxEdgeId] = newEdge;	//Добавляем дугу к общему списку
	vert1.value()->adjEdges.append(newEdge);		//Добавляем идентификатор дуги к соотвествующей вершине
	edgesByEnd.insert(to, maxEdgeId);				//Добавляем идентификатор дуги к упорядоченному по цели списку
	haveNegWeights = haveNegWeights || weight < 0;	//Проверка веса на знак
	return maxEdgeId;				//Возвращаем идентификатор дуги
}

/*!Метод удаляет вершину из графа
*\param id - идентификатор удаляемой вершины
*\return - false в случае неудачи, иначе true
*/
bool Graph::removeVertex(uint id)
{
	//Итератор для удаляемой вершины
	QMap<uint, Vertex*>::iterator i = allVertexes.find(id);
	
	if( i == allVertexes.end() )
	{//Если нет такой вершины
		return false;
	}
	//Итератор для удаляемой входящей дуги
	QMultiMap<uint, uint>::iterator j;	
	//Идентификатор вершины, ипсускающей дугу, входящую в удаляемую вершину
	uint fromId;
	//Индекс дуги, заходящей в удаляемую вершну
	int delEdge;

	for( int k = 0; k < i.value()->adjEdges.count(); k++ )
	{//Пока не удалим все дуги, выходящие из вершины
		removeEdge(i.value()->adjEdges[k]->id);
	}
	j = edgesByEnd.find(i.value()->id);	//Находим входящие в вершину дуги
	
	while( j != edgesByEnd.end() )
	{//Пока не удалим все входящие дуги
		//Удаляем  указатель входящей дуги из списков смежности	
		removeEdge(j.value());	
		j = edgesByEnd.find(i.value()->id);	//Находим входящие в вершину дуги
	}
	delete i.value();
	allVertexes.remove(id);	//Удаляем вершину
	return true;			//Успешное удаление
}

/*!Метод удаляет дугу из графа
*\param id - идентификатор удаляемой дуги
*\return - true в случае успеха, иначе false
*/
bool Graph::removeEdge(uint id)
{
	//Итератор для удаляемой дуги
	QMap<uint, Edge*>::iterator i = allEdges.find(id);
	//Итератор для вершины, смежной дуге
	QMap<uint, Vertex*>::iterator j;	

	if( i ==  allEdges.end() )
	{//Если дуга отсутствует
		return false;
	}
	//Удаляем связь между вершиной и дугой
	i.value()->from->adjEdges.remove(i.value()->from->adjEdges.indexOf(i.value()));								
	edgesByEnd.remove(i.value()->to->id);		//Удаляем дугу из всех списков
	delete i.value();
	allEdges.remove(id);				
	return true;								//Успешное удаление
}

/*!Метод очищает структуры данных графа
*/
void Graph::clear()
{
	//Итераторы для удаляемых элементов
	QMap<uint, Vertex*>::iterator i;
	QMap<uint, Edge*>::iterator j;

	for( i = allVertexes.begin(); i != allVertexes.end(); i++ )
	{//Освобождаем память из под всех вершин
		delete i.value();
	}
	for( j = allEdges.begin(); j != allEdges.end(); j++ )
	{//Освобождаем память из под всех дуг
		delete j.value();
	}
	sourceVert = 0;		//Нет источника поиска путей
	maxVertId = 0;		//Обнуление счётчиков идентификаторов
	maxEdgeId = 0;
	haveNegWeights = false;	//Нет дуг с отрицательным весом
	haveNegCycle = false;	//Нет отрицательных циклов
	acyclic = true;			//Ацикличен
	allVertexes.clear();	//Очистка структур данных
	allEdges.clear();
	edgesByEnd.clear();
	negCycle.clear();
	time = 0;				//Поиск в глубину не осуществлялся
}

/*!Метод возвращает матрицу смежности графа
*\param vertIds - массив для записи идентификаторов,
*соответсвующих строкам и столбцам матрицы смежности
*/
QVector<QVector<double>> Graph::getAdjMatrix(QVector<uint> *vertIds) const
{
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	//Возвращаемая матрица смежности
	QVector<QVector<double>> adjMatrix(allVertexes.count());
	//Идентификаторы вершин
	QVector<uint> ids;	
	//Индексы идентификаторов в матрице смежности
	QMap<uint, uint> indexes;	
	//Индекс для вершин
	int k = 0;	
	//Идентификатор вершины, принимающей дугу
	uint vertId;
	//Индекс строки матрицы смежности
	int j = 0;
	//Устанавливаем размер массива с идентификаторами
	i = allVertexes.constBegin();
	ids = getVertexIds();		

	for( k = 0; k < ids.count(); k++ )
	{//Заполняем массив идентификаторами и сохраняем их положение
		indexes[ids[k]] = k;	//Сохраняем индекс
	}
	
	i = allVertexes.constBegin();

	for( i; i != allVertexes.constEnd(); i++ )
	{//Пока не переберём все элементы
		//Устнавливаем количество столбцов
		adjMatrix[j].fill(INF, allVertexes.count());

		for( k = 0; k < i.value()->adjEdges.count(); k++ )
		{//Устанавливаем весы дуг
			vertId = i.value()->adjEdges[k]->to->id;//Находим идентификатор вершины, принимающей дугу
													//Записываем в матрицу вес дуги	
			adjMatrix[j][indexes[vertId]] = i.value()->adjEdges[k]->weight;
		}
		j++;	//Увеличиваем индекс строки
	}
	if( vertIds != NULL )
	{//Если нужно сохранить порядок идентификаторов
		(*vertIds) = ids;
	}
	return adjMatrix;	//Возвращаем матрицу смежности
}

/*!Метод возвращает массив идентификаторов всех вершин
* \return - массив идентификаторов
*/
QVector<uint> Graph::getVertexIds() const
{
	//Итератор для вершин графа
	QMap<uint, Vertex*>::const_iterator i = allVertexes.constBegin();
	//Массив идентификаторов всех вершин
	QVector<uint> vertIds(allVertexes.count());
	//Индекс элементов массива идентификаторов
	int k = 0;

	for( i; i != allVertexes.constEnd(); i++ )
	{//Пока не пройдём по всем вершинам
		vertIds[k] = i.key();	//Записываем значение идентификатора
		k++;					//Увеличиваем индекс элемента массива
	}
	return vertIds;	//Возвращаем массив идентификаторов
}

/*!Метод возвращает массив идентификаторов дуг
* \return - массив идентификаторов дуг
*/
QVector<uint> Graph::getEdgesIds() const
{
	//Итератор для дуг
	QMap<uint, Edge*>::const_iterator i = allEdges.constBegin();
	//Массив идентификаторов дуг
	QVector<uint> edgeIds(allEdges.count());
	//Индекс элемента массива идентификаторов
	int k = 0;

	for( i; i != allEdges.constEnd(); i++ )
	{//Пока не пройдём по всем дугам
		edgeIds[k] = i.key();	//Записываем значение идентификатора
		k++;					//Увеличиваем индекс элемента массива
	}
	return edgeIds;	//Возвращаем массив идентификаторв
}

/*!Метод устанавливает вес дуги
* \param id - идентификатор дуги
* \param weight - вес
* \return - идентификатор дуги
*/
uint Graph::setEdgeWeight(uint id, double weight)
{
	//Итератор для дуг
	QMap<uint, Edge*>::iterator i;
	i = allEdges.find(id);

	if( i == allEdges.end() )
	//Если дуги не существует
		return 0;
	i.value()->weight = weight;
	haveNegWeights = haveNegWeights || weight < 0;
	return i.value()->id;
}

/*!Функция выполняет поиск кратчайшего пути из
* одного источника во все остальные
* \param source - источник
* \return - true в случае успеха поиска кратчайшего пути
*/
bool Graph::findShortWays(uint source)
{
	if( allVertexes.find(source) == allVertexes.constEnd() )
	//Если вершины нет
		return false;

	resetWays();			//Сброс параметров поиска
	if( !haveNegWeights )
	{//Если нет отрицательных дуг
		dijkstra(source);	//Поиск по алгоритму Дейкстры
	}
	//Поиск по алгоритму Беллмана-Форда
	else if( !bellmanFord(source) )
	{//Если есть отрицательный цикл
		sourceVert = 0;		//Устанавливаем отсутствие источника
		return false;
	}
	sourceVert = source;	//Устанавливаем источник
	return true;
}

/*!Функция выполняет поиск кратчайшего пути из
* одной вершины в другую. Эквивалентно: 
* findShortWays(source); getShortWay(dest);
* \param source - идентификатор источника 
* \param dest - идентификатор цели
* \param vertIds - массив для записи пути
* \return - расстояние между вершинами
*/
double Graph::findShortWays(uint source, uint dest, QVector<uint> *vertIds)
{
	if( allVertexes.find(source) == allVertexes.constEnd() 
		|| allVertexes.find(dest) == allVertexes.constEnd() )
		//Если одна из вершин отсутствует
		return false;

	if( findShortWays(source) )
	{//Если нет отрицательных циклов
		if( vertIds != NULL )
		{//Если необходимо сохранить путь
			(*vertIds) = getShortWay(dest);
		}
		return getDistFor(dest);
	}
	else
	{//Если была ошибка
		resetWays();
		return INF;
	}

}
/*!Функция реализует алгоритм Дейкстры для поиска кратчайшего пути
* из одного источника во все остальные вершины
* \param src - идентификатор источника
*/
void Graph::dijkstra(uint src)
{
	//Список вершин - кандидатов на просмотр дуг
	QMultiMap<double, Vertex*> candForVisit;
	//Итератор для вершин
	QMap<uint, Vertex*>::iterator i = allVertexes.begin();
	//Смежная вершина
	Vertex *adjVert;
	//Смежная дуга
	Edge *curEdge;
	//Вершина, для которой найден кратчайший путь
	Vertex *min;
	//Итератор для обновляемой вершины
	QMultiMap<double, Vertex*>::iterator updVert;
	//Индекс смежной дуги
	int k;
	//Текущее расстояние до вершины
	double curCost;
	//Расстояние до предыдущей вершины
	double prevCost;

	for( i; i != allVertexes.end(); i++ )
	{//Пока не занесём в список все вершины
		if( i.value()->id == src )
		{//Если начальная вершина
			i.value()->cost = 0;
		}
		candForVisit.insert(i.value()->cost, i.value());
	}
	while( candForVisit.count() > 1 )
	{//Пока не посетим все вершины
		min = candForVisit.begin().value();	//Извлекаем вершину с кратчайшем путём
		prevCost = min->cost;				//Расстояние до данной вершины
											//Удаляем вершину из кандидатов на просмотр дуг
		candForVisit.erase(candForVisit.begin());	
		
		for( k = 0; k < min->adjEdges.count(); k++ )
		{//Пока не проверим все дуги
			curEdge = min->adjEdges[k];
			//Находим текущее расстояние
			curCost = prevCost + curEdge->weight;
			//Находим смежную вершину
			adjVert = curEdge->to;

			if( curCost < adjVert->cost )
			{//Если текущее расстояние меньше, чем уже отмеченное
				//Ищем обновляемую вершину
				updVert = candForVisit.find(adjVert->cost, adjVert);
				candForVisit.erase(updVert);			//Удаляем вершину из кандидатов
				adjVert->cost = curCost;				//Устанавливаем новое расстояние
				adjVert->last = min;					//Устанавливаем предыдущую вершину
				adjVert->lastShortEdge = curEdge;		//Устанавливаем индекс кратчайшей дуги
				candForVisit.insert(curCost, adjVert);	//Возвращаем обновлённую вершину в список кандидатов
			}
		}
	}
}

/*!Функция реализует алгоритм Беллмана-Форда
* \param src - идентификатор источника
* \return - успешность заверешения алгоритма(true)
*/
bool Graph::bellmanFord(uint src)
{
	//Очередь вершин - кандидатов на просмотр дуг
	QQueue<Vertex *> candForVisit;	
	//Указатель на вершину, у которой просматриваются дуги
	Vertex *curVert;
	//Указатель на смежную вершину
	Vertex *adjVert;
	//Итератор для вершин
	QMap<uint, Vertex*>::iterator i = allVertexes.begin();
	//Итератор для смежных дуг
	Edge *adjEdge;
	//Начальная и конечная вершина отрицательного цикла
	Vertex *cycleVert;
	//Индекс дуги
	int k;
	//Текущее расстояние до вершины
	double curCost;

	i = allVertexes.find(src);			//Находим источник
	i.value()->cost = 0;
	candForVisit.enqueue(i.value());	//Добавляем первую вершину в очередь

	while( !candForVisit.isEmpty() )
	{//Пока очередь не пуста
		//Находим вершину для просмотра дуг
		curVert = candForVisit.dequeue();
		curVert->inQueue = false;		//Помечаем как извелённую

		if( haveNegCycle && curVert == cycleVert  )
		{//Если сформирован отрицательный цикл
			negCycle << cycleVert->id;

			do
			{
				curVert = curVert->last;
				negCycle << curVert->id;
			}//Пока не сохраним весь цикл
			while( curVert != cycleVert );
			return false;
		}
		if( curVert->changesCount == allVertexes.count() )
		{//Если обнаружен отрицательный цикл
			haveNegCycle = true;
			cycleVert = curVert;
		}
		
		for( k = 0; k < curVert->adjEdges.count(); k++ )
		{//Пока не проверим все дуги
			//Находим смежную дугу
			adjEdge = curVert->adjEdges[k];
			//Определяем расстояние
			curCost = curVert->cost + adjEdge->weight;
			//Определяем смежную вершину
			adjVert = adjEdge->to;

			if( adjVert->cost > curCost )
			{//Если найден более короткий путь
				adjVert->cost = curCost;
				adjVert->last = curVert;
				adjVert->lastShortEdge = adjEdge;

				if( !adjVert->inQueue )
				{//Если вершина вне очереди
					adjVert->inQueue = true;	
					adjVert->changesCount++;
					candForVisit.enqueue(adjVert);
				}
				else
				{//Иначе добавляем в очередь
					adjVert->changesCount++;
				}
			}
		}
	}
	return true;
}

/*!Функция возвращает найденный отрицательный цикл графа
* \return - массив идентификаторов
*/
const QVector<uint> & Graph::getNegCycle() const
{
	return negCycle;

}
/*!Функция возвращает массив идентификаторов вершин,
* лежащих на кратчайшем пути от источника к цели
* \param dest - идентификатор цели
* \return - массив идентификаторов вершин  и дуг(начиная с вершины)
*/
QVector<uint> Graph::getShortWay(uint dest) const
{
	uint prevId = dest;	//Идентификатор предшествующей вершины
	QVector<uint> path;	//Путь от источника к цели
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.find(dest);
	
	if( !sourceVert || i == allVertexes.constEnd() || i.value()->cost == INF )
	{//Если вершина не существует или поиск не ьыл произведён
		return QVector<uint>();
	}
	do
	{//Пока не достигнем начальной вершины
		path.prepend(prevId);
		i = allVertexes.constFind(prevId);
		prevId = i.value()->last == NULL ? 0 :  i.value()->last->id;
	}
	while( prevId != 0 );
	return path;
}

/*!Функция возвращает расстояние от источника до цели
* \param dest - идентификатор цели
* \return - расстояние до цели
*/
double Graph::getDistFor(uint dest) const
{
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(dest);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//Если поиск не был произведён вершина отсутствует
		return INF;
	return i.value()->cost;
}

/*!Функция возвращает идентификатор предыдущей вершины
* на кратчайшкм пути
* \param id - идентификатор вершины
* \return - идентификатор предыдущей вершины, 0 в случае ошибки
*/
uint Graph::getLast(uint id) const
{
	//Итератор для вершины
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//Если вершины нет или источник не установлен
		return 0;
	return i.value()->last->id;
}

/*!Функция возвращает идентификатор дуги, входящей
*  в вершину, по которой проходит кратчайший путь
* \param id - идентификатор вершины
* \return - идентификатор дуги
*/
uint Graph::getLastShortEdge(uint id)
{
	//Итератор для вершины
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	
	if( i == allVertexes.constEnd() || sourceVert == 0 )
	//Если вершины нет или источник не установлен
		return 0;
	return i.value()->lastShortEdge->id;
}

/*!Функция возвращает идентификатор вершины, испускающей дугу
* \param id - идентификатор дуги
* \return - идентификатор вершины
*/
uint Graph::getEdgeSource(uint id) const
{
	//Итератор для дуг
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//Если дуги нет
		return 0;
	return i.value()->from->id;
}
										
/*!Функция возвращает идентификатор вершины, принимающей дугу
* \param id - идентификатор дуги
* \return - идентификатор вершины
*/
uint Graph::getEdgeDest(uint id) const
{
	//Итератор для вершины
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//Если дуги нет
		return 0;
	return i.value()->to->id;
}

/*!Функция возвращает вес дуги
* \param id  - идентификатор дуги
* \return - вес дуги
*/
double Graph::getEdgeWeight(uint id) const
{
	QMap<uint, Edge*>::const_iterator i;
	i = allEdges.constFind(id);
	
	if( i == allEdges.constEnd() )
	//Если дуги нет
		return 0;
	return i.value()->weight;
}

/*!Функция сбрасывает параметры поиска кратчайших путей
*/
void Graph::resetWays()
{
	//Итератор для вершин
	QMap<uint, Vertex*>::iterator i;
	i = allVertexes.begin();

	for( i; i != allVertexes.end(); i++ )
	{//Сброс параметров вершин
		i.value()->last = 0;
		i.value()->changesCount = 0;
		i.value()->cost = INF;
		i.value()->lastShortEdge = NULL;
	}
	//Очистка параметров отрицательного цикла
	haveNegCycle = false;
	negCycle.clear();
}

/*!Функция осуществляет поиск в глубину
*/
void Graph::depthFirstSearch()
{
	//Итератор для вершин
	QMap<uint, Vertex*>::iterator i;
	i = allVertexes.begin();
	
	for( i; i != allVertexes.end(); i++ )
	{//Пока не обработаем все вершины
		i.value()->dfsColor = 0;	//Вершина не посещена
		i.value()->dsfLast = NULL;	//Отсутсвует предок
	}	
	i = allVertexes.begin();
	
	for( i; i != allVertexes.end(); i++ )
	{//Пока не обработаем все вершины
		if( i.value()->dfsColor == 0 )
		{//Если вершина ещё не посещена
		//	time = 0;
			//Посетить её
			depthFirstSearchVisiter(i.value());
		}
	}	
}

/*!Функция осуществляет посещение вершин при поиске в глубину
* \param - greyVert - посещаемая вершина
*/
void Graph::depthFirstSearchVisiter(Vertex* greyVert)
{
	greyVert->dfsColor = 1;		//Вершина посещена
	time++;						//Новая итерация
	greyVert->dfsStart = time;	//Сохранение итерации начала обработки

	for( int i = 0; i < greyVert->adjEdges.count(); i++ )
	{//Пока не обработаем все смежные вершины
		if( greyVert->adjEdges[i]->to->dfsColor == 0 )
		{//Если смежная вершина не была посещена
			greyVert->adjEdges[i]->to->dsfLast = greyVert;
			depthFirstSearchVisiter(greyVert->adjEdges[i]->to);
		}
		else 
			//Если встретили посещённую вершину
			acyclic = greyVert->adjEdges[i]->to->dfsColor != 1;
	}
	greyVert->dfsColor = 2;		//Вершина обработана полностью
	greyVert->dfsFinish = time;	//Сохранение итерации завершения обработки
	time++;						//Новая итерация
}

/*!Функция возвращает идентификаторы вершин
* в порядке топологической сортировки
* \return - карта идентификаторов с порядковым номеров
* в качестве ключа
*/
QMultiMap<uint, uint> Graph::getTopSortOrder()
{
	QMap<uint, Vertex*>::iterator i;	//Итератор для вершин
										//Вершины, отсортированные по итерациям time
	QMultiMap<uint, uint> topSortOrderByTime;	
	QMultiMap<uint, uint> topSortOrder;
	QMultiMap<uint, uint>::iterator j;	//Итератор для перебоа списка вершин
	uint lastTime;						//Предыдущее значение параметра time
	uint pos;							//Индекс вершины в массиве
	depthFirstSearch();					//Поиск в глубину

	if( acyclic )
	{//Если граф ациклический
		i = allVertexes.begin();

		for( i; i != allVertexes.end(); i++ )
		{//Пока не обработаем все вершины
			//Упорядочиваем идентификаторы
			topSortOrderByTime.insert(-i.value()->dfsFinish, i.value()->id);
		}
		j = topSortOrderByTime.begin();
		pos = 1;

		if( j != topSortOrderByTime.end() )
			lastTime = j.key();

		for( j; j != topSortOrderByTime.end(); j++ )
		{//Пока не перенесём все вершины
			if( lastTime < j.key() )
				pos++;

			topSortOrder.insert(pos, j.value());
			lastTime = j.key();
		}

	}
	return topSortOrder;
}

/*!Функция показывает является ли граф ациклическим
* 'return - true или false
*/
bool Graph::isAcyclic() const
{
	return acyclic;
}
/*!Функция возвращает дуги, смежные вершине
* \param id - идентфикатор вершины
* \return - идентфикаторы дуг
*/
QVector<uint> Graph::getAdjEdges(uint id) const
{
	//Возвращаемый массив
	QVector<uint> adjEdges;
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);

	if( i == allVertexes.constEnd() )
	//Если вершины нет
		return adjEdges;
	adjEdges.resize(i.value()->adjEdges.count());

	for( int j = 0; j < i.value()->adjEdges.count(); j++ )
	//Пока не скопируем идентификаторы всех вершин
		adjEdges[j] = i.value()->adjEdges[j]->id;
	return adjEdges;
}

/*!Функция возвращает степень вершины
* \param id - идентификатор вершины
* \return - степень
*/
uint Graph::getVertexDegree(uint id) const
{
	return getVertexHalfDegreeIn(id) + getVertexHalfDegreeOut(id);
}

/*!Функция возвращает полустепень захода вершины
* \param id - идентификатор вершины
* \return - полустепень
*/
uint Graph::getVertexHalfDegreeIn(uint id) const
{	
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	//Итератор для заходящих дуг
	QMultiMap<uint, uint>::const_iterator j;
	//Возвращаемая полустепень
	uint degree = 0;
	i = allVertexes.constFind(id);
	j = edgesByEnd.constFind(i.value()->id);
	
	for( j; j != edgesByEnd.constEnd(); j++ )
		degree++;

	return degree;
}

/*!Функция возвращает полустепень исхода вершины
* \param id - идентификатор вершины
* \return - полустепень
*/
uint Graph::getVertexHalfDegreeOut(uint id) const
{
	//Итератор для вершин
	QMap<uint, Vertex*>::const_iterator i;
	i = allVertexes.constFind(id);
	return i.value()->adjEdges.count();
}