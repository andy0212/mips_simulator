#include "assembler.h"
#include<QFileDialog>
#include<QMessageBox>
#include<sstream>
assembler::assembler(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	newtext = nullptr;
	newbinary = nullptr;
    newconsole = nullptr;
	sim = new Simulator;

    ui.SimtextEdit->clear();
    ui.SimtextEdit->insertPlainText(QString::fromStdString(sim->outputSim()));
    ui.RegtextEdit->clear();
    ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    ui.datatextEdit->clear();
    ui.datatextEdit->insertPlainText(QString::fromStdString(sim->outputdata()));

	connect(ui.AssemblepushButton, SIGNAL(clicked()), this, SLOT(AssembleButtonPushed()));
	connect(ui.ReassemblepushButton, SIGNAL(clicked()), this, SLOT(ReAssembleButtonPushed()));
	connect(ui.AddToSimpushButton, SIGNAL(clicked()), this, SLOT(AddToSim()));
	connect(ui.RunpushButton, SIGNAL(clicked()), this, SLOT(RunButtonPushed()));
	connect(ui.SinglepushButton, SIGNAL(clicked()), this, SLOT(SingleButtonPushed()));
	connect(ui.InitialpushButton, SIGNAL(clicked()), this, SLOT(InitialButtonPushed()));
	CreateAction();
	CreateMenu();
}

void assembler::CreateAction()
{
	asmfile = new QAction(tr("Load asm File"), this);
	asmfile->setIcon(QIcon(":/assembler/Resources/file.png"));
	asmfile->setStatusTip(tr("Load asm file from file"));
	connect(asmfile, SIGNAL(triggered()), this, SLOT(addfromfile()));

	binaryfile = new QAction(tr("Load Binary File"), this);
	binaryfile->setIcon(QIcon(""));
	binaryfile->setStatusTip(tr("Load binary file"));
	connect(binaryfile, SIGNAL(triggered()), this, SLOT(addbinaryfromfile()));

    setreg = new QAction(tr("Set Register"),this);
    setreg->setIcon(QIcon(""));
    setreg->setStatusTip(tr("Set the value of $t0 ~ $t7"));
    connect(setreg,SIGNAL(triggered()),this,SLOT(setregister()));
}

void assembler::CreateMenu()
{	
	menu = menuBar()->addMenu(tr("Menu"));
	menu->addAction(asmfile);
	menu->addAction(binaryfile);

    simulator = menuBar()->addMenu(tr("Simulator"));
    simulator->addAction(setreg);
}

void assembler::AssembleButtonPushed(){
	if (newtext){
		delete newtext;
		newtext = nullptr;
	}
	int PC = 0;
	newtext = new ConverToBinary(ui.assembletextEdit->toPlainText().toStdString(),PC);
	//newtext->setPC(40);
	newtext->ConvertToBinary();
	ui.BinarytextEdit->clear();
	ui.BinarytextEdit->insertPlainText(QString::fromStdString(newtext->output()));
}

void assembler::ReAssembleButtonPushed(){
	if (newbinary)
	{
		delete newbinary;
		newbinary = nullptr;
	}
	int PC = 0;
	newbinary = new BinaryToAsm(ui.BinarytextEdit->toPlainText().toStdString(), PC);
	newbinary->ConvertToAsm();
	ui.assembletextEdit->clear();
	ui.assembletextEdit->insertPlainText(QString::fromStdString(newbinary->output()));
}

void assembler::addfromfile()
{
	using namespace std;
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open ASM File"), ".", tr("Data files (*.asm)"));
	if (fileName == NULL)
		return;
	fstream fin;
	string fname = QString(fileName).toStdString();
    fin.open(fname.c_str());
	ui.assembletextEdit->clear();
	int PC = 0;
	if (fin.is_open())
	{
		string aline;
		while (!fin.eof())
		{
			getline(fin, aline);
			aline += '\n';
			ui.assembletextEdit->insertPlainText(QString::fromStdString(aline));
		}
		if (newtext){
			delete newtext;
			newtext = nullptr;
		}
		fin.seekg(0);
		newtext = new ConverToBinary(fin, PC);
		//newtext->setPC(40);
		newtext->ConvertToBinary();
		ui.BinarytextEdit->clear();
		ui.BinarytextEdit->insertPlainText(QString::fromStdString(newtext->output()));
	}
	else
		QMessageBox::warning(this, tr("WARNING"), tr("Can't Open this file"));
}

void assembler::addbinaryfromfile()
{
	using namespace std;
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Binary File"), ".", tr("Data files (*.*)"));
	if (fileName == NULL)
		return;
	fstream fin;
    //const char *fname = QString(fileName).toStdString();
    fin.open(QString(fileName).toStdString().c_str());
	if (!fin.is_open())
	{
		QMessageBox::warning(this, tr("Warning"), tr("Can't open this file"), QMessageBox::Ok);
		return;
	}
	string aline;
	ui.BinarytextEdit->clear();
	while (!fin.eof())
	{
		getline(fin, aline);
		aline += '\n';
		ui.BinarytextEdit->insertPlainText(QString::fromStdString(aline));
	}
	ReAssembleButtonPushed();
}

