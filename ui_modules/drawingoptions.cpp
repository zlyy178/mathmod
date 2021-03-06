/***************************************************************************
 *   Copyright (C) 2018 by Abderrahman Taha                                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "drawingoptions.h"
#include <qmessagebox.h>

static int indexcurrentFormula=-1;
static int indexcurrentSet=0;
static int CurrentFormulaType =-1; //0:Pariso; 1:Parametric; 2:Isosurface
QTreeWidgetItem *MyselectionItemReference;
QStringList  qlstPos, qlstStep, qlstmin, qlstmax, qlstnames;

// Error messages returned by ErrorMsg():
const char* ScriptErrorMessage[]=
{
    "SCRIPT_NO_ERROR",                       // 0
    "MAXGRID_OUT_OF_RANGE",           // 1
    "MAXPT_OUT_OF_RANGE",             // 2
    "MAXTRI_OUT_OF_RANGE",            // 3
    "FXYZ_OUT_OF_RANGE",              // 4
    "FX_OUT_OF_RANGE",
    "XMAX_NBCOMPONENT_MISMATCH",      // 5
    "YMAX_NBCOMPONENT_MISMATCH",      // 6
    "ZMAX_NBCOMPONENT_MISMATCH",      // 7
    "XMIN_NBCOMPONENT_MISMATCH",      // 8
    "YMIN_NBCOMPONENT_MISMATCH",      // 9
    "ZMIN_NBCOMPONENT_MISMATCH",      //10
    "FY_FX_MISMATCH",                 //11
    "FZ_FX_MISMATCH",                 //12
    "FW_FX_MISMATCH",                 //13
    "UMAX_NBCOMPONENT_MISMATCH",      //14
    "VMAX_NBCOMPONENT_MISMATCH",      //15
    "WMAX_NBCOMPONENT_MISMATCH",      //16
    "UMIN_NBCOMPONENT_MISMATCH",      //17
    "VMIN_NBCOMPONENT_MISMATCH",      //18
    "WMIN_NBCOMPONENT_MISMATCH",      //19
    "GRID_NBCOMPONENT_MISMATCH",      //20
    "COMPONENT_NBCOMPONENT_MISMATCH", //21
    "CND_NBCOMPONENT_MISMATCH",       //22
    "NBVARIABLES_OUT_OF_RANGE",       //23
    "NBCONSTANTES_OUT_OF_RANGE",      //24
    "NBDEFINEDFUNCTIONS_OUT_OF_RANGE",//25
    "NBTEXTURES_OUT_OF_RANGE",        //26
    "NBSLIDERS_OUT_OF_RANGE",         //27
    "NBSLIDERSVALUES_OUT_OF_RANGE",   //28
    "VERTEX_TAB_MEM_OVERFLOW",        //29
    "TRIANGLES_TAB_MEM_OVERFLOW",     //30
    "MINPOLY_TAB_MEM_OVERFLOW",       //31
    "POLY_TAB_MEM_OVERFLOW",          //32
    "CND_TAB_MEM_OVERFLOW",           //33
    "CND_POL_MEM_OVERFLOW",           //34
    "DEFINED_PARAM_GRID_VERTEX_TAB_OVERFLOW", //35
    "DEFINED_PARAM_GRID_TRIANGLE_TAB_OVERFLOW"  //36
};

// --------------------------
void DrawingOptions::ErrorMsg() const
{
    QMessageBox msgBox;
    if(scriptErrorType != SCRIPT_NO_ERROR)
    {
        statusBar()->showMessage(ScriptErrorMessage[scriptErrorType]);
        msgBox.setText(ScriptErrorMessage[scriptErrorType]);
        msgBox.exec();
    }
}
// --------------------------
void DrawingOptions::MemoryErrorMsg(int err) const
{
    QMessageBox msgBox;
    if(err != SCRIPT_NO_ERROR)
    {
        msgBox.setText(ScriptErrorMessage[err]);
        msgBox.exec();
    }
}
// --------------------------
void DrawingOptions::editorwin()
{
    editor *editorwindow = new editor();
    editorwindow->show();
}

// --------------------------
void DrawingOptions::colorsoptions()
{
    ColorsOptions *colorwindow = new ColorsOptions();
    connect( colorwindow->ui.color_2, SIGNAL(activated(int)), this, SLOT(oncolor_2activated(int)));
    connect( colorwindow->ui.red_2, SIGNAL(valueChanged(int)), this, SLOT(onred_2valueChanged(int)));
    connect( colorwindow->ui.green_2, SIGNAL(valueChanged(int)), this, SLOT(ongreen_2valueChanged(int)));
    connect( colorwindow->ui.blue_2, SIGNAL(valueChanged(int)), this, SLOT(onblue_2valueChanged(int)));
    connect( colorwindow->ui.transparent_2, SIGNAL(valueChanged(int)), this, SLOT(ontransparent_2valueChanged(int)));
    connect( colorwindow->ui.transparence_2, SIGNAL(clicked(bool)), this, SLOT(ontransparence_2clicked(bool)));

    connect( colorwindow->ui.color_3, SIGNAL(activated(int)), this, SLOT(oncolor_3activated(int)));
    connect( colorwindow->ui.red_3, SIGNAL(valueChanged(int)), this, SLOT(onred_3valueChanged(int)));
    connect( colorwindow->ui.green_3, SIGNAL(valueChanged(int)), this, SLOT(ongreen_3valueChanged(int)));
    connect( colorwindow->ui.blue_3, SIGNAL(valueChanged(int)), this, SLOT(onblue_3valueChanged(int)));
    connect( colorwindow->ui.transparent_3, SIGNAL(valueChanged(int)), this, SLOT(ontransparent_3valueChanged(int)));
    connect( colorwindow->ui.transparence_3, SIGNAL(clicked(bool)), this, SLOT(ontransparence_3toggled(bool)));
    colorwindow->show();
}

// --------------------------
void DrawingOptions::videoplay()
{
    QImage buf = MathmodRef->ui.glWidget->Copyscreenshot();
    screenimage *player = new screenimage(buf);
    player->connectcontrols();
    player->show();
}

// --------------------------
DrawingOptions::~DrawingOptions()
{
    delete[] SliderArray;
}

// --------------------------
DrawingOptions::DrawingOptions(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    indexcurrentFormula = -1;
    IsolistItemRef = 0;
    select.selectedoptions.showall = false;
    select.selectedoptions.sensitive = false;
    select.selectedoptions.AND = true;
    select.selectedoptions.parsefunctions = true;
    select.selectedoptions.parsenames = true;
    select.selectedoptions.parsecmpnames = true;
    connect( sliderconf.ui.SaveButton, SIGNAL(clicked()), this, SLOT(update_slider_param()));
    connect( sliderconf.ui.ParametersComboBox, SIGNAL(activated(int)), this, SLOT(update_infos_param(int)));
    connect( addnewparam.ui.SaveButton, SIGNAL(clicked()), this, SLOT(add_new_param()));
    connect( &select, SIGNAL(UpdateSignal()), this, SLOT(SearchListModels()));
    statusBar()->addPermanentWidget(ui.Progressbarwidget, 1);
    SaveSlidersRef();
}

// --------------------------
void DrawingOptions::SaveSlidersRef(int nb)
{
    SliderArray = new SliderStruct[nb];

    SliderArray[0].SliderScrollBar = ui.C1ScrollBar;
    SliderArray[0].SliderLabel = ui.C1label;
    SliderArray[0].SliderLabelMax = ui.C1labelMax;
    SliderArray[0].SliderLabelMin = ui.C1labelMin;
    SliderArray[0].SliderGroupeBox = ui.groupBox_9;

    SliderArray[1].SliderScrollBar = ui.C2ScrollBar;
    SliderArray[1].SliderLabel = ui.C2label;
    SliderArray[1].SliderLabelMax = ui.C2labelMax;
    SliderArray[1].SliderLabelMin = ui.C2labelMin;
    SliderArray[1].SliderGroupeBox = ui.groupBox_10;

    SliderArray[2].SliderScrollBar = ui.C3ScrollBar;
    SliderArray[2].SliderLabel = ui.C3label;
    SliderArray[2].SliderLabelMax = ui.C3labelMax;
    SliderArray[2].SliderLabelMin = ui.C3labelMin;
    SliderArray[2].SliderGroupeBox = ui.groupBox_11;

    SliderArray[3].SliderScrollBar = ui.C4ScrollBar;
    SliderArray[3].SliderLabel = ui.C4label;
    SliderArray[3].SliderLabelMax = ui.C4labelMax;
    SliderArray[3].SliderLabelMin = ui.C4labelMin;
    SliderArray[3].SliderGroupeBox = ui.groupBox_12;

    SliderArray[4].SliderScrollBar = ui.C5ScrollBar;
    SliderArray[4].SliderLabel = ui.C5label;
    SliderArray[4].SliderLabelMax = ui.C5labelMax;
    SliderArray[4].SliderLabelMin = ui.C5labelMin;
    SliderArray[4].SliderGroupeBox = ui.groupBox_13;

    SliderArray[5].SliderScrollBar = ui.C6ScrollBar;
    SliderArray[5].SliderLabel = ui.C6label;
    SliderArray[5].SliderLabelMax = ui.C6labelMax;
    SliderArray[5].SliderLabelMin = ui.C6labelMin;
    SliderArray[5].SliderGroupeBox = ui.groupBox_14;

    SliderArray[6].SliderScrollBar = ui.C7ScrollBar;
    SliderArray[6].SliderLabel = ui.C7label;
    SliderArray[6].SliderLabelMax = ui.C7labelMax;
    SliderArray[6].SliderLabelMin = ui.C7labelMin;
    SliderArray[6].SliderGroupeBox = ui.groupBox_15;

    SliderArray[7].SliderScrollBar = ui.C8ScrollBar;
    SliderArray[7].SliderLabel = ui.C8label;
    SliderArray[7].SliderLabelMax = ui.C8labelMax;
    SliderArray[7].SliderLabelMin = ui.C8labelMin;
    SliderArray[7].SliderGroupeBox = ui.groupBox_16;

    SliderArray[8].SliderScrollBar = ui.C9ScrollBar;
    SliderArray[8].SliderLabel = ui.C9label;
    SliderArray[8].SliderLabelMax = ui.C9labelMax;
    SliderArray[8].SliderLabelMin = ui.C9labelMin;
    SliderArray[8].SliderGroupeBox = ui.groupBox_17;

    SliderArray[9].SliderScrollBar = ui.C10ScrollBar;
    SliderArray[9].SliderLabel = ui.C10label;
    SliderArray[9].SliderLabelMax = ui.C10labelMax;
    SliderArray[9].SliderLabelMin = ui.C10labelMin;
    SliderArray[9].SliderGroupeBox = ui.groupBox_18;

    SliderArray[10].SliderScrollBar = ui.C11ScrollBar;
    SliderArray[10].SliderLabel = ui.C11label;
    SliderArray[10].SliderLabelMax = ui.C11labelMax;
    SliderArray[10].SliderLabelMin = ui.C11labelMin;
    SliderArray[10].SliderGroupeBox = ui.groupBox_19;

    SliderArray[11].SliderScrollBar = ui.C12ScrollBar;
    SliderArray[11].SliderLabel = ui.C12label;
    SliderArray[11].SliderLabelMax = ui.C12labelMax;
    SliderArray[11].SliderLabelMin = ui.C12labelMin;
    SliderArray[11].SliderGroupeBox = ui.groupBox_20;

    SliderArray[12].SliderScrollBar = ui.C13ScrollBar;
    SliderArray[12].SliderLabel = ui.C13label;
    SliderArray[12].SliderLabelMax = ui.C13labelMax;
    SliderArray[12].SliderLabelMin = ui.C13labelMin;
    SliderArray[12].SliderGroupeBox = ui.groupBox_21;

    SliderArray[13].SliderScrollBar = ui.C14ScrollBar;
    SliderArray[13].SliderLabel = ui.C14label;
    SliderArray[13].SliderLabelMax = ui.C14labelMax;
    SliderArray[13].SliderLabelMin = ui.C14labelMin;
    SliderArray[13].SliderGroupeBox = ui.groupBox_22;

    SliderArray[14].SliderScrollBar = ui.C15ScrollBar;
    SliderArray[14].SliderLabel = ui.C15label;
    SliderArray[14].SliderLabelMax = ui.C15labelMax;
    SliderArray[14].SliderLabelMin = ui.C15labelMin;
    SliderArray[14].SliderGroupeBox = ui.groupBox_23;

    SliderArray[15].SliderScrollBar = ui.C16ScrollBar;
    SliderArray[15].SliderLabel = ui.C16label;
    SliderArray[15].SliderLabelMax = ui.C16labelMax;
    SliderArray[15].SliderLabelMin = ui.C16labelMin;
    SliderArray[15].SliderGroupeBox = ui.groupBox_24;

    SliderArray[16].SliderScrollBar = ui.C17ScrollBar;
    SliderArray[16].SliderLabel = ui.C17label;
    SliderArray[16].SliderLabelMax = ui.C17labelMax;
    SliderArray[16].SliderLabelMin = ui.C17labelMin;
    SliderArray[16].SliderGroupeBox = ui.groupBox_25;

    SliderArray[17].SliderScrollBar = ui.C18ScrollBar;
    SliderArray[17].SliderLabel = ui.C18label;
    SliderArray[17].SliderLabelMax = ui.C18labelMax;
    SliderArray[17].SliderLabelMin = ui.C18labelMin;
    SliderArray[17].SliderGroupeBox = ui.groupBox_26;

    SliderArray[18].SliderScrollBar = ui.C19ScrollBar;
    SliderArray[18].SliderLabel = ui.C19label;
    SliderArray[18].SliderLabelMax = ui.C19labelMax;
    SliderArray[18].SliderLabelMin = ui.C19labelMin;
    SliderArray[18].SliderGroupeBox = ui.groupBox_27;

    SliderArray[19].SliderScrollBar = ui.C20ScrollBar;
    SliderArray[19].SliderLabel = ui.C20label;
    SliderArray[19].SliderLabelMax = ui.C20labelMax;
    SliderArray[19].SliderLabelMin = ui.C20labelMin;
    SliderArray[19].SliderGroupeBox = ui.groupBox_28;
}
// --------------------------
void DrawingOptions::on_xyzg_valueChanged(int value)
{
    ui.Isogroupbox->setTitle("Iso Grid ( "+QString::number(value)+" ) :");
    if(!MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->isRunning())
    {
        MathmodRef->xyzg_valueChanged(value);
    }
    else
    {
        ui.ChangeGrid->blockSignals(true);
        ui.ChangeGrid->setChecked(false);
        ui.ChangeGrid->blockSignals(false);
        MathmodRef->xyzactivated  = -1;
    }
}

// --------------------------
void DrawingOptions::on_ChangeGrid_clicked()
{
    MathmodRef->slot_checkBox73_clicked();
}

// --------------------------
void DrawingOptions::AddModel(QString model, int where)
{
    QString cmp;
    int position;

    // Parametric:
    QStringList Parlist = MathmodRef->RootObjet.ParObject();
    if((position=Parlist.indexOf(model)) != -1)
    {
        QTreeWidgetItem *ParlistItem;
        if(where ==0)
            ParlistItem = new QTreeWidgetItem(ui.ObjectClasse);
        else
            ParlistItem = new QTreeWidgetItem(ui.ObjectClasseCurrent);

        ParlistItem->setText(0, model);
        QColor greenColor = QColor(0, 255, 0, 50);
        ParlistItem->setBackgroundColor(0, greenColor);

        if((cmp = MathmodRef->RootObjet.ParamTable[position].Component) != "")
        {
            QStringList listcmp = cmp.split(";", QString::SkipEmptyParts);
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(ParlistItem);
            cmpitem->setText(0,  "Components:");

            QString fx        = MathmodRef->RootObjet.ParamTable[position].fx;
            QString fy        = MathmodRef->RootObjet.ParamTable[position].fy;
            QString fz        = MathmodRef->RootObjet.ParamTable[position].fz;
            QString umin  = MathmodRef->RootObjet.ParamTable[position].umin;
            QString umax = MathmodRef->RootObjet.ParamTable[position].umax;
            QString vmin  = MathmodRef->RootObjet.ParamTable[position].vmin;
            QString vmax = MathmodRef->RootObjet.ParamTable[position].vmax;

            QStringList listformulax        = fx.split(";", QString::SkipEmptyParts);
            QStringList listformulay        = fy.split(";", QString::SkipEmptyParts);
            QStringList listformulaz        = fz.split(";", QString::SkipEmptyParts);
            QStringList listumin              = umin.split(";", QString::SkipEmptyParts);
            QStringList listumax             = umax.split(";", QString::SkipEmptyParts);
            QStringList listvmin              = vmin.split(";", QString::SkipEmptyParts);
            QStringList listvmax             = vmax.split(";", QString::SkipEmptyParts);

            //Grid resolution:
            QString grid = MathmodRef->RootObjet.ParamTable[position].Grid;
            QStringList listgrid               = grid.split(";", QString::SkipEmptyParts);

            for(int j=0; j<listcmp.count(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  listcmp.at(j));

                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "X(u,v) = "+listformulax.at(j));

                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "Y(u,v) = "+listformulay.at(j));

                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "Z(u,v) = "+listformulaz.at(j));

                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "U = ["+ listumin.at(j) +", "+ listumax.at(j) +"]");

                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "V = ["+ listvmin.at(j) +", "+ listvmax.at(j) +"]");

                //Grid resolution:
                if((cmp = MathmodRef->RootObjet.ParamTable[position].Grid) != "")
                {
                    QTreeWidgetItem *cmpitem8 = new QTreeWidgetItem(cmpitem2);
                    cmpitem8->setText(0,  "Grid = "+listgrid.at(j));
                }
            }


        }

        //Add Global parameters:
        if((cmp = MathmodRef->RootObjet.ParamTable[position].Varu)   != ""      ||
                (cmp = MathmodRef->RootObjet.ParamTable[position].Const) != "" ||
                (cmp = MathmodRef->RootObjet.ParamTable[position].Funct) != "")
        {
            QTreeWidgetItem *parameteritem = new QTreeWidgetItem(ParlistItem);
            parameteritem->setText(0,  "Parameters:");

            if((cmp = MathmodRef->RootObjet.ParamTable[position].Varu) != "")
            {
                QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                varitem->setText(0,  "Variables:");
                QStringList listvar = cmp.split(";", QString::SkipEmptyParts);
                for(int j=0; j<listvar.count(); j++)
                {
                    QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                    varitem2->setText(0,  listvar.at(j));
                }
            }
            if((cmp = MathmodRef->RootObjet.ParamTable[position].Const) != "")
            {
                QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                cstitem->setText(0,  "Constantes:");
                QStringList listcst = cmp.split(";", QString::SkipEmptyParts);
                for(int j=0; j<listcst.count(); j++)
                {
                    QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                    cstitem2->setText(0,  listcst.at(j));
                }
            }
            if((cmp = MathmodRef->RootObjet.ParamTable[position].Funct) != "")
            {
                QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                fctitem->setText(0,  "Functions:");
                QStringList listfct = cmp.split(";", QString::SkipEmptyParts);
                for(int j=0; j<listfct.count(); j++)
                {
                    QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                    fctitem2->setText(0,  listfct.at(j));
                }
            }
            //Grid resolution:
            if((cmp = MathmodRef->RootObjet.ParamTable[position].Grid) != "")
            {
                QTreeWidgetItem *resitem = new QTreeWidgetItem(parameteritem);
                resitem->setText(0,  "Grid:");
                QStringList listres = cmp.split(";", QString::SkipEmptyParts);
                for(int j=0; j<listres.count(); j++)
                {
                    QTreeWidgetItem *resitem2 = new QTreeWidgetItem(resitem);
                    resitem2->setText(0,  listres.at(j));
                }
            }
            // Colors:
            if((cmp = MathmodRef->RootObjet.ParamTable[position].RGBT) != "")
            {
                QTreeWidgetItem *rgbtitem = new QTreeWidgetItem(parameteritem);
                rgbtitem->setText(0,  "Colors:");
                QStringList listrgbt = cmp.split(";", QString::SkipEmptyParts);
                for(int j=0; j<listrgbt.count(); j++)
                {
                    QTreeWidgetItem *rgbtitem2 = new QTreeWidgetItem(rgbtitem);
                    rgbtitem2->setText(0,  listrgbt.at(j));
                }
            }
        }
    }
    else
    {
        // Isosurfaces:
        QStringList Isolist = MathmodRef->RootObjet.IsoObject();
        if((position=Isolist.indexOf(model)) != -1)
        {
            QTreeWidgetItem *IsolistItem;
            if(where ==0)
                IsolistItem = new QTreeWidgetItem(ui.ObjectClasse);
            else
                IsolistItem = new QTreeWidgetItem(ui.ObjectClasseCurrent);
            IsolistItem->setText(0, model);
            QColor greenColor = QColor(0, 255, 0, 50);
            IsolistItem->setBackgroundColor(0, greenColor);
            if((cmp = MathmodRef->RootObjet.IsoTable[position].Component) != "")
            {
                QTreeWidgetItem *cmpitem = new QTreeWidgetItem(IsolistItem);
                cmpitem->setText(0,  "Components:");
                QStringList listcmp = cmp.split(";", QString::SkipEmptyParts);
                QString fxyz     = MathmodRef->RootObjet.IsoTable[position].fxyz;
                QString Xmin  = MathmodRef->RootObjet.IsoTable[position].xmin;
                QString Xmax = MathmodRef->RootObjet.IsoTable[position].xmax;
                QString Ymin  = MathmodRef->RootObjet.IsoTable[position].ymin;
                QString Ymax = MathmodRef->RootObjet.IsoTable[position].ymax;
                QString Zmin  = MathmodRef->RootObjet.IsoTable[position].zmin;
                QString Zmax = MathmodRef->RootObjet.IsoTable[position].zmax;
                QStringList listformulas = fxyz.split(";", QString::SkipEmptyParts);
                QStringList listxmin        = Xmin.split(";", QString::SkipEmptyParts);
                QStringList listxmax       = Xmax.split(";", QString::SkipEmptyParts);
                QStringList listymin        = Ymin.split(";", QString::SkipEmptyParts);
                QStringList listymax       = Ymax.split(";", QString::SkipEmptyParts);
                QStringList listzmin        = Zmin.split(";", QString::SkipEmptyParts);
                QStringList listzmax       = Zmax.split(";", QString::SkipEmptyParts);


                QString grid = MathmodRef->RootObjet.IsoTable[position].Grid;
                QStringList listgrid               = grid.split(";", QString::SkipEmptyParts);

                for(int j=0; j<listcmp.count(); j++)
                {
                    QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                    cmpitem2->setText(0,  listcmp.at(j));
                    QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                    cmpitem3->setText(0,  "f(x,y,z) = "+listformulas.at(j));
                    QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                    cmpitem4->setText(0,  "X = ["+ listxmin.at(j) +", "+ listxmax.at(j) +"]");
                    QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                    cmpitem5->setText(0,  "Y = ["+ listymin.at(j) +", "+ listymax.at(j) +"]");
                    QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                    cmpitem6->setText(0,  "Z = ["+ listzmin.at(j) +", "+ listzmax.at(j) +"]");
                    //Grid resolution:
                    if((cmp = MathmodRef->RootObjet.IsoTable[position].Grid) != "")
                    {
                        QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                        cmpitem7->setText(0,  "Grid = "+listgrid.at(j));
                    }
                }
            }

            //Add Global parameters:
            if((cmp = MathmodRef->RootObjet.IsoTable[position].Varu)   != "" ||
                    (cmp = MathmodRef->RootObjet.IsoTable[position].Const) != "" ||
                    (cmp = MathmodRef->RootObjet.IsoTable[position].Funct)  != "")
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(IsolistItem);
                parameteritem->setText(0,  "Parameters:");
                if((cmp = MathmodRef->RootObjet.IsoTable[position].Varu) != "")
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    QStringList listvar = cmp.split(";", QString::SkipEmptyParts);
                    for(int j=0; j<listvar.count(); j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  listvar.at(j));
                    }
                }
                if((cmp = MathmodRef->RootObjet.IsoTable[position].Const) != "")
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    QStringList listcst = cmp.split(";", QString::SkipEmptyParts);
                    for(int j=0; j<listcst.count(); j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  listcst.at(j));
                    }
                }
                if((cmp = MathmodRef->RootObjet.IsoTable[position].Funct) != "")
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    QStringList listfct = cmp.split(";", QString::SkipEmptyParts);
                    for(int j=0; j<listfct.count(); j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  listfct.at(j));
                    }
                }

                //Grid resolution:
                if((cmp = MathmodRef->RootObjet.IsoTable[position].Grid) != "")
                {
                    QTreeWidgetItem *resitem = new QTreeWidgetItem(parameteritem);
                    resitem->setText(0,  "Grid:");
                    QStringList listres = cmp.split(";", QString::SkipEmptyParts);
                    for(int j=0; j<listres.count(); j++)
                    {
                        QTreeWidgetItem *resitem2 = new QTreeWidgetItem(resitem);
                        resitem2->setText(0,  listres.at(j));
                    }
                }
                // Colors:
                if((cmp = MathmodRef->RootObjet.IsoTable[position].RGBT) != "")
                {
                    QTreeWidgetItem *rgbtitem = new QTreeWidgetItem(parameteritem);
                    rgbtitem->setText(0,  "Colors:");
                    QStringList listrgbt = cmp.split(";", QString::SkipEmptyParts);
                    for(int j=0; j<listrgbt.count(); j++)
                    {
                        QTreeWidgetItem *rgbtitem2 = new QTreeWidgetItem(rgbtitem);
                        rgbtitem2->setText(0,  listrgbt.at(j));
                    }
                }
            }
        }
    }
    return;
}

// --------------------------
void DrawingOptions::AddObjectToMySelectionTree()
{
    if(MathmodRef->RootObjet.CurrentJsonObject["Iso3D"].isObject())
    {
        QTreeWidgetItem *IsolistItem = new QTreeWidgetItem(MyselectionItemReference);
        IsolistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        IsolistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(IsolistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "f(x,y,z) = "+MathmodRef->RootObjet.CurrentTreestruct.fxyz.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "X = ["+ MathmodRef->RootObjet.CurrentTreestruct.xmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.xmax.at(j) +"]");
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "Y = ["+ MathmodRef->RootObjet.CurrentTreestruct.ymin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.ymax.at(j) +"]");
                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "Z = ["+ MathmodRef->RootObjet.CurrentTreestruct.zmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.zmax.at(j) +"]");
                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0)
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(IsolistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }
    else    if(MathmodRef->RootObjet.CurrentJsonObject["Param3D"].isObject())
    {
        QTreeWidgetItem *paramlistItem = new QTreeWidgetItem(MyselectionItemReference);
        paramlistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        paramlistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(paramlistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "fx(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fx.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "fy(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fy.at(j));
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "fz(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fz.at(j));

                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "U = ["+ MathmodRef->RootObjet.CurrentTreestruct.umin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.umax.at(j) +"]");
                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "V = ["+ MathmodRef->RootObjet.CurrentTreestruct.vmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.vmax.at(j) +"]");

                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0)
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(paramlistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }
    else    if(MathmodRef->RootObjet.CurrentJsonObject["Param4D"].isObject())
    {
        QTreeWidgetItem *paramlistItem = new QTreeWidgetItem(MyselectionItemReference);
        paramlistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        paramlistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(paramlistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "fx(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fx.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "fy(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fy.at(j));
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "fz(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fz.at(j));
                QTreeWidgetItem *cmpitem8 = new QTreeWidgetItem(cmpitem2);
                cmpitem8->setText(0,  "fw(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fw.at(j));
                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "U = ["+ MathmodRef->RootObjet.CurrentTreestruct.umin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.umax.at(j) +"]");
                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "V = ["+ MathmodRef->RootObjet.CurrentTreestruct.vmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.vmax.at(j) +"]");

                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0)
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(paramlistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }
}

// --------------------------
void DrawingOptions::UpdateCurrentTreeObject()
{
    if(MathmodRef->RootObjet.CurrentJsonObject["Iso3D"].isObject())  //isoObject
    {
        // Update MathMod editor
        ui.ParamEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.text);

        QTreeWidgetItem *IsolistItem = new QTreeWidgetItem(ui.ObjectClasseCurrent);
        IsolistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        IsolistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(IsolistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "f(x,y,z) = "+MathmodRef->RootObjet.CurrentTreestruct.fxyz.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "X = ["+ MathmodRef->RootObjet.CurrentTreestruct.xmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.xmax.at(j) +"]");
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "Y = ["+ MathmodRef->RootObjet.CurrentTreestruct.ymin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.ymax.at(j) +"]");
                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "Z = ["+ MathmodRef->RootObjet.CurrentTreestruct.zmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.zmax.at(j) +"]");
                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0 && j < MathmodRef->RootObjet.CurrentTreestruct.Grid.size())
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(IsolistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }
    else    if(MathmodRef->RootObjet.CurrentJsonObject["Param3D"].isObject())  //ParamObject
    {
        // Update MathMod editor
        ui.ParamEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.text);

        QTreeWidgetItem *paramlistItem = new QTreeWidgetItem(ui.ObjectClasseCurrent);
        paramlistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        paramlistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(paramlistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "fx(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fx.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "fy(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fy.at(j));
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "fz(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fz.at(j));

                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "U = ["+ MathmodRef->RootObjet.CurrentTreestruct.umin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.umax.at(j) +"]");
                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "V = ["+ MathmodRef->RootObjet.CurrentTreestruct.vmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.vmax.at(j) +"]");

                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0 && j < MathmodRef->RootObjet.CurrentTreestruct.Grid.size())
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(paramlistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }


    else    if(MathmodRef->RootObjet.CurrentJsonObject["Param4D"].isObject())  //ParamObject
    {
        // Update MathMod editor
        ui.ParamEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.text);

        QTreeWidgetItem *paramlistItem = new QTreeWidgetItem(ui.ObjectClasseCurrent);
        paramlistItem->setText(0, MathmodRef->RootObjet.CurrentTreestruct.name[0]);
        QColor greenColor = QColor(0, 255, 0, 50);
        paramlistItem->setBackgroundColor(0, greenColor);

        if(MathmodRef->RootObjet.CurrentTreestruct.Component.size() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(paramlistItem);
            cmpitem->setText(0,  "Components:");

            for(int j=0; j<MathmodRef->RootObjet.CurrentTreestruct.Component.size(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Component.at(j));
                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "fx(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fx.at(j));
                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "fy(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fy.at(j));
                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "fz(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fz.at(j));
                QTreeWidgetItem *cmpitem8 = new QTreeWidgetItem(cmpitem2);
                cmpitem8->setText(0,  "fw(u,v,t) = "+MathmodRef->RootObjet.CurrentTreestruct.fw.at(j));
                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "X = ["+ MathmodRef->RootObjet.CurrentTreestruct.umin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.umax.at(j) +"]");
                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "Y = ["+ MathmodRef->RootObjet.CurrentTreestruct.vmin.at(j) +", "+ MathmodRef->RootObjet.CurrentTreestruct.vmax.at(j) +"]");

                //Grid resolution:
                if(MathmodRef->RootObjet.CurrentTreestruct.Grid.size() > 0 && j < MathmodRef->RootObjet.CurrentTreestruct.Grid.size())
                {
                    QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                    cmpitem7->setText(0,  "Grid = "+MathmodRef->RootObjet.CurrentTreestruct.Grid.at(j));
                }
            }

            //Add Global parameters:
            if(MathmodRef->RootObjet.CurrentTreestruct.Varu.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Const.size() > 0 ||
                    MathmodRef->RootObjet.CurrentTreestruct.Funct.size() > 0)
            {
                QTreeWidgetItem *parameteritem = new QTreeWidgetItem(paramlistItem);
                parameteritem->setText(0,  "Parameters:");
                int size= 0;
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Varu.size()) > 0)
                {
                    QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                    varitem->setText(0,  "Variables:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                        varitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Varu.at(j));
                    }
                }

                if((size = MathmodRef->RootObjet.CurrentTreestruct.Const.size()) > 0)
                {
                    QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                    cstitem->setText(0,  "Constantes:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                        cstitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Const.at(j));
                    }
                }
                if((size = MathmodRef->RootObjet.CurrentTreestruct.Funct.size()) > 0)
                {
                    QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                    fctitem->setText(0,  "Functions:");
                    for(int j=0; j<size; j++)
                    {
                        QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                        fctitem2->setText(0,  MathmodRef->RootObjet.CurrentTreestruct.Funct.at(j));
                    }
                }
            }
        }
    }

    if(MathmodRef->RootObjet.CurrentJsonObject["Iso3D"].isObject())
    {
        ui.stackedProperties->setCurrentIndex(1);
        ui.groupBox->setTitle(MathmodRef->RootObjet.CurrentTreestruct.name.at(0));
        ui.IsoComponent->clear();
        ui.IsoComponent->insertItems(0, MathmodRef->RootObjet.CurrentTreestruct.Component);
        UpdateDescription(0);
    }
    else if(MathmodRef->RootObjet.CurrentJsonObject["Param3D"].isObject())
    {
        ui.ParamComponent->clear();
        ui.groupBox->setTitle(MathmodRef->RootObjet.CurrentTreestruct.name.at(0));
        ui.ParamComponent->insertItems(0, MathmodRef->RootObjet.CurrentTreestruct.Component);
        ui.stackedProperties->setCurrentIndex(2);
        UpdateDescription(0);
    }
    else if(MathmodRef->RootObjet.CurrentJsonObject["Param4D"].isObject())
    {
        ui.ParamComponent_2->clear();
        ui.groupBox->setTitle(MathmodRef->RootObjet.CurrentTreestruct.name.at(0));
        ui.ParamComponent_2->insertItems(0, MathmodRef->RootObjet.CurrentTreestruct.Component);
        ui.stackedProperties->setCurrentIndex(3);
        UpdateDescription(0);
    }
    else
        ui.stackedProperties->setCurrentIndex(0);
}

// --------------------------
void DrawingOptions::HideSliders()
{
    ui.groupBox_9->hide();
    ui.groupBox_10->hide();
    ui.groupBox_11->hide();
    ui.groupBox_12->hide();
    ui.groupBox_13->hide();
    ui.groupBox_14->hide();
    ui.groupBox_15->hide();
    ui.groupBox_16->hide();
    ui.groupBox_17->hide();
    ui.groupBox_18->hide();
    ui.groupBox_19->hide();
    ui.groupBox_20->hide();
    ui.groupBox_21->hide();
    ui.groupBox_22->hide();
    ui.groupBox_23->hide();
    ui.groupBox_24->hide();
    ui.groupBox_25->hide();
    ui.groupBox_26->hide();
    ui.groupBox_27->hide();
    ui.groupBox_28->hide();
    ui.PredefinedSets->clear();
    ui.PredefinedSets->addItem("Predefined Sets");
    ui.ParametersList->clear();
    ui.ParametersList->addItem("Parameters List");
    sliderconf.ui.ParametersComboBox->clear();
    sliderconf.ui.ParametersComboBox->addItem("Parameters List");
}

// --------------------------
void DrawingOptions::ShowSliders(const QJsonObject & Jobj)
{
    QString result;
    QJsonArray lst;
    QJsonObject QObj;
    if(Jobj["Sliders"].isObject())
    {
        //Hide all sliders
        HideSliders();

        QObj = Jobj["Sliders"].toObject();

        // Min
        lst = QObj["Min"].toArray();
        result = "";
        for(int j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        qlstmin = result.split(";", QString::SkipEmptyParts);

        // Max
        lst = QObj["Max"].toArray();
        result = "";
        for(int j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        qlstmax = result.split(";", QString::SkipEmptyParts);

        // Position
        lst = QObj["Position"].toArray();
        result = "";
        for(int j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        qlstPos = result.split(";", QString::SkipEmptyParts);

        for (int i = 0; i < qlstPos.size(); ++i)
        {
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[i] = qlstPos.at(i).toDouble();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[i] = qlstPos.at(i).toDouble();
        }

        // Name
        lst = QObj["Name"].toArray();
        result = "";
        for(int j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        qlstnames = result.split(";", QString::SkipEmptyParts);

        for (int i = 0; i < qlstnames.size(); ++i)
        {
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderNames[i] = qlstnames.at(i).toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderNames[i] = qlstnames.at(i).toStdString();
        }
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Nb_Sliders = qlstnames.size();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Nb_Sliders = qlstnames.size();



        ui.ParametersList->clear();
        ui.ParametersList->addItem("Parameters List  ("+QString::number(qlstnames.size())+")");
        ui.ParametersList->addItems(qlstnames);

        sliderconf.ui.ParametersComboBox->clear();
        sliderconf.ui.ParametersComboBox->addItem("Parameters List  ("+QString::number(qlstnames.size())+")");
        sliderconf.ui.ParametersComboBox->addItems(qlstnames);

        // Step
        lst = QObj["Step"].toArray();
        result = "";
        for(int j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        qlstStep = result.split(";", QString::SkipEmptyParts);
        /*
         if(qlstPos.size() <= qlstnames.size())
         {
             //ui.PredefinedSet->hide();
          }
          */
        QStringList qlist;
        if(qlstPos.size() >= qlstnames.size())
        {
            ui.PredefinedSets->clear();
            int NbSets = (qlstPos.size()==0 || qlstnames.size()==0) ? 0 : qlstPos.size() / qlstnames.size();
            qlist += "Predefined Sets (" + QString::number(NbSets) + ")";
            if( QObj["SetNames"].isArray()   && (lst = QObj["SetNames"].toArray()).size())
            {
                result = "";
                for(int j=0; j < lst.size()-1; j++)
                    result += lst[j].toString() + ";";
                if(lst.size() >= 1)
                    result += lst[lst.size()-1].toString();
                result.replace("\n","");
                result.replace("\t","");
                result.replace(" ","");
                qlist += result.split(";", QString::SkipEmptyParts);
            }
            else
            {
                for(int i=1; i< NbSets+1; i++)
                {
                    qlist  += "Set_"+QString::number(i);
                }
            }
            ui.PredefinedSets->addItems(qlist);
        }

        for(int sl=0; sl<20; sl++)
        {
            if(qlstnames.size() >= (sl+1))
            {
                (SliderArray[sl].SliderScrollBar)->blockSignals(true);
                (SliderArray[sl].SliderScrollBar)->setMaximum(qlstmax.at(sl).toDouble());
                (SliderArray[sl].SliderScrollBar)->setMinimum(qlstmin.at(sl).toDouble());
                (SliderArray[sl].SliderScrollBar)->setSingleStep(qlstStep.at(sl).toDouble());
                (SliderArray[sl].SliderScrollBar)->setPageStep(qlstStep.at(sl).toDouble());
                (SliderArray[sl].SliderScrollBar)->setSliderPosition(qlstPos.at(sl).toDouble());
                (SliderArray[sl].SliderLabel)->setText(qlstnames.at(sl) + " = " +qlstPos.at(sl)+"("+ qlstStep.at(sl) +")");
                (SliderArray[sl].SliderLabelMin)->setText(qlstmin.at(sl));
                (SliderArray[sl].SliderLabelMax)->setText(qlstmax.at(sl));
                (SliderArray[sl].SliderScrollBar)->blockSignals(false);
                (SliderArray[sl].SliderGroupeBox)->show();
            }
        }
    }
    else
    {
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Nb_Sliders =  -1;
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Nb_Sliders =  -1;

        HideSliders();
    }
}

