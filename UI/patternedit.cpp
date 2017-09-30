#include "patternedit.h"
#include "ui_patternedit.h"
#include "patterneditview.h"
#include "markparam.h"

PatternEdit::PatternEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatternEdit)
{
    ui->setupUi(this);
    m_view= new PatternEditView(ui->patternView);
    connect(ui->rb_cross, SIGNAL(toggled(bool)), SLOT(typeChanged(bool)));
    connect(ui->rb_circle, SIGNAL(toggled(bool)), SLOT(typeChanged(bool)));
    connect(ui->rb_point, SIGNAL(toggled(bool)), SLOT(typeChanged(bool)));
}

void PatternEdit::setPattern(const PatternParam &pattern, const TargetParam &target)
{
    ui->sp_similar->setValue(pattern.similar*100+0.5);
    if(target.type==TargetParam::Cross)
        ui->rb_cross->setChecked(true);
    else if(target.type==TargetParam::Circle)
        ui->rb_circle->setChecked(true);
    else
        ui->rb_point->setChecked(true);
    ui->cb_blackWihte->setCurrentIndex(target.white);
    ui->sp_gradiantThresh->setValue(target.gradient);
    ui->sp_gradientStep->setValue(target.gradStep);
    ui->sp_range->setValue(target.range);
    m_view->receive_image(pattern.image);
    m_view->setTarget(target);
}

void PatternEdit::getPattern(PatternParam &pattern, TargetParam &target)
{
    pattern.similar=ui->sp_similar->value()/100.0;
    m_view->getTarget(target);
    target.white=ui->cb_blackWihte->currentIndex();
    target.gradient=ui->sp_gradiantThresh->value();
    target.gradStep=ui->sp_gradientStep->value();
    target.range=ui->sp_range->value();
}

PatternEdit::~PatternEdit()
{
    delete ui;
}

void PatternEdit::typeChanged(bool val)
{
    if(val){
        QRadioButton* rb=qobject_cast<QRadioButton*>(sender());
        if(rb==ui->rb_circle)
            m_view->changeToCircle();
        else if(rb==ui->rb_cross)
            m_view->changeToCross();
        else
            m_view->changeToPoint();
    }
}





