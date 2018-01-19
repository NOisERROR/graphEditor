#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include <QtGui/QMainWindow>
#include "ui_grapheditor.h"
#include "graphscene.h"
class GraphEditor : public QMainWindow
{
	Q_OBJECT

public:
	GraphEditor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GraphEditor();
	GraphScene scene;
	void printInfo(const QString &text);	//Вывод сообщений
public slots:
	//Очистка окна свойств
	void clearView();	
	//Установка режима добавления вершин
	void setVertMode();
	//Устанвока режима добавления дуг
	void setEdgeMode();
	//Установка режима перетаскивания дуг
	void setMoveMode();
	//Установка режима удаления
	void setDelMode();
	//Установка режима поиска пути между парой вершин
	void setOneShortWayMode();
	//Установка режима поиска всех кратчайших путей из вершины
	void setShortWaysMode();
	//Показ информации о вершине
	void viewVertexInfo(const QString &name);
	//Пока информации о дуге
	void viewEdgeInfo(const QString &name);
	//Установка параметров вершин и дуг
	void setGraphElementParametrs(int row, int column);
	//Поиск кратчайших путей
	void findShortWays(const QString &vertexName);
	//Топологическая сортировка
	void topologicalSort();
	//ление графа
	void zoom(int val);
private:
	//Текущий выбранный элемент
	enum Elements{Vertex, Edge};
	//Режимы
	enum Mode{Basic, OneShortWay, ShortWays};
	//Имя текущего элемента
	QString elementName;
	//Имя начальной вершины
	QString sourceName;
	//Имя конечной вершины
	QString destName;
	//Текущий элемент
	int curElement;
	//Режим
	int mode;
	Ui::graphEditorClass ui;
	

};

#endif // GRAPHEDITOR_H
