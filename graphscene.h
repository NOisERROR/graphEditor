#ifndef GENERAL_GRAPH_SCENE_H
#define GENERAL_GRAPH_SCENE_H
#include <QGraphicsScene>
#include <QGraphicsSceneHelpEvent>
#include <QGraphicsView>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QGraphicsSceneWheelEvent>
#include <QMatrix>
#include "graph.h"
#include "vertexitem.h"
#include "edgeitem.h"

typedef unsigned int uint;
class EdgeItem;
class VertexItem;

//Структура для возврата данных о вершине
struct VertexInfo
{
	QString name;	//Имя
	double dist;	//Расстояние от источника
};
//Структура для возврата данных о дуге
struct EdgeInfo
{
	QString name;	//Имя
	double weight;	//Вес
};

class GraphScene : public QGraphicsScene
{
	Q_OBJECT
public:
	//Состояние режима добавления дуг
	enum EdgesVertState{Selected, HoverOn, HoverOut, Cancel};
	//Режимы
	enum Mods{MovingMode, AddVertexMode, AddEdgeMode, DeleteMode,
				ShowingShortWays, ShowingOneShortWay};
	enum WaysSearchingState{Show, Hide};
	//Конструктор
	GraphScene();
	//Добавление вершины
	bool addVertex(QPointF pos, const QString &name);
	//Добавление дуги
	bool addEdge(const QString &from, const QString &to, const QString &name = "");
	//Удаление вершины
	bool removeVertex(const QString &name);
	//Удаление дуги
	bool removeEdge(const QString &name);
	//Менеджер состояний дуг
	void edgesManager(EdgesVertState, VertexItem *vert = NULL);
	//Установка режима
	void setMode(Mods m);
	//Чтение режима
	int getMode();
	//Установка имени вершины
	void setVertexName(const QString &oldName, const QString &newName);
	//Установка имени дуги
	void setEdgeName(const QString &oldName, const QString &newName);
	//Установка веса дуги
	void setEdgeWeight(const QString &name, double weight);
	//Информация о вершине
	VertexInfo getVertexInfo(const QString &name);
	//Информация о дуге
	EdgeInfo getEdgeInfo(const QString &name);
	//Вывод на экран кратчайшего пути между парой вершин
	double showShortWay(const QString &source, const QString &dest, 
									QVector<QString> *way = NULL);
	//Поиск крачайших путей от одной вершины до остальных
	void shortWays(const QString &source);
	//Менеджер путей
	void waysManager(WaysSearchingState state, VertexItem *distVertex = NULL);
	//Топологическая сортировка
	void topologicalSorting();
signals://Сигналы
	//Сигнал выделения вершины
	void vertexSelected(const QString &name);	
	//Сигнал добавления вершины
	void vertexAdding(const QString &name);	
	//Отсутсвуют выделения
	void nothingSelected();
	//Сигнал выделения дуги
	void edgeSelected(const QString &name);
	//Сигнал добавления дуги
	void edgeAdding(const QString &name);		
private:
	//Обработчики событий
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void keyPressEvent(QKeyEvent *event);
	//Добавление дуги в сцену
	EdgeItem* addEdgeItem(VertexItem *from, VertexItem *to, const QString &name);
	//Добавление вершины в сцену
	VertexItem* addVertexItem(QPointF pos, const QString &name, bool timeVert = false);
	//Удаление дуги
	void removeEdgeItem(EdgeItem* edge);
	//Удаление вершины
	void removeVertexItem(VertexItem *vert);
	//Увеличение сцены
	void incSceneRect();
	//Сброс выделения путей
	void resetShortWays();
	bool isOrient;							//Флаг ориентированности
	int mode;								//Режим
	QMap<uint, VertexItem*> vertexes;		//Список вершин по идентификаторам
	QMap<uint, EdgeItem*> edges;			//Список дуг по идентификаторам
	QMap<QString, VertexItem*> vertByName;	//Список вершин по именам
	QMap<QString, EdgeItem*> edgeByName;	//Список дуг по именам
	Graph graph;							//Граф
	uint vertNameCount;						//Максимальное имя вершины
	uint edgeNameCount;						//Максимальное имя дуги
	VertexItem *startForEdge;				//Начальная вершина для временной дуги
	EdgeItem* addingEdge;					//Временная дуга
	VertexItem *addingVertex;				//Временная вершина
	VertexItem* sourceOfSearching;			//Источник поиска кратчайшего пути
	QVector<QGraphicsItem* > specItems;		//Список отмеченных элементов
};

#endif