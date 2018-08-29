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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fp
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *start_fp;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *set_button;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QLabel *cLBLabel;
    QLineEdit *cLBLineEdit;
    QLabel *bRAMLabel;
    QLineEdit *bRAMLineEdit;
    QLabel *dSPLabel;
    QLineEdit *dSPLineEdit;
    QComboBox *comboBox_2;
    QLabel *slotLabel;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *comboBox;
    QPushButton *enter_button;
    QWidget *formLayoutWidget_3;
    QFormLayout *formLayout_3;
    QLabel *num_slotsLabel_2;
    QLineEdit *num_slotsLineEdit_2;
    QGraphicsView *graphicsView;

    void setupUi(QDialog *fp)
    {
        if (fp->objectName().isEmpty())
            fp->setObjectName(QStringLiteral("fp"));
        fp->resize(1557, 1278);
        verticalLayoutWidget = new QWidget(fp);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(60, 350, 91, 51));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        start_fp = new QPushButton(verticalLayoutWidget);
        start_fp->setObjectName(QStringLiteral("start_fp"));

        verticalLayout->addWidget(start_fp);

        horizontalLayoutWidget = new QWidget(fp);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(50, 120, 82, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        set_button = new QPushButton(horizontalLayoutWidget);
        set_button->setObjectName(QStringLiteral("set_button"));

        horizontalLayout->addWidget(set_button);

        horizontalLayoutWidget_2 = new QWidget(fp);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(10, 30, 171, 80));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        formLayoutWidget_2 = new QWidget(fp);
        formLayoutWidget_2->setObjectName(QStringLiteral("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(10, 160, 183, 132));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        cLBLabel = new QLabel(formLayoutWidget_2);
        cLBLabel->setObjectName(QStringLiteral("cLBLabel"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, cLBLabel);

        cLBLineEdit = new QLineEdit(formLayoutWidget_2);
        cLBLineEdit->setObjectName(QStringLiteral("cLBLineEdit"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, cLBLineEdit);

        bRAMLabel = new QLabel(formLayoutWidget_2);
        bRAMLabel->setObjectName(QStringLiteral("bRAMLabel"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, bRAMLabel);

        bRAMLineEdit = new QLineEdit(formLayoutWidget_2);
        bRAMLineEdit->setObjectName(QStringLiteral("bRAMLineEdit"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, bRAMLineEdit);

        dSPLabel = new QLabel(formLayoutWidget_2);
        dSPLabel->setObjectName(QStringLiteral("dSPLabel"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, dSPLabel);

        dSPLineEdit = new QLineEdit(formLayoutWidget_2);
        dSPLineEdit->setObjectName(QStringLiteral("dSPLineEdit"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, dSPLineEdit);

        comboBox_2 = new QComboBox(formLayoutWidget_2);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox_2);

        slotLabel = new QLabel(formLayoutWidget_2);
        slotLabel->setObjectName(QStringLiteral("slotLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, slotLabel);

        horizontalLayoutWidget_3 = new QWidget(fp);
        horizontalLayoutWidget_3->setObjectName(QStringLiteral("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(10, 0, 171, 51));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        comboBox = new QComboBox(horizontalLayoutWidget_3);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout_3->addWidget(comboBox);

        enter_button = new QPushButton(fp);
        enter_button->setObjectName(QStringLiteral("enter_button"));
        enter_button->setGeometry(QRect(60, 310, 89, 25));
        formLayoutWidget_3 = new QWidget(fp);
        formLayoutWidget_3->setObjectName(QStringLiteral("formLayoutWidget_3"));
        formLayoutWidget_3->setGeometry(QRect(10, 70, 181, 41));
        formLayout_3 = new QFormLayout(formLayoutWidget_3);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        num_slotsLabel_2 = new QLabel(formLayoutWidget_3);
        num_slotsLabel_2->setObjectName(QStringLiteral("num_slotsLabel_2"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, num_slotsLabel_2);

        num_slotsLineEdit_2 = new QLineEdit(formLayoutWidget_3);
        num_slotsLineEdit_2->setObjectName(QStringLiteral("num_slotsLineEdit_2"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, num_slotsLineEdit_2);

        graphicsView = new QGraphicsView(fp);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(290, 20, 1161, 911));

        retranslateUi(fp);

        QMetaObject::connectSlotsByName(fp);
    } // setupUi

    void retranslateUi(QDialog *fp)
    {
        fp->setWindowTitle(QApplication::translate("fp", "fp", nullptr));
        start_fp->setText(QApplication::translate("fp", "start_fp", nullptr));
        set_button->setText(QApplication::translate("fp", "set", nullptr));
        cLBLabel->setText(QApplication::translate("fp", "CLB", nullptr));
        bRAMLabel->setText(QApplication::translate("fp", "BRAM", nullptr));
        dSPLabel->setText(QApplication::translate("fp", "DSP", nullptr));
        slotLabel->setText(QApplication::translate("fp", "slot", nullptr));
        enter_button->setText(QApplication::translate("fp", "Enter", nullptr));
        num_slotsLabel_2->setText(QApplication::translate("fp", "num_slots", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fp: public Ui_fp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FP_H
