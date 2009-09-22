#ifndef VIEW_PREFERENCE_H
#define VIEW_PREFERENCE_H

class QHBoxLayout;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

#include <QDialog>

class ViewPreferenceDialog : public QDialog
{
    public:
        ViewPreferenceDialog(QWidget* parent = 0);
        ~ViewPreferenceDialog();

        int addTab(QWidget* page, QString& label);

    private:
        QVBoxLayout* layout;
        QHBoxLayout* buttonLayout;

        QTabWidget* tabWidget;

        QPushButton* okButton;
        QPushButton* cancelButton;
};//ViewPreferenceDialog

#endif