void assembler::AddToSim()
{
    string asmstring,binarystring;
    asmstring = ui.assembletextEdit->toPlainText().toStdString();
    binarystring = ui.BinarytextEdit->toPlainText().toStdString();
    /*
	vector<pair<unsigned long, string>> instrset;
	pair<unsigned long, string> insert;
	int i = 0;
    string line,firstword,secondword;
    istringstream read(asmstring);
    vector<string> dataset;
    while (getline(read,line))
    {
        istringstream readline(line);
        if(readline >> firstword)
            if(firstword[0] == ';'||firstword[0] == '.')
            {
                dataset.push_back(line);
                continue;
            }
            else if(readline>>secondword)
                if(secondword[0] == '.'){
                    dataset.push_back(line);
                    continue;
                }
        //readline.str("");
        insert.second = line;
        instrset.push_back(insert);
    }
	unsigned long aline = 0;
    i = 0;
    int j = 0;
	while (i<binarystring.size())
	{
        if (binarystring[i].toLatin1() == '\n'||i == binarystring.size() - 1)
		{
            if (binarystring[i].toLatin1() == '0')
                aline = (aline << 1);
            else if (binarystring[i].toLatin1() == '1')
                aline = (aline << 1) + 1;
            if(j<instrset.size())
                instrset[j++].first = aline;
			aline = 0;
		}
		else if (binarystring[i].toLatin1() == '0')
			aline = (aline << 1);
		else if (binarystring[i].toLatin1() == '1')
			aline = (aline << 1) + 1;
		i++;
    }*/
    //sim->ReceiveInstr(instrset);
    //sim->ReceiveData(dataset);
    sim->InsertToSim(asmstring,binarystring);
	ui.SimtextEdit->clear();
	ui.SimtextEdit->insertPlainText(QString::fromStdString(sim->outputSim()));
	ui.RegtextEdit->clear();
	ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    ui.datatextEdit->clear();
    ui.datatextEdit->insertPlainText(QString::fromStdString(sim->outputdata()));
}

void assembler::RunButtonPushed()
{
    if(newconsole != nullptr)
        delete newconsole;
    newconsole = new QDialog(this);
    console.setupUi(newconsole);
    newconsole->show();
    if(sim->Run() == false)
	{
		QMessageBox::warning(this, tr("Terminated by System"), tr("Error:system terminate run"), QMessageBox::Ok);
	}
	ui.SimtextEdit->clear();
	ui.SimtextEdit->insertPlainText(QString::fromStdString(sim->outputSim()));
	ui.RegtextEdit->clear();
	ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    ui.datatextEdit->clear();
    ui.datatextEdit->insertPlainText(QString::fromStdString(sim->outputdata()));
    console.textEdit->clear();
    console.textEdit->insertPlainText(QString::fromStdString(sim->inputconsole));
}

void assembler::SingleButtonPushed()
{
    if(newconsole == nullptr){
        newconsole = new QDialog(this);
        console.setupUi(newconsole);
        newconsole->show();
    }
    if(!newconsole->isVisible())
        newconsole->show();
    sim->SingleStep();
	ui.SimtextEdit->clear();
    //ui.SimtextEdit->insertPlainText(QString::fromStdString(sim->outputSim()));
    //ui.SimtextEdit->moveCursor(QTextCursor::NextRow);
    //string aline;
    vector<string> output;
    output = sim->outputonebyone();
    for(int i = 0;i<output.size();i++){
        ui.SimtextEdit->insertPlainText(QString::fromStdString(output[i]));
        if(sim->CurrentPC() == i*4)
            ui.SimtextEdit->moveCursor(QTextCursor::EndOfLine);
    }
	ui.RegtextEdit->clear();
	ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    //if(sim->consolechanged == true){
        console.textEdit->clear();
        console.textEdit->insertPlainText(QString::fromStdString(sim->inputconsole));
        //sim->consolechanged = false;
    //}
}

void assembler::InitialButtonPushed()
{
	sim->clear();
	ui.SimtextEdit->clear();
	ui.SimtextEdit->insertPlainText(QString::fromStdString(sim->outputSim()));
	ui.RegtextEdit->clear();
	ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    ui.datatextEdit->clear();
    ui.datatextEdit->insertPlainText(QString::fromStdString(sim->outputdata()));
}


void assembler::setregister(){
    setregdialog = new QDialog(this);
    regui.setupUi(setregdialog);
    regui.t0lineEdit->setText(QString::number(sim->reg[8]));
    regui.t1lineEdit->setText(QString::number(sim->reg[9]));
    regui.t2lineEdit->setText(QString::number(sim->reg[10]));
    regui.t3lineEdit->setText(QString::number(sim->reg[11]));
    regui.t4lineEdit->setText(QString::number(sim->reg[12]));
    regui.t5lineEdit->setText(QString::number(sim->reg[13]));
    regui.t6lineEdit->setText(QString::number(sim->reg[14]));
    regui.t7lineEdit->setText(QString::number(sim->reg[15]));
    connect(regui.OkpushButton,SIGNAL(clicked()),this,SLOT(oktosetreg()));
    connect(regui.CancelpushButton,SIGNAL(clicked()),setregdialog,SLOT(close()));
    setregdialog->show();
}

void assembler::oktosetreg(){
    sim->reg[8] = regui.t0lineEdit->text().toInt();
    sim->reg[9] = regui.t1lineEdit->text().toInt();
    sim->reg[10] = regui.t2lineEdit->text().toInt();
    sim->reg[11] = regui.t3lineEdit->text().toInt();
    sim->reg[12] = regui.t4lineEdit->text().toInt();
    sim->reg[13] = regui.t5lineEdit->text().toInt();
    sim->reg[14] = regui.t6lineEdit->text().toInt();
    sim->reg[15] = regui.t7lineEdit->text().toInt();
    ui.RegtextEdit->clear();
    ui.RegtextEdit->insertPlainText(QString::fromStdString(sim->outputReg()));
    setregdialog->close();
}
