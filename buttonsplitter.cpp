#include "buttonsplitter.h"
#include <QtGui>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

ButtonSplitterHandle::ButtonSplitterHandle(Qt::Orientation o, QSplitter *parent)
   : QSplitterHandle(o,parent)
{
   m_lastUncollapseSize=0;
   QBoxLayout *layout;
   button=new QToolButton(this);
   QFrame *line=new QFrame(this);
   Qt::Alignment lineAlignment;

   if(o==Qt::Horizontal)
   {
      layout=new QVBoxLayout();
      line->setFrameShape(QFrame::VLine);
      lineAlignment=Qt::AlignHCenter;
      m_expandingText="";
      m_collapsingText="";
      m_expandingArrow=Qt::ArrowType::RightArrow;
      m_collapsingArrow=Qt::ArrowType::UpArrow;
   }
   else
   {
      layout=new QHBoxLayout();
      line->setFrameShape(QFrame::HLine);
      lineAlignment=Qt::AlignVCenter;
      m_expandingText="[-]";
      m_collapsingText="[+]";
      m_expandingArrow=Qt::ArrowType::DownArrow;
      m_collapsingArrow=Qt::ArrowType::RightArrow;
   }
   layout->setContentsMargins(0,0,0,0);
   button->setStyleSheet("QToolButton {border: none;}");
   button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
   buttonUpdate(true);
   line->setFrameShadow(QFrame::Sunken);

   layout->addWidget(button,0);
   layout->addWidget(line,1,lineAlignment);
   this->setLayout(layout);
   if(o==Qt::Horizontal)
      this->splitter()->setHandleWidth(button->sizeHint().width());
   else
      this->splitter()->setHandleWidth(button->sizeHint().height());
//   this->splitter()->setCollapsible(0,false);

   connect(button,SIGNAL(clicked(bool)),this,SLOT(toggleCollapsed()));
}

void ButtonSplitterHandle::toggleCollapsed()
{
   QSplitter *s=splitter();
   Qt::Orientation o=orientation();
   int idx=s->indexOf(this);
   QWidget *w=s->widget(idx);
   QList<int> sizes=s->sizes();

   if(m_lastUncollapseSize==0)
   {
      if(o==Qt::Horizontal)
         m_lastUncollapseSize=w->sizeHint().width();
      else
         m_lastUncollapseSize=w->sizeHint().height();
   }

   int curSize=s->sizes().at(idx);
   if(curSize==0)
   {
      for(int i=0;i<sizes.size();i++)
         sizes[i]-=m_lastUncollapseSize/(sizes.size()-1);
      sizes[idx]=m_lastUncollapseSize;
   }
   else
   {
      m_lastUncollapseSize=curSize;
      sizes[idx]=0;
   }
   s->setSizes(sizes);
   refreshAll();
}

void ButtonSplitterHandle::refreshAll()
{
   QSplitter *s=splitter();
   for(int i=0;i<s->count();i++)
   {
      ButtonSplitterHandle *bh=qobject_cast<ButtonSplitterHandle*>(s->handle(i));
      if(bh)
         bh->refresh();
   }
}

void ButtonSplitterHandle::refresh()
{
   QSplitter *s=splitter();
   int idx=s->indexOf(this);

   if(s->sizes().at(idx)==0)
      buttonUpdate(false);
   else
      buttonUpdate(true);
}

void ButtonSplitterHandle::buttonUpdate(bool expanding)
{
   if(expanding)
   {
      button->setText(m_expandingText);
      button->setArrowType(m_expandingArrow);
   }
   else
   {
      button->setText(m_collapsingText);
      button->setArrowType(m_collapsingArrow);
   }

}

void ButtonSplitter::initialize()
{
   connect(this,SIGNAL(splitterMoved(int,int)),this,SLOT(onSplitterMoved(int,int)));
}

void ButtonSplitter::onSplitterMoved(int pos,int index)
{
   Q_UNUSED(pos); Q_UNUSED(index);
//   qDebug() << sizes();
   for(int i=0;i<count();i++)
   {
      ButtonSplitterHandle *bh=qobject_cast<ButtonSplitterHandle*>(handle(i));
      if(bh)
         bh->refresh();
   }
}

QString ButtonSplitter::expandingText(int index) const
{
   const ButtonSplitterHandle *bh=qobject_cast<const ButtonSplitterHandle*>(handle(index));
   if(bh)
      return bh->expandingText();
   return QString();
}

QString ButtonSplitter::collapsingText(int index) const
{
   const ButtonSplitterHandle *bh=qobject_cast<const ButtonSplitterHandle*>(handle(index));
   if(bh)
      return bh->collapsingText();
   return QString();
}

void ButtonSplitter::setExpandingText(int index,const QString &text)
{
   ButtonSplitterHandle *bh=qobject_cast<ButtonSplitterHandle*>(handle(index));
   if(bh)
   {
      bh->setExpandingText(text);
      bh->refresh();
   }
}

void ButtonSplitter::setCollapsingText(int index,const QString &text)
{
   ButtonSplitterHandle *bh=qobject_cast<ButtonSplitterHandle*>(handle(index));
   if(bh)
   {
      bh->setCollapsingText(text);
      bh->refresh();
   }
}
