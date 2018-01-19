#ifndef GRAPH_H
#define GRAPH_H
#include <QVector>
#include <QHash>
#include <QMultiMap>
#include <QQueue>
#include <limits>
#include <iostream>
#define INF std::numeric_limits<double>::infinity()
#define COUT std::cout

typedef unsigned int uint;
struct Vertex;
class Graph;

struct Edge
{
	uint id;			//Идентификатор дуги
	Vertex *from;		//Первая вершина дуги
	Vertex *to;			//Вторая вершина дуги
	double weight;		//Вес дуги
};

//Класс, описывающий вершину
struct Vertex
{
	uint id;					//Идентификатор вершины
	Vertex *last;				//Предыдущая вершины на кратчайшем пути
	Edge* lastShortEdge;		//Предыдущая дуга на кратчайшем пути
	QVector<Edge *> adjEdges;	//Смежные вершине дуги(список смежности)
	double cost;				//Расстояние до данной вершины от начальной
	int changesCount;			//Количество изменений расстояния до вершины
	bool inQueue;				//Флаг присутствия вершины в очереди
	int dfsColor;				//Цвет для поиска в глубину
	Vertex *dsfLast;			//Предшественник в поиске в глубину
	int dfsStart;				//Шаг начала обработки вершины
	int dfsFinish;				//Обработанная вершина
};

//Класс графа
class Graph
{
public:
	/////////////////////////Открытые методы//////////////////////////
	Graph();								//Конструктор по умолчанию	
											//Конструктор из матрицы смежности
	Graph(const QVector<QVector<double>> & adjMatrix);
	Graph(const Graph &gr);					//Конструктор копий
	~Graph() {}								//Деструктор
	uint addVertex();						//Добавление вершины
	//Добавление ребра
	uint addEdge(uint from, uint to, double weight = 1);
	bool removeVertex(uint id);				//Удаление вершины
	bool removeEdge(uint id);				//Удаление ребра
	void clear();							//Очистка данных графа
											//Получение матрицы смежности графа
	QVector<QVector<double>> getAdjMatrix(QVector<uint> *vertIds = NULL) const;
											//Получение идентификаторов всех вершин
	QVector<uint> getVertexIds() const;	
											//Получение идентификаторов всех дуг
	QVector<uint> getEdgesIds() const;
	QVector<uint> getAdjEdges(uint id) const;	//Дуги, смежные вершине
	uint setEdgeWeight(uint id, double weight);	//Установка веса дуги
	bool findShortWays(uint source);			//Поиск кратчайших путей из одного источника
												//Поиск кратчайшего пути между парой вершин
	double findShortWays(uint source, uint dest, QVector<uint> *vertIds = NULL);
												//Массив идентификаторов вершин на пути от источника
	QVector<uint>getShortWay(uint dest) const;	//к цели
	double getDistFor(uint id) const;			//Расстояние до вершины от истока
	uint getLast(uint id) const;				//Предыдущая вершина на кратчайшем пути
	uint getLastShortEdge(uint id);				//Предыдущая дуга на кратчайшем пути					
	uint getEdgeSource(uint id) const;			//Вершина, испускающая дугу		
	uint getEdgeDest(uint id) const;			//Вершина, принимающая дугу
	double getEdgeWeight(uint id) const;		//Вес дуги
	void resetWays();							//Сброс всех флагов и результатов поиска путей
	bool isAcyclic() const;						//Является ли граф ациклическим
	QMultiMap<uint, uint> getTopSortOrder();	//Идентификаторы в порядке топологической сортировки
	const QVector<uint> & getNegCycle() const;	//Идентификаторы отрицательного цикла
	uint getVertexDegree(uint id) const;		//Степени вершины
	uint getVertexHalfDegreeIn(uint id) const;
	uint getVertexHalfDegreeOut(uint id) const;
private:
	//////////////////////Закрытые члены класса///////////////////////
	void dijkstra(uint src);					//Алгоритм Дейкстры
	bool bellmanFord(uint src);					//Алгоритм Беллмана-Форда 
	void depthFirstSearch();					//Поиск в глубину
	void depthFirstSearchVisiter(Vertex* greyVert);
	QMap<uint, Vertex*> allVertexes;			//Все вершины графа
	QMap<uint, Edge*> allEdges;					//Все дуги графа
												//Дуги, упорядоченные по конечной вершине
	QMultiMap<uint, uint> edgesByEnd;			
	int maxVertId;								//Максимальный идентификатор вершины
	int maxEdgeId;								//Максимальный идентификатор дуги
	bool haveNegWeights;						//Флаг наличия отрицательных весов дуг
	bool haveNegCycle;							//Флаг наличия отрицательного цикла
	QVector<uint> negCycle;						//Отрицательный цикл	
	uint sourceVert;							//Вершина, считающаяся начальной
	bool acyclic;								//Флаг ацикличности
	int time;									//Показатель времени для поиска в глубину
};	

#endif