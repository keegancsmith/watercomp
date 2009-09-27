#include "view_preference.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

ViewPreferenceDialog::ViewPreferenceDialog(QWidget* parent)
    : QDialog(parent)
{
    layout = new QVBoxLayout;

    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget);

    buttonLayout = new QHBoxLayout;

    // okButton = new QPushButton(tr("&Ok"), this);
    // connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    // buttonLayout->addWidget(okButton);

    cancelButton = new QPushButton(tr("&Close"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout);

    setLayout(layout);

    setWindowTitle(tr("View preferences"));
    setModal(false);
}//constructor

ViewPreferenceDialog::~ViewPreferenceDialog()
{
}//destructor


int ViewPreferenceDialog::addTab(QWidget* page, QString& label)
{
    return tabWidget->addTab(page, label);
}//addTab

void ViewPreferenceDialog::setTabPage(int index)
{
    tabWidget->setCurrentIndex(index);
}//setTabPage

