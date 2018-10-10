/********************************************************************************
** Form generated from reading UI file 'fp.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FP_H
#define UI_FP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fp
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *fp)
    {
        if (fp->objectName().isEmpty())
            fp->setObjectName(QStringLiteral("fp"));
        fp->resize(1329, 828);
        centralWidget = new QWidget(fp);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(270, 220, 89, 25));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(560, 150, 89, 25));
        fp->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(fp);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1329, 22));
        fp->setMenuBar(menuBar);
        mainToolBar = new QToolBar(fp);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        fp->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(fp);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        fp->setStatusBar(statusBar);

        retranslateUi(fp);

        QMetaObject::connectSlotsByName(fp);
    } // setupUi

    void retranslateUi(QMainWindow *fp)
    {
        fp->setWindowTitle(QApplication::translate("fp", "fp", nullptr));
        pushButton->setText(QApplication::translate("fp", "PushButton", nullptr));
        pushButton_3->setText(QApplication::translate("fp", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fp: public Ui_fp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FP_H
