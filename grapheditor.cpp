#include "grapheditor.h"

GraphEditor::GraphEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	//Установка параметров графического виджета
	ui.grView->setCacheMode(QGraphicsView::CacheBackground);
	ui.grView->setScene(&scene);
	ui.grView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	//Установка базового режима
	mode = Basic;
	//Соединение сигналов и слотов
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

/*!Установка режима добавления вершин
*/
void GraphEditor::setVertMode()
{
	scene.setMode(GraphScene::AddVertexMode);
	mode = Basic;
	printInfo("Установлен режим добавления вершин");
}

/*!Установка режима добавления дуг
*/
void GraphEditor::setEdgeMode()
{
	scene.setMode(GraphScene::AddEdgeMode);
	mode = Basic;
	printInfo("Установлен режим добавления дуг");
}

/*!Установка режима перемещения вершин
*/
void GraphEditor::setMoveMode()
{	
	scene.setMode(GraphScene::MovingMode);
	printInfo("Установлен режим перемещения вершин");
	mode = Basic;
}

/*!Установка режима удаления
*/
void GraphEditor::setDelMode()
{
	scene.setMode(GraphScene::DeleteMode);
	mode = Basic;
	printInfo("Установлен режим удаления элементов");
}

/*!Функция устанавливает режим поиска кратчайшего пути
* между парой вершин
*/
void GraphEditor::setOneShortWayMode()
{
	scene.setMode(GraphScene::MovingMode);
	mode = OneShortWay;
	sourceName = "";
	destName = "";
	printInfo("Поиск кратчайшего пути между парой вершин.");
	printInfo("Выберите начальную и конечную вершины пути...");
}

/*!Установка режима поиска кратчайших путей
*/
void GraphEditor::setShortWaysMode()
{
	scene.setMode(GraphScene::ShowingShortWays);
	mode = ShortWays;
	sourceName = "";
	printInfo("Поиск кратчайшего пути от начальной вершины до остальных.");
	printInfo("Выберите начальную вершину пути...");
}
/*!Слот выводит информацию о вершине
* \param name - имя вершины
*/
void GraphEditor::viewVertexInfo(const QString &name)
{
	VertexInfo v = scene.getVertexInfo(name);
	elementName = v.name;
	clearView();
	curElement = Vertex;
	ui.infoView->setRowCount(3);
	QTableWidgetItem *newItem = new QTableWidgetItem("Имя");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(0, 0, newItem);
	newItem = new QTableWidgetItem("Тип");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 0, newItem);
	newItem = new QTableWidgetItem("Удалённость");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 0, newItem);
	newItem = new QTableWidgetItem(v.name);
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(0, 1, newItem);
	newItem = new QTableWidgetItem("Вершина");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 1, newItem);
	newItem = new QTableWidgetItem(QString::number(v.dist));
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 1, newItem);
}

/*!Слот выводит информацию о дуге
* \param name - имя дуги
*/
void GraphEditor::viewEdgeInfo(const QString &name)
{
	EdgeInfo v = scene.getEdgeInfo(name);
	elementName = v.name;
	clearView();
	curElement = Edge;
	ui.infoView->setRowCount(3);
	QTableWidgetItem *newItem = new QTableWidgetItem("Имя");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(0, 0, newItem);
	newItem = new QTableWidgetItem("Тип");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 0, newItem);
	newItem = new QTableWidgetItem("Вес");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(2, 0, newItem);
	newItem = new QTableWidgetItem(v.name);
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(0, 1, newItem);
	newItem = new QTableWidgetItem("Дуга");
	newItem->setFlags(Qt::ItemIsEnabled);
	ui.infoView->setItem(1, 1, newItem);
	newItem = new QTableWidgetItem(QString::number(v.weight));
	newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui.infoView->setItem(2, 1, newItem);
}

/*!Слот очищает окно свойств
*/
void GraphEditor::clearView()
{
	for( int i = 0; i < ui.infoView->rowCount(); i++ )
		for( int j = 0; j < ui.infoView->columnCount(); j++ )
			delete ui.infoView->item(i, j);
	ui.infoView->clearContents();
	ui.infoView->setRowCount(0);
}

/*!Слот передаёт данные из окна свойств в класс сцены
* \param row - строка окна свойств
* \param column - колонка окна свойств
*/
void GraphEditor::setGraphElementParametrs(int row, int column)
{
	if( curElement == Vertex && column == 1 )
	{//Если текущий элемент вершина
		if( row == 0 )
		{
			scene.setVertexName(elementName, ui.infoView->item(row, column)->text());
			elementName =  ui.infoView->item(row, column)->text();
		}
	}
	else if( curElement == Edge  && column == 1)
	{//Если дуга
		if( row == 0 )
		{//Если редактируется имя
			scene.setEdgeName(elementName, ui.infoView->item(row, column)->text());
			ui.infoView->item(row, column)->text();
		}
		else if(  row == 2 )
		//Если редактируется вес
			scene.setEdgeWeight(elementName,  ui.infoView->item(row, column)->text().toDouble());
	}
}

/*!Слот осуществляет поиск кратчайших путей
*/
void GraphEditor::findShortWays(const QString &vertexName)
{
	if( mode == OneShortWay )
	{//Поиск между парой вершин
		if( sourceName.isEmpty() )
		{//Если источнк ещё не выбран
			sourceName = vertexName;
			printInfo("Выбрана вершина " + vertexName + " как начальная.");
			return;
		}
		if( destName.isEmpty() )
		{//Если выбрана вторая вершина
			QVector<QString> path;
			double dist;
			destName = vertexName;
			printInfo("Выбрана вершина " + vertexName + " как конечная.");
			dist = scene.showShortWay(sourceName, destName, &path);
			viewVertexInfo(destName);
			printInfo("Длина пути: " + QString::number(dist));
			
			if( path.count() == 0 )
			{//Если путь отсутствует
				printInfo("Такого пути не существует.");
				return;
			}
			printInfo("Путь имеет вид: ");

			for( int i = 0; i < path.count(); i++ )
			{//Выводим путь
				printInfo( ( i % 2 == 0 ? "Вершина: " : "Дуга: ") + path[i]);
			}
			sourceName.clear();
			destName.clear();
		}
	}
	else if( mode == ShortWays && sourceName.isEmpty() )
	{//Поиск между всеми вершинами
		sourceName = vertexName;
		printInfo("Выбрана вершина " + vertexName + " как начальная.");
		scene.shortWays(sourceName);
	}
}

/*!Функция выводит на в окно текст сообщения
* \param text - текст
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

/*!Топологическая сортировка
*/
void GraphEditor::topologicalSort()
{
	scene.topologicalSorting();
}

/*!Функция уменьшает и увеличивает размер графа
*/
void GraphEditor::zoom(int val)
{
	QMatrix oldMatrix = ui.grView->matrix();
	ui.grView->resetMatrix();
	ui.grView->scale(1 - val * 0.2, 1 - val * 0.2);
	ui.grView->translate(oldMatrix.dx(), oldMatrix.dy());
}