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
//Класс дуг, как графических элементов
class EdgeItem: public QGraphicsItem
{
public:
	enum {Type = UserType + 2};			//Тип
	enum EmptyVertexes{From, To, All};	//Типы неотображаемых дуг
	EdgeItem(VertexItem *v1, VertexItem *v2, const QString &text = "?", bool withArr = true);
	~EdgeItem();
	QRectF boundingRect() const;		//Граница дуги
	QPainterPath shape() const;			//Форма дуги
										//Отрисовка дуги
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget);
	void updatePos();					//Обновление дуги
	void setText(const QString &text);	//Установка текста
	const QString & getText() const;	//Получение имени дуги
	int type() const;					//Тип
	int getLevelIndex();				//Получение уровня дуги
										//Устанока невидимости дуги
	void setImaginary(bool isImaginary, int empVert);
	VertexItem *getStart();				//Получение начальной вершины
	VertexItem *getEnd();				//Получение конечной вершины
	void setColor(const QColor &c);		//Установка цвета
	void setId(unsigned int newId);		//Установка идентификатора
	unsigned int getId();				//Получение идентификатора
	void findLevel(bool twoZero = false);	//Поиск подходящего уровня дуги
	void setSpecial(bool flag = true);	//Установка дуги как помеченной
private:
	//Обработчики событий
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	int findPoints();			//Поиск начальны и конечных точек дуги
	void formEdge(int curDist);	//Формирование дуги
	void formArr();			//Формирование стрелки
	void formLoop();		//Формирование петли
	bool withArr;			//Флаг наличия стрелки
	QString name;			//Имя дуги
	int edgeLevel;			//Уровень искривления
	int levelIndex;			//Показатель уровня искривления
	QPainterPath edge;		//Сформированная дуга
	QPainterPath arr;		//Стрелка
	VertexItem *vert1;		//Указатель на вершину исхода
	VertexItem *vert2;		//Указатель на вершину захода
	QPointF from;			//Точка начала дуги
	QPointF to;				//Точка конца дуги
	QPen pen;				//Карандаш дуги
	QColor color;			//Цвет дуги
	QBrush brush;			//Кисть стрелки
							//Текст дуги
	QGraphicsSimpleTextItem *outputText;
	bool imaginary;			//Флаг неотображаемости дуги
	int emptyVert;			//Тип неотображаемой дуги
	unsigned int id;		//Идентификатор дуги
	bool special;			//Флаг помеченной дуги
	bool specialBlocked;	//Флаг блокировки помеченной дуги
};


#endif