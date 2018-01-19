#ifndef VERTEX_ITEM_H
#define VERTEX_ITEM_H
#include <QGraphicsItem>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QFontMetrics>
#include <QColor>
#include <QRadialGradient>
#include "graphscene.h"
#include "edgeitem.h"

class EdgeItem;
class GeneralGraphScene;

//����� ������, ��� ����������� ���������
class VertexItem : public QGraphicsItem
{ 
public:	
	enum {Type = UserType + 1};				//���
	VertexItem(const QString & name, bool tV = false);
	~VertexItem();
	QRectF boundingRect() const;		//������� �������
										//��������� �������
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget);
	QPainterPath shape() const;			//����� �������
	void setText(const QString & name);	//��������� ������ �������
	const QString & getText() const;	//��������� ������ �������
	void setBorderColor(const QColor & color);	//��������� ����� �������
	void setTextColor(const QColor & color);	//��������� ����� ������
	void setVertColor(const QColor & color);	//��������� ��������� ���� �������
	void addInEdge(EdgeItem *edg);		//���������� �������� ����
	void addOutEdge(EdgeItem *edg);		//���������� ��������� ����
	void removeInEdge(EdgeItem *edg);	//�������� �������� ����
	void removeOutEdge(EdgeItem *edg);	//�������� ��������� ����
	int type() const;					///���
	const QVector<EdgeItem *> & getInEdges() const;	//������ �������� ���
	const QVector<EdgeItem *> & getOutEdges() const;//������ ��������� ���
	void setId(unsigned int newId);		//��������� ��������������
	void setTransp(int alpha);			//��������� ������������
	unsigned int getId();				//������� ���������������
	bool isTimeVert() {return timeVert;};	//�������� �� ��������� ��������
	void setSpecial(bool flag = true);		//���������� ������� ��� ����������
private:	
	//����������� �������
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	QVariant itemChange(GraphicsItemChange change, const QVariant & value); 
	void collidesManager();	//�������� ��������� ������
	QColor textColor;		//���� ������
	QColor borderColor;		//���� �������
	QBrush brush;			//�����
	QPointF textXY;			//���������� ���������� ������
	QFont font;				//�����
	QPen pen;				//�������� ��� ���������
	QPen textPen;			//�������� ��� ������
	QString text;			//����� �������
	QString outputText;		//��������� �����
	QVector<EdgeItem *> in;	//������ �������� ���
	QVector<EdgeItem *> out;//������ ��������� ���
	unsigned int id;		//�������������
	bool timeVert;			//���� ������������ �������
	bool special;			//���� ���������� �������
	bool specialBlocked;	//������ �� ������ ����� ���������� �������
};	

#endif