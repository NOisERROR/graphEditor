#include "vertexItem.h"

/*!Конструктор вершины
* \param name - имя вершины
* \param tV - является ли вершина временной
*/
VertexItem::VertexItem(const QString & name = "?", bool tV):
	font("Cambria", 8)
{
	//Установка начальных цветов
	setBorderColor(QColor::fromRgb(0, 0, 0));
	setTextColor(QColor::fromRgb(0, 0, 0));
	setVertColor(QColor::fromRgb(220, 220, 220));
	font.setBold(true);	//Установка шрифта
	pen.setWidth(2);	//Установка толщины линий
	setText(name);		//Установка текста
						//Установка флагов взаимодействия вершины
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setFlag(QGraphicsItem::ItemIsPanel, false);
	id = 0;				//Инициализаци
	timeVert = tV;
	special = false;
	specialBlocked = false;
	setAcceptHoverEvents(true);
	collidesManager();
}

/*!Дестуктор
*/
VertexItem::~VertexItem()
{
	for(int i = 0; i < in.count(); i++ )
	{//Подготавливаем все входящие дуги к удалению
		in[i]->setImaginary(true, EdgeItem::To);
	}
	for(int i = 0; i < out.count(); i++ )
	{//Подготавливаем все выходящие дуги к удалению
		out[i]->setImaginary(true, EdgeItem::From);
	}
}

/*!Функция устанавливает цвет эллипса
* \param color - цвет
*/
void VertexItem::setBorderColor(const QColor & color)
{
	borderColor = color;
	pen.setColor(borderColor);
}

/*!Функция устанавливает цвет текста
* \param color - цвет
*/
void VertexItem::setTextColor(const QColor & color)
{
	textColor = color;
	textPen.setColor(textColor);
}

/*!Функция устанавливает цвет заливки
* \param color - цвет
*/
void VertexItem::setVertColor(const QColor & color)
{
	QRadialGradient vertColor(0, 0, 20, 0, 0);
	vertColor.setColorAt(0, color.lighter(220));
	vertColor.setColorAt(1, color);
	brush = QBrush(vertColor);
}

/*!Функция устанавливает прозрачность
* \param alpha - параметр прозрачности
*/
void VertexItem::setTransp(int alpha)
{
	QColor newColor;
	newColor = textColor;
	newColor.setAlpha(alpha);
	setTextColor(newColor);
	newColor = borderColor;
	newColor.setAlpha(alpha);
	setBorderColor(newColor);
	newColor = QColor::fromRgb(220, 220, 220);
	newColor.setAlpha(alpha);
	setVertColor(newColor);
}
/*! Функция устанавливает текст вершины
* \param name - устанавливаемый текст
*/
void VertexItem::setText(const QString & name)
{
	bool haveUpper = false;	//Флаг наличия большой буквы в тексте
	text = name;			//Сохраняем текст

	if( name.length() > 3 )
	{//Если имя не вместится в круг
		outputText = name.left(3);
		outputText.append("..");
	}
	else
	{//Если имя вмещается в круг
		outputText = name;
	}
	for( int i = 0; i < outputText.count(); i++ )
	{//Проверка на наличие больших букв в тексте
		haveUpper = haveUpper || outputText[i].isUpper();
	}
	textXY.setY(3 + int(haveUpper) * 1.5);	//Если в тексте есть большие буквы
	textXY.setX(-QFontMetrics(font).width(outputText) / 2 );
	setToolTip(text);
}

/*!Функция возвращает текст вершины
* \return - текст вершины
*/
const QString & VertexItem::getText() const
{
	return text;
}

/*!Функция добавляет входящую дугу
* \param edg - указатель на дугу
*/
void VertexItem::addInEdge(EdgeItem *edg)
{
	in.append(edg);
}

/*!Функция добавляет исходящую дугу
* \param edg - указатель на дугу
*/
void VertexItem::addOutEdge(EdgeItem *edg)
{
	out.append(edg);
}

/*!Функция удаляет входящую дугу
* \param edg - указатель на дугу
*/
void VertexItem::removeInEdge(EdgeItem *edg)
{
	for( int i = 0; i < in.count(); i++ )
	{//Пока не переберём все элементы
		if( in[i] == edg )
		{//Если нашли нужный
			in.remove(i);	//Удаляем
			return;
		}
	}

}

/*!Функция удаляет исходящую дугу
* \param edg - указатель на дугу
*/
void VertexItem::removeOutEdge(EdgeItem *edg)
{

	for( int i = 0; i < out.count(); i++ )
	{//Пока не переберём все элементы
		if( out[i] == edg )
		{//Если нашли нужный
			out.remove(i);	//Удаляем
			return;
		}
	}
}


/*!Функция возвращает список входящих дуг
* \return - массив указателей на дуги
*/
const QVector<EdgeItem *> & VertexItem::getInEdges() const
{
	return in;
}

/*!Функция возвращает список исходящих дуг
* \return - массив указателей на дуги
*/
const QVector<EdgeItem *> & VertexItem::getOutEdges() const
{
	return out;
}

