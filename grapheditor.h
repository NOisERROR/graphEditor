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
	void printInfo(const QString &text);	//����� ���������
public slots:
	//������� ���� �������
	void clearView();	
	//��������� ������ ���������� ������
	void setVertMode();
	//��������� ������ ���������� ���
	void setEdgeMode();
	//��������� ������ �������������� ���
	void setMoveMode();
	//��������� ������ ��������
	void setDelMode();
	//��������� ������ ������ ���� ����� ����� ������
	void setOneShortWayMode();
	//��������� ������ ������ ���� ���������� ����� �� �������
	void setShortWaysMode();
	//����� ���������� � �������
	void viewVertexInfo(const QString &name);
	//���� ���������� � ����
	void viewEdgeInfo(const QString &name);
	//��������� ���������� ������ � ���
	void setGraphElementParametrs(int row, int column);
	//����� ���������� �����
	void findShortWays(const QString &vertexName);
	//�������������� ����������
	void topologicalSort();
	//����� �����
	void zoom(int val);
private:
	//������� ��������� �������
	enum Elements{Vertex, Edge};
	//������
	enum Mode{Basic, OneShortWay, ShortWays};
	//��� �������� ��������
	QString elementName;
	//��� ��������� �������
	QString sourceName;
	//��� �������� �������
	QString destName;
	//������� �������
	int curElement;
	//�����
	int mode;
	Ui::graphEditorClass ui;
	

};

#endif // GRAPHEDITOR_H
