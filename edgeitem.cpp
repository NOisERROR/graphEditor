#include "edgeitem.h"


/*!����������� ����
* \param v1 - ��������� �������
* \param v2 - �������� �������
* \param text - ����� ����
* \param arr - ������� �������
*/
EdgeItem::EdgeItem(VertexItem *v1, VertexItem *v2, const QString &text, bool arr)
{
	vert1 = v1;			//��������� ������ � ����� ����
	vert2 = v2;	
	vert1->addOutEdge(this);//��������� ���� � �������
	vert2->addInEdge(this);
	imaginary = false;	//������������
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
	findLevel(vert1 == vert2);	//����� ������ ����
	updatePos();				//���������� ��������� ����
	setAcceptHoverEvents(true);	//��������� ������
	setFlag(QGraphicsItem::ItemIsSelectable);
}

/*!���������� ����
*/
EdgeItem::~EdgeItem()
{
	delete outputText;	//������� �����

	if( imaginary )
	{//���� ���� ��������������
		if( vert1 != vert2 )
		{//� �� �����
			if( emptyVert == From )
			//���� ��� ������
				vert2->removeInEdge(this);
			else if( emptyVert == To )
			//���� ��� �����
				vert1->removeOutEdge(this);
		}
	}
	else
	{//������� ���� �� ��� �������
		vert1->removeOutEdge(this);
		vert2->removeInEdge(this);
	}
}

/*!��������� �������������� ����
 \param isImaginary - �������� �����
 \param empVert - ����� �� ������ ����������
*/
void EdgeItem::setImaginary(bool isImaginary, int empVert)
{
	imaginary = isImaginary;

	if( emptyVert == From || emptyVert == To )
		emptyVert = All;
	else if( emptyVert != All )
		emptyVert = empVert;
}

/*!������� ������������� ��� ����
* \param text - ���
*/
void EdgeItem::setText(const QString &text)
{
	name = text;
	setToolTip(name);

	if( imaginary )
		return;
	outputText->setToolTip(name);
	
	if( name.count() > 2 )
	{//���� ����� �� ���������
		outputText->setText(name.left(2).append(".."));
	}
	else
	{
		outputText->setText(name);
	}
}

/*!��������� �����
* \param c - ����
*/
void EdgeItem::setColor(const QColor & c)
{
	color = c;
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(color);
	pen.setColor(color);
}

/*!������� ���������� ��� ����
* \return - ���
*/
const QString & EdgeItem::getText() const
{
	return name;
}

/*!������� ���������� ���
* \return - ���
*/
int EdgeItem::type() const
{
	return Type;
}

/*!������� ���������� ���������� ������ ����
* \return - ���������� ������
*/
int EdgeItem::getLevelIndex()
{
	return levelIndex;
}

/*!������� ��������� ��������� ����
*/
void EdgeItem::updatePos()
{
	if( imaginary )
		return;

	if( vert1->collidesWithItem(vert2)  && vert1 != vert2 )
	{//���� ��������� ��������� ������
		edge = QPainterPath();
		arr = QPainterPath();
		outputText->setVisible(false);
		return;
	}
	else if( vert1 == vert2 )
	{//���� �������� �����
		formLoop();
		return;
	}
	float curDist = findPoints();	//���������� ���������� ������ ����
	formEdge(curDist);				//��������� ����

	if( scene() != NULL )
	//���� ���� ��������� � �����
		scene()->update(boundingRect().translated(pos()));
}

