#include "grapheditor.h"

GraphEditor::GraphEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	//��������� ���������� ������������ �������
	ui.grView->setCacheMode(QGraphicsView::CacheBackground);
	ui.grView->setScene(&scene);
	ui.grView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	//��������� �������� ������
	mode = Basic;
	//���������� �������� � ������
	connect(ui.vertButton, SIGNAL(clicked()), this, SLOT(setVertMode()));
	connect(ui.edgeButton, SIGNAL(clicked()), this, SLOT(setEdgeMode()));
	connect(ui.moveButton, SIGNAL(clicked()), this, SLOT(setMoveMode()));
	connect(ui.delButton, SIGNAL(clicked()), this, SLOT(setDelMode()));
	connect(&scene, SIGNAL(vertexSelected(const QString &)), 
				this, SLOT(viewVertexInfo(const QString &)));
	connect(&scene, SIGNAL(edgeSelected(const QString &)), 
				this, SLOT(viewEdgeInfo(const QString &)));
	connect(ui.infoView, SIGNAL(cellChanged(int, int)), 
				this, SLOT(setGraphElementParametrs(int, int)));
	connect(&scene, SIGNAL(nothingSelected()), this, SLOT(clearView()));
	connect(ui.shWay1, SIGNAL(triggered()), 
				this, SLOT(setShortWaysMode()));
	connect(ui.shWay2, SIGNAL(triggered()), 
				this, SLOT(setOneShortWayMode()));
	connect(&scene, SIGNAL(vertexSelected(const QString &)), 
				this, SLOT(findShortWays(const QString &)));
	connect(ui.topSort, SIGNAL(triggered()), this, SLOT(topologicalSort()));
	connect(ui.zoom, SIGNAL(valueChanged(int)), this, SLOT(zoom(int)));
	ui.instPanel->setEnabled(true);
}

GraphEditor::~GraphEditor()
{

}

/*!��������� ������ ���������� ������
*/
void GraphEditor::setVertMode()
{
	scene.setMode(GraphScene::AddVertexMode);
	mode = Basic;
	printInfo("���������� ����� ���������� ������");
}

/*!��������� ������ ���������� ���
*/
void GraphEditor::setEdgeMode()
{
	scene.setMode(GraphScene::AddEdgeMode);
	mode = Basic;
	printInfo("���������� ����� ���������� ���");
}

/*!��������� ������ ����������� ������
*/
void GraphEditor::setMoveMode()
{	
	scene.setMode(GraphScene::MovingMode);
	printInfo("���������� ����� ����������� ������");
	mode = Basic;
}

/*!��������� ������ ��������
*/
void GraphEditor::setDelMode()
{
	scene.setMode(GraphScene::DeleteMode);
	mode = Basic;
	printInfo("���������� ����� �������� ���������");
}

/*!������� ������������� ����� ������ ����������� ����
* ����� ����� ������
*/
void GraphEditor::setOneShortWayMode()
{
	scene.setMode(GraphScene::MovingMode);
	mode = OneShortWay;
	sourceName = "";
	destName = "";
	printInfo("����� ����������� ���� ����� ����� ������.");
	printInfo("�������� ��������� � �������� ������� ����...");
}

/*!��������� ������ ������ ���������� �����
*/
void GraphEditor::setShortWaysMode()
{
	scene.setMode(GraphScene::ShowingShortWays);
	mode = ShortWays;
	sourceName = "";
	printInfo("����� ����������� ���� �� ��������� ������� �� ���������.");
	printInfo("�������� ��������� ������� ����...");
}
/*!���� ������� ���������� � �������
* \param name - ��� �������
*/
void GraphEditor::viewVertexInfo(const QString &name)
{
	VertexInfo v = scene.getVertexInfo(name);
	elementName = v.name;
	clearView();
	curElement = Vertex;
	ui.infoView->setRowCount(3);
	QTableWidgetItem *newItem = new QTableWidgetItem("���");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(0, 0, newItem);
	newItem = new QTableWidgetItem("���");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 0, newItem);
	newItem = new QTableWidgetItem("����������");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 0, newItem);
	newItem = new QTableWidgetItem(v.name);
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(0, 1, newItem);
	newItem = new QTableWidgetItem("�������");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 1, newItem);
	newItem = new QTableWidgetItem(QString::number(v.dist));
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 1, newItem);
}

