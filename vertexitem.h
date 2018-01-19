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

//Класс вершин, как графических элементов
class VertexItem : public QGraphicsItem
{ 
public:	
	enum {Type = UserType + 1};				//Тип
	VertexItem(const QString & name, bool tV = false);
	~VertexItem();
	QRectF boundingRect() const;		//Граница вершины
										//Отрисовка вершины
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget);
	QPainterPath shape() const;			//Форма вершины
	void setText(const QString & name);	//Установка текста вершины
	const QString & getText() const;	//Получение текста вершины
	void setBorderColor(const QColor & color);	//Установка цвета границы
	void setTextColor(const QColor & color);	//Установка цвета текста
	void setVertColor(const QColor & color);	//Установка основного цвет заливки
	void addInEdge(EdgeItem *edg);		//Добавление входящей дуги
	void addOutEdge(EdgeItem *edg);		//Добавление исходящей дуги
	void removeInEdge(EdgeItem *edg);	//Удаление входящей дуги
	void removeOutEdge(EdgeItem *edg);	//Удаление исходящей дуги
	int type() const;					///Тип
	const QVector<EdgeItem *> & getInEdges() const;	//Список входящих дуг
	const QVector<EdgeItem *> & getOutEdges() const;//Список выходящих дуг
	void setId(unsigned int newId);		//Установка идентификатора
	void setTransp(int alpha);			//Установка прозрачности
	unsigned int getId();				//Возврат иденттификатора
	bool isTimeVert() {return timeVert;};	//Является ли временной вершиной
	void setSpecial(bool flag = true);		//Установить вершину как помеченную
private:	
	//Обработчики событий
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	QVariant itemChange(GraphicsItemChange change, const QVariant & value); 
	void collidesManager();	//Контроль наложения вершин
	QColor textColor;		//Цвет текста
	QColor borderColor;		//Цвет эллипса
	QBrush brush;			//Кисть
	QPointF textXY;			//Координаты выводимого текста
	QFont font;				//Шрифт
	QPen pen;				//Карандаш для рисования
	QPen textPen;			//Карандаш для текста
	QString text;			//Текст вершины
	QString outputText;		//Выводимый текст
	QVector<EdgeItem *> in;	//Массив входящих дуг
	QVector<EdgeItem *> out;//Массив выходящих дуг
	unsigned int id;		//Идентификатор
	bool timeVert;			//Флаг непостоянной вершины
	bool special;			//Флаг помеченной вершины
	bool specialBlocked;	//Запрет на снятие флага помеченной вершины
};	

#endif