// --------------------------
void DrawingOptions::DrawJsonModel(const QJsonObject & Jobj, int textureIndex, bool Inspect)
{
    if(Inspect & !VerifiedJsonModel(Jobj, Inspect))
        return;
    ShowJsonModel(Jobj, textureIndex);
    ui.ObjectClasseCurrent->takeTopLevelItem(0);
    UpdateCurrentTreeObject();
}

// --------------------------
bool DrawingOptions::VerifiedJsonModel(const QJsonObject & Jobj, bool Inspect)
{
    QJsonArray lst;
    QJsonObject QObj;
    int NbFxyz, NbFx;

    if(!Inspect)
        return true;
    if(Jobj["Iso3D"].isObject())
    {
        QObj = Jobj["Iso3D"].toObject();
        // Fxyz
        NbFxyz = (QObj["Fxyz"].toArray()).size();
        if(NbFxyz > Parameters->NbComponent || NbFxyz == 0)
        {
            scriptErrorType = FXYZ_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
        if((QObj["Xmax"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = XMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Ymax"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = YMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Zmax"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = ZMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }

        if((QObj["Xmin"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = XMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Ymin"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = YMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Zmin"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = ZMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }

        if((QObj["Component"].toArray()).size() != NbFxyz)
        {
            scriptErrorType = COMPONENT_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if(((QObj["Grid"].toArray()).size() > 0) && ((QObj["Grid"].toArray()).size() != NbFxyz))
        {
            scriptErrorType = GRID_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if(((QObj["Cnd"].toArray()).size() > 0) && ((QObj["Cnd"].toArray()).size() != NbFxyz))
        {
            scriptErrorType = CND_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Funct"].toArray()).size() > Parameters->NbDefinedFunctions)
        {
            scriptErrorType = NBDEFINEDFUNCTIONS_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
        if((QObj["Const"].toArray()).size() > Parameters->NbConstantes)
        {
            scriptErrorType = NBCONSTANTES_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
        if((QObj["Varu"].toArray()).size() > Parameters->NbVariables)
        {
            scriptErrorType = NBVARIABLES_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }

        return true;
    }
    if(Jobj["Param3D"].isObject())
    {
        QObj = Jobj["Param3D"].toObject();
        // Fx
        NbFx = (QObj["Fx"].toArray()).size();
        if(NbFx > Parameters->NbComponent || NbFx == 0)
        {
            scriptErrorType = FX_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
        // Fy
        if((QObj["Fy"].toArray()).size() != NbFx)
        {
            scriptErrorType = FY_FX_MISMATCH;
            ErrorMsg();
            return false;
        }
        // Fz
        if((QObj["Fz"].toArray()).size() != NbFx)
        {
            scriptErrorType = FZ_FX_MISMATCH;
            ErrorMsg();
            return false;
        }
        if(((QObj["Fw"].toArray()).size()!=0) && ((QObj["Fw"].toArray()).size() != NbFx))
        {
            scriptErrorType = FW_FX_MISMATCH;
            ErrorMsg();
            return false;
        }

        if((QObj["Umax"].toArray()).size() != NbFx)
        {
            scriptErrorType = UMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Vmax"].toArray()).size() != NbFx)
        {
            scriptErrorType = VMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if(((QObj["Wmax"].toArray()).size() > 0) && ((QObj["Wmax"].toArray()).size() != NbFx))
        {
            scriptErrorType = WMAX_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }

        if((QObj["Umin"].toArray()).size() != NbFx)
        {
            scriptErrorType = UMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Vmin"].toArray()).size() != NbFx)
        {
            scriptErrorType = VMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if(((QObj["Wmin"].toArray()).size() > 0) && ((QObj["Wmin"].toArray()).size() != NbFx))
        {
            scriptErrorType = WMIN_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }

        if((QObj["Component"].toArray()).size() != NbFx)
        {
            scriptErrorType = COMPONENT_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }

        //Start Grid field processing
        QString result = "";
        if((lst = QObj["Grid"].toArray()).size() > 0 )
        {
            if(lst.size() != 2*NbFx)
            {
                scriptErrorType = GRID_NBCOMPONENT_MISMATCH;
                ErrorMsg();
                return false;
            }
            else
            {
                // Grid
                for(int j=0; j < lst.size()-1; j++)
                    result += lst[j].toString() + ";";
                if(lst.size() >= 1)
                    result += lst[lst.size()-1].toString();
                result.replace("\n","");
                result.replace("\t","");
                result.replace(" ","");
                if(result!="")
                {
                    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = true;
                    for(int j=0; j < lst.size(); j++)
                        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[j] = (lst[j].toString()).toInt();
                    int TotalPts = 0;
                    int TotalTri = 0;
                    for(int j=0; j < lst.size()/2; j++)
                    {
                        TotalPts += MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[2*j  ]*
                                    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[2*j+1];
                        TotalTri += (MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[2*j  ] - 1)*
                                    (MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[2*j+1] - 1);
                    }
                    if(TotalPts > NbMaxPts)
                    {
                        scriptErrorType = DEFINED_PARAM_GRID_VERTEX_TAB_OVERFLOW;
                        ErrorMsg();
                        return false;
                    }
                    if(TotalTri > NbMaxTri)
                    {
                        scriptErrorType = DEFINED_PARAM_GRID_TRIANGLE_TAB_OVERFLOW;
                        ErrorMsg();
                        return false;
                    }
                }
            }
        }
        else
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = false;

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Grid = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);
        //End Grid field processing

        if(((QObj["Cnd"].toArray()).size() > 0) && ((QObj["Cnd"].toArray()).size() != NbFx))
        {
            scriptErrorType = CND_NBCOMPONENT_MISMATCH;
            ErrorMsg();
            return false;
        }
        if((QObj["Funct"].toArray()).size() > Parameters->NbDefinedFunctions)
        {
            scriptErrorType = NBDEFINEDFUNCTIONS_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
        if((QObj["Const"].toArray()).size() > Parameters->NbConstantes)
        {
            scriptErrorType = NBCONSTANTES_OUT_OF_RANGE;
            ErrorMsg();
            return false;
        }
    }
    return true;
}

// --------------------------
void DrawingOptions::ShowJsonModel(const QJsonObject & Jobj, int textureIndex)
{
    int j;
    QString result;
    QJsonArray lst;
    QJsonObject QObj;
    QJsonObject QTextureObj, QPigmentObj;

    if(textureIndex != -1)
    {
        if(textureIndex < 1000)
            QTextureObj = MathmodRef->pariso.JTextures[textureIndex].toObject()["Texture"].toObject();
        else
            QPigmentObj = MathmodRef->pariso.JPigments[textureIndex-1000].toObject()["Pigment"].toObject();
    }
    else
    {
        if(Jobj["Texture"].isObject())
            QTextureObj = Jobj["Texture"].toObject();

        if(Jobj["Pigment"].isObject())
            QPigmentObj = Jobj["Pigment"].toObject();
    }

    ShowSliders(Jobj);

    if(Jobj["Iso3D"].isObject())
    {
        QObj = Jobj["Iso3D"].toObject();
        // Fxyz
        lst = QObj["Fxyz"].toArray();
        result = "";

        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ImplicitFunction = result.toStdString();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ImplicitFunctionSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fxyz = result.split(";", QString::SkipEmptyParts);
        // Condition:
        lst = QObj["Cnd"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Condition = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

        // Varu
        lst = QObj["Varu"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Varu = result.toStdString();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->VaruSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

        // Const
        lst = QObj["Const"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Const = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

        // Funct
        lst = QObj["Funct"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Funct = result.toStdString();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->FunctSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);

        //Noise:
        QString noise = "";
        noise = Jobj["Noise"].toString();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;

        // Colors
        QString noise1 = "";
        noise1 = QTextureObj["Noise"].toString();
        lst = QTextureObj["Colors"].toArray();
        noise1.replace("\n","");
        noise1.replace("\t","");
        noise1.replace(" ","");

        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Rgbt = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "")
        {
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise1.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
        }

        // Pigment
        QString noise2 = "";
        QJsonArray tmp;
        QString strtmp = QPigmentObj["Gradient"].toString();
        noise2 = QPigmentObj["Noise"].toString();
        lst =  QPigmentObj["Colors"].toArray();
        result = "";
        for(j=0; j < lst.size(); j++)
        {
            tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
            for(int k=0; k < tmp.count(); k++)
            {
                result += tmp[k].toString() + ";";
            }
        }

        strtmp.replace("\n","");
        strtmp.replace("\t","");
        strtmp.replace(" ","");

        noise2.replace("\n","");
        noise2.replace("\t","");
        noise2.replace(" ","");

        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Gradient = strtmp.toStdString();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->VRgbt = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "" && noise1 == "")
        {
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise2.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
        }
        // Grid
        lst = QObj["Grid"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Grid = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);


        // XlimitSup
        lst = QObj["Xmax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->XlimitSup = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.xmax = result.split(";", QString::SkipEmptyParts);

        // YlimitSup
        lst = QObj["Ymax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->YlimitSup = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.ymax = result.split(";", QString::SkipEmptyParts);

        // ZlimitSup
        lst = QObj["Zmax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ZlimitSup = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.zmax = result.split(";", QString::SkipEmptyParts);

        // XlimitInf
        lst = QObj["Xmin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->XlimitInf = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.xmin = result.split(";", QString::SkipEmptyParts);

        // YlimitInf
        lst = QObj["Ymin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->YlimitInf = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.ymin = result.split(";", QString::SkipEmptyParts);

        // ZlimitInf
        lst = QObj["Zmin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ZlimitInf = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.zmin = result.split(";", QString::SkipEmptyParts);

        // Component
        lst = QObj["Component"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

        // Name
        lst = QObj["Name"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);

        QJsonObject Jobjtmp = Jobj;
        //Some keys cleaning..
        Jobjtmp.remove("Param3D");
        Jobjtmp.remove("Param4D");

        QJsonDocument document;
        document.setObject(Jobjtmp);
        MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

        //Update the current parametric struct
        MathmodRef->RootObjet.CurrentJsonObject = Jobjtmp;
        CurrentFormulaType = 2;
        /// process the new surface
        if(textureIndex == -1)
        {
            if(MathmodRef->RootObjet.CurrentTreestruct.fxyz.count() > 0)
                MathmodRef->ProcessNewIsoSurface( );
        }
        else
        {
            int result = MathmodRef->ParseIso();
            if(result == -1) return;
            textureIndex < 1000 ? MathmodRef->ui.glWidget->CalculateTexturePoints(1) : MathmodRef->ui.glWidget->CalculatePigmentPoints(1);
            MathmodRef->ui.glWidget->update();
        }
    }
    else if(Jobj["Param3D"].isObject())
    {
        QObj = Jobj["Param3D"].toObject();
        // Fx
        lst = QObj["Fx"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_X = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_XSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fx = result.split(";", QString::SkipEmptyParts);

        // Fy
        lst = QObj["Fy"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Y = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_YSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fy = result.split(";", QString::SkipEmptyParts);

        // Fz
        lst = QObj["Fz"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Z = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_ZSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fz = result.split(";", QString::SkipEmptyParts);

        // inf_u
        lst = QObj["Umin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_u = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.umin = result.split(";", QString::SkipEmptyParts);

        // sup_u
        lst = QObj["Umax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_u = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.umax = result.split(";", QString::SkipEmptyParts);

        // inf_v
        lst = QObj["Vmin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_v = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.vmin = result.split(";", QString::SkipEmptyParts);

        // sup_v
        lst = QObj["Vmax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_v = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.vmax = result.split(";", QString::SkipEmptyParts);

        // Varu
        lst = QObj["Varu"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Varu = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VaruSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

        // Cnd
        lst = QObj["Cnd"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_CND = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

        // Const
        lst = QObj["Const"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Const = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

        // Funct
        lst = QObj["Funct"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Funct = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->FunctSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);


        //Noise:
        QString noise = "";
        noise = Jobj["Noise"].toString();
        noise.replace("\n","");
        noise.replace("\t","");
        noise.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;

        // Colors
        QString noise1 = "";
        noise1 =  QTextureObj["Noise"].toString();
        noise1.replace("\n","");
        noise1.replace("\t","");
        noise1.replace(" ","");

        lst = QTextureObj["Colors"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Rgbt = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise1.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
        }

        // Pigment
        QString noise2 = "";
        noise2 =  QPigmentObj["Noise"].toString();
        noise2.replace("\n","");
        noise2.replace("\t","");
        noise2.replace(" ","");

        QJsonArray tmp;
        QString strgradient = QPigmentObj["Gradient"].toString();
        lst =  QPigmentObj["Colors"].toArray();
        result = "";
        for(j=0; j < lst.size(); j++)
        {
            tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
            for(int k=0; k < tmp.count(); k++)
            {
                result += tmp[k].toString() + ";";
            }
        }

        strgradient.replace("\n","");
        strgradient.replace("\t","");
        strgradient.replace(" ","");

        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Gradient = strgradient.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VRgbt = result.toStdString();

        MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "" && noise1 == "")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise2.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
        }

        // Grid
        lst = QObj["Grid"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        if(result!="")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = true;
            for(j=0; j < lst.size(); j++)
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[j] = (lst[j].toString()).toInt();
        }
        else
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = false;

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Grid = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);

        // Component
        lst = QObj["Component"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

        // Name
        lst = QObj["Name"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);

        QJsonObject Jobjtmp = Jobj;
        //Some keys cleaning..
        Jobjtmp.remove("Iso3D");
        Jobjtmp.remove("Param4D");

        QJsonDocument document;
        document.setObject(Jobjtmp);
        MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

        MathmodRef->RootObjet.CurrentJsonObject = Jobjtmp;
        CurrentFormulaType = 1;
        /// process the new surface
        if(textureIndex == -1)
        {
            MathmodRef->ParametricSurfaceProcess(1);
        }
        else
        {
            int result = MathmodRef->ParsePar();
            if(result == -1) return;
            textureIndex < 1000 ? MathmodRef->ui.glWidget->CalculateTexturePoints(0) : MathmodRef->ui.glWidget->CalculatePigmentPoints(0);
            MathmodRef->ui.glWidget->update();
        }
    }

    else if(Jobj["Param4D"].isObject())
    {
        QObj = Jobj["Param4D"].toObject();
        // Fx
        lst = QObj["Fx"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_X = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_XSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fx = result.split(";", QString::SkipEmptyParts);

        // Fy
        lst = QObj["Fy"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Y = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_YSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fy = result.split(";", QString::SkipEmptyParts);

        // Fz
        lst = QObj["Fz"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Z = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_ZSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.fz = result.split(";", QString::SkipEmptyParts);

        //Fw
        lst = QObj["Fw"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_W = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.fw = result.split(";", QString::SkipEmptyParts);

        // inf_u
        lst = QObj["Umin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_u = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.umin = result.split(";", QString::SkipEmptyParts);

        // sup_u
        lst = QObj["Umax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_u = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.umax = result.split(";", QString::SkipEmptyParts);

        // inf_v
        lst = QObj["Vmin"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_v = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.vmin = result.split(";", QString::SkipEmptyParts);

        // sup_v
        lst = QObj["Vmax"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_v = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.vmax = result.split(";", QString::SkipEmptyParts);

        // Varu
        lst = QObj["Varu"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Varu = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VaruSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

        // Const
        lst = QObj["Const"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Const = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

        // Cnd
        lst = QObj["Cnd"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_CND= result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

        // Funct
        lst = QObj["Funct"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Funct = result.toStdString();
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->FunctSize = lst.size();
        MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);

        //Noise:
        QString noise = "";
        noise = Jobj["Noise"].toString();
        noise.replace("\n","");
        noise.replace("\t","");
        noise.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;

        // Colors
        QString noise1 = "";
        noise1 =  QTextureObj["Noise"].toString();
        noise1.replace("\n","");
        noise1.replace("\t","");
        noise1.replace(" ","");

        lst = QTextureObj["Colors"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Rgbt = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise1.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
        }

        // Pigment
        QString noise2 = "";
        noise2 =  QPigmentObj["Noise"].toString();
        noise2.replace("\n","");
        noise2.replace("\t","");
        noise2.replace(" ","");

        QJsonArray tmp;
        QString strgradient = QPigmentObj["Gradient"].toString();
        lst =  QPigmentObj["Colors"].toArray();
        result = "";
        for(j=0; j < lst.size(); j++)
        {
            tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
            for(int k=0; k < tmp.count(); k++)
            {
                result += tmp[k].toString() + ";";
            }
        }

        strgradient.replace("\n","");
        strgradient.replace("\t","");
        strgradient.replace(" ","");

        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Gradient = strgradient.toStdString();

        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VRgbt = result.toStdString();

        MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);
        if(noise == "" && noise1 == "")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise2.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
        }

        // Grid
        lst = QObj["Grid"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        if(result!="")
        {
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = true;
            for(j=0; j < lst.size(); j++)
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[j] = (lst[j].toString()).toInt();
        }
        else
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = false;
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Grid = result.toStdString();
        MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);

        // Component
        lst = QObj["Component"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

        // Name
        lst = QObj["Name"].toArray();
        result = "";
        for(j=0; j < lst.size()-1; j++)
            result += lst[j].toString() + ";";
        if(lst.size() >= 1)
            result += lst[lst.size()-1].toString();
        result.replace("\n","");
        result.replace("\t","");
        result.replace(" ","");
        MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);

        QJsonObject Jobjtmp = Jobj;
        //Some keys cleaning..
        Jobjtmp.remove("Iso3D");
        Jobjtmp.remove("Param3D");

        QJsonDocument document;
        document.setObject(Jobjtmp);
        MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

        MathmodRef->RootObjet.CurrentJsonObject = Jobjtmp;
        CurrentFormulaType = 3;

        /// process the new surface
        if(textureIndex == -1)
        {
            MathmodRef->ParametricSurfaceProcess(3);
        }
        else
        {
            int result = MathmodRef->ParsePar();
            if(result == -1) return;
            textureIndex < 1000 ? MathmodRef->ui.glWidget->CalculateTexturePoints(0) : MathmodRef->ui.glWidget->CalculatePigmentPoints(0);
            MathmodRef->ui.glWidget->update();
        }
    }
}

// --------------------------
int DrawingOptions::JSON_choice_activated(const QString &arg1)
{
    int i,j;
    QString result;

    QJsonArray array = JSONMathModels["MathModels"].toArray();
    QJsonArray lst;
    QJsonObject QObj;


    for(i=0; i< array.size(); i++)
    {
        if((array[i].toObject())["Iso3D"].isObject()  &&
                (QObj = (array[i].toObject())["Iso3D"].toObject())["Name"].toArray()[0].toString() == arg1)
        {
            if(!VerifiedJsonModel((array[i].toObject())))
                return (0);

            ShowSliders(array[i].toObject());
            // Fxyz
            lst = QObj["Fxyz"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ImplicitFunction = result.toStdString();
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ImplicitFunctionSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fxyz = result.split(";", QString::SkipEmptyParts);

            // Cnd
            lst = QObj["Cnd"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Condition = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

            // Varu
            lst = QObj["Varu"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Varu = result.toStdString();
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->VaruSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

            // Const
            lst = QObj["Const"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Const = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

            // Funct
            lst = QObj["Funct"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Funct = result.toStdString();
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->FunctSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);


            //Noise:
            QString noise = "";
            noise = (array[i].toObject())["Noise"].toString();
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;


            // Texture:
            QString noise1 = "";
            noise1 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise1.replace("\n","");
            noise1.replace("\t","");


            lst =  (array[i].toObject())["Texture"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Rgbt = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);

            if(noise == "")
            {
                MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise1.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
            }

            // Pigment
            QString noise2 = "";
            noise2 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise2.replace("\n","");
            noise2.replace("\t","");

            QJsonArray tmp;
            QString strtmp = (array[i].toObject())["Pigment"].toObject()["Gradient"].toString();
            lst =  (array[i].toObject())["Pigment"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size(); j++)
            {
                tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
                for(int k=0; k < tmp.count(); k++)
                {
                    result += tmp[k].toString() + ";";
                }
            }

            strtmp.replace("\n","");
            strtmp.replace("\t","");
            strtmp.replace(" ","");
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Gradient = strtmp.toStdString();
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->VRgbt = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);

            if(noise == "" && noise1 == "")
            {
                MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Noise = noise2.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
            }

            // Grid
            lst = QObj["Grid"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Grid = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);

            // XlimitSup
            lst = QObj["Xmax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->XlimitSup = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.xmax = result.split(";", QString::SkipEmptyParts);

            // YlimitSup
            lst = QObj["Ymax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->YlimitSup = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.ymax = result.split(";", QString::SkipEmptyParts);

            // ZlimitSup
            lst = QObj["Zmax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ZlimitSup = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.zmax = result.split(";", QString::SkipEmptyParts);

            // XlimitInf
            lst = QObj["Xmin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->XlimitInf = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.xmin = result.split(";", QString::SkipEmptyParts);

            // YlimitInf
            lst = QObj["Ymin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->YlimitInf = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.ymin = result.split(";", QString::SkipEmptyParts);

            // ZlimitInf
            lst = QObj["Zmin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->ZlimitInf = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.zmin = result.split(";", QString::SkipEmptyParts);

            // Component
            lst = QObj["Component"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

            // Name
            lst = QObj["Name"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);


            QJsonDocument document;
            document.setObject(array[i].toObject());
            MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

            //Update the current parametric struct
            //MathmodRef->RootObjet.CurrentIsoStruct = MathmodRef->RootObjet.IsoTable[indextable];
            MathmodRef->RootObjet.CurrentJsonObject = array[i].toObject();
            CurrentFormulaType = 2;
            /// process the new surface
            MathmodRef->ProcessNewIsoSurface( );
            return(2);
        }
        else if((array[i].toObject())["Param3D"].isObject()     &&
                (QObj = (array[i].toObject())["Param3D"].toObject())["Name"].toArray()[0].toString() == arg1)
        {
            if(!VerifiedJsonModel((array[i].toObject())))
                return (0);
            ShowSliders(array[i].toObject());
            // Fx
            lst = QObj["Fx"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_X = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_XSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fx = result.split(";", QString::SkipEmptyParts);

            // Fy
            lst = QObj["Fy"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Y = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_YSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fy = result.split(";", QString::SkipEmptyParts);


            // Fz
            lst = QObj["Fz"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Z = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_ZSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fz = result.split(";", QString::SkipEmptyParts);

            // inf_u
            lst = QObj["Umin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_u = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.umin = result.split(";", QString::SkipEmptyParts);

            // sup_u
            lst = QObj["Umax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_u = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.umax = result.split(";", QString::SkipEmptyParts);

            // inf_v
            lst = QObj["Vmin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_v = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.vmin = result.split(";", QString::SkipEmptyParts);

            // sup_v
            lst = QObj["Vmax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_v = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.vmax = result.split(";", QString::SkipEmptyParts);

            // Varu
            lst = QObj["Varu"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Varu = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VaruSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

            // Const
            lst = QObj["Const"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Const = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

            // Cnd
            lst = QObj["Cnd"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_CND= result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

            // Funct
            lst = QObj["Funct"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Funct = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->FunctSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);
            //Noise:
            QString noise = "";
            noise = (array[i].toObject())["Noise"].toString();

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise.toStdString();

            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;

            // Texture:
            QString noise1 = "";
            noise1 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise1.replace("\n","");
            noise1.replace("\t","");


            lst =  (array[i].toObject())["Texture"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Rgbt = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);

            if(noise == "")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise1.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
            }

            // Pigment
            QString noise2 = "";
            noise2 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise2.replace("\n","");
            noise2.replace("\t","");

            QJsonArray tmp;
            QString strtmp = (array[i].toObject())["Pigment"].toObject()["Gradient"].toString();
            lst =  (array[i].toObject())["Pigment"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size(); j++)
            {
                tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
                for(int k=0; k < tmp.count(); k++)
                {
                    result += tmp[k].toString() + ";";
                }
            }

            strtmp.replace("\n","");
            strtmp.replace("\t","");
            strtmp.replace(" ","");
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Gradient = strtmp.toStdString();

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VRgbt = result.toStdString();

            MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);

            if(noise == "" && noise1 == "")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise2.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
            }

            // Grid
            lst = QObj["Grid"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            if(result!="")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = true;
                for(j=0; j < lst.size(); j++)
                    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[j] = (lst[j].toString()).toInt();
            }
            else
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = false;
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Grid = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);

            // Component
            lst = QObj["Component"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

            // Name
            lst = QObj["Name"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);

            QJsonDocument document;
            document.setObject(array[i].toObject());
            MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

            MathmodRef->RootObjet.CurrentJsonObject = array[i].toObject();
            CurrentFormulaType = 1;
            /// process the new surface
            MathmodRef->ParametricSurfaceProcess(1);
            return(1);
        }
        else if((array[i].toObject())["Param4D"].isObject()     &&
                (QObj = (array[i].toObject())["Param4D"].toObject())["Name"].toArray()[0].toString() == arg1)
        {
            if(!VerifiedJsonModel((array[i].toObject())))
                return (0);
            ShowSliders(array[i].toObject());
            // Fx
            lst = QObj["Fx"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_X = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_XSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fx = result.split(";", QString::SkipEmptyParts);

            // Fy
            lst = QObj["Fy"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Y = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_YSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fy = result.split(";", QString::SkipEmptyParts);

            // Fz
            lst = QObj["Fz"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_Z = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_ZSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.fz = result.split(";", QString::SkipEmptyParts);

            // Fw
            lst = QObj["Fw"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_W = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.fw = result.split(";", QString::SkipEmptyParts);

            // inf_u
            lst = QObj["Umin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_u = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.umin = result.split(";", QString::SkipEmptyParts);

            // sup_u
            lst = QObj["Umax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_u = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.umax = result.split(";", QString::SkipEmptyParts);

            // inf_v
            lst = QObj["Vmin"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->inf_v = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.vmin = result.split(";", QString::SkipEmptyParts);

            // sup_v
            lst = QObj["Vmax"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->sup_v = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.vmax = result.split(";", QString::SkipEmptyParts);

            // Varu
            lst = QObj["Varu"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Varu = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VaruSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Varu = result.split(";", QString::SkipEmptyParts);

            // Const
            lst = QObj["Const"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Const = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Const = result.split(";", QString::SkipEmptyParts);

            // Cnd
            lst = QObj["Cnd"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->expression_CND= result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Cnd = result.split(";", QString::SkipEmptyParts);

            // Funct
            lst = QObj["Funct"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Funct = result.toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->FunctSize = lst.size();
            MathmodRef->RootObjet.CurrentTreestruct.Funct = result.split(";", QString::SkipEmptyParts);

            //Noise:
            QString noise = "";
            noise = (array[i].toObject())["Noise"].toString();

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Noise = noise;

            // Texture:
            QString noise1 = "";
            noise1 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise1.replace("\n","");
            noise1.replace("\t","");
            lst =  (array[i].toObject())["Texture"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Rgbt = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.RGBT = result.split(";", QString::SkipEmptyParts);
            if(noise == "")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise1.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise1;
            }

            // Pigment
            QString noise2 = "";
            noise2 = (array[i].toObject())["Texture"].toObject()["Noise"].toString();
            noise2.replace("\n","");
            noise2.replace("\t","");
            QJsonArray tmp;
            QString strtmp = (array[i].toObject())["Pigment"].toObject()["Gradient"].toString();
            lst =  (array[i].toObject())["Pigment"].toObject()["Colors"].toArray();
            result = "";
            for(j=0; j < lst.size(); j++)
            {
                tmp = (lst[j].toObject())["Color"].toObject()["Vrgba"].toArray();
                for(int k=0; k < tmp.count(); k++)
                {
                    result += tmp[k].toString() + ";";
                }
            }
            strtmp.replace("\n","");
            strtmp.replace("\t","");
            strtmp.replace(" ","");
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Gradient = strtmp.toStdString();

            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->VRgbt = result.toStdString();

            MathmodRef->RootObjet.CurrentTreestruct.VRGBT = result.split(";", QString::SkipEmptyParts);
            if(noise == "" && noise1 == "")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Noise = noise2.toStdString();
                MathmodRef->RootObjet.CurrentTreestruct.Noise = noise2;
            }


            // Grid
            lst = QObj["Grid"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            if(result!="")
            {
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = true;
                for(j=0; j < lst.size(); j++)
                    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->grid[j] = (lst[j].toString()).toInt();
            }
            else
                MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->gridnotnull = false;
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Grid = result.toStdString();
            MathmodRef->RootObjet.CurrentTreestruct.Grid = result.split(";", QString::SkipEmptyParts);

            // Component
            lst = QObj["Component"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.Component = result.split(";", QString::SkipEmptyParts);

            // Name
            lst = QObj["Name"].toArray();
            result = "";
            for(j=0; j < lst.size()-1; j++)
                result += lst[j].toString() + ";";
            if(lst.size() >= 1)
                result += lst[lst.size()-1].toString();
            result.replace("\n","");
            result.replace("\t","");
            result.replace(" ","");
            MathmodRef->RootObjet.CurrentTreestruct.name = result.split(";", QString::SkipEmptyParts);

            QJsonDocument document;
            document.setObject(array[i].toObject());
            MathmodRef->RootObjet.CurrentTreestruct.text = QString (document.toJson());

            MathmodRef->RootObjet.CurrentJsonObject = array[i].toObject();
            CurrentFormulaType = 3;
            /// process the new surface
            MathmodRef->ParametricSurfaceProcess(3);
            return(3);
        }
    }
    return(0);
}

// --------------------------
int DrawingOptions::on_choice_activated(const QString &arg)
{
    // Draw here
    int Result =JSON_choice_activated(arg);
    ui.ObjectClasseCurrent->takeTopLevelItem(0);
    UpdateCurrentTreeObject();
    return Result;
}

// --------------------------
void DrawingOptions::Run_JsonObject_activeted()
{
    QJsonParseError err;
    QString sortie;
    QString script  = ui.ParamEdit->toPlainText().trimmed().replace("\n","").replace("\t","").replace("DOTSYMBOL",Parameters->dotsymbol.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(script.toUtf8(), &err);
    if (err.error)
    {
        QMessageBox message ;
        message.setWindowTitle("Error at : ");
        sortie = (script);
        int before, after;
        if(sortie.length() > (err.offset +30))
            after = 30;
        else after = sortie.length() - err.offset;
        sortie.truncate(err.offset +after);
        if(err.offset-30 > 0)
            before = 30;
        else
            before = 0;
        sortie = sortie.remove(0,err.offset-before);
        sortie.replace("\t", " ");
        sortie.replace("\n", " ");
        sortie.insert(before, " >>> Error <<< ");
        message.setText("Error : " + err.errorString() + " at position: " + QString::number(err.offset) + "\n\n***********\n" +
                        "..." + sortie + "..."
                       );
        message.adjustSize () ;
        message.exec();
    }

    // Draw here
    DrawJsonModel(doc.object());
}

// --------------------------
void DrawingOptions::slot_comboBox18_3_activated(const QString &arg1)
{
    if(arg1 == "Triangles")
    {
        MathmodRef->slot_triangles_clicked();
        return;
    }
    if(arg1 == "Normales")
    {
        MathmodRef->draw_norm_clicked();
        return;
    }
    if(arg1 == "Infos")
    {
        MathmodRef->iso_infos();
        return;
    }
    if(arg1 == "Fill")
    {
        MathmodRef->fill();
        return;
    }
    if(arg1 == "Smooth")
    {
        MathmodRef->smoothline();
        return;
    }
    if(arg1 == "Mesh")
    {
        MathmodRef->Mesh();
        return;
    }
}

// --------------------------
void DrawingOptions::slot_checkBox_clicked()
{
    MathmodRef->slot_uv_clicked();
}

// --------------------------
void DrawingOptions::LoadNewFileModels(bool upd)
{
    QString mathmodcollection;

    mathmodcollection = QFileDialog::getOpenFileName(0, tr("Load Json Script"),
                        "",
                        tr("Json (*.js)"));

    if(upd)
    {
        ui.ObjectClasse->clear();
        for(int i=0; i<MathmodRef->RootObjet.MyJsonObjectSelection.count(); i++)
            MathmodRef->RootObjet.MyJsonObjectSelection.removeAt(i);
        MathmodRef->RootObjet.NbIsoStruct = MathmodRef->RootObjet.NbParamStruct = 0;
        MathmodRef->pariso.JPar.clear();
        MathmodRef->pariso.JIso.clear();
    }

    MathmodRef->RootObjet.ReadJsonFile(mathmodcollection, JSONMathModels);

    QJsonArray array = JSONMathModels["MathModels"].toArray();
    QStringList lst;
    QString a;
    QJsonObject jsobj;

    for(int i=0; i< array.size(); i++)
    {
        if((array[i].toObject())["Iso3D"].isObject())
        {
            jiso newjiso;
            jsobj = (array[i].toObject())["Iso3D"].toObject();
            newjiso.read( jsobj );
            MathmodRef->pariso.JIso.append(newjiso);
            a = (jsobj)["Name"].toArray()[0].toString();
            lst.append(a);
        }
        else if((array[i].toObject())["Param3D"].isObject())
        {
            jpar newjpar;
            jsobj = ((array[i].toObject())["Param3D"].toObject());
            newjpar.read( jsobj );
            MathmodRef->pariso.JPar.append(newjpar);
            a = ( jsobj )["Name"].toArray()[0].toString();
            lst.append(a);
        }
        else if((array[i].toObject())["Param4D"].isObject())
        {
            jpar newjpar;
            jsobj = ((array[i].toObject())["Param4D"].toObject());
            newjpar.read( jsobj );
            MathmodRef->pariso.JPar.append(newjpar);
            a = ( jsobj )["Name"].toArray()[0].toString();
            lst.append(a);
        }
    }

    lst.insert(0, "Examples ("+QString::number(lst.count())+")");
    //Load the script containing isosurface and parametric formulas:
    ui.choice->clear();
    ui.choice->insertItems(0, lst);
    AddListModels(upd);
}

// --------------------------
void DrawingOptions::LoadK3DSurfScript (QString filename, int type)
{
    int current = 0, currentfunction = 0, separator =0;
    QString name, line, IsoFct, Xfct, Yfct, Zfct, xlimit, ylimit, zlimit, ulimit, vlimit,
            xmin, xmax, ymin, ymax, zmin, zmax,
            umin, umax, vmin, vmax,
            condition,
            Au, Bu, Cu, Duv, Euv, Fuv, comments, fctcomts;
    int EquationsNumber=0;
    QString JsonString="";
    int FctType = -1;

    filename = QFileDialog::getOpenFileName(0, QObject::tr("Open K3DSurf script File"),
                                            "",
                                            QObject::tr("K3DS (*.k3ds)"));

    QFile file( filename );
    if(type == 1)
    {
        if ( file.open(  QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QTextStream stream( &file );
            while ( !stream.atEnd() )
            {
                line = (stream.readLine()).trimmed();// line of text excluding '\n'
                if (line.contains("F():"))
                {
                    currentfunction =  1;
                    FctType = 0;
                }
                else if (line.contains("[x]:"))    currentfunction =  2;
                else if (line.contains("[y]:"))    currentfunction =  3;
                else if (line.contains("[z]:"))    currentfunction =  4;


                else if (line.contains("X():"))
                {
                    currentfunction = 21;
                    FctType = 1;
                }
                else if (line.contains("Y():")) currentfunction = 22;
                else if (line.contains("Z():")) currentfunction = 23;
                else if (line.contains("[u]:")) currentfunction = 24;
                else if (line.contains("[v]:")) currentfunction = 25;

                else if (line.contains("Cnd:"))    currentfunction =  5;
                else if (line.contains("A[u]:"))   currentfunction =  6;
                else if (line.contains("B[u]:"))   currentfunction =  7;
                else if (line.contains("C[u]:"))   currentfunction =  8;
                else if (line.contains("D[u,v]:")) currentfunction =  9;
                else if (line.contains("E[u,v]:")) currentfunction = 10;
                else if (line.contains("F[u,v]:")) currentfunction = 11;
                else if (line.contains("#"))       currentfunction = 12;
                else if (line.contains("/*"))      currentfunction = 13;
                else if (line.contains("*/"))      currentfunction = 14;
                else if (line.contains("Name:"))   currentfunction = 15;
                else if (line.contains(";"))       currentfunction = 16;
                else                               currentfunction = 18;
                switch(currentfunction)
                {
                case  1:
                    IsoFct  += line.remove  (0, 4);
                    current = 1;
                    break;

                case  21:
                    Xfct  += line.replace("X():", "");
                    current = 21;
                    break;
                case  22:
                    Yfct  += line.replace("Y():", "");
                    current = 22;
                    break;
                case  23:
                    Zfct  += line.replace("Z():", "");
                    current = 23;
                    break;
                case  24:
                    ulimit  += line.replace("[u]:", "");
                    current = 24;
                    break;
                case  25:
                    vlimit  += line.replace("[v]:", "");
                    current = 25;
                    break;

                case  2:
                    xlimit    = line.remove(0, 4);
                    current = 2;
                    break;
                case  3:
                    ylimit    = line.remove(0, 4);
                    current = 3;
                    break;
                case  4:
                    zlimit    = line.remove(0, 4);
                    current = 4;
                    break;
                case  5:
                    condition = line.remove(0, 4);
                    current = 5;
                    break;
                case  6:
                    Au        = line.remove(0, 5);
                    current = 6;
                    break;
                case  7:
                    Bu        = line.remove(0, 5);
                    current = 7;
                    break;
                case  8:
                    Cu        = line.remove(0, 5);
                    current = 8;
                    break;
                case  9:
                    Duv       = line.remove(0, 7);
                    current = 9;
                    break;
                case 10:
                    Euv       = line.remove(0, 7);
                    current =10;
                    break;
                case 11:
                    Fuv       = line.remove(0, 7);
                    current =11;
                    break;
                case 12:
                    comments  += line.replace("#", "");
                    current =12;
                    break;
                case 13:
                    comments  += line.replace("/*", "").replace("*/", "");
                    current =13;
                    break;
                case 14:
                    comments += line.replace("*/", "");
                    current =14;
                    break;
                case 15:
                    name      = line.remove(0, 5);
                    current =15;
                    break;
                case 16:
                    // Save the equation:
                    name = name.trimmed();
                    if(FctType == 0)
                    {
                        EquationsNumber++;
                        if(name == "")
                            name= "_" + QString::number(EquationsNumber);

                        xlimit    = xlimit.trimmed();
                        separator = xlimit.indexOf(",");
                        xmin      = xlimit.left(separator);
                        xmax      = xlimit.remove(xmin+",");

                        ylimit    = ylimit.trimmed();
                        separator = ylimit.indexOf(",");
                        ymin      = ylimit.left(separator);
                        ymax      = ylimit.remove(ymin+",");

                        zlimit    = zlimit.trimmed();
                        separator = zlimit.indexOf(",");
                        zmin      = zlimit.left(separator);
                        zmax      = zlimit.remove(zmin+",");

                        if(JsonString != "")
                            JsonString += ",";

                        JsonString += "{\"Iso3D\": {  \
                         \"Description \": [ \"" + comments + "\"], ";

                        JsonString +=  "\"Name\": [\"" + name + "\"], ";

                        JsonString +=  "\"Component\": [\"" + name + "\"], ";

                        JsonString += "\"Fxyz\": [\"" + IsoFct.trimmed() + "\"], ";

                        JsonString += "\"Cnd\": [\"" + condition + "\"], ";

                        JsonString += "\"Xmin\": [\"" + xmin + "\"], ";

                        JsonString += "\"Xmax\": [\"" + xmax + "\"], ";

                        JsonString += "\"Ymin\": [\"" + ymin + "\"], ";

                        JsonString += "\"Ymax\": [\"" + ymax + "\"], ";

                        JsonString += "\"Zmin\": [\"" + zmin + "\"], ";

                        JsonString += "\"Zmax\": [\"" + zmax + "\"] ";

                        JsonString += "}}";
                    }

                    else if(FctType == 1)
                    {
                        EquationsNumber++;
                        if(name == "")
                            name= "_" + QString::number(EquationsNumber);

                        ulimit    = ulimit.trimmed();
                        separator = ulimit.indexOf(",");
                        umin      = ulimit.left(separator);
                        umax      = ulimit.remove(umin+",");

                        vlimit    = vlimit.trimmed();
                        separator = vlimit.indexOf(",");
                        vmin      = vlimit.left(separator);
                        vmax      = vlimit.remove(vmin+",");

                        if(JsonString != "")
                            JsonString += ",";

                        JsonString += "{\"Param3D\": {  \
                         \"Description \": [ \"" + comments + "\"],";

                        JsonString +=  "\"Name\": [\"" + name + "\"],";

                        JsonString +=  "\"Component\": [\"" + name + "\"],";

                        JsonString += "\"Fx\": [\"" + Xfct.trimmed() + "\"],";

                        JsonString += "\"Fy\": [\"" + Yfct.trimmed() + "\"],";

                        JsonString += "\"Fz\": [\"" + Zfct.trimmed() + "\"],";

                        JsonString += "\"Cnd\": [\"" + condition + "\"],";

                        JsonString += "\"Umin\": [\"" + umin + "\"],";

                        JsonString += "\"Umax\": [\"" + umax + "\"],";

                        JsonString += "\"Vmin\": [\"" + vmin + "\"],";

                        JsonString += "\"Vmax\": [\"" +vmax + "\"]";

                        JsonString += "}}";
                    }
                    current =16;
                    // Init all Parameters fo new formula:
                    IsoFct = Xfct = Yfct = Zfct = name = comments = condition = "";
                    umin = umax = vmin = vmax = xmin = xmax = ymin = ymax = zmin = zmax = "";
                    ulimit = vlimit =  xlimit = ylimit = zlimit = "";
                    FctType = -1;
                    break;
                case 18:
                    if (current ==  1) IsoFct  += line;
                    else if (current ==  2) xlimit    += line;
                    else if (current ==  3) ylimit    += line;
                    else if (current ==  4) zlimit    += line;

                    else if (current ==  21) Xfct       += line;
                    else if (current ==  22) Yfct       += line;
                    else if (current ==  23) Zfct       += line;
                    else if (current ==  24) ulimit     += line;
                    else if (current ==  25) vlimit     += line;

                    else if (current ==  5) condition += line;
                    else if (current ==  6) Au        += line;
                    else if (current ==  7) Bu        += line;
                    else if (current ==  8) Cu        += line;
                    else if (current ==  9) Duv       += line;
                    else if (current == 10) Euv       += line;
                    else if (current == 11) Fuv       += line;
                    else if (current == 12) comments  += line;
                    else if (current == 13) comments  += line;
                    else if (current == 14) comments  += line;
                    else if (current == 15) name      += line;
                    break;
                }
            }

            file.close();

            JsonString  =  " { \"MathModels\": [ " + JsonString + "]}";

            QFile f( filename.replace(".k3ds", ".js"));
            if(f.exists())
                f.remove();
            if ( !f.open(QIODevice::ReadWrite | QIODevice::Text) )
            {
                statusBar()->showMessage( tr("Could not write to %1").arg( filename),2000 );
                return;
            }
            QTextStream t( &f );
            t << JsonString;
            f.close();
        }
    }
}

// --------------------------
QTreeWidgetItem * DrawingOptions::ChildItemTreeProperty(QTreeWidgetItem * item, QString proprty)
{
    int childcount = item->childCount();
    for(int j =0; j < childcount; j++)
    {
        if((item->child(j))->text(0).contains(proprty))
            return item->child(j);
    }
    return NULL;
}

// --------------------------
void DrawingOptions::ParseItemTree(QTreeWidgetItem * item, QList<bool> &list,bool viewall)
{
    int childcount = item->childCount();
    bool sel = false;
    bool result;
    for(int j =0; j < childcount; j++)
    {
        item->child(j)->setTextColor(0, QColor(255, 255, 255, 255));
        if(!viewall)
        {
            result = false;
            for(int k =1; k < select.selectedoptions.selectedwords.count(); k++)
            {
                sel = (item->child(j))->text(0).contains(select.selectedoptions.selectedwords[k], (select.selectedoptions.sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
                result = result || sel;
                list[k-1] = (list.at(k-1) || sel);
            }
            if(result)
                item->child(j)->setTextColor(0, QColor(255, 0, 0, 255));
        }
    }
}

// --------------------------
void DrawingOptions::SearchListModels()
{
    QTreeWidgetItem * Toplevel;
    QTreeWidgetItem * Childlevel;
    QTreeWidgetItem * SubChildlevel;
    int topcount = ui.ObjectClasse->topLevelItemCount();
    int childcount;
    bool sel1 = false;
    int searchresult=0;

    //init boolean lists:
    for(int i =0; i < select.selectedoptions.selectedwords.count()-1; i++)
    {
        select.selectedoptions.namelist.append(false);
        select.selectedoptions.functlist.append(false);
        select.selectedoptions.cmpnamelist.append(false);
        select.selectedoptions.complist.append(false);
    }

    for(int i=0; i<topcount; ++i)
    {
        Toplevel = ui.ObjectClasse->topLevelItem(i);
        childcount = Toplevel->childCount();
        searchresult=0;
        for(int j =0; j < childcount; j++)
        {
            sel1 = true;
            if(!select.selectedoptions.showall)
            {
                //init boolean lists:
                for(int m =0; m < select.selectedoptions.selectedwords.count()-1; m++)
                {
                    select.selectedoptions.namelist[m] = false;
                    select.selectedoptions.functlist[m] = false;
                    select.selectedoptions.cmpnamelist[m] = false;
                    select.selectedoptions.complist[m] = false;
                }
                // Search in scripts names:
                if(select.selectedoptions.parsenames)
                    for(int k =1; k < select.selectedoptions.selectedwords.count(); k++)
                    {
                        select.selectedoptions.namelist[k-1] = (Toplevel->child(j))->text(0).contains(select.selectedoptions.selectedwords[k], (select.selectedoptions.sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
                    }
                // continue searching in the functions list when needed:
                if(select.selectedoptions.parsefunctions && (Childlevel=ChildItemTreeProperty(Toplevel->child(j), "Parameters")) != NULL)
                    if((SubChildlevel=ChildItemTreeProperty(Childlevel, "Functions")) != NULL )
                    {
                        ParseItemTree(SubChildlevel, select.selectedoptions.functlist);
                    }
                // continue searching in the components names list when needed:
                if(select.selectedoptions.parsecmpnames && (Childlevel=ChildItemTreeProperty(Toplevel->child(j), "Components")) != NULL)
                {
                    ParseItemTree(Childlevel, select.selectedoptions.cmpnamelist);
                    int ct= Childlevel->childCount();
                    for(int m=0; m<ct; m++)
                        ParseItemTree(Childlevel->child(m), select.selectedoptions.complist);
                }



                // now look in the search results
                if(select.selectedoptions.AND)
                {
                    sel1 = true;
                    for(int l=0; l< select.selectedoptions.selectedwords.count() -1; l++)
                        sel1 = sel1 && (select.selectedoptions.namelist.at(l) || select.selectedoptions.functlist.at(l)
                                        || select.selectedoptions.cmpnamelist.at(l)  || select.selectedoptions.complist.at(l));
                }
                else
                {
                    sel1 = false;
                    for(int l=0; l< select.selectedoptions.selectedwords.count() -1; l++)
                        sel1 = sel1 || (select.selectedoptions.namelist.at(l) || select.selectedoptions.functlist.at(l)
                                        || select.selectedoptions.cmpnamelist.at(l) || select.selectedoptions.complist.at(l));
                }
            }
            else
            {
                // Make sure the text color is white when showall is activated
                if((Childlevel=ChildItemTreeProperty(Toplevel->child(j), "Parameters")) != NULL)
                    if((SubChildlevel=ChildItemTreeProperty(Childlevel, "Functions")) != NULL )
                    {
                        ParseItemTree(SubChildlevel, select.selectedoptions.functlist, true);
                    }
                // Components names and their childes:
                if((Childlevel=ChildItemTreeProperty(Toplevel->child(j), "Components")) != NULL)
                {
                    ParseItemTree(Childlevel, select.selectedoptions.cmpnamelist, true);
                    int ct= Childlevel->childCount();
                    for(int m=0; m<ct; m++)
                        ParseItemTree(Childlevel->child(m), select.selectedoptions.complist, true);
                }
            }

            // Now count and show only scripts with appropiate search results:
            if(sel1)
                searchresult ++;

            (Toplevel->child(j))->setHidden(!sel1);
            if(Toplevel->text(0).contains("IsoSurfaces"))
                Toplevel->setText(0, "IsoSurfaces (" + QString::number(searchresult) + ")");
            else if(Toplevel->text(0).contains("Parametric"))
                Toplevel->setText(0, "Parametric (" + QString::number(searchresult) + ")");
            else
                Toplevel->setText(0, "My Selection (" + QString::number(searchresult) + ")");
        }
    }

    //Clear boolean lists:
    select.selectedoptions.namelist.clear();
    select.selectedoptions.functlist.clear();
    select.selectedoptions.cmpnamelist.clear();
    select.selectedoptions.complist.clear();
}

// --------------------------
void DrawingOptions::AddListModels(bool update)
{
    if(!update)
    {
        QMenu*  contextMenu = new QMenu(ui.ObjectClasse);
        ui.ObjectClasse->setContextMenuPolicy(Qt::ActionsContextMenu);
        QAction* addElement = new QAction("Add Current Model to MySelection",contextMenu);
        QAction* deleteElement = new QAction("Erase selected row from MySelection",contextMenu);
        QAction *separator = new QAction(ui.ObjectClasse);
        separator->setSeparator(true);

        ui.ObjectClasse->addAction(addElement);
        ui.ObjectClasse->addAction(separator);
        ui.ObjectClasse->addAction(deleteElement);
        connect(addElement,    SIGNAL(triggered()), this, SLOT(slot_pushButton_2_clicked()));
        connect(deleteElement, SIGNAL(triggered()), this, SLOT(slot_unselect_clicked()));
    }


    QColor greenColor = QColor(0, 255, 0, 50);
    // Parametric:
    QTreeWidgetItem *ParlistItem = new QTreeWidgetItem(ui.ObjectClasse);
    QString Text = "Parametric (" + QString::number(MathmodRef->pariso.JPar.count()) + ")" ;
    ParlistItem->setBackgroundColor(0, greenColor);
    ParlistItem->setText(0, Text);
    for (int i = 0; i < MathmodRef->pariso.JPar.count(); ++i)
    {
        QTreeWidgetItem *nameitem = new QTreeWidgetItem(ParlistItem);
        nameitem->setText(0,  MathmodRef->pariso.JPar[i].Name[0]);
        if(MathmodRef->pariso.JPar[i].Component.count() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(nameitem);
            cmpitem->setText(0,  "Components");

            for(int j=0; j<MathmodRef->pariso.JPar[i].Component.count(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->pariso.JPar[i].Component.at(j));

                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "X(u,v) = "+MathmodRef->pariso.JPar[i].Fx.at(j));

                QTreeWidgetItem *cmpitem4 = new QTreeWidgetItem(cmpitem2);
                cmpitem4->setText(0,  "Y(u,v) = "+MathmodRef->pariso.JPar[i].Fy.at(j));

                QTreeWidgetItem *cmpitem5 = new QTreeWidgetItem(cmpitem2);
                cmpitem5->setText(0,  "Z(u,v) = "+MathmodRef->pariso.JPar[i].Fz.at(j));

                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "U = ["+ MathmodRef->pariso.JPar[i].Umin.at(j)  +", "+ MathmodRef->pariso.JPar[i].Umax.at(j) +"]");

                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "V = ["+ MathmodRef->pariso.JPar[i].Vmin.at(j)  +", "+ MathmodRef->pariso.JPar[i].Vmax.at(j)  +"]");
            }
        }

        //Add Global parameters:
        if (
            MathmodRef->pariso.JPar[i].Varu.count() ||
            MathmodRef->pariso.JPar[i].Csts.count() ||
            MathmodRef->pariso.JPar[i].Funct.count() ||
            MathmodRef->pariso.JPar[i].Grid.count()
        )
        {
            QTreeWidgetItem *parameteritem = new QTreeWidgetItem(nameitem);
            parameteritem->setText(0,  "Parameters:");

            if(MathmodRef->pariso.JPar[i].Varu.count() > 0)
            {
                QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                varitem->setText(0,  "Variables:");
                for(int j=0; j<MathmodRef->pariso.JPar[i].Varu.count(); j++)
                {
                    QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                    varitem2->setText(0,  MathmodRef->pariso.JPar[i].Varu.at(j));
                }
            }

            if(MathmodRef->pariso.JPar[i].Csts.count() > 0)
            {
                QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                cstitem->setText(0,  "Constantes:");
                for(int j=0; j<MathmodRef->pariso.JPar[i].Csts.count(); j++)
                {
                    QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                    cstitem2->setText(0,  MathmodRef->pariso.JPar[i].Csts.at(j));
                }
            }

            if(MathmodRef->pariso.JPar[i].Funct.count() > 0)
            {
                QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                fctitem->setText(0,  "Functions:");
                for(int j=0; j<MathmodRef->pariso.JPar[i].Funct.count(); j++)
                {
                    QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                    fctitem2->setText(0,  MathmodRef->pariso.JPar[i].Funct.at(j));
                }
            }

            if(MathmodRef->pariso.JPar[i].Grid.count() > 0)
            {
                QTreeWidgetItem *grditem = new QTreeWidgetItem(parameteritem);
                grditem->setText(0,  "Grid:");
                for(int j=0; j<MathmodRef->pariso.JPar[i].Grid.count(); j++)
                {
                    QTreeWidgetItem *grditem2 = new QTreeWidgetItem(grditem);
                    grditem2->setText(0,  MathmodRef->pariso.JPar[i].Grid.at(j));
                }
            }

            if(MathmodRef->pariso.JPar[i].RGBT.count() > 0)
            {
                QTreeWidgetItem *grbitem = new QTreeWidgetItem(parameteritem);
                grbitem->setText(0,  "Colors:");
                for(int j=0; j<MathmodRef->pariso.JPar[i].RGBT.count(); j++)
                {
                    QTreeWidgetItem *grbitem2 = new QTreeWidgetItem(grbitem);
                    grbitem2->setText(0,  MathmodRef->pariso.JPar[i].RGBT.at(j));
                }
            }
        }
    }
    ParlistItem->sortChildren(0, Qt::AscendingOrder);

    // Iso:
    QTreeWidgetItem *IsolistItem = IsolistItemRef = new QTreeWidgetItem(ui.ObjectClasse);
    Text = "IsoSurfaces (" + QString::number(MathmodRef->pariso.JIso.count()) + ")" ;
    IsolistItem->setBackgroundColor(0, greenColor);
    IsolistItem->setText(0, Text);
    for (int i = 0; i < MathmodRef->pariso.JIso.count(); ++i)
    {
        QTreeWidgetItem *nameitem = new QTreeWidgetItem(IsolistItem);
        nameitem->setText(0,  MathmodRef->pariso.JIso[i].Name[0]);
        if(MathmodRef->pariso.JIso[i].Component.count() > 0)
        {
            QTreeWidgetItem *cmpitem = new QTreeWidgetItem(nameitem);
            cmpitem->setText(0,  "Components");

            for(int j=0; j<MathmodRef->pariso.JIso[i].Component.count(); j++)
            {
                QTreeWidgetItem *cmpitem2 = new QTreeWidgetItem(cmpitem);
                cmpitem2->setText(0,  MathmodRef->pariso.JIso[i].Component.at(j));

                QTreeWidgetItem *cmpitem3 = new QTreeWidgetItem(cmpitem2);
                cmpitem3->setText(0,  "F(x,y,z) = "+MathmodRef->pariso.JIso[i].Fxyz.at(j));

                QTreeWidgetItem *cmpitem6 = new QTreeWidgetItem(cmpitem2);
                cmpitem6->setText(0,  "X = ["+ MathmodRef->pariso.JIso[i].Xmin.at(j)  +", "+ MathmodRef->pariso.JIso[i].Xmax.at(j) +"]");

                QTreeWidgetItem *cmpitem7 = new QTreeWidgetItem(cmpitem2);
                cmpitem7->setText(0,  "Y = ["+ MathmodRef->pariso.JIso[i].Ymin.at(j)  +", "+ MathmodRef->pariso.JIso[i].Ymax.at(j)  +"]");

                QTreeWidgetItem *cmpitem8 = new QTreeWidgetItem(cmpitem2);
                cmpitem8->setText(0,  "Z = ["+ MathmodRef->pariso.JIso[i].Zmin.at(j)  +", "+ MathmodRef->pariso.JIso[i].Zmax.at(j)  +"]");
            }
        }

        //Add Global parameters:
        if (
            MathmodRef->pariso.JIso[i].Varu.count() ||
            MathmodRef->pariso.JIso[i].Csts.count() ||
            MathmodRef->pariso.JIso[i].Funct.count() ||
            MathmodRef->pariso.JIso[i].Grid.count()
        )
        {
            QTreeWidgetItem *parameteritem = new QTreeWidgetItem(nameitem);
            parameteritem->setText(0,  "Parameters:");

            if(MathmodRef->pariso.JIso[i].Varu.count() > 0)
            {
                QTreeWidgetItem *varitem = new QTreeWidgetItem(parameteritem);
                varitem->setText(0,  "Variables:");
                for(int j=0; j<MathmodRef->pariso.JIso[i].Varu.count(); j++)
                {
                    QTreeWidgetItem *varitem2 = new QTreeWidgetItem(varitem);
                    varitem2->setText(0,  MathmodRef->pariso.JIso[i].Varu.at(j));
                }
            }

            if(MathmodRef->pariso.JIso[i].Csts.count() > 0)
            {
                QTreeWidgetItem *cstitem = new QTreeWidgetItem(parameteritem);
                cstitem->setText(0,  "Constantes:");
                for(int j=0; j<MathmodRef->pariso.JIso[i].Csts.count(); j++)
                {
                    QTreeWidgetItem *cstitem2 = new QTreeWidgetItem(cstitem);
                    cstitem2->setText(0,  MathmodRef->pariso.JIso[i].Csts.at(j));
                }
            }

            if(MathmodRef->pariso.JIso[i].Funct.count() > 0)
            {
                QTreeWidgetItem *fctitem = new QTreeWidgetItem(parameteritem);
                fctitem->setText(0,  "Functions:");
                for(int j=0; j<MathmodRef->pariso.JIso[i].Funct.count(); j++)
                {
                    QTreeWidgetItem *fctitem2 = new QTreeWidgetItem(fctitem);
                    fctitem2->setText(0,  MathmodRef->pariso.JIso[i].Funct.at(j));
                }
            }

            if(MathmodRef->pariso.JIso[i].Grid.count() > 0)
            {
                QTreeWidgetItem *grditem = new QTreeWidgetItem(parameteritem);
                grditem->setText(0,  "Grid:");
                for(int j=0; j<MathmodRef->pariso.JIso[i].Grid.count(); j++)
                {
                    QTreeWidgetItem *grditem2 = new QTreeWidgetItem(grditem);
                    grditem2->setText(0,  MathmodRef->pariso.JIso[i].Grid.at(j));
                }
            }

            if(MathmodRef->pariso.JIso[i].RGBT.count() > 0)
            {
                QTreeWidgetItem *grbitem = new QTreeWidgetItem(parameteritem);
                grbitem->setText(0,  "Colors:");
                for(int j=0; j<MathmodRef->pariso.JIso[i].RGBT.count(); j++)
                {
                    QTreeWidgetItem *grbitem2 = new QTreeWidgetItem(grbitem);
                    grbitem2->setText(0,  MathmodRef->pariso.JIso[i].RGBT.at(j));
                }
            }
        }
    }

    IsolistItemRef->sortChildren(0, Qt::AscendingOrder);

    //My Selection:
    // Isosurfaces:
    QTreeWidgetItem *MyselectionItem = new QTreeWidgetItem(ui.ObjectClasse);
    MyselectionItemReference = MyselectionItem;
    MyselectionItem->setBackgroundColor(0, greenColor);
    MyselectionItem->setText(0, "My Selection");
    return;
}

// --------------------------
void DrawingOptions::on_pushButton_clicked()
{
    static int checked=1;
    checked *=-1;
    if(checked == -1)
    {
        ui.ObjectClasse->hide();
    }
    else
    {
        ui.ObjectClasse->show();
    }
}

// --------------------------
void DrawingOptions::on_action_Exit_triggered()
{
    MathmodRef->close();
    this->close();
}

// --------------------------
void DrawingOptions::on_actionTriangles_triggered()
{
    MathmodRef->slot_triangles_clicked();
}

// --------------------------
void DrawingOptions::on_actionNormales_triggered()
{
    MathmodRef->draw_norm_clicked();
}

// --------------------------
void DrawingOptions::on_actionInfos_triggered()
{
    MathmodRef->iso_infos();
}

// --------------------------
void DrawingOptions::on_actionFill_triggered()
{
    MathmodRef->fill();
}

// --------------------------
void DrawingOptions::on_actionSmooth_triggered()
{
    MathmodRef->smoothline();
}

// --------------------------
void DrawingOptions::on_ObjectClasse_clicked(const QModelIndex &index)
{
    if(!MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->isRunning())
    {
        //int result;
        QString tst;
        MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = -1;
        if(index.data().isValid())
        {
            QVariant data = index.data();
            QString s = data.toString();
            QModelIndex parent = index.parent();
            if(parent.data().isValid())
            {
                QString prt = parent.data().toString();
                if(prt != "My Selection")
                {
                    on_choice_activated(s);
                }
                else
                {
                    for(int i=0; i<MathmodRef->RootObjet.MyJsonObjectSelection.size(); i++)
                        if((MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Iso3D"].isObject()))
                        {
                            if (s == (tst = ( MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Iso3D"]).toObject()["Name"].toArray()[0].toString()))
                            {
                                // Draw here

                                DrawJsonModel(MathmodRef->RootObjet.MyJsonObjectSelection.at(i).toObject());
                                return;
                            }
                        }
                        else     if((MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Param3D"].isObject()))
                        {
                            if (s == (tst = ( MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Param3D"]).toObject()["Name"].toArray()[0].toString()))
                            {
                                // Draw here
                                DrawJsonModel(MathmodRef->RootObjet.MyJsonObjectSelection.at(i).toObject());
                                return;
                            }
                        }
                        else     if((MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Param4D"].isObject()))
                        {
                            if (s == (tst = ( MathmodRef->RootObjet.MyJsonObjectSelection[i].toObject()["Param4D"]).toObject()["Name"].toArray()[0].toString()))
                            {
                                // Draw here
                                DrawJsonModel(MathmodRef->RootObjet.MyJsonObjectSelection.at(i).toObject());
                                return;
                            }
                        }
                }
            }
        }
        return;
    }
    else
    {
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->quit();
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->wait();
    }
}

// --------------------------
void DrawingOptions::slot_pushButton_2_clicked()
{
    MathmodRef->RootObjet.MyJsonObjectSelection.append(MathmodRef->RootObjet.CurrentJsonObject);
    AddObjectToMySelectionTree();
}

// --------------------------
void DrawingOptions::slot_unselect_clicked()
{
    int index = MyselectionItemReference->indexOfChild(ui.ObjectClasse->currentItem());
    MathmodRef->RootObjet.MyJsonObjectSelection.removeAt(index);
    MyselectionItemReference->removeChild(ui.ObjectClasse->currentItem());
}

// --------------------------
void DrawingOptions::slot_XYZscrollBar_valueChanged(int value)
{
    MathmodRef->xyzg_valueChanged(value);
}

// --------------------------
void DrawingOptions::UpdateDescription(int position)
{
    if(MathmodRef->RootObjet.CurrentJsonObject["Param3D"].isObject())
    {
        if(position > -1)
        {
            indexcurrentFormula = position;
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula == position?
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = -1:
                    MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = position;
            //currentFormula = sparent;
            ui.paramNameEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.Component.at(position));
            ui.XEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.fx.at(position));
            ui.YEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.fy.at(position));
            ui.ZEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.fz.at(position));
            ui.umin->setText(MathmodRef->RootObjet.CurrentTreestruct.umin.at(position));
            ui.vmin->setText(MathmodRef->RootObjet.CurrentTreestruct.vmin.at(position));
            ui.umax->setText(MathmodRef->RootObjet.CurrentTreestruct.umax.at(position));
            ui.vmax->setText(MathmodRef->RootObjet.CurrentTreestruct.vmax.at(position));

            if(!MathmodRef->RootObjet.CurrentTreestruct.Component.empty())
                ui.paramNameEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.Component.at(position));
            else
                ui.paramNameEdit->setText("");

            if(!MathmodRef->RootObjet.CurrentTreestruct.Cnd.empty())
            {
                ui.CndUpdateEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.Cnd.at(0));
            }
            else
            {
                ui.CndUpdateEdit_2->setText("");
            }

            //Function:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Funct.empty())
            {
                ui.tableWidget_Fct_2->clearContents();
                ui.tableWidget_Fct_2->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Funct.size(); i++)
                {
                    ui.tableWidget_Fct_2->setRowCount(i+1);
                    ui.tableWidget_Fct_2->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Funct.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Fct_2->clearContents();
                ui.tableWidget_Fct_2->setRowCount(0);
            }

            //Constantes:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Const.empty())
            {
                ui.tableWidget_Cst_2->clearContents();
                ui.tableWidget_Cst_2->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Const.size(); i++)
                {
                    ui.tableWidget_Cst_2->setRowCount(i+1);
                    ui.tableWidget_Cst_2->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Const.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Cst_2->clearContents();
                ui.tableWidget_Cst_2->setRowCount(0);
            }

            //Variables:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Varu.empty())
            {
                ui.tableWidget_Var_2->clearContents();
                ui.tableWidget_Var_2->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Varu.size(); i++)
                {
                    ui.tableWidget_Var_2->setRowCount(i+1);
                    ui.tableWidget_Var_2->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Varu.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Var_2->clearContents();
                ui.tableWidget_Var_2->setRowCount(0);
            }
        }
        ui.stackedProperties->setCurrentIndex(2);
    }

    if(MathmodRef->RootObjet.CurrentJsonObject["Param4D"].isObject())
    {
        if(position > -1)
        {
            indexcurrentFormula = position;
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula == position?
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = -1:
                    MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = position;
            //currentFormula = sparent;
            ui.paramNameEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.Component.at(position));
            ui.XEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.fx.at(position));
            ui.YEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.fy.at(position));
            ui.ZEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.fz.at(position));
            ui.WEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.fw.at(position));
            ui.umin_2->setText(MathmodRef->RootObjet.CurrentTreestruct.umin.at(position));
            ui.vmin_2->setText(MathmodRef->RootObjet.CurrentTreestruct.vmin.at(position));
            ui.umax_2->setText(MathmodRef->RootObjet.CurrentTreestruct.umax.at(position));
            ui.vmax_2->setText(MathmodRef->RootObjet.CurrentTreestruct.vmax.at(position));
            if(!MathmodRef->RootObjet.CurrentTreestruct.Component.empty())
                ui.paramNameEdit_2->setText(MathmodRef->RootObjet.CurrentTreestruct.Component.at(position));
            else
                ui.paramNameEdit_2->setText("");
        }
        ui.stackedProperties->setCurrentIndex(3);
    }

    if(MathmodRef->RootObjet.CurrentJsonObject["Iso3D"].isObject())
    {
        if(position > -1)
        {
            indexcurrentFormula = position;
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula == position?
            MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = -1:
                    MathmodRef->ui.glWidget->LocalScene.IndexCurrentFormula = position;

            ui.UpdateEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.fxyz.at(position));
            ui.xmin->setText(MathmodRef->RootObjet.CurrentTreestruct.xmin.at(position));
            ui.ymin->setText(MathmodRef->RootObjet.CurrentTreestruct.ymin.at(position));
            ui.zmin->setText(MathmodRef->RootObjet.CurrentTreestruct.zmin.at(position));
            ui.xmax->setText(MathmodRef->RootObjet.CurrentTreestruct.xmax.at(position));
            ui.ymax->setText(MathmodRef->RootObjet.CurrentTreestruct.ymax.at(position));
            ui.zmax->setText(MathmodRef->RootObjet.CurrentTreestruct.zmax.at(position));

            if(!MathmodRef->RootObjet.CurrentTreestruct.Component.empty())
                ui.isoNameEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.Component.at(position));
            else
                ui.isoNameEdit->setText("");

            if(!MathmodRef->RootObjet.CurrentTreestruct.Cnd.empty())
            {
                position < MathmodRef->RootObjet.CurrentTreestruct.Cnd.size() ?
                ui.CndUpdateEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.Cnd.at(position)) :
                ui.CndUpdateEdit->setText(MathmodRef->RootObjet.CurrentTreestruct.Cnd.at(0));
            }
            else
            {
                ui.CndUpdateEdit->setText("");
            }

            //Function:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Funct.empty())
            {
                ui.tableWidget_Fct->clearContents();
                ui.tableWidget_Fct->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Funct.size(); i++)
                {
                    ui.tableWidget_Fct->setRowCount(i+1);
                    ui.tableWidget_Fct->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Funct.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Fct->clearContents();
                ui.tableWidget_Fct->setRowCount(0);
            }

            //Constantes:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Const.empty())
            {
                ui.tableWidget_Cst->clearContents();
                ui.tableWidget_Cst->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Const.size(); i++)
                {
                    ui.tableWidget_Cst->setRowCount(i+1);
                    ui.tableWidget_Cst->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Const.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Cst->clearContents();
                ui.tableWidget_Cst->setRowCount(0);
            }

            //Variables:
            if(!MathmodRef->RootObjet.CurrentTreestruct.Varu.empty())
            {
                ui.tableWidget_Var->clearContents();
                ui.tableWidget_Var->setRowCount(0);
                for(int i=0; i < MathmodRef->RootObjet.CurrentTreestruct.Varu.size(); i++)
                {
                    ui.tableWidget_Var->setRowCount(i+1);
                    ui.tableWidget_Var->setItem(i, 0, new QTableWidgetItem(MathmodRef->RootObjet.CurrentTreestruct.Varu.at(i)));
                }
            }
            else
            {
                ui.tableWidget_Var->clearContents();
                ui.tableWidget_Var->setRowCount(0);
            }
        }

        ui.stackedProperties->setCurrentIndex(1);
    }

    //MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::oncolor_2activated(int index)
{
    MathmodRef->ui.glWidget->colorstype(index);
}

// --------------------------
void DrawingOptions::onred_2valueChanged(int value)
{
    MathmodRef->ui.glWidget->red(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ongreen_2valueChanged(int value)
{
    MathmodRef->ui.glWidget->green(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::onblue_2valueChanged(int value)
{
    MathmodRef->ui.glWidget->blue(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparent_2valueChanged(int value)
{
    MathmodRef->ui.glWidget->transparency(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparence_2clicked(bool checked)
{
    MathmodRef->ui.glWidget->transparence(checked);
}

// --------------------------
void DrawingOptions::on_red_ParIso_valueChanged(int value)
{
    switch (CurrentFormulaType)
    {
    case 0:
        break;
    case 1:
        MathmodRef->ui.glWidget->redpar(value, indexcurrentFormula);
        break;
    case 2:
        MathmodRef->ui.glWidget->red(value, indexcurrentFormula);
        break;
    case 3:
        MathmodRef->ui.glWidget->redpar(value, indexcurrentFormula);
        break;

    default:
        break;
    }
}

// --------------------------
void DrawingOptions::on_green_ParIso_valueChanged(int value)
{
    switch (CurrentFormulaType)
    {
    case 0:
        break;
    case 1:
        MathmodRef->ui.glWidget->greenpar(value, indexcurrentFormula);
        break;
    case 2:
        MathmodRef->ui.glWidget->green(value, indexcurrentFormula);
        break;
    case 3:
        MathmodRef->ui.glWidget->greenpar(value, indexcurrentFormula);
        break;

    default:
        break;
    }
}

// --------------------------
void DrawingOptions::on_blue_ParIso_valueChanged(int value)
{
    switch (CurrentFormulaType)
    {
    case 0:
        break;
    case 1:
        MathmodRef->ui.glWidget->bluepar(value, indexcurrentFormula);
        break;
    case 2:
        MathmodRef->ui.glWidget->blue(value, indexcurrentFormula);
        break;
    case 3:
        MathmodRef->ui.glWidget->bluepar(value, indexcurrentFormula);
        break;

    default:
        break;
    }
}

// --------------------------
void DrawingOptions::on_updateButton_clicked()
{
    //int i, j;
    QJsonObject copyCurrentObject = MathmodRef->RootObjet.CurrentJsonObject;
    QJsonArray array;

    if(copyCurrentObject["Iso3D"].isObject())
    {
        if(indexcurrentFormula != -1)
        {
            QJsonObject copyCurrentObject2 = MathmodRef->RootObjet.CurrentJsonObject["Iso3D"].toObject();
            //Cut the component at the index indexcurrentFormula :
            if((ui.isoNameEdit->toPlainText()).replace(" ", "") == "")
            {
                array=copyCurrentObject2["Fxyz"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fxyz"] = array;

                array=copyCurrentObject2["Xmin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Xmin"] = array;

                array=copyCurrentObject2["Xmax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Xmax"] = array;

                array=copyCurrentObject2["Ymin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Ymin"] = array;

                array=copyCurrentObject2["Ymax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Ymax"] = array;

                array=copyCurrentObject2["Zmin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Zmin"] = array;

                array=copyCurrentObject2["Zmax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Zmax"] = array;

                if(copyCurrentObject2["Component"].isArray())
                {
                    array=copyCurrentObject2["Component"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Component"] = array;
                }

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");

                if(copyCurrentObject2["Cnd"].isArray() && copyCurrentObject2["Cnd"].toArray().count() > indexcurrentFormula)
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Cnd"] = array;
                }
                else
                    copyCurrentObject2.remove("Cnd");

                /************************************************************************************************/
                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");


                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");


                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");
                /************************************************************************************************/

                //Some keys cleaning..
                copyCurrentObject2.remove("Param3D");
                copyCurrentObject2.remove("Param4D");
                copyCurrentObject["Iso3D"] = copyCurrentObject2;
            }
            //Update the component at the index indexcurrentFormula;
            else if(
                MathmodRef->RootObjet.CurrentTreestruct.Component.count() > 0 &&
                indexcurrentFormula >-1 &&
                indexcurrentFormula < MathmodRef->RootObjet.CurrentTreestruct.Component.size() &&
                MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") == (ui.isoNameEdit->toPlainText()).replace(" ", ""))
            {

                array=copyCurrentObject2["Fxyz"].toArray();
                array.replace(indexcurrentFormula, ui.UpdateEdit->toPlainText());
                copyCurrentObject2["Fxyz"] = array;

                array=copyCurrentObject2["Xmin"].toArray();
                array.replace(indexcurrentFormula, ui.xmin->toPlainText());
                copyCurrentObject2["Xmin"] = array;

                array=copyCurrentObject2["Xmax"].toArray();
                array.replace(indexcurrentFormula, ui.xmax->toPlainText());
                copyCurrentObject2["Xmax"] = array;

                array=copyCurrentObject2["Ymin"].toArray();
                array.replace(indexcurrentFormula, ui.ymin->toPlainText());
                copyCurrentObject2["Ymin"] = array;

                array=copyCurrentObject2["Ymax"].toArray();
                array.replace(indexcurrentFormula, ui.ymax->toPlainText());
                copyCurrentObject2["Ymax"] = array;

                array=copyCurrentObject2["Zmin"].toArray();
                array.replace(indexcurrentFormula, ui.zmin->toPlainText());
                copyCurrentObject2["Zmin"] = array;

                array=copyCurrentObject2["Zmax"].toArray();
                array.replace(indexcurrentFormula, ui.zmax->toPlainText());
                copyCurrentObject2["Zmax"] = array;

                if(copyCurrentObject2["Cnd"].isArray()  && copyCurrentObject2["Cnd"].toArray().count() > indexcurrentFormula)
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    array.replace(indexcurrentFormula, ui.CndUpdateEdit->toPlainText());
                    copyCurrentObject2["Cnd"] = array;
                }
                else if(ui.CndUpdateEdit->toPlainText() != "")
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    for(int i=0; i<copyCurrentObject2["Fxyz"].toArray().count(); i++)
                        array.append(ui.CndUpdateEdit->toPlainText());
                    copyCurrentObject2["Cnd"] = array;
                }
                else
                    copyCurrentObject2.remove("Cnd");

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.replace(indexcurrentFormula, QString::number(ui.xyzg->value()));
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");


                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");


                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");


                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");


                copyCurrentObject["Iso3D"] = copyCurrentObject2;
            }
            else
            {
                //Add new component:
                if(MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") != (ui.isoNameEdit->toPlainText()).replace(" ", ""))
                {
                    array=copyCurrentObject2["Component"].toArray();
                    array.append(ui.isoNameEdit->toPlainText());
                    copyCurrentObject2["Component"] = array;

                    array=copyCurrentObject2["Fxyz"].toArray();
                    array.append(ui.UpdateEdit->toPlainText());
                    copyCurrentObject2["Fxyz"] = array;

                    array=copyCurrentObject2["Xmin"].toArray();
                    array.append( ui.xmin->toPlainText());
                    copyCurrentObject2["Xmin"] = array;

                    array=copyCurrentObject2["Xmax"].toArray();
                    array.append(ui.xmax->toPlainText());
                    copyCurrentObject2["Xmax"] = array;

                    array=copyCurrentObject2["Ymin"].toArray();
                    array.append(ui.ymin->toPlainText());
                    copyCurrentObject2["Ymin"] = array;

                    array=copyCurrentObject2["Ymax"].toArray();
                    array.append(ui.ymax->toPlainText());
                    copyCurrentObject2["Ymax"] = array;

                    array=copyCurrentObject2["Zmin"].toArray();
                    array.append(ui.zmin->toPlainText());
                    copyCurrentObject2["Zmin"] = array;

                    array=copyCurrentObject2["Zmax"].toArray();
                    array.append(ui.zmax->toPlainText());
                    copyCurrentObject2["Zmax"] = array;

                    if(copyCurrentObject2["Cnd"].isArray())
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        array.append(ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else if(ui.CndUpdateEdit->toPlainText() != "")
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        for(int i=0; i<copyCurrentObject2["Fxyz"].toArray().count(); i++)
                            array.append(ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Cnd");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.append(QString::number(ui.xyzg->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");
                }
                else
                {
                    array=copyCurrentObject2["Fxyz"].toArray();
                    array.insert(indexcurrentFormula, ui.UpdateEdit->toPlainText());
                    copyCurrentObject2["Fxyz"] = array;

                    array=copyCurrentObject2["Xmin"].toArray();
                    array.insert(indexcurrentFormula, ui.xmin->toPlainText());
                    copyCurrentObject2["Xmin"] = array;

                    array=copyCurrentObject2["Xmax"].toArray();
                    array.insert(indexcurrentFormula, ui.xmax->toPlainText());
                    copyCurrentObject2["Xmax"] = array;

                    array=copyCurrentObject2["Ymin"].toArray();
                    array.insert(indexcurrentFormula, ui.ymin->toPlainText());
                    copyCurrentObject2["Ymin"] = array;

                    array=copyCurrentObject2["Ymax"].toArray();
                    array.insert(indexcurrentFormula, ui.ymax->toPlainText());
                    copyCurrentObject2["Ymax"] = array;

                    array=copyCurrentObject2["Zmin"].toArray();
                    array.insert(indexcurrentFormula, ui.zmin->toPlainText());
                    copyCurrentObject2["Zmin"] = array;

                    array=copyCurrentObject2["Zmax"].toArray();
                    array.insert(indexcurrentFormula, ui.zmax->toPlainText());
                    copyCurrentObject2["Zmax"] = array;

                    if(copyCurrentObject2["Component"].isArray())
                    {
                        array=copyCurrentObject2["Component"].toArray();
                        array.insert(indexcurrentFormula, ui.isoNameEdit->toPlainText());
                        copyCurrentObject2["Component"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Component");

                    if(copyCurrentObject2["Cnd"].isArray())
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        array.insert(indexcurrentFormula, ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else if(ui.CndUpdateEdit->toPlainText() != "")
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        for(int i=0; i<copyCurrentObject2["Fxyz"].toArray().count(); i++)
                            array.append(ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Cnd");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.insert(indexcurrentFormula, QString::number(ui.xyzg->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");
                }

                /************************************************************************************************/
                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");


                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");

                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");
                /************************************************************************************************/

                copyCurrentObject["Iso3D"] = copyCurrentObject2;
            }
            //Some keys cleaning..
            copyCurrentObject.remove("Param3D");
            copyCurrentObject.remove("Param4D");

            // Draw here
            DrawJsonModel(copyCurrentObject);
        }
    }
    else if(copyCurrentObject["Param3D"].isObject())
    {
        QJsonObject copyCurrentObject2 = MathmodRef->RootObjet.CurrentJsonObject["Param3D"].toObject();
        if(indexcurrentFormula != -1)
        {
            if((ui.paramNameEdit->toPlainText()).replace(" ", "") == "")
            {
                array=copyCurrentObject2["Fx"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fx"] = array;

                array=copyCurrentObject2["Fy"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fy"] = array;

                array=copyCurrentObject2["Fz"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fz"] = array;

                array=copyCurrentObject2["Umin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Umin"] = array;

                array=copyCurrentObject2["Umax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Umax"] = array;

                array=copyCurrentObject2["Vmin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Vmin"] = array;

                array=copyCurrentObject2["Vmax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Vmax"] = array;

                if(copyCurrentObject2["Component"].isArray())
                {
                    array=copyCurrentObject2["Component"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Component"] = array;
                }
                else
                    copyCurrentObject2.remove("Component");

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");

                if(copyCurrentObject2["Cnd"].isArray() && copyCurrentObject2["Cnd"].toArray().count() > indexcurrentFormula)
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Cnd"] = array;
                }
                else
                    copyCurrentObject2.remove("Cnd");

                /************************************************************************************************/
                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");

                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");

                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst_2->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");
                /************************************************************************************************/

                //Some keys cleaning..
                copyCurrentObject2.remove("Iso3D");
                copyCurrentObject2.remove("Param4D");
                copyCurrentObject["Param3D"] = copyCurrentObject2;

            }
            else if(
                MathmodRef->RootObjet.CurrentTreestruct.Component.count() > 0 &&
                indexcurrentFormula >-1&&
                indexcurrentFormula < MathmodRef->RootObjet.CurrentTreestruct.Component.size() &&
                MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") == (ui.paramNameEdit->toPlainText()).replace(" ", ""))
            {

                array=copyCurrentObject2["Fx"].toArray();
                array.replace(indexcurrentFormula, ui.XEdit->toPlainText());
                copyCurrentObject2["Fx"] = array;

                array=copyCurrentObject2["Fy"].toArray();
                array.replace(indexcurrentFormula, ui.YEdit->toPlainText());
                copyCurrentObject2["Fy"] = array;

                array=copyCurrentObject2["Fz"].toArray();
                array.replace(indexcurrentFormula, ui.ZEdit->toPlainText());
                copyCurrentObject2["Fz"] = array;

                array=copyCurrentObject2["Umin"].toArray();
                array.replace(indexcurrentFormula, ui.umin->toPlainText());
                copyCurrentObject2["Umin"] = array;

                array=copyCurrentObject2["Umax"].toArray();
                array.replace(indexcurrentFormula, ui.umax->toPlainText());
                copyCurrentObject2["Umax"] = array;

                array=copyCurrentObject2["Vmin"].toArray();
                array.replace(indexcurrentFormula, ui.vmin->toPlainText());
                copyCurrentObject2["Vmin"] = array;

                array=copyCurrentObject2["Vmax"].toArray();
                array.replace(indexcurrentFormula, ui.vmax->toPlainText());
                copyCurrentObject2["Vmax"] = array;

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.replace(indexcurrentFormula, QString::number(ui.linecolumn_2->value()));
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");

                if(copyCurrentObject2["Cnd"].isArray()  && copyCurrentObject2["Cnd"].toArray().count() > indexcurrentFormula)
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    array.replace(indexcurrentFormula, ui.CndUpdateEdit_2->toPlainText());
                    copyCurrentObject2["Cnd"] = array;
                }
                else if(ui.CndUpdateEdit->toPlainText() != "")
                {
                    array=copyCurrentObject2["Cnd"].toArray();
                    for(int i=0; i<copyCurrentObject2["Fx"].toArray().count(); i++)
                        array.append(ui.CndUpdateEdit->toPlainText());
                    copyCurrentObject2["Cnd"] = array;
                }
                else
                    copyCurrentObject2.remove("Cnd");

                /************************************************************************************************/
                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");


                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");


                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst_2->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");
                /************************************************************************************************/

                copyCurrentObject["Param3D"] = copyCurrentObject2;
            }
            else
            {
                if(MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") != (ui.paramNameEdit->toPlainText()).replace(" ", ""))
                {
                    array=copyCurrentObject2["Fx"].toArray();
                    array.append(ui.XEdit->toPlainText());
                    copyCurrentObject2["Fx"] = array;

                    array=copyCurrentObject2["Fy"].toArray();
                    array.append(ui.YEdit->toPlainText());
                    copyCurrentObject2["Fy"] = array;

                    array=copyCurrentObject2["Fz"].toArray();
                    array.append(ui.ZEdit->toPlainText());
                    copyCurrentObject2["Fz"] = array;

                    array=copyCurrentObject2["Umin"].toArray();
                    array.append(ui.umin->toPlainText());
                    copyCurrentObject2["Umin"] = array;

                    array=copyCurrentObject2["Umax"].toArray();
                    array.append(ui.umax->toPlainText());
                    copyCurrentObject2["Umax"] = array;

                    array=copyCurrentObject2["Vmin"].toArray();
                    array.append(ui.vmin->toPlainText());
                    copyCurrentObject2["Vmin"] = array;

                    array=copyCurrentObject2["Vmax"].toArray();
                    array.append(ui.vmax->toPlainText());
                    copyCurrentObject2["Vmax"] = array;

                    if(copyCurrentObject2["Component"].isArray())
                    {
                        array=copyCurrentObject2["Component"].toArray();
                        array.append(ui.paramNameEdit->toPlainText());
                        copyCurrentObject2["Component"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Component");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.append( QString::number(ui.linecolumn_2->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");

                    if(copyCurrentObject2["Cnd"].isArray())
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        array.append(ui.CndUpdateEdit_2->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else if(ui.CndUpdateEdit->toPlainText() != "")
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        for(int i=0; i<copyCurrentObject2["Fx"].toArray().count(); i++)
                            array.append(ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Cnd");

                    //copyCurrentObject["Param3D"] = copyCurrentObject2;
                }
                else
                {
                    array=copyCurrentObject2["Fx"].toArray();
                    array.insert(indexcurrentFormula, ui.XEdit->toPlainText());
                    copyCurrentObject2["Fx"] = array;

                    array=copyCurrentObject2["Fy"].toArray();
                    array.insert(indexcurrentFormula, ui.YEdit->toPlainText());
                    copyCurrentObject2["Fy"] = array;

                    array=copyCurrentObject2["Fz"].toArray();
                    array.insert(indexcurrentFormula, ui.ZEdit->toPlainText());
                    copyCurrentObject2["Fz"] = array;

                    array=copyCurrentObject2["Umin"].toArray();
                    array.insert(indexcurrentFormula, ui.umin->toPlainText());
                    copyCurrentObject2["Umin"] = array;

                    array=copyCurrentObject2["Umax"].toArray();
                    array.insert(indexcurrentFormula, ui.umax->toPlainText());
                    copyCurrentObject2["Umax"] = array;

                    array=copyCurrentObject2["Vmin"].toArray();
                    array.insert(indexcurrentFormula, ui.vmin->toPlainText());
                    copyCurrentObject2["Vmin"] = array;

                    array=copyCurrentObject2["Vmax"].toArray();
                    array.insert(indexcurrentFormula, ui.vmax->toPlainText());
                    copyCurrentObject2["Vmax"] = array;

                    if(copyCurrentObject2["Component"].isArray())
                    {
                        array=copyCurrentObject2["Component"].toArray();
                        array.insert(indexcurrentFormula, ui.paramNameEdit->toPlainText());
                        copyCurrentObject2["Component"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Component");


                    if(copyCurrentObject2["Cnd"].isArray())
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        array.append(ui.CndUpdateEdit_2->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else if(ui.CndUpdateEdit->toPlainText() != "")
                    {
                        array=copyCurrentObject2["Cnd"].toArray();
                        for(int i=0; i<copyCurrentObject2["Fx"].toArray().count(); i++)
                            array.append(ui.CndUpdateEdit->toPlainText());
                        copyCurrentObject2["Cnd"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Cnd");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.insert(indexcurrentFormula, QString::number(ui.linecolumn_2->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");
                }

                /************************************************************************************************/
                //Variables:
                if(copyCurrentObject2["Varu"].isArray() && ui.tableWidget_Var_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Var_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Var_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Var_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Varu"] = array2;
                }
                else
                    copyCurrentObject2.remove("Varu");


                //Functions:
                if(copyCurrentObject2["Funct"].isArray() && ui.tableWidget_Fct_2->rowCount() > 0)
                {
                    QJsonArray array2;

                    for(int i=0; i< ui.tableWidget_Fct_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Fct_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Fct_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Funct"] = array2;
                }
                else
                    copyCurrentObject2.remove("Funct");


                //Constantes:
                if(copyCurrentObject2["Const"].isArray() && ui.tableWidget_Cst_2->rowCount() > 0)
                {
                    QJsonArray array2;
                    for(int i=0; i< ui.tableWidget_Cst_2->rowCount(); i++)
                    {
                        if( (ui.tableWidget_Cst_2->item(i, 0))->text() != "")
                            array2.append((ui.tableWidget_Cst_2->item(i, 0))->text());
                    }

                    copyCurrentObject2["Const"] = array2;
                }
                else
                    copyCurrentObject2.remove("Const");
                /************************************************************************************************/
                copyCurrentObject["Param3D"] = copyCurrentObject2;
            }

            //Some keys cleaning..
            copyCurrentObject.remove("Iso3D");
            copyCurrentObject.remove("Param4D");
            // Draw here
            DrawJsonModel(copyCurrentObject);
        }
    }
    else if(copyCurrentObject["Param4D"].isObject())
    {
        QJsonObject copyCurrentObject2 = MathmodRef->RootObjet.CurrentJsonObject["Param4D"].toObject();
        if(indexcurrentFormula != -1)
        {
            if((ui.paramNameEdit_2->toPlainText()).replace(" ", "") == "")
            {
                array=copyCurrentObject2["Fx"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fx"] = array;

                array=copyCurrentObject2["Fy"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fy"] = array;

                array=copyCurrentObject2["Fz"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fz"] = array;

                array=copyCurrentObject2["Fw"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Fw"] = array;

                array=copyCurrentObject2["Umin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Umin"] = array;

                array=copyCurrentObject2["Umax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Umax"] = array;

                array=copyCurrentObject2["Vmin"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Vmin"] = array;

                array=copyCurrentObject2["Vmax"].toArray();
                array.removeAt(indexcurrentFormula);
                copyCurrentObject2["Vmax"] = array;

                if(copyCurrentObject2["Component"].isArray())
                {
                    array=copyCurrentObject2["Component"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Component"] = array;
                }
                else
                    copyCurrentObject2.remove("Component");

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.removeAt(indexcurrentFormula);
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");

                copyCurrentObject["Param4D"] = copyCurrentObject2;
            }
            else if(
                MathmodRef->RootObjet.CurrentTreestruct.Component.count() > 0 &&
                indexcurrentFormula >-1&&
                indexcurrentFormula < MathmodRef->RootObjet.CurrentTreestruct.Component.size() &&
                MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") == (ui.paramNameEdit_2->toPlainText()).replace(" ", ""))
            {

                array=copyCurrentObject2["Fx"].toArray();
                array.replace(indexcurrentFormula, ui.XEdit_2->toPlainText());
                copyCurrentObject2["Fx"] = array;

                array=copyCurrentObject2["Fy"].toArray();
                array.replace(indexcurrentFormula, ui.YEdit_2->toPlainText());
                copyCurrentObject2["Fy"] = array;

                array=copyCurrentObject2["Fz"].toArray();
                array.replace(indexcurrentFormula, ui.ZEdit_2->toPlainText());
                copyCurrentObject2["Fz"] = array;

                array=copyCurrentObject2["Fw"].toArray();
                array.replace(indexcurrentFormula, ui.WEdit_2->toPlainText());
                copyCurrentObject2["Fw"] = array;

                array=copyCurrentObject2["Umin"].toArray();
                array.replace(indexcurrentFormula, ui.umin_2->toPlainText());
                copyCurrentObject2["Umin"] = array;

                array=copyCurrentObject2["Umax"].toArray();
                array.replace(indexcurrentFormula, ui.umax_2->toPlainText());
                copyCurrentObject2["Umax"] = array;

                array=copyCurrentObject2["Vmin"].toArray();
                array.replace(indexcurrentFormula, ui.vmin_2->toPlainText());
                copyCurrentObject2["Vmin"] = array;

                array=copyCurrentObject2["Vmax"].toArray();
                array.replace(indexcurrentFormula, ui.vmax_2->toPlainText());
                copyCurrentObject2["Vmax"] = array;

                if(copyCurrentObject2["Grid"].isArray())
                {
                    array=copyCurrentObject2["Grid"].toArray();
                    array.replace(indexcurrentFormula, QString::number(ui.linecolumn_2->value()));
                    copyCurrentObject2["Grid"] = array;
                }
                else
                    copyCurrentObject2.remove("Grid");

                copyCurrentObject["Param4D"] = copyCurrentObject2;
            }
            else
            {
                if(MathmodRef->RootObjet.CurrentTreestruct.Component[indexcurrentFormula].replace(" ", "") != (ui.paramNameEdit_2->toPlainText()).replace(" ", ""))
                {
                    array=copyCurrentObject2["Fx"].toArray();
                    array.append(ui.XEdit_2->toPlainText());
                    copyCurrentObject2["Fx"] = array;

                    array=copyCurrentObject2["Fy"].toArray();
                    array.append(ui.YEdit_2->toPlainText());
                    copyCurrentObject2["Fy"] = array;

                    array=copyCurrentObject2["Fz"].toArray();
                    array.append(ui.ZEdit_2->toPlainText());
                    copyCurrentObject2["Fz"] = array;

                    array=copyCurrentObject2["Fw"].toArray();
                    array.append(ui.WEdit_2->toPlainText());
                    copyCurrentObject2["Fw"] = array;

                    array=copyCurrentObject2["Umin"].toArray();
                    array.append(ui.umin_2->toPlainText());
                    copyCurrentObject2["Umin"] = array;

                    array=copyCurrentObject2["Umax"].toArray();
                    array.append(ui.umax_2->toPlainText());
                    copyCurrentObject2["Umax"] = array;

                    array=copyCurrentObject2["Vmin"].toArray();
                    array.append(ui.vmin_2->toPlainText());
                    copyCurrentObject2["Vmin"] = array;

                    array=copyCurrentObject2["Vmax"].toArray();
                    array.append(ui.vmax_2->toPlainText());
                    copyCurrentObject2["Vmax"] = array;

                    if(copyCurrentObject2["Component"].isArray())
                    {
                        array=copyCurrentObject2["Component"].toArray();
                        array.append(ui.paramNameEdit_2->toPlainText());
                        copyCurrentObject2["Component"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Component");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.append( QString::number(ui.linecolumn_2->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");

                    copyCurrentObject["Param4D"] = copyCurrentObject2;
                }
                else
                {
                    array=copyCurrentObject2["Fx"].toArray();
                    array.insert(indexcurrentFormula, ui.XEdit_2->toPlainText());
                    copyCurrentObject2["Fx"] = array;

                    array=copyCurrentObject2["Fy"].toArray();
                    array.insert(indexcurrentFormula, ui.YEdit_2->toPlainText());
                    copyCurrentObject2["Fy"] = array;

                    array=copyCurrentObject2["Fz"].toArray();
                    array.insert(indexcurrentFormula, ui.ZEdit_2->toPlainText());
                    copyCurrentObject2["Fz"] = array;

                    array=copyCurrentObject2["Fw"].toArray();
                    array.insert(indexcurrentFormula, ui.WEdit_2->toPlainText());
                    copyCurrentObject2["Fw"] = array;

                    array=copyCurrentObject2["Umin"].toArray();
                    array.insert(indexcurrentFormula, ui.umin_2->toPlainText());
                    copyCurrentObject2["Umin"] = array;

                    array=copyCurrentObject2["Umax"].toArray();
                    array.insert(indexcurrentFormula, ui.umax_2->toPlainText());
                    copyCurrentObject2["Umax"] = array;

                    array=copyCurrentObject2["Vmin"].toArray();
                    array.insert(indexcurrentFormula, ui.vmin_2->toPlainText());
                    copyCurrentObject2["Vmin"] = array;

                    array=copyCurrentObject2["Vmax"].toArray();
                    array.insert(indexcurrentFormula, ui.vmax_2->toPlainText());
                    copyCurrentObject2["Vmax"] = array;

                    if(copyCurrentObject2["Component"].isArray())
                    {
                        array=copyCurrentObject2["Component"].toArray();
                        array.insert(indexcurrentFormula, ui.paramNameEdit_2->toPlainText());
                        copyCurrentObject2["Component"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Component");

                    if(copyCurrentObject2["Grid"].isArray())
                    {
                        array=copyCurrentObject2["Grid"].toArray();
                        array.insert(indexcurrentFormula, QString::number(ui.linecolumn_2->value()));
                        copyCurrentObject2["Grid"] = array;
                    }
                    else
                        copyCurrentObject2.remove("Grid");

                    copyCurrentObject["Param4D"] = copyCurrentObject2;
                }
            }


            //Some keys cleaning..
            copyCurrentObject.remove("Param3D");
            copyCurrentObject.remove("Iso3D");
            // Draw here
            DrawJsonModel(copyCurrentObject);
        }
    }
}

// --------------------------
void DrawingOptions::on_updateParam_clicked()
{
    on_updateButton_clicked();
}

// --------------------------
void DrawingOptions::on_actionPolygonWavefront_obj_triggered()
{
    MathmodRef->ui.glWidget->SaveSceneAsObjPoly();
}

// --------------------------
void DrawingOptions::on_actionColorPolygonWavefront_obj_triggered()
{
    MathmodRef->ui.glWidget->SaveSceneAsObjPoly(1);
}

// --------------------------
void DrawingOptions::on_linecolumn_2_valueChanged(int value)
{
    ui.ParamgroupBox_2->setTitle("Param Grid(u,v) = ( "+QString::number(value)+", "+QString::number(value)+" ) :");
    if(!MathmodRef->ui.glWidget->ParObjetThread->ParObjet->isRunning())
    {
        MathmodRef->linecolumn_valueChanged(value);
    }
    else
    {
        ui.uv->blockSignals(true);
        ui.uv->setChecked(false);
        ui.uv->blockSignals(false);
        MathmodRef->uvactivated  = -1;
    }
}

// --------------------------
void DrawingOptions::on_lineScrollBar_valueChanged(int value)
{
    ui.ParamgroupBox_2->setTitle("Param Grid(u,v) = ( "+QString::number(value)+", "+QString::number(MathmodRef->ui.glWidget->ParObjetThread->ParObjet->Ugrid)+" ) :");
    if(!MathmodRef->ui.glWidget->ParObjetThread->ParObjet->isRunning())
    {
        MathmodRef->line_valueChanged(value);
    }
    else
    {
        ui.uv->blockSignals(true);
        ui.uv->setChecked(false);
        ui.uv->blockSignals(false);
        MathmodRef->uvactivated  = -1;
    }
}

// --------------------------
void DrawingOptions::on_coloneScrollBar_valueChanged(int value)
{
    ui.ParamgroupBox_2->setTitle("Param Grid(u,v) = ( "+QString::number(MathmodRef->ui.glWidget->ParObjetThread->ParObjet->Vgrid)+", "+QString::number(value)+" ) :");
    if(!MathmodRef->ui.glWidget->ParObjetThread->ParObjet->isRunning())
    {
        MathmodRef->column_valueChanged(value);
    }
    else
    {
        ui.uv->blockSignals(true);
        ui.uv->setChecked(false);
        ui.uv->blockSignals(false);
        MathmodRef->uvactivated  = -1;
    }
}

// --------------------------
void DrawingOptions::oncolor_3activated(int index)
{
    MathmodRef->ui.glWidget->colorstypeParam(index);
}

// --------------------------
void DrawingOptions::onred_3valueChanged(int value)
{
    MathmodRef->ui.glWidget->redpar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ongreen_3valueChanged(int value)
{
    MathmodRef->ui.glWidget->greenpar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::onblue_3valueChanged(int value)
{
    MathmodRef->ui.glWidget->bluepar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparent_3valueChanged(int value)
{
    MathmodRef->ui.glWidget->transparencypar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparence_3toggled(bool checked)
{
    MathmodRef->ui.glWidget->transparence(checked);
}

// --------------------------
void DrawingOptions::on_XhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->xg_valueChanged(value);
}

// --------------------------
void DrawingOptions::on_YhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->yg_valueChanged(value);
}

// --------------------------
void DrawingOptions::on_ZhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->zg_valueChanged(value);
}

// --------------------------
void DrawingOptions::on_IsoComponent_activated(int index)
{
    UpdateDescription(index);
}

// --------------------------
void DrawingOptions::on_ParamComponent_activated(int index)
{
    UpdateDescription(index);
}

// --------------------------
void DrawingOptions::createActions()
{
    /*
     QAction* newAct = new QAction(tr("&Select"), this);
     newAct->setShortcuts(QKeySequence::New);
     newAct->setStatusTip(tr("Add Current Model to MySelection"));
     connect(newAct, SIGNAL(triggered()), this, SLOT(slot_pushButton_2_clicked()));

     QAction*  openAct = new QAction(tr("&Deselect"), this);
     openAct->setShortcuts(QKeySequence::Open);
     openAct->setStatusTip(tr("Erase selected row from MySelection"));
     connect(openAct, SIGNAL(triggered()), this, SLOT(slot_unselect_clicked()));

     QMenu*  qm;
     qm->addAction(newAct);
     qm->addAction(newAct);
     */
}

// --------------------------
void DrawingOptions::on_xyzcheckBox2_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.animxyz *= -1;
}

// --------------------------
void DrawingOptions::on_xycheckBox2_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.animx *= -1;
}

// --------------------------
void DrawingOptions::on_xyhorizontalScrollBar2_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.animxValueStep =(float) value/4.0;
}

// --------------------------
void DrawingOptions::on_xzcheckBox2_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.animy *= -1;
}

// --------------------------
void DrawingOptions::on_xzhorizontalScrollBar2_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.animyValueStep =(float) value/4.0;
}

// --------------------------
void DrawingOptions::on_yzcheckBox2_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.animz *= -1;
}

// --------------------------
void DrawingOptions::on_yzhorizontalScrollBar2_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.animzValueStep =(float) value/4.0;
}

// --------------------------
void DrawingOptions::on_InitMatrix_2_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.RotStrength =
        MathmodRef->ui.glWidget->LocalScene.animxValueStep =
            MathmodRef->ui.glWidget->LocalScene.animyValueStep =
                MathmodRef->ui.glWidget->LocalScene.animzValueStep = 0.0;
}

// --------------------------
void DrawingOptions::on_xycheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxy_ok *= -1;
}

// --------------------------
void DrawingOptions::on_xyhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxy = value/4.0;
}

// --------------------------
void DrawingOptions::on_xzcheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxz_ok *= -1;
}

// --------------------------
void DrawingOptions::on_xzhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxz = value/4.0;
}

// --------------------------
void DrawingOptions::on_yzcheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetayz_ok *= -1;
}

// --------------------------
void DrawingOptions::on_yzhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetayz = value/4.0;
}

// --------------------------
void DrawingOptions::on_xwcheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxw_ok *= -1;
}

// --------------------------
void DrawingOptions::on_xwhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetaxw = value/4.0;
}

// --------------------------
void DrawingOptions::on_ywcheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetayw_ok *= -1;
}

// --------------------------
void DrawingOptions::on_ywhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetayw = value/4.0;
}

// --------------------------
void DrawingOptions::on_zwcheckBox_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetazw_ok *= -1;
}

// --------------------------
void DrawingOptions::on_zwhorizontalScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->tetazw = value/4.0;
}

// --------------------------
void DrawingOptions::on_linecolumn_3_valueChanged(int value)
{
    MathmodRef->linecolumn_valueChanged_2(value);
}

// --------------------------
void DrawingOptions::oncolor_4activated(int index)
{
    MathmodRef->ui.glWidget->colorstypeParam(index);
}

// --------------------------
void DrawingOptions::onred_4valueChanged(int value)
{
    MathmodRef->ui.glWidget->redpar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ongreen_4valueChanged(int value)
{
    MathmodRef->ui.glWidget->greenpar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::onblue_4valueChanged(int value)
{
    MathmodRef->ui.glWidget->bluepar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparent_4valueChanged(int value)
{
    MathmodRef->ui.glWidget->transparencypar(value, indexcurrentFormula);
}

// --------------------------
void DrawingOptions::ontransparence_4toggled(bool checked)
{
    MathmodRef->ui.glWidget->transparence(checked);
}

// --------------------------
void DrawingOptions::on_uv4D_clicked()
{
    MathmodRef->slot_uv4D_clicked();
}

// --------------------------
void DrawingOptions::on_uv_clicked()
{
    MathmodRef->slot_uv_clicked();
}

// --------------------------
void DrawingOptions::on_InitMatrix_clicked()
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->mat4D.unit();
    on_calculate_clicked();
}

// --------------------------
void DrawingOptions::on_ActivateCND_clicked(bool checked)
{
    ui.CNDgroupBox->setEnabled(checked);
    MathmodRef->ui.glWidget->LocalScene.activarecnd = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_TCNDcheckBox_clicked(bool checked)
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.DMTrianglesVerifyCND = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_FCNDcheckBox_clicked(bool checked)
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.DFTrianglesVerifyCND = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_TNCNDcheckBox_clicked(bool checked)
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.DMTrianglesNotVerifyCND = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_FNCNDcheckBox_clicked(bool checked)
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.DFTrianglesNotVerifyCND = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_TBordercheckBox_clicked(bool checked)
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.DMTrianglesBorderCND = checked;
    MathmodRef->ui.glWidget->update();
}

// --------------------------
void DrawingOptions::on_actionMesh_triggered()
{
    MathmodRef->Mesh();
}

// --------------------------
void DrawingOptions::on_calculate_clicked()
{
    Run_JsonObject_activeted();
}

// --------------------------
void DrawingOptions::on_actionAbout_2_triggered()
{
    ab.show();
}

// --------------------------
void DrawingOptions::on_actionLoad_json_script_triggered()
{
    LoadNewFileModels(true);
}

// --------------------------
void DrawingOptions::on_actionScreenshot_triggered()
{
    videoplay();
}

// --------------------------
void DrawingOptions::on_actionEditor_triggered()
{
    editorwin();
}

// --------------------------
void DrawingOptions::on_actionColors_triggered()
{
    colorsoptions();
}

// --------------------------
void DrawingOptions::on_anim_clicked()
{
    MathmodRef->ui.glWidget->anim();
}

// --------------------------
void DrawingOptions::on_morph_clicked()
{
    MathmodRef->ui.glWidget->morph();
}

// --------------------------
void DrawingOptions::keyPressEvent ( QKeyEvent *key)
{
    int button = key->key();
    if(button == Qt::Key_A )
        ui.anim->click();
    else if(button == Qt::Key_M)
        ui.morph->click();
}

// --------------------------
void DrawingOptions::on_actionSmooth_2_clicked()
{
    MathmodRef->smoothline();
}

// --------------------------
void DrawingOptions::on_Infos_clicked()
{
    MathmodRef->iso_infos();
}

// --------------------------
void DrawingOptions::on_Mesh_clicked()
{
    MathmodRef->Mesh();
}

// --------------------------
void DrawingOptions::on_Norm_clicked()
{
    MathmodRef->draw_norm_clicked();
}

// --------------------------
void DrawingOptions::on_Fill_clicked()
{
    MathmodRef->fill();
}

// --------------------------
void DrawingOptions::on_Trian_clicked()
{
    MathmodRef->slot_triangles_clicked();
}

// --------------------------
void DrawingOptions::on_actionOpenGL_triggered()
{
    Parameters->on_loadconfig_clicked();
    Parameters->show();
}

// --------------------------
void DrawingOptions::on_cut_clicked()
{
    if(ui.IsoComponent->count() > 1)
    {
        ui.isoNameEdit->setText("");
        on_updateButton_clicked();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Only one component in this mathematical model.");
        msgBox.exec();
    }
}

// --------------------------
void DrawingOptions::on_actionConvert_K3DS_script_triggered()
{
    LoadK3DSurfScript("", 1);
}

// --------------------------
void DrawingOptions::on_actionConfiguration_triggered()
{
    on_actionOpenGL_triggered();
}

// --------------------------
void DrawingOptions::on_actionColors_2_triggered()
{
    on_actionColors_triggered();
}

// --------------------------
void DrawingOptions::on_Load_clicked()
{
    LoadNewFileModels(true);
}

// --------------------------
void DrawingOptions::on_ParamComponent_2_activated(int index)
{
    UpdateDescription(index);
}

// --------------------------
void DrawingOptions::on_updateParam_2_clicked()
{
    on_updateButton_clicked();
}

// --------------------------
void DrawingOptions::Multiplier(int x, int y, int z, QJsonObject &iso, int index)
{
    QString Minx, Miny, Minz, Maxx, Maxy, Maxz;
    QString Difx, Dify, Difz;
    QString componentName, fct, cnd;
    QJsonArray    oldminx, oldminy, oldminz,
                  oldmaxx, oldmaxy, oldmaxz,
                  oldcmpname, oldfxyz, oldcnd;
    bool cndb=false;

    oldminx = iso["Iso3D"].toObject()["Xmin"].toArray();
    oldminy = iso["Iso3D"].toObject()["Ymin"].toArray();
    oldminz = iso["Iso3D"].toObject()["Zmin"].toArray();

    oldmaxx = iso["Iso3D"].toObject()["Xmax"].toArray();
    oldmaxy = iso["Iso3D"].toObject()["Ymax"].toArray();
    oldmaxz = iso["Iso3D"].toObject()["Zmax"].toArray();

    oldcmpname = iso["Iso3D"].toObject()["Component"].toArray();
    oldfxyz = iso["Iso3D"].toObject()["Fxyz"].toArray();

    if((cndb = (iso["Iso3D"].toObject()["Cnd"].isArray())))
        cnd = ( oldcnd = iso["Iso3D"].toObject()["Cnd"].toArray())[index].toString();

    componentName = oldcmpname[index].toString();
    fct = oldfxyz[index].toString();

    Minx = "("+oldminx[index].toString()+")";
    Miny = "("+oldminy[index].toString()+")";
    Minz = "("+oldminz[index].toString()+")";

    Maxx = "("+oldmaxx[index].toString()+")";
    Maxy = "("+oldmaxy[index].toString()+")";
    Maxz = "("+oldmaxz[index].toString()+")";

    Difx = "(("+Maxx+"-"+Minx+")/"+QString::number(x)+")";
    Dify = "(("+Maxy+"-"+Miny+")/"+QString::number(y)+")";
    Difz = "(("+Maxz+"-"+Minz+")/"+QString::number(z)+")";
    int l =0;
    for(int i =0; i<x; i++)
        for(int j=0; j<y; j++)
            for(int k =0; k<z; k++)
            {
                oldminx.append(Minx+"+"+QString::number(i)+"*"+Difx);
                oldmaxx.append(Minx+"+"+QString::number(i+1)+"*"+Difx);

                oldminy.append(Miny+"+"+QString::number(j)+"*"+Dify);
                oldmaxy.append(Miny+"+"+QString::number(j+1)+"*"+Dify);

                oldminz.append(Minz+"+"+QString::number(k)+"*"+Difz);
                oldmaxz.append(Minz+"+"+QString::number(k+1)+"*"+Difz);

                oldcmpname.append(componentName+QString::number(l));

                oldfxyz.append(fct);

                if(cndb)
                    oldcnd.append(cnd);

                l++;
            }

    oldminx.removeAt(index);
    oldminy.removeAt(index);
    oldminz.removeAt(index);

    oldmaxx.removeAt(index);
    oldmaxy.removeAt(index);
    oldmaxz.removeAt(index);

    oldcmpname.removeAt(index);
    oldfxyz.removeAt(index);
    if(cndb)
        oldcnd.removeAt(index);

    QJsonObject tmp = iso["Iso3D"].toObject();
    tmp["Xmin"] = oldminx;
    tmp["Ymin"] = oldminy;
    tmp["Zmin"] = oldminz;
    tmp["Xmax"] = oldmaxx;
    tmp["Ymax"] = oldmaxy;
    tmp["Zmax"] = oldmaxz;
    tmp["Component"] = oldcmpname;
    tmp["Fxyz"] = oldfxyz;
    if(cndb)
        tmp["Cnd"] = oldcnd;
    iso["Iso3D"] = tmp;
}

// --------------------------
void DrawingOptions::on_Multiplier_clicked()
{
    int i = ui.xcomboBox->currentIndex(),
        j = ui.ycomboBox->currentIndex(),
        k = ui.zcomboBox->currentIndex();
    if(i >0 && j > 0 && k > 0)
    {
        QJsonObject copyCurrentObject = MathmodRef->RootObjet.CurrentJsonObject;
        //Erase unused Keys:
        copyCurrentObject.remove("Param3D");
        copyCurrentObject.remove("Param4D");

        Multiplier(i, j, k, copyCurrentObject, indexcurrentFormula);
        // Draw here
        DrawJsonModel(copyCurrentObject);
    }
    //Init Combo
    ui.xcomboBox->setCurrentIndex(0);
    ui.ycomboBox->setCurrentIndex(0);
    ui.zcomboBox->setCurrentIndex(0);
}

// --------------------------
void DrawingOptions::UpdateGui(int argc)
{
    ui.xyzg->setMaximum(Parameters->MaxGrid);
    ui.XhorizontalScrollBar->setMaximum(Parameters->MaxGrid);
    ui.YhorizontalScrollBar->setMaximum(Parameters->MaxGrid);
    ui.ZhorizontalScrollBar->setMaximum(Parameters->MaxGrid);

    int sqr = (Parameters->MaxPt > Parameters->MaxTri ? sqrt(Parameters->MaxTri):sqrt(Parameters->MaxPt));
    ui.linecolumn_2->setMaximum(sqr);
    ui.linecolumn_3->setMaximum(sqr);

    ListeModelTexture LstModelTexture =
        (Parameters->LoadCollectionModels(
             JSONMathModels,
             MathmodRef->pariso,
             argc));

    //Load the script containing isosurface and parametric formulas:
    ui.choice->insertItems(0, LstModelTexture.listeModels);
    ui.comboBoxTexture->insertItems(0, LstModelTexture.listeTextures);
    ui.comboBoxPigment->insertItems(0, LstModelTexture.listePigments);
    AddListModels();

    //Threads setting:
    SetThreadValues(Parameters->Threads);

    //OpenGl specular:
    SetSpecularValues(Parameters->Specular);

    //OpenGl shininess:
    SetShininessValue(Parameters->Shininess);

    //Show the two windows of the application:
    move(Parameters->ControlX,Parameters->ControlY);
    resize(Parameters->ControlW, Parameters->ControlH);
    //ui.CndGroupBox->hide();
    //ui.NameLabel->hide();
    MathmodRef->move(Parameters->GlwinX, Parameters->GlwinY);
    MathmodRef->resize(Parameters->GlwinW, Parameters->GlwinH);
    //Pigment/texture
    ui.textureEdit->hide();
    //Hide all sliders
    HideSliders();
}

// --------------------------
void DrawingOptions::on_TimeStepScrollBar_valueChanged(int value)
{
    int maxnbthreads= MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->WorkerThreadsNumber;
    ui.label_5->setText("1/"+QString::number(value));

    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->pace = (double)1/(double)value;
    for(int nbthreds=0; nbthreds < maxnbthreads -1; nbthreds++)
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->workerthreads[nbthreds].pace = (double)1/(double)value;

    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->pace = (double)1/(double)value;
    for(int nbthreds=0; nbthreds < maxnbthreads-1; nbthreds++)
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->workerthreads[nbthreds].pace  = (double)1/(double)value;
}

// --------------------------
void DrawingOptions::on_InitTButton_clicked()
{
    int maxnbthreads= MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->WorkerThreadsNumber;

    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->stepMorph = 0;
    for(int nbthreds=0; nbthreds < maxnbthreads-1; nbthreds++)
        MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->workerthreads[nbthreds].stepMorph = 0;

    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->stepMorph = 0;
    for(int nbthreds=0; nbthreds < maxnbthreads-1; nbthreds++)
        MathmodRef->ui.glWidget->ParObjetThread->ParObjet->workerthreads[nbthreds].stepMorph = 0;
}

// --------------------------
void DrawingOptions::on_pushButton_2_clicked()
{
    QJsonParseError err;
    QString sortie;
    QString script  = ui.ParamEdit->toPlainText().trimmed().replace("\n","").replace("\t","").replace("DOTSYMBOL",Parameters->dotsymbol.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(script.toUtf8(), &err);
    if (err.error)
    {
        QMessageBox message ;
        message.setWindowTitle("Error at : ");
        sortie = (script);
        int before, after;
        if(sortie.length() > (err.offset +30))
            after = 30;
        else after = sortie.length() - err.offset;
        sortie.truncate(err.offset +after);
        if(err.offset-30 > 0)
            before = 30;
        else
            before = 0;
        sortie = sortie.remove(0,err.offset-before);
        sortie.replace("\t", " ");
        sortie.replace("\n", " ");
        sortie.insert(before, " >>> Error <<< ");
        message.setText("Error : " + err.errorString() + " at position: " + QString::number(err.offset) + "\n\n***********\n" +
                        "..." + sortie + "..."
                       );
        message.adjustSize () ;
        message.exec();
        return ;
    }

    Parameters->SaveToFile_CurentMathModel(doc.object());
}

// --------------------------
void DrawingOptions::on_pushButton_3_clicked()
{
    ui.isoNameEdit->setText(ui.isoNameEdit->toPlainText() + "_01");
    on_updateButton_clicked();
}

// --------------------------
void DrawingOptions::on_cut_2_clicked()
{
    if(ui.ParamComponent->count() > 1)
    {
        ui.paramNameEdit->setText("");
        on_updateButton_clicked();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Only one component in this mathematical model.");
        msgBox.exec();
    }
}

// --------------------------
void DrawingOptions::on_pushButton_4_clicked()
{
    ui.paramNameEdit->setText(ui.paramNameEdit->toPlainText() + "_01");
    on_updateButton_clicked();
}

// --------------------------
void DrawingOptions::on_color_5_activated(int index)
{
    MathmodRef->ui.glWidget->colorstypeParIso(index);
}

// --------------------------
void DrawingOptions::on_transparence_ParIso_clicked(bool checked)
{
    MathmodRef->ui.glWidget->transparence(checked);
}

// --------------------------
void DrawingOptions::on_transparent_ParIso_valueChanged(int value)
{
    switch (CurrentFormulaType)
    {
    case 1:
        MathmodRef->ui.glWidget->transparencypar(value, indexcurrentFormula);
        break;
    case 2:
        MathmodRef->ui.glWidget->transparency(value, indexcurrentFormula);
        break;
    case 3:
        MathmodRef->ui.glWidget->transparencypar(value, indexcurrentFormula);
        break;
    default:
        break;
    }
}

// --------------------------
void DrawingOptions::on_comboBoxTexture_activated(int index)
{
    QJsonObject tmp;
    tmp = MathmodRef->RootObjet.CurrentJsonObject;

    if(index > 0)
    {
        QJsonDocument document;
        document.setObject(MathmodRef->pariso.JTextures[index - 1].toObject());
        ui.textureEdit->setText(QString (document.toJson()));
        DrawJsonModel(tmp, index-1);
    }
    return;
}

// --------------------------
void DrawingOptions::on_comboBoxPigment_activated(int index)
{
    QJsonObject tmp;
    tmp = MathmodRef->RootObjet.CurrentJsonObject;
    if(index > 0)
    {
        QJsonDocument document;
        document.setObject(MathmodRef->pariso.JPigments[index - 1].toObject());
        ui.textureEdit->setText(QString (document.toJson()));
        DrawJsonModel(tmp, 1000+index-1);
    }
    return;
}

// --------------------------
void DrawingOptions::on_actionTrianglesWavefront_obj_triggered()
{
    MathmodRef->ui.glWidget->SaveSceneAsObjTrian();
}

// --------------------------
void DrawingOptions::on_actionColorTrianglesWavefront_obj_triggered()
{
    MathmodRef->ui.glWidget->SaveSceneAsObjTrian(1);
}

// --------------------------
void DrawingOptions::on_OctavesScrollBar_valueChanged(int value)
{
    int Octaves = MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Octaves = value;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Octaves = Octaves;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Octaves = Octaves;

    ui.OctavesLabel->setText("Octaves = "+QString::number(MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Octaves));
    on_pushButton_5_clicked();
}

// --------------------------
void DrawingOptions::on_LacunarityScrollBar_valueChanged(int value)
{
    float Lacunarity = MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Lacunarity = (float)value/10.0;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Lacunarity = Lacunarity;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Lacunarity = Lacunarity;
    ui.LacunarityLabel->setText("Lacunarity = "+QString::number(MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Lacunarity));
    on_pushButton_5_clicked();
}

// --------------------------
void DrawingOptions::on_GainScrollBar_valueChanged(int value)
{
    float Gain = MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Gain = (float)value/10.0;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->Gain = Gain;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->Gain = Gain;
    ui.GainLabel->setText("Gain = "+QString::number(MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.Gain));
    on_pushButton_5_clicked();
}

// --------------------------
void DrawingOptions::on_ShowtextureScript_clicked()
{
    static int show = -1;
    show *= -1;
    if(show == 1)
    {
        ui.textureEdit->show();
        ui.ShowtextureScript->setText("Hide Script");
    }
    else
    {
        ui.textureEdit->hide();
        ui.ShowtextureScript->setText("Show Script");
    }
}

// --------------------------
void DrawingOptions::on_TurbulenceCheckBox_clicked()
{
    MathmodRef->ui.glWidget->LocalScene.componentsinfos.NoiseParam.NoiseActive *= -1;
    on_pushButton_5_clicked();
}

// --------------------------
void DrawingOptions::on_pushButton_5_clicked()
{
    QJsonParseError err;
    QString sortie;
    QString script  = ui.textureEdit->toPlainText().trimmed().replace("\n","").replace("\t","").replace("DOTSYMBOL",Parameters->dotsymbol.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(script.toUtf8(), &err);
    if (err.error)
    {
        QMessageBox message ;
        message.setWindowTitle("Error at : ");
        sortie = (script);
        int before, after;
        if(sortie.length() > (err.offset +30))
            after = 30;
        else after = sortie.length() - err.offset;
        sortie.truncate(err.offset +after);
        if(err.offset-30 > 0)
            before = 30;
        else
            before = 0;
        sortie = sortie.remove(0,err.offset-before);
        sortie.replace("\t", " ");
        sortie.replace("\n", " ");
        sortie.insert(before, " >>> Error <<< ");
        message.setText("Error : " + err.errorString() + " at position: " + QString::number(err.offset) + "\n\n***********\n" +
                        "..." + sortie + "..."
                       );
        message.adjustSize () ;
        message.exec();
        return;
    }

    QJsonObject tmp = doc.object();
    if(tmp["Texture"].isObject())
    {
        MathmodRef->pariso.JTextures[ui.comboBoxTexture->currentIndex() - 1] = doc.object() ;
        on_comboBoxTexture_activated(ui.comboBoxTexture->currentIndex());
    }
    else
    {
        MathmodRef->pariso.JPigments[ui.comboBoxPigment->currentIndex() - 1] = doc.object();
        on_comboBoxPigment_activated(ui.comboBoxPigment->currentIndex());
    }
}

// --------------------------
void DrawingOptions::on_C1ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;

    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[0] = value;

    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[0] = value;
    ui.C1label->setText(qlstnames.at(0) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C2ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;

    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[1] = value;

    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[1] = value;
    ui.C2label->setText(qlstnames.at(1) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C3ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;

    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[2] = value;

    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[2] = value;

    ui.C3label->setText(qlstnames.at(2) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C4ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[3] = value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[3] = value;
    ui.C4label->setText(qlstnames.at(3) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C5ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[4] = value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[4] = value;
    ui.C5label->setText(qlstnames.at(4) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C6ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[5] = value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[5] = value;
    ui.C6label->setText(qlstnames.at(5) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C7ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[6] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[6] = value;
    ui.C7label->setText(qlstnames.at(6) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C8ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[7] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[7] = value;
    ui.C8label->setText(qlstnames.at(7) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C9ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[8] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[8] = value;
    ui.C9label->setText(qlstnames.at(8) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C10ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[9] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[9] = value;
    ui.C10label->setText(qlstnames.at(9) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C11ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[10] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[10] = value;
    ui.C11label->setText(qlstnames.at(10) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C12ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[11] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[11] = value;
    ui.C12label->setText(qlstnames.at(11) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C13ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[12] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[12] = value;
    ui.C13label->setText(qlstnames.at(12) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C14ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[13] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[13] = value;
    ui.C14label->setText(qlstnames.at(13) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C15ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[14] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[14] = value;
    ui.C15label->setText(qlstnames.at(14) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C16ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[15] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[15] = value;
    ui.C16label->setText(qlstnames.at(15) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C17ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[16] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[16] = value;
    ui.C17label->setText(qlstnames.at(16) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C18ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[17] =value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[17] = value;
    ui.C18label->setText(qlstnames.at(17) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C19ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[18] = value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[18] = value;
    ui.C19label->setText(qlstnames.at(18) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_C20ScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[19] = value;
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[19] = value;
    ui.C20label->setText(qlstnames.at(19) + " = " +QString::number(value));
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

// --------------------------
void DrawingOptions::on_PredefinedSets_activated(int index)
{
    indexcurrentSet = index;
    if(index >0)
    {
        int size = qlstnames.size();
        MathmodRef->ui.glWidget->LocalScene.slider = 1;
        for (int i = 0; i < size; ++i)
        {
            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[i] = qlstPos.at(i+(index-1)*size).toDouble();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[i] = qlstPos.at(i+(index-1)*size).toDouble();
        }


        for(int sl=0; sl<20; sl++)
        {
            if(size >= (sl+1))
            {
                (SliderArray[sl].SliderLabel)->setText(qlstnames.at(sl) + " = " +qlstPos.at(sl+(index-1)*size)+"("+ qlstStep.at(sl) +")");
                (SliderArray[sl].SliderScrollBar)->blockSignals(true);
                if(qlstmin.size() > qlstnames.size())
                {
                    (SliderArray[sl].SliderScrollBar)->setMaximum(qlstmax.at(sl+(index-1)*size).toDouble());
                    (SliderArray[sl].SliderScrollBar)->setMinimum(qlstmin.at(sl+(index-1)*size).toDouble());
                    (SliderArray[sl].SliderScrollBar)->setSingleStep(qlstStep.at(sl+(index-1)*size).toDouble());

                    (SliderArray[sl].SliderLabel)->setText(qlstnames.at(sl) + " = " +qlstPos.at(sl+(index-1)*size)+"("+ qlstStep.at(sl+(index-1)*size) +")");
                    (SliderArray[sl].SliderLabelMin)->setText(qlstmin.at(sl+(index-1)*size));
                    (SliderArray[sl].SliderLabelMax)->setText(qlstmax.at(sl+(index-1)*size));
                }
                (SliderArray[sl].SliderScrollBar)->setSliderPosition(qlstPos.at(sl+(index-1)*size).toDouble());
                (SliderArray[sl].SliderScrollBar)->blockSignals(false);
            }
        }

        //Draw
        if(CurrentFormulaType==2)
            MathmodRef->ProcessNewIsoSurface( );
        else
            MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
        MathmodRef->ui.glWidget->LocalScene.slider = -1;
    }
}

// --------------------------
void DrawingOptions::on_C20toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 19 : (indexcurrentSet-1)*qlstnames.size() + 19;
    sliderconf.currentSlider=19;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(20);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C19toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 18 : (indexcurrentSet-1)*qlstnames.size() + 18;
    sliderconf.currentSlider=18;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(19);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C18toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 17 : (indexcurrentSet-1)*qlstnames.size() + 17;
    sliderconf.currentSlider=17;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(18);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C17toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 16 : (indexcurrentSet-1)*qlstnames.size() + 16;
    sliderconf.currentSlider=16;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(17);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C16toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 15 : (indexcurrentSet-1)*qlstnames.size() + 15;
    sliderconf.currentSlider=15;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(16);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C15toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 14 : (indexcurrentSet-1)*qlstnames.size() + 14;
    sliderconf.currentSlider=14;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(15);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C14toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 13 : (indexcurrentSet-1)*qlstnames.size() + 13;
    sliderconf.currentSlider=13;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(14);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C13toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 12 : (indexcurrentSet-1)*qlstnames.size() + 12;
    sliderconf.currentSlider=12;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(13);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C12toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 11 : (indexcurrentSet-1)*qlstnames.size() + 11;
    sliderconf.currentSlider=11;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(12);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C11toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 10 : (indexcurrentSet-1)*qlstnames.size() + 10;
    sliderconf.currentSlider=10;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(11);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C10toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 9 : (indexcurrentSet-1)*qlstnames.size() + 9;
    sliderconf.currentSlider=9;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(10);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C9toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 8 : (indexcurrentSet-1)*qlstnames.size() + 8;
    sliderconf.currentSlider=8;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(9);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C8toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 7 : (indexcurrentSet-1)*qlstnames.size() + 7;
    sliderconf.currentSlider=7;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(8);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C7toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 6 : (indexcurrentSet-1)*qlstnames.size() + 6;
    sliderconf.currentSlider=6;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(7);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C6toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 5 : (indexcurrentSet-1)*qlstnames.size() + 5;
    sliderconf.currentSlider=5;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(6);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C5toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 4 : (indexcurrentSet-1)*qlstnames.size() + 4;
    sliderconf.currentSlider=4;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(5);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C4toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 3 : (indexcurrentSet-1)*qlstnames.size() + 3;
    sliderconf.currentSlider=3;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(4);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C3toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 2 : (indexcurrentSet-1)*qlstnames.size() + 2;
    sliderconf.currentSlider=2;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(3);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C2toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 1 : (indexcurrentSet-1)*qlstnames.size() + 1;
    sliderconf.currentSlider=1;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(2);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::on_C1toolButton_clicked()
{
    int range = (indexcurrentSet <1) ? 0 : (indexcurrentSet-1)*qlstnames.size() + 0;
    sliderconf.currentSlider=0;
    sliderconf.ui.MaxEdit->setText(qlstmax[range]);
    sliderconf.ui.MinEdit->setText(qlstmin[range]);
    sliderconf.ui.StepEdit->setText(qlstStep[range]);
    sliderconf.ui.PosEdit->setText(qlstPos[range]);
    sliderconf.ui.ParametersComboBox->blockSignals(true);
    sliderconf.ui.ParametersComboBox->setCurrentIndex(1);
    sliderconf.ui.ParametersComboBox->blockSignals(false);
    sliderconf.show();
}

// --------------------------
void DrawingOptions::update_slider_param()
{
    int SliderIndex=sliderconf.currentSlider;

    for(int sl=0; sl<20; sl++)
    {
        if(SliderIndex==sl)
        {
            qlstmax[sl] = sliderconf.ui.MaxEdit->text();
            qlstmin[sl] = sliderconf.ui.MinEdit->text();
            qlstStep[sl] = sliderconf.ui.StepEdit->text();
            qlstPos[sl] = sliderconf.ui.PosEdit->text();

            (SliderArray[sl].SliderScrollBar)->blockSignals(true);
            (SliderArray[sl].SliderScrollBar)->setMaximum(qlstmax.at(sl).toDouble());
            (SliderArray[sl].SliderScrollBar)->setMinimum(qlstmin.at(sl).toDouble());
            (SliderArray[sl].SliderScrollBar)->setSingleStep(qlstStep.at(sl).toDouble());
            (SliderArray[sl].SliderScrollBar)->setPageStep(qlstStep.at(sl).toDouble());
            (SliderArray[sl].SliderScrollBar)->setSliderPosition(qlstPos.at(sl).toDouble());
            (SliderArray[sl].SliderLabel)->setText(qlstnames.at(sl) + " = " +qlstPos.at(sl)+"("+ qlstStep.at(sl) +")");
            (SliderArray[sl].SliderLabelMin)->setText(qlstmin.at(sl));
            (SliderArray[sl].SliderLabelMax)->setText(qlstmax.at(sl));
            (SliderArray[sl].SliderScrollBar)->blockSignals(false);

            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderNames[sl] = qlstnames.at(sl).toStdString();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderNames[sl] = qlstnames.at(sl).toStdString();

            MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->masterthread->SliderValues[sl] = qlstPos.at(sl).toDouble();
            MathmodRef->ui.glWidget->ParObjetThread->ParObjet->masterthread->SliderValues[sl] = qlstPos.at(sl).toDouble();
        }
    }

    //Draw
    MathmodRef->ui.glWidget->LocalScene.slider = 1;
    if(CurrentFormulaType==2)
        MathmodRef->ProcessNewIsoSurface( );
    else
        MathmodRef->ParametricSurfaceProcess(CurrentFormulaType);
    MathmodRef->ui.glWidget->LocalScene.slider = -1;
}

void DrawingOptions::on_AddSetButton_clicked()
{
    QJsonArray array1, array2, array3, array4, array5;
    QJsonObject tmp, tmp2;

    tmp = MathmodRef->RootObjet.CurrentJsonObject;
    tmp2 = tmp ["Sliders"].toObject();

    array2 = tmp2["Position"].toArray();
    array3 = tmp2["Min"].toArray();
    array4 = tmp2["Max"].toArray();
    array5 = tmp2["Step"].toArray();
    int size= qlstnames.size();

    if(tmp2["SetNames"].isArray())
    {
        array1 = tmp2["SetNames"].toArray();
        array1.append("NewSetName");
    }
    else
        tmp2.remove("SetNames");

    for(int sl=0; sl<20; sl++)
    {
        if(size >=(sl+1))
        {
            array2.append(QString::number((SliderArray[sl].SliderScrollBar)->sliderPosition()));
            array3.append(QString::number((SliderArray[sl].SliderScrollBar)->minimum()));
            array4.append(QString::number((SliderArray[sl].SliderScrollBar)->maximum()));
            array5.append(QString::number((SliderArray[sl].SliderScrollBar)->singleStep()));
        }
    }

    if(tmp2["SetNames"].isArray())
    {
        tmp2["SetNames"] = array1;
    }
    else
        tmp2.remove("SetNames");

    tmp2["Position"] = array2;
    tmp2["Min"]        = array3;
    tmp2["Max"]       = array4;
    tmp2["Step"]      = array5;
    tmp["Sliders"]    = tmp2;
    // Draw here
    DrawJsonModel(tmp);
}

void DrawingOptions::on_CutSetButton_clicked()
{
    int size2= qlstPos.size();
    int size= qlstnames.size();
    if(size2 >= 2*size)
    {
        QJsonArray array1, array2, array3, array4, array5;
        QJsonObject tmp, tmp2;

        tmp = MathmodRef->RootObjet.CurrentJsonObject;
        tmp2 = tmp ["Sliders"].toObject();

        if(tmp2["SetNames"].isArray())
        {
            array1 = tmp2["SetNames"].toArray();
            array1.removeAt(indexcurrentSet-1);
        }

        array2 = tmp2["Position"].toArray();
        for(int i=0; i<size; i++)
            array2.removeAt((indexcurrentSet-1)*size);

        array3 = tmp2["Min"].toArray();
        for(int i=0; i<size; i++)
            array3.removeAt((indexcurrentSet-1)*size);

        array4 = tmp2["Max"].toArray();
        for(int i=0; i<size; i++)
            array4.removeAt((indexcurrentSet-1)*size);

        array5 = tmp2["Step"].toArray();
        for(int i=0; i<size; i++)
            array5.removeAt((indexcurrentSet-1)*size);

        if(tmp2["SetNames"].isArray())
        {
            tmp2["SetNames"]  = array1;
        }
        else
            tmp2.remove("SetNames");

        tmp2["Position"]  = array2;
        tmp2["Min"]         = array3;
        tmp2["Max"]        = array4;
        tmp2["Step"]       = array5;

        tmp["Sliders"] = tmp2;
        // Draw here
        DrawJsonModel(tmp);
    }
}

void DrawingOptions::on_AddParam_clicked()
{
    addnewparam.ui.NameEdit->setText("ParamName");
    addnewparam.ui.MaxEdit->setText("50");
    addnewparam.ui.MinEdit->setText("0");
    addnewparam.ui.StepEdit->setText("1");
    addnewparam.ui.PosEdit->setText("10");
    addnewparam.show();
}

void DrawingOptions::on_CutParam_clicked()
{
    int index = ui.ParametersList->currentIndex();
    if(index > 0)
    {
        QJsonArray array2;
        QJsonObject tmp, tmp2;
        int names, position;
        tmp = MathmodRef->RootObjet.CurrentJsonObject;
        tmp2 = tmp ["Sliders"].toObject();

        array2 = tmp2["Name"].toArray();
        names = array2.size();
        array2.removeAt(index-1);
        tmp2["Name"] = array2;

        array2 = tmp2["Max"].toArray();
        position = array2.size();
        for(int i=0; i*names < position; i++)
            array2.removeAt(index-1-i + i*names);
        tmp2["Max"] = array2;

        array2 = tmp2["Min"].toArray();
        position = array2.size();
        for(int i=0; i*names < position; i++)
            array2.removeAt(index-1-i + i*names);
        tmp2["Min"] = array2;

        array2 = tmp2["Step"].toArray();
        position = array2.size();
        for(int i=0; i*names < position; i++)
            array2.removeAt(index-1 -i+ i*names);
        tmp2["Step"] = array2;

        array2 = tmp2["Position"].toArray();
        position = array2.size();
        for(int i=0; i*names < position; i++)
            array2.removeAt(index-1 -i+ i*names);
        tmp2["Position"] = array2;

        tmp["Sliders"] = tmp2;
        // Draw here
        DrawJsonModel(tmp);
    }
}

void DrawingOptions::add_new_param()
{
    QJsonArray array2;
    QJsonObject tmp, tmp2;
    int names, position;
    tmp = MathmodRef->RootObjet.CurrentJsonObject;
    tmp2 = tmp ["Sliders"].toObject();

    array2 = tmp2["Name"].toArray();
    names = array2.empty()? 0: array2.size();
    array2.append(addnewparam.ui.NameEdit->text());
    tmp2["Name"] = array2;

    array2 = tmp2["Position"].toArray();
    position = array2.empty()? 0: array2.size();
    if(names == 0)
        array2.append(addnewparam.ui.PosEdit->text());
    else
        for(int i=1; i*names <= position; i++)
            array2.insert(i*names+i-1, addnewparam.ui.PosEdit->text());
    tmp2["Position"] = array2;


    array2 = tmp2["Max"].toArray();
    position = array2.empty()? 0: array2.size();
    if(names == 0)
        array2.append(addnewparam.ui.MaxEdit->text());
    else
        for(int i=1; i*names <= position; i++)
            array2.insert(i*names+i-1, addnewparam.ui.MaxEdit->text());
    tmp2["Max"] = array2;


    array2 = tmp2["Min"].toArray();
    position = array2.empty()? 0: array2.size();
    if(names == 0)
        array2.append(addnewparam.ui.MinEdit->text());
    else
        for(int i=1; i*names <= position; i++)
            array2.insert(i*names+i-1, addnewparam.ui.MinEdit->text());
    tmp2["Min"] = array2;


    array2 = tmp2["Step"].toArray();
    position = array2.empty()? 0: array2.size();
    if(names == 0)
        array2.append(addnewparam.ui.StepEdit->text());
    else
        for(int i=1; i*names <= position; i++)
            array2.insert(i*names+i-1, addnewparam.ui.StepEdit->text());
    tmp2["Step"] = array2;


    tmp["Sliders"] = tmp2;
    // Draw here
    DrawJsonModel(tmp);
}

void DrawingOptions::update_infos_param(int index)
{
    switch(index)
    {
    case 1 :
        on_C1toolButton_clicked();
        break;
    case 2 :
        on_C2toolButton_clicked();
        break;
    case 3 :
        on_C3toolButton_clicked();
        break;
    case 4 :
        on_C4toolButton_clicked();
        break;
    case 5 :
        on_C5toolButton_clicked();
        break;
    case 6 :
        on_C6toolButton_clicked();
        break;
    case 7 :
        on_C7toolButton_clicked();
        break;
    case 8 :
        on_C8toolButton_clicked();
        break;
    case 9 :
        on_C9toolButton_clicked();
        break;
    case 10 :
        on_C10toolButton_clicked();
        break;
    case 11 :
        on_C11toolButton_clicked();
        break;
    case 12 :
        on_C12toolButton_clicked();
        break;
    case 13 :
        on_C13toolButton_clicked();
        break;
    case 14 :
        on_C14toolButton_clicked();
        break;
    case 15 :
        on_C15toolButton_clicked();
        break;
    case 16 :
        on_C16toolButton_clicked();
        break;
    case 17 :
        on_C17toolButton_clicked();
        break;
    case 18 :
        on_C18toolButton_clicked();
        break;
    case 19 :
        on_C19toolButton_clicked();
        break;
    case 20 :
        on_C20toolButton_clicked();
        break;
    }
    sliderconf.currentSlider = index -1;
}

void DrawingOptions::on_actionFrames_triggered()
{
    MathmodRef->frames_clicked();
}

void DrawingOptions::SetSpecularValues(float *spec)
{
    ui.red_Specular->blockSignals(true);
    ui.red_Specular->setSliderPosition((int)(spec[0]*100));
    ui.red_Specular->blockSignals(false);

    ui.green_Specular->blockSignals(true);
    ui.green_Specular->setSliderPosition((int)(spec[1]*100));
    ui.green_Specular->blockSignals(false);

    ui.blue_Specular->blockSignals(true);
    ui.blue_Specular->setSliderPosition((int)(spec[2]*100));
    ui.blue_Specular->blockSignals(false);

    ui.transparent_Specular->blockSignals(true);
    ui.transparent_Specular->setSliderPosition((int)(spec[3]*100));
    ui.transparent_Specular->blockSignals(false);
}

void DrawingOptions::SetThreadValues(int *thread)
{
    ui.ThreadNumberScrollBar->blockSignals(true);
    ui.ThreadNumberScrollBar->setSliderPosition(thread[0]);
    ui.ThreadNumberScrollBar->setMinimum(thread[1]);
    ui.ThreadNumberScrollBar->setMaximum(thread[2]);
    ui.ThreadgroupBox->setTitle("Threads: "+ QString::number(ui.ThreadNumberScrollBar->minimum())  +"  <  "+QString::number(thread[0])+"  <  "+ QString::number(ui.ThreadNumberScrollBar->maximum()));
    ui.ThreadNumberScrollBar->blockSignals(false);
}

void DrawingOptions::SetShininessValue(int shin)
{
    ui.ShininessScrollBar->blockSignals(true);
    ui.ShininessScrollBar->setSliderPosition(shin);
    ui.ShininessScrollBar->blockSignals(false);
}
void DrawingOptions::on_red_Specular_valueChanged(int value)
{
    MathmodRef->ui.glWidget->redSpec(value);
    if(ui.GcheckBox->isChecked())
    {
        ui.green_Specular->blockSignals(true);
        ui.green_Specular->setSliderPosition(value);
        ui.green_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->greenSpec(value);
    }
    if(ui.BcheckBox->isChecked())
    {
        ui.blue_Specular->blockSignals(true);
        ui.blue_Specular->setSliderPosition(value);
        ui.blue_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->blueSpec(value);
    }
    MathmodRef->ui.glWidget->InitSpecularParameters();
}

void DrawingOptions::on_green_Specular_valueChanged(int value)
{
    MathmodRef->ui.glWidget->greenSpec(value);
    if(ui.RcheckBox->isChecked())
    {
        ui.red_Specular->blockSignals(true);
        ui.red_Specular->setSliderPosition(value);
        ui.red_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->redSpec(value);
    }
    if(ui.BcheckBox->isChecked())
    {
        ui.blue_Specular->blockSignals(true);
        ui.blue_Specular->setSliderPosition(value);
        ui.blue_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->blueSpec(value);
    }
    MathmodRef->ui.glWidget->InitSpecularParameters();
}

void DrawingOptions::on_blue_Specular_valueChanged(int value)
{
    MathmodRef->ui.glWidget->blueSpec(value);
    if(ui.GcheckBox->isChecked())
    {
        ui.green_Specular->blockSignals(true);
        ui.green_Specular->setSliderPosition(value);
        ui.green_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->greenSpec(value);
    }
    if(ui.RcheckBox->isChecked())
    {
        ui.red_Specular->blockSignals(true);
        ui.red_Specular->setSliderPosition(value);
        ui.red_Specular->blockSignals(false);
        MathmodRef->ui.glWidget->redSpec(value);
    }
    MathmodRef->ui.glWidget->InitSpecularParameters();
}

void DrawingOptions::on_transparent_Specular_valueChanged(int value)
{
    MathmodRef->ui.glWidget->transSpec(value);
}

void DrawingOptions::on_ShininessScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->Shininess(value);
}

void DrawingOptions::on_ThreadNumberScrollBar_valueChanged(int value)
{
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->UpdateThredsNumber(value);
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->UpdateThredsNumber(value);
    ui.ThreadgroupBox->setTitle("Threads: "+ QString::number(ui.ThreadNumberScrollBar->minimum())  +"  <  "+QString::number(value)+"  <  "+ QString::number(ui.ThreadNumberScrollBar->maximum()));
}

void DrawingOptions::on_StopCalculationsButton_clicked()
{
    MathmodRef->ui.glWidget->IsoObjetThread->IsoObjet->stopcalculations(true);
    MathmodRef->ui.glWidget->ParObjetThread->ParObjet->stopcalculations(true);
}

void DrawingOptions::on_stopButton_clicked()
{
    on_StopCalculationsButton_clicked();
}

void DrawingOptions::on_actionScripts_selection_triggered()
{
    select.show();
}

void DrawingOptions::on_actionBoundingBox_clicked()
{
    MathmodRef->ui.glWidget->boundingboxOk();
    MathmodRef->ui.glWidget->update();
}