/*!������� ��������� �����
*/
void EdgeItem::formLoop()
{
	QPointF cPoint1;	//������ ����������� �����
	QPointF cPoint2;	//������ ����������� �����
						//������ ������
	float textWidth = QFontMetrics(outputText->font()).width(outputText->text());
	QPointF textXY;		//����� ������ ������

	if( edgeLevel >= 0 )
	{//���� ������� ���� ������ 0
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
						//�������� ������ �����
	edge.cubicTo(cPoint1, cPoint2, QPointF());
						//��������� � ������ �����
	edge.translate(from);
						//���������� ���������� ������
	textXY.setY((cPoint1.y() + cPoint2.y()) / 2);
	textXY.setX(cPoint1.x());
	textXY *= 0.75;
	textXY += from;
	textXY.setX(textXY.x() - textWidth * 1.3);
	outputText->setPos(textXY);
	outputText->setVisible(true);
	formArr();			//��������� �������
}

/*!������� ���� ��������� � �������� ����� ����
* \return - ���������� ����� �������
*/
int EdgeItem::findPoints()
{
	//������� ���������� ����� �����
	float curDist;
	//���� �������� ������, ����������� �������
	float ang = QLineF(0, 0, 20, 0).angleTo(QLineF(vert1->pos(), vert2->pos()));
	QTransform trans;
	//�������� ���� � ����������� �� ������
	float angOffset = levelIndex < 6 ? edgeLevel * 10 : edgeLevel <= 0 ? -30 : 30;
	//������� ����� � ������ ������� ���������
	trans.translate(vert1->pos().x(), vert1->pos().y());
	trans.rotate(-ang - angOffset);
	from = trans.map(QPointF(20, 0));
	trans.reset();
	trans.translate(vert2->pos().x(), vert2->pos().y());
	trans.rotate(-ang + angOffset);
	to = trans.map(QPointF(-20, 0));
	//���������� ����� ���������
	curDist = sqrt(pow(to.x() - from.x(), 2) + 
						pow(to.y() - from.y(), 2));
	return curDist;
}
/*!������� ������� ������� ���� � ��� ����������
*
*/
void EdgeItem::findLevel(bool twoZero)
{
	if( imaginary )
		return;

	QMultiMap<int, int> commonEdges;	//������ ����������� ������ � ����������� ���
	QMultiMap<int, int>::const_iterator k;
	//����������� ������ ����� ��� ���� ������
	for( int i = 0; i < vert1->getOutEdges().count(); i++ )
		for( int j = 0; j < vert2->getInEdges().count(); j++ )
		{//���� �� ����������� ��� ������ ���
			if( vert1->getOutEdges()[i] != this && vert2->getInEdges()[j] != this )
				if( vert1->getOutEdges()[i] == vert2->getInEdges()[j] )
				{//���� ������� ����� �������������� ����
					commonEdges.insert(vert1->getOutEdges()[i]->getLevelIndex(), 1);
				}
		}
	for( int i = 0; i < vert1->getInEdges().count(); i++ )
		for( int j = 0; j < vert2->getOutEdges().count(); j++ )
		{//���� �� ����������� ��� ������ ���
			if( vert1->getInEdges()[i] != this && vert2->getOutEdges()[j] != this )
				if( vert1->getInEdges()[i] == vert2->getOutEdges()[j] )
				{//���� ������� ����� ��������������� ����
					commonEdges.insert(vert1->getInEdges()[i]->getLevelIndex(), -1);
				}
		}
	k = commonEdges.constEnd();
	k--;
	int dir;		//������ �������� ���������� ������
	levelIndex = 0;	//��������� �������� ���������� ������
	
	if( commonEdges.count() > 0 )
	{
		levelIndex = k.key();

		if( k.value() == -1 && k.key() != 0 )
		{//���� ���� ��������������
			//���� ������� �������� ���� ���� �����������
			dir = commonEdges.count() > 1 && (k-1).value() == 1
				&& (k-1).key() == k.key();
			//��� ���� ������� �������� ���� ������ �����������
			dir = dir || (commonEdges.count() > 1 && (k-1).value() == -1
				&& (k-1).key() == k.key() - 1);
			dir *= 2;	//����������� �� 2

		}
		else
		{
			dir = 1;	//����������� �� 1
			//���� ������� �������� ���� ���� ����������
			dir += commonEdges.count() > 1 && (k-1).value() == -1
					&& (k-1).key() == k.key();
		}
		if( twoZero )
		//���� ��������� ������� ��� �����
			dir = 1;
		levelIndex += dir;
	}	
	//������� ������
	edgeLevel = (1 - ( levelIndex % 2 != 0 ) * 2) * (levelIndex / 2 + levelIndex % 2);
}

/*!������� ��������� �������� ���� �����
* \dist - ���������� �� ������ �� ����� ����
*/
void EdgeItem::formEdge(int curDist)
{
	//����������� ��������� ����
	edge = QPainterPath();		//����
	//���� �������� ����
	float edgeAng = QLineF(to, from).angleTo(QLineF(0, 0, curDist, 0));
	QTransform edgeTrans;		//������� �������� ��� ����
	edgeTrans.rotate(edgeAng);	//����������� �������
								//����������� �����
	QPointF cPoint(curDist / 2, edgeLevel * 60);
								//�������� �����
	QPointF ePoint(curDist, 0);
								//�������� ����
	edge.quadTo(edgeTrans.map(cPoint), edgeTrans.map(ePoint));
	edge.translate(to);			//���������� � ����� ����� � �������
	float extraInd = 0;			//���� ��������������� �������
	float textWidth;			//������ ������
	textWidth = QFontMetrics(outputText->font()).width(outputText->text());
	//������������� ��������������� ������
	extraInd = (edgeLevel >= 0) - (edgeLevel < 0) - (edgeLevel == 0) * 0.3;
	extraInd *= 17;
	//���������� ����� ��� ������ ������
	float textX = curDist / 2 + (1 - (edgeAng <= 90 || edgeAng > 270) * 2) * textWidth / 2;
	float textY = cPoint.y() * 0.5 + extraInd ;
	QPointF textXY = QPointF(textX, textY);
	textXY = edgeTrans.map(textXY);
	textXY += to;
	outputText->setPos(textXY);	//������������� �����
	outputText->setVisible(true);
	formArr();					//��������� �������
}

void EdgeItem::formArr()
{
	if( !withArr )
		return;

	QPolygonF edgePolygon;		//������ ����� ����
	edgePolygon = edge.toFillPolygon();
	float arrAng;				//���� �������� �������

	if( edgePolygon.count() > 1 )
	{//���� ���� ���� �� ��� ����� � ����
		//���������� ����������� ���� �������� �������
		arrAng = QLineF(0, 0, 10, 0).angleTo(QLineF(edgePolygon[0], edgePolygon[1]));
	}
	arr = QPainterPath();		//�������
	QTransform arrTrans;		//������� �������� ��� �������
	arrTrans.rotate(-arrAng);	//����������� �������
								//�������� �������
	arr.lineTo(QPointF(14, 2.5) * arrTrans);
	arr.lineTo(QPointF(14, -2.5) * arrTrans);
	arr.translate(to);			//���������� � ����� ����� � �������
	arr.closeSubpath();
}

/*!������� ���������� ����� ����
*/
QPainterPath EdgeItem::shape() const
{
	QPainterPath sh(QPainterPathStroker().createStroke(edge));
	sh.addPath(arr);
	return sh;
}

/*!������� ���������� ������������� �������
* ����������� �������
*/
QRectF EdgeItem::boundingRect() const	
{
	return shape().boundingRect();
}

/*!������� ������ ����
*/
void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget)
{		
	painter->save();	
	painter->setRenderHint(QPainter::Antialiasing);
	
	if( isSelected() || special )
	{//���� ���� �������� ��� �������
		pen.setColor(QColor(0, 200, 0, 255));
	}
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	painter->drawPath(edge);
	painter->setBrush(isSelected() || special ? QColor(0, 200, 0, 255) : brush);
	painter->drawPath(arr);

	if( isSelected() )
	{//���� ���� ��������, �� ������ �����
		painter->setPen(QColor::fromRgb(220, 220, 220, 255));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect());
	}
	pen.setColor(color);
	painter->restore();
}

