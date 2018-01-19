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

//��������� ��� �������� ������ � �������
struct VertexInfo
{
	QString name;	//���
	double dist;	//���������� �� ���������
};
//��������� ��� �������� ������ � ����
struct EdgeInfo
{
	QString name;	//���
	double weight;	//���
};

class GraphScene : public QGraphicsScene
{
	Q_OBJECT
public:
	//��������� ������ ���������� ���
	enum EdgesVertState{Selected, HoverOn, HoverOut, Cancel};
	//������
	enum Mods{MovingMode, AddVertexMode, AddEdgeMode, DeleteMode,
				ShowingShortWays, ShowingOneShortWay};
	enum WaysSearchingState{Show, Hide};
	//�����������
	GraphScene();
	//���������� �������
	bool addVertex(QPointF pos, const QString &name);
	//���������� ����
	bool addEdge(const QString &from, const QString &to, const QString &name = "");
	//�������� �������
	bool removeVertex(const QString &name);
	//�������� ����
	bool removeEdge(const QString &name);
	//�������� ��������� ���
	void edgesManager(EdgesVertState, VertexItem *vert = NULL);
	//��������� ������
	void setMode(Mods m);
	//������ ������
	int getMode();
	//��������� ����� �������
	void setVertexName(const QString &oldName, const QString &newName);
	//��������� ����� ����
	void setEdgeName(const QString &oldName, const QString &newName);
	//��������� ���� ����
	void setEdgeWeight(const QString &name, double weight);
	//���������� � �������
	VertexInfo getVertexInfo(const QString &name);
	//���������� � ����
	EdgeInfo getEdgeInfo(const QString &name);
	//����� �� ����� ����������� ���� ����� ����� ������
	double showShortWay(const QString &source, const QString &dest, 
									QVector<QString> *way = NULL);
	//����� ��������� ����� �� ����� ������� �� ���������
	void shortWays(const QString &source);
	//�������� �����
	void waysManager(WaysSearchingState state, VertexItem *distVertex = NULL);
	//�������������� ����������
	void topologicalSorting();
signals://�������
	//������ ��������� �������
	void vertexSelected(const QString &name);	
	//������ ���������� �������
	void vertexAdding(const QString &name);	
	//���������� ���������
	void nothingSelected();
	//������ ��������� ����
	void edgeSelected(const QString &name);
	//������ ���������� ����
	void edgeAdding(const QString &name);		
private:
	//����������� �������
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void keyPressEvent(QKeyEvent *event);
	//���������� ���� � �����
	EdgeItem* addEdgeItem(VertexItem *from, VertexItem *to, const QString &name);
	//���������� ������� � �����
	VertexItem* addVertexItem(QPointF pos, const QString &name, bool timeVert = false);
	//�������� ����
	void removeEdgeItem(EdgeItem* edge);
	//�������� �������
	void removeVertexItem(VertexItem *vert);
	//���������� �����
	void incSceneRect();
	//����� ��������� �����
	void resetShortWays();
	bool isOrient;							//���� �����������������
	int mode;								//�����
	QMap<uint, VertexItem*> vertexes;		//������ ������ �� ���������������
	QMap<uint, EdgeItem*> edges;			//������ ��� �� ���������������
	QMap<QString, VertexItem*> vertByName;	//������ ������ �� ������
	QMap<QString, EdgeItem*> edgeByName;	//������ ��� �� ������
	Graph graph;							//����
	uint vertNameCount;						//������������ ��� �������
	uint edgeNameCount;						//������������ ��� ����
	VertexItem *startForEdge;				//��������� ������� ��� ��������� ����
	EdgeItem* addingEdge;					//��������� ����
	VertexItem *addingVertex;				//��������� �������
	VertexItem* sourceOfSearching;			//�������� ������ ����������� ����
	QVector<QGraphicsItem* > specItems;		//������ ���������� ���������
};

#endif