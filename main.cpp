#include "assembler.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	assembler w;
	w.show();
	return a.exec();
}