/*!Функция возвращает тип элемента
* \return - целочисленная константа
*/
int VertexItem::type() const
{
	return Type;
}

/*!Функция определяет область вершины
* \return - прямоугольник, в котором заключена вершина
*/
QRectF VertexItem::boundingRect() const
{
	return QRectF(-21, -21, 42, 42); 
}

/*!Функция отрисовывает вершину
*/
void VertexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(brush);				//Устанвока цвета вершины
	painter->setFont(font);					//Установка шрифта
											
	if( isSelected() || special)
	{//Если вершина находится под курсором
		pen.setColor(QColor(0, 200, 0, 255));
	}		
	painter->setPen(pen);					//Установка толщины линий
	painter->drawEllipse(-20, -20, 40, 40);	//Рисуем круг					
	pen.setColor(textColor);				//Рисуем текст
	painter->setPen(textPen);
	painter->drawText(textXY, outputText);	

	if( isSelected() )
	{//Если вершина выбрана
		setZValue(1);
		QPen rectPen(QColor::fromRgb(220, 220, 220, 255));
		painter->setPen(rectPen);
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect());
		pen.setColor(borderColor);
		setZValue(0);
	}
	if( special )
		pen.setColor(borderColor);
	painter->restore();
}

/*!Функция вовзращает форму вершины
* \return - форма
*/
QPainterPath VertexItem::shape() const
{
	QPainterPath sh;
	sh.addEllipse(-21, -21, 42, 42);
	return sh;
}

/*!Обработчик нажатия кнопки мыши
*/
void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
}

/*!Обработчки отпускания кнопки мыши
*/
void VertexItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if( event->button() == Qt::LeftButton )
	{//Если отпущена левая кнопка
		collidesManager();	//Проверка на наложение вершин
							//Сообщение для менеджера дуг
		static_cast<GraphScene*>(scene())->edgesManager(GraphScene::Selected, this);
	}
	QGraphicsItem::mouseReleaseEvent(event);

}

/*!Обработчик наведения курсора
*/
void VertexItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverEnterEvent(event);
	special = true;
	setZValue(1);
	static_cast<GraphScene*>(scene())->edgesManager(GraphScene::HoverOn, this);
	static_cast<GraphScene*>(scene())->waysManager(GraphScene::Show, this);
}
	
/*!Обработчик ухода курсора с элемента
*/
void VertexItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverLeaveEvent(event);
	special = false || specialBlocked;
	setZValue(0);
	static_cast<GraphScene*>(scene())->edgesManager(GraphScene::HoverOut, this);
	static_cast<GraphScene*>(scene())->waysManager(GraphScene::Hide);
}

/*!Обработчик изменения параметров вершины
*/
QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if( change == QGraphicsItem::ItemPositionChange )
	{//Если произошло изменение положения вершины
		for(int i = 0; i < in.count(); i++ )
		{//Обновляем положение всех входящих дуг
			in[i]->updatePos();
		}
		for(int i = 0; i < out.count(); i++ )
		{//Обновляем положение всех исходящих дуг
			out[i]->updatePos();
		}
	}
	return value;
}

/*!Функция задаёт вершине идентификатор
* \param newId - новый идентификатор
*/
void VertexItem::setId(unsigned int newId)
{
	id = newId;
}

/*!Функция возвращает новый идентификатор вершины
* \return - новый идентификатор
*/
unsigned int VertexItem::getId()
{
	return id;
}

/*!Функция контролирует наложение вершин
*/
void VertexItem::collidesManager()
{
	bool collideWithVert = false;	//Флаг наложения вершин
									//Список наложенных элементов
	QList<QGraphicsItem *> otherVerts = collidingItems();
	int i;			//Индекс элемента
	//Обработка наложения вершин
	for( i = 0; i < otherVerts.count() && !collideWithVert; i++ )
	{//Проверяем наличие среди перекрытых элементов других вершин
		collideWithVert = otherVerts[i]->type() == Type;

		if( collideWithVert )
		//Если произошло наложение
			//Является ли вершина временной
			collideWithVert = !qgraphicsitem_cast<VertexItem*>(otherVerts[i])->isTimeVert();
	}
	while( collideWithVert )
	{//Пока не устранено наложение
		moveBy(40, 40);	//Сдвигаем положение вершины
		itemChange(ItemPositionChange, pos());
		collideWithVert = false;
		otherVerts = collidingItems();
		
		for( i = 0; i < otherVerts.count() && !collideWithVert; i++ )
		{//Проверяем наличие среди перекрытых элементов других вершин
			collideWithVert = otherVerts[i]->type() == Type;

			if( collideWithVert )
			//Если произошло наложение
				//Является ли вершина временной
				collideWithVert = !qgraphicsitem_cast<VertexItem*>(otherVerts[i])->isTimeVert();
		}
	}
}

/*!Функция устанавлваием вершину как помеченную
* \param flag - значение флага special
*/
void VertexItem::setSpecial(bool flag )
{
	specialBlocked = flag;
	special = flag;
}