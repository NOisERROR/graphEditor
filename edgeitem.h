#ifndef EDGE_ITEM_H
#define EDGE_ITEM_H
#define _USE_MATH_DEFINES
#include <QGraphicsItem>
#include <QPointF>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QLineF>
#include <QTransform>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneHoverEvent>
#include "vertexitem.h"
#include <cmath>
using namespace std;
class VertexItem;
class GeneralGraphScene;
//����� ���, ��� ����������� ���������
class EdgeItem: public QGraphicsItem
{
public:
	enum {Type = UserType + 2};			//���
	enum EmptyVertexes{From, To, All};	//���� �������������� ���
	EdgeItem(VertexItem *v1, VertexItem *v2, const QString &text = "?", bool withArr = true);
	~EdgeItem();
	QRectF boundingRect() const;		//������� ����
	QPainterPath shape() const;			//����� ����
										//��������� ����
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget);
	void updatePos();					//���������� ����
	void setText(const QString &text);	//��������� ������
	const QString & getText() const;	//��������� ����� ����
	int type() const;					//���
	int getLevelIndex();				//��������� ������ ����
										//�������� ����������� ����
	void setImaginary(bool isImaginary, int empVert);
	VertexItem *getStart();				//��������� ��������� �������
	VertexItem *getEnd();				//��������� �������� �������
	void setColor(const QColor &c);		//��������� �����
	void setId(unsigned int newId);		//��������� ��������������
	unsigned int getId();				//��������� ��������������
	void findLevel(bool twoZero = false);	//����� ����������� ������ ����
	void setSpecial(bool flag = true);	//��������� ���� ��� ����������
private:
	//����������� �������
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	int findPoints();			//����� �������� � �������� ����� ����
	void formEdge(int curDist);	//������������ ����
	void formArr();			//������������ �������
	void formLoop();		//������������ �����
	bool withArr;			//���� ������� �������
	QString name;			//��� ����
	int edgeLevel;			//������� �����������
	int levelIndex;			//���������� ������ �����������
	QPainterPath edge;		//�������������� ����
	QPainterPath arr;		//�������
	VertexItem *vert1;		//��������� �� ������� ������
	VertexItem *vert2;		//��������� �� ������� ������
	QPointF from;			//����� ������ ����
	QPointF to;				//����� ����� ����
	QPen pen;				//�������� ����
	QColor color;			//���� ����
	QBrush brush;			//����� �������
							//����� ����
	QGraphicsSimpleTextItem *outputText;
	bool imaginary;			//���� ���������������� ����
	int emptyVert;			//��� �������������� ����
	unsigned int id;		//������������� ����
	bool special;			//���� ���������� ����
	bool specialBlocked;	//���� ���������� ���������� ����
};


#endif