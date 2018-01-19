#include "edgeitem.h"


/*!Конструктор дуги
* \param v1 - начальная вершина
* \param v2 - конечная вершина
* \param text - текст дуги
* \param arr - наличие стрелки
*/
EdgeItem::EdgeItem(VertexItem *v1, VertexItem *v2, const QString &text, bool arr)
{
	vert1 = v1;			//Сохраняем начало и конец дуги
	vert2 = v2;	
	vert1->addOutEdge(this);//Добавляем дугу к спискам
	vert2->addInEdge(this);
	imaginary = false;	//Инциализация
	special = false;
	specialBlocked = false;
	emptyVert = -1;
	edgeLevel = 0;
	levelIndex = 0;
	withArr = arr;
	QFont font("Cambria", 8);
	font.setBold(true);
	outputText = new QGraphicsSimpleTextItem(this);	
	outputText->setFont(font);
	outputText->setZValue(2);
	pen.setWidth(2);
	setColor(QColor::fromRgb(0, 0, 0, 255));
	setText(text);
	findLevel(vert1 == vert2);	//Поиск уровня дуги
	updatePos();				//Обновления положения дуги
	setAcceptHoverEvents(true);	//Установка флагов
	setFlag(QGraphicsItem::ItemIsSelectable);
}

/*!Деструктор дуги
*/
EdgeItem::~EdgeItem()
{
	delete outputText;	//Удаляем текст

	if( imaginary )
	{//Если дугу неотображаемая
		if( vert1 != vert2 )
		{//и не петля
			if( emptyVert == From )
			//Если нет начала
				vert2->removeInEdge(this);
			else if( emptyVert == To )
			//Если нет конца
				vert1->removeOutEdge(this);
		}
	}
	else
	{//Удаляем дугу из все списков
		vert1->removeOutEdge(this);
		vert2->removeInEdge(this);
	}
}

/*!Устанвока отображаемости дуги
 \param isImaginary - значение флага
 \param empVert - какая из вершин отсуствует
*/
void EdgeItem::setImaginary(bool isImaginary, int empVert)
{
	imaginary = isImaginary;

	if( emptyVert == From || emptyVert == To )
		emptyVert = All;
	else if( emptyVert != All )
		emptyVert = empVert;
}

/*!Функция устанавливает имя дуги
* \param text - имя
*/
void EdgeItem::setText(const QString &text)
{
	name = text;
	setToolTip(name);

	if( imaginary )
		return;
	outputText->setToolTip(name);
	
	if( name.count() > 2 )
	{//Если текст не вмещается
		outputText->setText(name.left(2).append(".."));
	}
	else
	{
		outputText->setText(name);
	}
}

/*!Установка цвета
* \param c - цвет
*/
void EdgeItem::setColor(const QColor & c)
{
	color = c;
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(color);
	pen.setColor(color);
}

/*!Функция возвращает имя дуги
* \return - имя
*/
const QString & EdgeItem::getText() const
{
	return name;
}

/*!Фукнция возвращает тип
* \return - тип
*/
int EdgeItem::type() const
{
	return Type;
}

/*!Функция возвращает показатель уровня дуги
* \return - показатель уровня
*/
int EdgeItem::getLevelIndex()
{
	return levelIndex;
}

/*!Функция обновляет положение дуги
*/
void EdgeItem::updatePos()
{
	if( imaginary )
		return;

	if( vert1->collidesWithItem(vert2)  && vert1 != vert2 )
	{//Если произошло наложение вершин
		edge = QPainterPath();
		arr = QPainterPath();
		outputText->setVisible(false);
		return;
	}
	else if( vert1 == vert2 )
	{//Если рисуется петля
		formLoop();
		return;
	}
	float curDist = findPoints();	//Определяем координаты начала дуги
	formEdge(curDist);				//Формируем дугу

	if( scene() != NULL )
	//Если дуга добавлена к сцене
		scene()->update(boundingRect().translated(pos()));
}

/*!Функция формирует петлю
*/
void EdgeItem::formLoop()
{
	QPointF cPoint1;	//Первая контрольная точка
	QPointF cPoint2;	//Вторая контрольная точка
						//Ширина текста
	float textWidth = QFontMetrics(outputText->font()).width(outputText->text());
	QPointF textXY;		//Точка вывода текста

	if( edgeLevel >= 0 )
	{//Если уровень дуги больше 0
		from = to = vert1->pos() + QPointF(20, 0);
		cPoint1.setX(60 * (edgeLevel + 1));
		cPoint2.setX(60 * (edgeLevel + 1));
	}
	else
	{
		from = to = vert1->pos() - QPointF(20, 0);
		cPoint1.setX(60 * (edgeLevel));
		cPoint2.setX(60 * (edgeLevel));
	}
	cPoint1.setY(-100);		
	cPoint2.setY(100);
	edge = QPainterPath();
						//Проводим кривую Безье
	edge.cubicTo(cPoint1, cPoint2, QPointF());
						//Переносим в нужную точку
	edge.translate(from);
						//Определяем координаты текста
	textXY.setY((cPoint1.y() + cPoint2.y()) / 2);
	textXY.setX(cPoint1.x());
	textXY *= 0.75;
	textXY += from;
	textXY.setX(textXY.x() - textWidth * 1.3);
	outputText->setPos(textXY);
	outputText->setVisible(true);
	formArr();			//Формируем стрелку
}

