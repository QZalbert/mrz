#ifndef PATTERNEDIT_H
#define PATTERNEDIT_H

#include <QDialog>

class PatternEditView;

struct PatternParam;
class TargetParam;

namespace Ui {
class PatternEdit;
}

class PatternEdit : public QDialog
{
    Q_OBJECT
    
public:
    explicit PatternEdit(QWidget *parent = 0);
    void setPattern(const PatternParam& pattern, const TargetParam& target);
    void getPattern(PatternParam& pattern, TargetParam& target);
    ~PatternEdit();

private slots:
    void typeChanged(bool val);
    
private:
    Ui::PatternEdit *ui;
    PatternEditView* m_view;
};

#endif // PATTERNEDIT_H
