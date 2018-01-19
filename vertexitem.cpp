#include "vertexItem.h"

/*!����������� �������
* \param name - ��� �������
* \param tV - �������� �� ������� ���������
*/
VertexItem::VertexItem(const QString & name = "?", bool tV):
	font("Cambria", 8)
{
	//��������� ��������� ������
	setBorderColor(QColor::fromRgb(0, 0, 0));
	setTextColor(QColor::fromRgb(0, 0, 0));
	setVertColor(QColor::fromRgb(220, 220, 220));
	font.setBold(true);	//��������� ������
	pen.setWidth(2);	//��������� ������� �����
	setText(name);		//��������� ������
						//��������� ������ �������������� �������
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setFlag(QGraphicsItem::ItemIsPanel, false);
	id = 0;				//������������
	timeVert = tV;
	special = false;
	specialBlocked = false;
	setAcceptHoverEvents(true);
	collidesManager();
}

/*!���������
*/
VertexItem::~VertexItem()
{
	for(int i = 0; i < in.count(); i++ )
	{//�������������� ��� �������� ���� � ��������
		in[i]->setImaginary(true, EdgeItem::To);
	}
	for(int i = 0; i < out.count(); i++ )
	{//�������������� ��� ��������� ���� � ��������
		out[i]->setImaginary(true, EdgeItem::From);
	}
}

/*!������� ������������� ���� �������
* \param color - ����
*/
void VertexItem::setBorderColor(const QColor & color)
{
	borderColor = color;
	pen.setColor(borderColor);
}

/*!������� ������������� ���� ������
* \param color - ����
*/
void VertexItem::setTextColor(const QColor & color)
{
	textColor = color;
	textPen.setColor(textColor);
}

/*!������� ������������� ���� �������
* \param color - ����
*/
void VertexItem::setVertColor(const QColor & color)
{
	QRadialGradient vertColor(0, 0, 20, 0, 0);
	vertColor.setColorAt(0, color.lighter(220));
	vertColor.setColorAt(1, color);
	brush = QBrush(vertColor);
}

/*!������� ������������� ������������
* \param alpha - �������� ������������
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
/*! ������� ������������� ����� �������
* \param name - ��������������� �����
*/
void VertexItem::setText(const QString & name)
{
	bool haveUpper = false;	//���� ������� ������� ����� � ������
	text = name;			//��������� �����

	if( name.length() > 3 )
	{//���� ��� �� ��������� � ����
		outputText = name.left(3);
		outputText.append("..");
	}
	else
	{//���� ��� ��������� � ����
		outputText = name;
	}
	for( int i = 0; i < outputText.count(); i++ )
	{//�������� �� ������� ������� ���� � ������
		haveUpper = haveUpper || outputText[i].isUpper();
	}
	textXY.setY(3 + int(haveUpper) * 1.5);	//���� � ������ ���� ������� �����
	textXY.setX(-QFontMetrics(font).width(outputText) / 2 );
	setToolTip(text);
}

/*!������� ���������� ����� �������
* \return - ����� �������
*/
const QString & VertexItem::getText() const
{
	return text;
}

/*!������� ��������� �������� ����
* \param edg - ��������� �� ����
*/
void VertexItem::addInEdge(EdgeItem *edg)
{
	in.append(edg);
}

/*!������� ��������� ��������� ����
* \param edg - ��������� �� ����
*/
void VertexItem::addOutEdge(EdgeItem *edg)
{
	out.append(edg);
}

/*!������� ������� �������� ����
* \param edg - ��������� �� ����
*/
void VertexItem::removeInEdge(EdgeItem *edg)
{
	for( int i = 0; i < in.count(); i++ )
	{//���� �� �������� ��� ��������
		if( in[i] == edg )
		{//���� ����� ������
			in.remove(i);	//�������
			return;
		}
	}

}

/*!������� ������� ��������� ����
* \param edg - ��������� �� ����
*/
void VertexItem::removeOutEdge(EdgeItem *edg)
{

	for( int i = 0; i < out.count(); i++ )
	{//���� �� �������� ��� ��������
		if( out[i] == edg )
		{//���� ����� ������
			out.remove(i);	//�������
			return;
		}
	}
}


/*!������� ���������� ������ �������� ���
* \return - ������ ���������� �� ����
*/
const QVector<EdgeItem *> & VertexItem::getInEdges() const
{
	return in;
}

/*!������� ���������� ������ ��������� ���
* \return - ������ ���������� �� ����
*/
const QVector<EdgeItem *> & VertexItem::getOutEdges() const
{
	return out;
}