/*!Функция ищет начальную и конечную точки дуги
* \return - расстояние между точками
*/
int EdgeItem::findPoints()
{
	//Текущая кратчайшая длина линии
	float curDist;
	//Угол поворота прямой, соезиняющей вершины
	float ang = QLineF(0, 0, 20, 0).angleTo(QLineF(vert1->pos(), vert2->pos()));
	QTransform trans;
	//Смещение угла в зависимости от уровня
	float angOffset = levelIndex < 6 ? edgeLevel * 10 : edgeLevel <= 0 ? -30 : 30;
	//Перенос точек в нужную систему координат
	trans.translate(vert1->pos().x(), vert1->pos().y());
	trans.rotate(-ang - angOffset);
	from = trans.map(QPointF(20, 0));
	trans.reset();
	trans.translate(vert2->pos().x(), vert2->pos().y());
	trans.rotate(-ang + angOffset);
	to = trans.map(QPointF(-20, 0));
	//Расстояние между вершинами
	curDist = sqrt(pow(to.x() - from.x(), 2) + 
						pow(to.y() - from.y(), 2));
	return curDist;
}
/*!Функция находит уровень дуги и его показатель
*
*/
void EdgeItem::findLevel(bool twoZero)
{
	if( imaginary )
		return;

	QMultiMap<int, int> commonEdges;	//Список показателей уровня и направления дуг
	QMultiMap<int, int>::const_iterator k;
	//Составление списка общих дуг двух вершин
	for( int i = 0; i < vert1->getOutEdges().count(); i++ )
		for( int j = 0; j < vert2->getInEdges().count(); j++ )
		{//Пока не пересмотрим все списки дуг
			if( vert1->getOutEdges()[i] != this && vert2->getInEdges()[j] != this )
				if( vert1->getOutEdges()[i] == vert2->getInEdges()[j] )
				{//Если нашлись общие сонарпавленные дуги
					commonEdges.insert(vert1->getOutEdges()[i]->getLevelIndex(), 1);
				}
		}
	for( int i = 0; i < vert1->getInEdges().count(); i++ )
		for( int j = 0; j < vert2->getOutEdges().count(); j++ )
		{//Пока не пересмотрим все списки дуг
			if( vert1->getInEdges()[i] != this && vert2->getOutEdges()[j] != this )
				if( vert1->getInEdges()[i] == vert2->getOutEdges()[j] )
				{//Если нашлись общие противоположные дуги
					commonEdges.insert(vert1->getInEdges()[i]->getLevelIndex(), -1);
				}
		}
	k = commonEdges.constEnd();
	k--;
	int dir;		//Нужное смещение показателя уровня
	levelIndex = 0;	//Начальное значение показателя уровня
	
	if( commonEdges.count() > 0 )
	{
		levelIndex = k.key();

		if( k.value() == -1 && k.key() != 0 )
		{//Если дуга противположная
			//Если уровень занимают дуги двух направлений
			dir = commonEdges.count() > 1 && (k-1).value() == 1
				&& (k-1).key() == k.key();
			//Или если уровень занимают дуги одного направления
			dir = dir || (commonEdges.count() > 1 && (k-1).value() == -1
				&& (k-1).key() == k.key() - 1);
			dir *= 2;	//Увеличиваем на 2

		}
		else
		{
			dir = 1;	//Увеличиваем на 1
			//Если уровень занимают дуги двух наравлений
			dir += commonEdges.count() > 1 && (k-1).value() == -1
					&& (k-1).key() == k.key();
		}
		if( twoZero )
		//Если считается уровень для петли
			dir = 1;
		levelIndex += dir;
	}	
	//Подсчёт уровня
	edgeLevel = (1 - ( levelIndex % 2 != 0 ) * 2) * (levelIndex / 2 + levelIndex % 2);
}