/*!��������� ������� ��������� ����
*/
void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	//�����, �������������� ��������� �������
	int midX = event->lastPos().x();	
	int midY = event->lastPos().y();
	QPainterPath sh = shape();
	special = specialBlocked;

	for( int x = midX - 3; x < midX + 3 && !special; x++ )
		for(  int y = midY - 3; y < midY + 3 && !special; y++ )
		{//��������� ���������� � ������� 7x7 ������ ������� ����� ����
			special = sh.contains(QPointF(x, y));
		}
	QGraphicsItem::hoverEnterEvent(event);
}

/*!���������� ����� ���� � ����
*/
void EdgeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	special = specialBlocked;
	setZValue(0);
	QGraphicsItem::hoverLeaveEvent(event);
}

/*!��������� �������������� ����
 \param newid - ����� �������������
*/
void EdgeItem::setId(unsigned int newId)
{
	id = newId;
}

/*!������� ���������� ������������
 \return - �������������
*/
unsigned int EdgeItem::getId()
{
	return id;
}

/*!������� ���������� ��������� �������
* \return - ��������� �� �������
*/
VertexItem *EdgeItem::getStart()
{
	return vert1;
}

/*!������� ���������� �������� �������
* \return - ��������� �� �������
*/
VertexItem *EdgeItem::getEnd()
{	
	return vert2;
}

/*!������� ������������� ���� ��� ����������
* \param flag - �������� ����� special
*/
void EdgeItem::setSpecial(bool flag )
{
	specialBlocked = flag;
	special = flag;
}