/*!������� ���������� ��� ��������
* \return - ������������� ���������
*/
int VertexItem::type() const
{
	return Type;
}

/*!������� ���������� ������� �������
* \return - �������������, � ������� ��������� �������
*/
QRectF VertexItem::boundingRect() const
{
	return QRectF(-21, -21, 42, 42); 
}

/*!������� ������������ �������
*/
void VertexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(brush);				//��������� ����� �������
	painter->setFont(font);					//��������� ������
											
	if( isSelected() || special)
	{//���� ������� ��������� ��� ��������
		pen.setColor(QColor(0, 200, 0, 255));
	}		
	painter->setPen(pen);					//��������� ������� �����
	painter->drawEllipse(-20, -20, 40, 40);	//������ ����					
	pen.setColor(textColor);				//������ �����
	painter->setPen(textPen);
	painter->drawText(textXY, outputText);	

	if( isSelected() )
	{//���� ������� �������
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

/*!������� ���������� ����� �������
* \return - �����
*/
QPainterPath VertexItem::shape() const
{
	QPainterPath sh;
	sh.addEllipse(-21, -21, 42, 42);
	return sh;
}

/*!���������� ������� ������ ����
*/
void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
}

/*!���������� ���������� ������ ����
*/
void VertexItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if( event->button() == Qt::LeftButton )
	{//���� �������� ����� ������
		collidesManager();	//�������� �� ��������� ������
							//��������� ��� ��������� ���
		static_cast<GraphScene*>(scene())->edgesManager(GraphScene::Selected, this);
	}
	QGraphicsItem::mouseReleaseEvent(event);

}

/*!���������� ��������� �������
*/
void VertexItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverEnterEvent(event);
	special = true;
	setZValue(1);
	static_cast<GraphScene*>(scene())->edgesManager(GraphScene::HoverOn, this);
	static_cast<GraphScene*>(scene())->waysManager(GraphScene::Show, this);
}
	
/*!���������� ����� ������� � ��������
*/
void VertexItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverLeaveEvent(event);
	special = false || specialBlocked;
	setZValue(0);
	static_cast<GraphScene*>(scene())->edgesManager(GraphScene::HoverOut, this);
	static_cast<GraphScene*>(scene())->waysManager(GraphScene::Hide);
}

/*!���������� ��������� ���������� �������
*/
QVariant VertexItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if( change == QGraphicsItem::ItemPositionChange )
	{//���� ��������� ��������� ��������� �������
		for(int i = 0; i < in.count(); i++ )
		{//��������� ��������� ���� �������� ���
			in[i]->updatePos();
		}
		for(int i = 0; i < out.count(); i++ )
		{//��������� ��������� ���� ��������� ���
			out[i]->updatePos();
		}
	}
	return value;
}

/*!������� ����� ������� �������������
* \param newId - ����� �������������
*/
void VertexItem::setId(unsigned int newId)
{
	id = newId;
}

/*!������� ���������� ����� ������������� �������
* \return - ����� �������������
*/
unsigned int VertexItem::getId()
{
	return id;
}

/*!������� ������������ ��������� ������
*/
void VertexItem::collidesManager()
{
	bool collideWithVert = false;	//���� ��������� ������
									//������ ���������� ���������
	QList<QGraphicsItem *> otherVerts = collidingItems();
	int i;			//������ ��������
	//��������� ��������� ������
	for( i = 0; i < otherVerts.count() && !collideWithVert; i++ )
	{//��������� ������� ����� ���������� ��������� ������ ������
		collideWithVert = otherVerts[i]->type() == Type;

		if( collideWithVert )
		//���� ��������� ���������
			//�������� �� ������� ���������
			collideWithVert = !qgraphicsitem_cast<VertexItem*>(otherVerts[i])->isTimeVert();
	}
	while( collideWithVert )
	{//���� �� ��������� ���������
		moveBy(40, 40);	//�������� ��������� �������
		itemChange(ItemPositionChange, pos());
		collideWithVert = false;
		otherVerts = collidingItems();
		
		for( i = 0; i < otherVerts.count() && !collideWithVert; i++ )
		{//��������� ������� ����� ���������� ��������� ������ ������
			collideWithVert = otherVerts[i]->type() == Type;

			if( collideWithVert )
			//���� ��������� ���������
				//�������� �� ������� ���������
				collideWithVert = !qgraphicsitem_cast<VertexItem*>(otherVerts[i])->isTimeVert();
		}
	}
}

/*!������� ������������� ������� ��� ����������
* \param flag - �������� ����� special
*/
void VertexItem::setSpecial(bool flag )
{
	specialBlocked = flag;
	special = flag;
}