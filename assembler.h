#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QtWidgets/QMainWindow>
#include "ui_assembler.h"
#include "ui_setRegDialog.h"
#include "ui_ConsoleDialog.h"
#include "convertobinary.h"
#include "binarytoasm.h"
#include "Simulator.h"
class assembler : public QMainWindow
{
	Q_OBJECT

public:
	assembler(QWidget *parent = 0);
	//~assembler();

private:
	Ui::assemblerClass ui;
    Ui::setRegDialog regui;
    Ui::ConsoleDialog console;
	ConverToBinary *newtext;
	BinaryToAsm *newbinary;
	Simulator *sim;
	QAction *asmfile;
	QAction *binaryfile;
    QAction *setreg;
    QDialog *setregdialog;
    QDialog *newconsole;
	QMenu *menu;
    QMenu *simulator;
	void CreateAction();
	void CreateMenu();
private slots:
	void AssembleButtonPushed();
	void ReAssembleButtonPushed();
	void addfromfile();
	void addbinaryfromfile();
	void AddToSim();
	void RunButtonPushed();
	void SingleButtonPushed();
	void InitialButtonPushed();
    void setregister();
    void oktosetreg();
	friend ostream & operator<<(ostream &os, ConverToBinary &c);
};

#endif // ASSEMBLER_H
