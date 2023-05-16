#ifndef BUTTONSPLITTER_H
#define BUTTONSPLITTER_H
#include <QSplitter>
#include <QSplitterHandle>
#include <QToolButton>

class ButtonSplitterHandle : public QSplitterHandle
{
   Q_OBJECT
public:
   ButtonSplitterHandle(Qt::Orientation o,QSplitter *parent=0);
   //void buttonUpdate();
   void refresh();
   void refreshAll();
   const QString &expandingText() const
   { return m_expandingText; }
   const QString &collapsingText() const
   { return m_collapsingText; }
   void setExpandingText(const QString &text)
   { m_expandingText=text; }
   void setCollapsingText(const QString &text)
   { m_collapsingText=text; }

public slots:
   void toggleCollapsed();
private:
   void buttonUpdate(bool expanding);
   QToolButton *button;
   int m_lastUncollapseSize;
   Qt::ArrowType m_expandingArrow,m_collapsingArrow;
   QString m_expandingText,m_collapsingText;
};

class ButtonSplitter : public QSplitter
{
   friend class ButtonSplitterHandle;
   Q_OBJECT
public:
   ButtonSplitter(Qt::Orientation o,QWidget *parent=0)
      : QSplitter(o,parent)
   {  initialize(); }
   ButtonSplitter(QWidget *parent=0)
      : QSplitter(parent)
   {  initialize(); }

   /*
   const QString &expandingText() const
   { return m_expandingText; }
   const QString &collapsingText() const
   { return m_collapsingText; }
   */
   QString expandingText(int index) const;
   QString collapsingText(int index) const;
   void setExpandingText(int index,const QString &text);
   void setCollapsingText(int index,const QString &text);

private slots:
   void onSplitterMoved(int pos,int index);
protected:
   QSplitterHandle *createHandle()
   {
      return new ButtonSplitterHandle(orientation(),this);
   }
private:
   void initialize();
};

#endif // BUTTONSPLITTER_H
