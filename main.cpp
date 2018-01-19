#include "grapheditor.h"
#include <QTGui/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	GraphEditor w;
	w.show();
	return a.exec();
}