/*!Функция формирует элементы дуги графа
* \dist - расстояние от начала до конца дуги
*/
void EdgeItem::formEdge(int curDist)
{
	//Простроение элементов дуги
	edge = QPainterPath();		//Дуга
	//Угол поворота дуги
	float edgeAng = QLineF(to, from).angleTo(QLineF(0, 0, curDist, 0));
	QTransform edgeTrans;		//Матрица поворота для дуги
	edgeTrans.rotate(edgeAng);	//Преобразуем матрицу
								//Контрольная точка
	QPointF cPoint(curDist / 2, edgeLevel * 60);
								//Конечная точка
	QPointF ePoint(curDist, 0);
								//Получаем дугу
	edge.quadTo(edgeTrans.map(cPoint), edgeTrans.map(ePoint));
	edge.translate(to);			//Перемещаем в точку входа в вершину
	float extraInd = 0;			//Флаг дополнительного отступа
	float textWidth;			//Ширина текста
	textWidth = QFontMetrics(outputText->font()).width(outputText->text());
	//Устанавливаем ддополнительный отступ
	extraInd = (edgeLevel >= 0) - (edgeLevel < 0) - (edgeLevel == 0) * 0.3;
	extraInd *= 17;
	//Определяем точку для вывода текста
	float textX = curDist / 2 + (1 - (edgeAng <= 90 || edgeAng > 270) * 2) * textWidth / 2;
	float textY = cPoint.y() * 0.5 + extraInd ;
	QPointF textXY = QPointF(textX, textY);
	textXY = edgeTrans.map(textXY);
	textXY += to;
	outputText->setPos(textXY);	//Устанавливаем точку
	outputText->setVisible(true);
	formArr();					//Формируем стрелку
}

void EdgeItem::formArr()
{
	if( !withArr )
		return;

	QPolygonF edgePolygon;		//Массив точек дуги
	edgePolygon = edge.toFillPolygon();
	float arrAng;				//Угол поворота стрелки

	if( edgePolygon.count() > 1 )
	{//Если есть хотя бы две точки в дуге
		//Определяем необходимый угол поворота стрелки
		arrAng = QLineF(0, 0, 10, 0).angleTo(QLineF(edgePolygon[0], edgePolygon[1]));
	}
	arr = QPainterPath();		//Стрелка
	QTransform arrTrans;		//Матрица поворота для стрелки
	arrTrans.rotate(-arrAng);	//Преобразуем матрицу
								//Получаем стрелку
	arr.lineTo(QPointF(14, 2.5) * arrTrans);
	arr.lineTo(QPointF(14, -2.5) * arrTrans);
	arr.translate(to);			//Перемещаем в точку входа в вершину
	arr.closeSubpath();
}

/*!Функция возвращает форму дуги
*/
QPainterPath EdgeItem::shape() const
{
	QPainterPath sh(QPainterPathStroker().createStroke(edge));
	sh.addPath(arr);
	return sh;
}

/*!Функция возвращает прямоугольную область
* заключающую стрелку
*/
QRectF EdgeItem::boundingRect() const	
{
	return shape().boundingRect();
}

/*!Функция рисует дугу
*/
void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget)
{		
	painter->save();	
	painter->setRenderHint(QPainter::Antialiasing);
	
	if( isSelected() || special )
	{//Если дуга выделена или отмчена
		pen.setColor(QColor(0, 200, 0, 255));
	}
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(edge);
	painter->setBrush(isSelected() || special ? QColor(0, 200, 0, 255) : brush);
	painter->drawPath(arr);

	if( isSelected() )
	{//Если дуга выделена, то рисуем рамку
		painter->setPen(QColor::fromRgb(220, 220, 220, 255));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect());
	}
	pen.setColor(color);
	painter->restore();
}

/*!Обработка события наведения мыши
*/
void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	//Точки, соотвествующие положению курсора
	int midX = event->lastPos().x();	
	int midY = event->lastPos().y();
	QPainterPath sh = shape();
	special = specialBlocked;

	for( int x = midX - 3; x < midX + 3 && !special; x++ )
		for(  int y = midY - 3; y < midY + 3 && !special; y++ )
		{//Проверяем содержание в квдрате 7x7 вокруг курсора точки дуги
			special = sh.contains(QPointF(x, y));
		}
	QGraphicsItem::hoverEnterEvent(event);
}

/*!Обработчик ухода мыши с дуги
*/
void EdgeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	special = specialBlocked;
	setZValue(0);
	QGraphicsItem::hoverLeaveEvent(event);
}

/*!Установка идентификатора дуги
 \param newid - новый идентификатор
*/
void EdgeItem::setId(unsigned int newId)
{
	id = newId;
}

/*!Функция возвращает идентифкатор
 \return - идентфиикатор
*/
unsigned int EdgeItem::getId()
{
	return id;
}

/*!Функция возвращает начальную вершину
* \return - указатель на вершину
*/
VertexItem *EdgeItem::getStart()
{
	return vert1;
}

/*!Функция возвращает конечную вершину
* \return - указатель на вершину
*/
VertexItem *EdgeItem::getEnd()
{	
	return vert2;
}

/*!Функция устанавлваием дугу как помеченную
* \param flag - значение флага special
*/
void EdgeItem::setSpecial(bool flag )
{
	specialBlocked = flag;
	special = flag;
}