/*!���� ������� ���������� � ����
* \param name - ��� ����
*/
void GraphEditor::viewEdgeInfo(const QString &name)
{
	EdgeInfo v = scene.getEdgeInfo(name);
	elementName = v.name;
	clearView();
	curElement = Edge;
	ui.infoView->setRowCount(3);
	QTableWidgetItem *newItem = new QTableWidgetItem("���");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(0, 0, newItem);
	newItem = new QTableWidgetItem("���");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 0, newItem);
	newItem = new QTableWidgetItem("���");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 0, newItem);
	newItem = new QTableWidgetItem(v.name);
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(0, 1, newItem);
	newItem = new QTableWidgetItem("����");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 1, newItem);
	newItem = new QTableWidgetItem(QString::number(v.weight));
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(2, 1, newItem);
}

/*!���� ������� ���� �������
*/
void GraphEditor::clearView()
{
	for( int i = 0; i < ui.infoView->rowCount(); i++ )
		for( int j = 0; j < ui.infoView->columnCount(); j++ )
			delete ui.infoView->item(i, j);
	ui.infoView->clearContents();
	ui.infoView->setRowCount(0);
}

/*!���� ������� ������ �� ���� ������� � ����� �����
* \param row - ������ ���� �������
* \param column - ������� ���� �������
*/
void GraphEditor::setGraphElementParametrs(int row, int column)
{
	if( curElement == Vertex && column == 1 )
	{//���� ������� ������� �������
		if( row == 0 )
		{
			scene.setVertexName(elementName, ui.infoView->item(row, column)->text());
			elementName =  ui.infoView->item(row, column)->text();
		}
	}
	else if( curElement == Edge  && column == 1)
	{//���� ����
		if( row == 0 )
		{//���� ������������� ���
			scene.setEdgeName(elementName, ui.infoView->item(row, column)->text());
			ui.infoView->item(row, column)->text();
		}
		else if(  row == 2 )
		//���� ������������� ���
			scene.setEdgeWeight(elementName,  ui.infoView->item(row, column)->text().toDouble());
	}
}

/*!���� ������������ ����� ���������� �����
*/
void GraphEditor::findShortWays(const QString &vertexName)
{
	if( mode == OneShortWay )
	{//����� ����� ����� ������
		if( sourceName.isEmpty() )
		{//���� ������� ��� �� ������
			sourceName = vertexName;
			printInfo("������� ������� " + vertexName + " ��� ���������.");
			return;
		}
		if( destName.isEmpty() )
		{//���� ������� ������ �������
			QVector<QString> path;
			double dist;
			destName = vertexName;
			printInfo("������� ������� " + vertexName + " ��� ��������.");
			dist = scene.showShortWay(sourceName, destName, &path);
			viewVertexInfo(destName);
			printInfo("����� ����: " + QString::number(dist));
			
			if( path.count() == 0 )
			{//���� ���� �����������
				printInfo("������ ���� �� ����������.");
				return;
			}
			printInfo("���� ����� ���: ");

			for( int i = 0; i < path.count(); i++ )
			{//������� ����
				printInfo( ( i % 2 == 0 ? "�������: " : "����: ") + path[i]);
			}
			sourceName.clear();
			destName.clear();
		}
	}
	else if( mode == ShortWays && sourceName.isEmpty() )
	{//����� ����� ����� ���������
		sourceName = vertexName;
		printInfo("������� ������� " + vertexName + " ��� ���������.");
		scene.shortWays(sourceName);
	}
}

/*!������� ������� �� � ���� ����� ���������
* \param text - �����
*/
void GraphEditor::printInfo(const QString &text)
{
	QString lastText = ui.output->toPlainText();
	QString htmlText = ui.output->toHtml();

	if( QFontMetrics(ui.output->font()).width(lastText) / ui.output->width() > 4 )
		htmlText.clear();
	htmlText += text;
	ui.output->setHtml(htmlText + "<br>");
}

/*!�������������� ����������
*/
void GraphEditor::topologicalSort()
{
	scene.topologicalSorting();
}

/*!������� ��������� � ����������� ������ �����
*/
void GraphEditor::zoom(int val)
{
	QMatrix oldMatrix = ui.grView->matrix();
	ui.grView->resetMatrix();
	ui.grView->scale(1 - val * 0.2, 1 - val * 0.2);
	ui.grView->translate(oldMatrix.dx(), oldMatrix.dy());
}