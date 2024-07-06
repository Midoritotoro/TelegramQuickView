#pragma once

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QFrame* frame;
    QPushButton* loginButton;
    QLineEdit* lineEdit;
    QLineEdit* lineEdit_2;
    QLineEdit* lineEdit_3;
    QToolButton* toolButton;

    void setupUi(QDialog* Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(720, 720);
        Dialog->setStyleSheet(QString::fromUtf8("*{\n"
            "font-family: centry gothic;\n"
            "font-size: 24px;\n"
            "}"));
        frame = new QFrame(Dialog);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(90, 120, 550, 550));
        frame->setStyleSheet(QString::fromUtf8("QFrame{ \n"
            "background: #333;\n"
            "border-radius: 15px\n"
            "}"));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        loginButton = new QPushButton(frame);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(50, 450, 450, 80));
        loginButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
            "background: red;\n"
            "border-radius: 15px;\n"
            "color: white;\n"
            "}\n"
            "QPushButton::hover { \n"
            "color: red;\n"
            "border-radius: 15px;\n"
            "background: #49ebff\n"
            "}"));
        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(50, 130, 450, 24));
        lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
            "background: transparent;\n"
            "border: none;\n"
            "color: #717072;\n"
            "border-bottom: 1px solid #717072;\n"
            "}\n"
            ""));
        lineEdit_2 = new QLineEdit(frame);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(50, 240, 450, 24));
        lineEdit_2->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
            "background: transparent;\n"
            "border: none;\n"
            "color: #717072;\n"
            "border-bottom: 1px solid #717072;\n"
            "}\n"
            ""));
        lineEdit_3 = new QLineEdit(frame);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setGeometry(QRect(50, 360, 450, 24));
        lineEdit_3->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
            "background: transparent;\n"
            "border: none;\n"
            "color: #717072;\n"
            "border-bottom: 1px solid #717072;\n"
            "}\n"
            ""));
        toolButton = new QToolButton(Dialog);
        toolButton->setObjectName("toolButton");
        toolButton->setGeometry(QRect(310, 60, 120, 120));
        toolButton->setStyleSheet(QString::fromUtf8("QToolButton{ \n"
            "background: red;\n"
            "border-radius: 60px;\n"
            "}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/assets/images/auth.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton->setIcon(icon);
        toolButton->setIconSize(QSize(48, 48));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    }

    void retranslateUi(QDialog* Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        loginButton->setText(QCoreApplication::translate("Dialog", "\320\222\320\276\320\271\321\202\320\270", nullptr));
        lineEdit->setText(QCoreApplication::translate("Dialog", "Api Hash", nullptr));
        lineEdit_2->setText(QCoreApplication::translate("Dialog", "Api Id", nullptr));
        lineEdit_3->setText(QCoreApplication::translate("Dialog", "\320\242\320\265\320\273\320\265\321\204\320\276\320\275\320\275\321\213\320\271 \320\275\320\276\320\274\320\265\321\200", nullptr));
        toolButton->setText(QString());
    }

};

namespace Ui {
    class Dialog : public Ui_Dialog {};
}

QT_END_NAMESPACE