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
	uint id;			//������������� ����
	Vertex *from;		//������ ������� ����
	Vertex *to;			//������ ������� ����
	double weight;		//��� ����
};

//�����, ����������� �������
struct Vertex
{
	uint id;					//������������� �������
	Vertex *last;				//���������� ������� �� ���������� ����
	Edge* lastShortEdge;		//���������� ���� �� ���������� ����
	QVector<Edge *> adjEdges;	//������� ������� ����(������ ���������)
	double cost;				//���������� �� ������ ������� �� ���������
	int changesCount;			//���������� ��������� ���������� �� �������
	bool inQueue;				//���� ����������� ������� � �������
	int dfsColor;				//���� ��� ������ � �������
	Vertex *dsfLast;			//�������������� � ������ � �������
	int dfsStart;				//��� ������ ��������� �������
	int dfsFinish;				//������������ �������
};

//����� �����
class Graph
{
public:
	/////////////////////////�������� ������//////////////////////////
	Graph();								//����������� �� ���������	
											//����������� �� ������� ���������
	Graph(const QVector<QVector<double>> & adjMatrix);
	Graph(const Graph &gr);					//����������� �����
	~Graph() {}								//����������
	uint addVertex();						//���������� �������
	//���������� �����
	uint addEdge(uint from, uint to, double weight = 1);
	bool removeVertex(uint id);				//�������� �������
	bool removeEdge(uint id);				//�������� �����
	void clear();							//������� ������ �����
											//��������� ������� ��������� �����
	QVector<QVector<double>> getAdjMatrix(QVector<uint> *vertIds = NULL) const;
											//��������� ��������������� ���� ������
	QVector<uint> getVertexIds() const;	
											//��������� ��������������� ���� ���
	QVector<uint> getEdgesIds() const;
	QVector<uint> getAdjEdges(uint id) const;	//����, ������� �������
	uint setEdgeWeight(uint id, double weight);	//��������� ���� ����
	bool findShortWays(uint source);			//����� ���������� ����� �� ������ ���������
												//����� ����������� ���� ����� ����� ������
	double findShortWays(uint source, uint dest, QVector<uint> *vertIds = NULL);
												//������ ��������������� ������ �� ���� �� ���������
	QVector<uint>getShortWay(uint dest) const;	//� ����
	double getDistFor(uint id) const;			//���������� �� ������� �� ������
	uint getLast(uint id) const;				//���������� ������� �� ���������� ����
	uint getLastShortEdge(uint id);				//���������� ���� �� ���������� ����					
	uint getEdgeSource(uint id) const;			//�������, ����������� ����		
	uint getEdgeDest(uint id) const;			//�������, ����������� ����
	double getEdgeWeight(uint id) const;		//��� ����
	void resetWays();							//����� ���� ������ � ����������� ������ �����
	bool isAcyclic() const;						//�������� �� ���� ������������
	QMultiMap<uint, uint> getTopSortOrder();	//�������������� � ������� �������������� ����������
	const QVector<uint> & getNegCycle() const;	//�������������� �������������� �����
	uint getVertexDegree(uint id) const;		//������� �������
	uint getVertexHalfDegreeIn(uint id) const;
	uint getVertexHalfDegreeOut(uint id) const;
private:
	//////////////////////�������� ����� ������///////////////////////
	void dijkstra(uint src);					//�������� ��������
	bool bellmanFord(uint src);					//�������� ��������-����� 
	void depthFirstSearch();					//����� � �������
	void depthFirstSearchVisiter(Vertex* greyVert);
	QMap<uint, Vertex*> allVertexes;			//��� ������� �����
	QMap<uint, Edge*> allEdges;					//��� ���� �����
												//����, ������������� �� �������� �������
	QMultiMap<uint, uint> edgesByEnd;			
	int maxVertId;								//������������ ������������� �������
	int maxEdgeId;								//������������ ������������� ����
	bool haveNegWeights;						//���� ������� ������������� ����� ���
	bool haveNegCycle;							//���� ������� �������������� �����
	QVector<uint> negCycle;						//������������� ����	
	uint sourceVert;							//�������, ����������� ���������
	bool acyclic;								//���� ������������
	int time;									//���������� ������� ��� ������ � �������
};	

#endif