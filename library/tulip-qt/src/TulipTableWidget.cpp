#include <iostream>
#include <sstream>
#include <string>

#if (QT_REL == 3)
#include <qstring.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qcolordialog.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qvalidator.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#else
#include <QtCore/qstring.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qpainter.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qapplication.h>
#include <QtGui/qlayout.h>
#include <QtGui/qlineedit.h>
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qvalidator.h>
#include <QtGui/qsizepolicy.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qinputdialog.h>
#include <QtGui/qevent.h>
#include "tulip/Qt3ForTulip.h"
#endif

#include <tulip/Graph.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/GlGraph.h>

#include "tulip/TulipTableWidget.h"

#define COLORTABLEITEM_RTTI 1001
#define FILETABLEITEM_RTTI  1002
#define SIZETABLEITEM_RTTI  1003
#define COORDTABLEITEM_RTTI 1004
#define GLYPHTABLEITEM_RTTI 1005
#define SELECTIONTABLEITEM_RTTI 1006
#define EDGESHAPETABLEITEM_RTTI 1007

using namespace std;
using namespace tlp;
//================================================================================
ColorButton::ColorButton(const QRgb &c, QWidget *parent, const char *name, Qt::WFlags f) :
    QButton(parent, name, f), color(c) {
  connect(this, SIGNAL(pressed()), SLOT(colorDialog()));
  emit pressed();
  setFocusPolicy(StrongFocus);
}
ColorButton::~ColorButton() {}
QRgb ColorButton::getColor() const {
  return color;
}
void ColorButton::colorDialog() {
  bool ok;
  QRgb tmpcolor = QColorDialog::getRgba(color, &ok);
  if (ok) color = tmpcolor;
}
void ColorButton::paintEvent(QPaintEvent *qpe) {
  QPainter p(this);
  p.fillRect(qpe->rect(), QColor(color));
#if (QT_REL == 3)
  p.flush();
#endif
}
//================================================ 
ColorTableItem::ColorTableItem(QTable *table, const QRgb &color) :
  QTableItem(table, OnTyping, ""), color(color) {}
ColorTableItem::~ColorTableItem() {}
int ColorTableItem::rtti() const {return COLORTABLEITEM_RTTI;}
QRgb ColorTableItem::getColor() const {return color;}
QWidget *ColorTableItem::createEditor() const {
  ColorButton *cb = new ColorButton(color, table()->viewport());
  return cb;
}
void ColorTableItem::setContentFromEditor(QWidget *w) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  color = ((ColorButton *)w)->getColor();
  stringstream ss;
  ss << "(" << qRed(color) << "," << qGreen(color) << "," << qBlue(color) << "," << qAlpha(color) << ")";
  setText(ss.str().c_str());
}
void ColorTableItem::paint(QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected) {
  p->fillRect(table()->cellRect(row(), col()), QColor(color));
#if (QT_REL == 3)
  p->flush();
#endif
}
//================================================================================
FilenameEditor::FilenameEditor(const QString &filter, QWidget *parent, const char *name) :
  QWidget(parent, name), fileFilter(filter) {
  FilenameEditor(parent, name);
}
FilenameEditor::FilenameEditor(QWidget *parent, const char *name) : QWidget(parent, name) {
  QHBoxLayout *layout = new QHBoxLayout(this, 0, -1, "FilenameEditor->Layout");
  lineedit = new QLineEdit(this, "FilenameEditor->LineEdit");
  lineedit->setFrame(false);
  connect(lineedit, SIGNAL(textChanged(const QString &)),
          this, SIGNAL(fileNameChanged(const QString &)));
  layout->addWidget(lineedit);
  button = new QPushButton("...", this, "FilenameEditor->PushButton");
  button->setFixedWidth(button->fontMetrics().width(" ... "));
  layout->addWidget(button);
  connect(button, SIGNAL(clicked()), this, SLOT(buttonPressed()));
  setFocusProxy(lineedit);
}
FilenameEditor::~FilenameEditor() {}
QString FilenameEditor::fileName() const {
  return lineedit->text();
}
void FilenameEditor::setFileName(const QString &s) {
  cerr << __PRETTY_FUNCTION__ << (const char *) s << endl;
  lineedit->setText(s);
}
QString FilenameEditor::filter() const {
  return fileFilter;
}
void FilenameEditor::setFilter(const QString &f) {
  fileFilter = f;
}
void FilenameEditor::buttonPressed() {
#if (QT_REL == 3)
  QFileDialog *dlg = new QFileDialog("./",
                                     fileFilter,
                                     this,
                                     "FilenameEditor->FileDialog",
                                     true);
  dlg->setCaption(tr("Choose a file"));
#else
  QFileDialog *dlg = new QFileDialog(this,
				     "Choose a file",
				     "./",
                                     fileFilter);
  dlg->setModal(true);
#endif
  dlg->setMode(QFileDialog::ExistingFile);
  if (dlg->exec() == QDialog::Accepted ) {
    QString file = dlg->selectedFile();
    if (!file.isNull()) {
      QStringList currentDir = QStringList::split(QDir::separator(), QDir::currentDirPath(), true);
      QStringList filePath = QStringList::split(QDir::separator(), QFileInfo(file).dirPath(true), true);
      QString relativePath = "";
      while((!currentDir.empty() && !filePath.empty())
            && (currentDir.front() == filePath.front())) {
	currentDir.pop_front();
	filePath.pop_front();
      }
      while (!currentDir.empty()) {
        relativePath += "..";
        relativePath += QDir::separator();
        currentDir.pop_front();
      }
      if (!filePath.empty())
        relativePath += filePath.join((const QString) (QChar) QDir::separator()) + QDir::separator();
      setFileName(relativePath + QFileInfo(file).fileName());
    }
  }
  delete dlg;
  // button->setDown(false);
}
//================================================================================
FileTableItem::FileTableItem(QTable *table) : QTableItem(table, OnTyping, "") {}
FileTableItem::~FileTableItem() {}
int FileTableItem::rtti() const {return FILETABLEITEM_RTTI;}
QWidget *FileTableItem::createEditor() const {
  FilenameEditor *w = new FilenameEditor(table()->viewport(), "FilenameEditor");
  w->setFilter("Images (*.png *.jpeg *.jpg *.bmp)");
  w->setFileName(text());
  return w;
}
void FileTableItem::setContentFromEditor(QWidget *w) {
  cerr << __PRETTY_FUNCTION__ << endl;
  QString s = ((FilenameEditor *)w)->fileName();
  cerr << "String : " << (const char *) s << endl;
  if (!s.isNull()) {
    string str(s.latin1());
    cerr << "setText" << endl;
    setText(str.c_str());
    qApp->processEvents();
  }
}
//================================================================================
SizeEditor::SizeEditor(const Size &s, QWidget *parent, const char *name) : QWidget(parent, name),
                                                                          size(s) {
  QHBoxLayout *layout = new QHBoxLayout(this, 0, 3, "SizeEditor->Layout");
  QDoubleValidator *validator = new QDoubleValidator(this, "SizeEditor->validator");
  stringstream ss;
  ss << size.getW() << " " << size.getH() << " " << size.getD();
  for (int i=0; i<3; ++i) {
    string str;
    ss >> str;
    edit[i] = new QLineEdit(str.c_str(), this, "SizeEditor->edit");
    edit[i]->setValidator(validator);
    edit[i]->setFrame(false);
    edit[i]->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));
    layout->addWidget(edit[i]);
  }
  connect(edit[0], SIGNAL(textChanged(const QString &)), this, SLOT(changeW(const QString &)));
  connect(edit[1], SIGNAL(textChanged(const QString &)), this, SLOT(changeH(const QString &)));
  connect(edit[2], SIGNAL(textChanged(const QString &)), this, SLOT(changeD(const QString &)));
  setFocusPolicy(StrongFocus);
}

SizeEditor::~SizeEditor() {}
Size SizeEditor::getSize() const {return size;}
void SizeEditor::setFocus() {edit[0]->setFocus();}
void SizeEditor::changeW(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  size.setW(f);
}
void SizeEditor::changeH(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  size.setH(f);
}
void SizeEditor::changeD(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  size.setD(f);
}
//================================================================================
SizeTableItem::SizeTableItem(QTable *table) : QTableItem(table, OnTyping, "") {}
SizeTableItem::~SizeTableItem() {}
int SizeTableItem::rtti() const {return SIZETABLEITEM_RTTI;}
void SizeTableItem::setSize(const Size &s) {
  size = s;
  stringstream ss;
  ss << "(" << size.getW() << "," << size.getH() << "," << size.getD() << ")";
  setText(ss.str().c_str());
} 
QWidget *SizeTableItem::createEditor() const {
  SizeEditor *w = new SizeEditor(size, table()->viewport(), "SizeEditor");
  return w;
}
void SizeTableItem::setContentFromEditor(QWidget *w) {
  SizeEditor *editor = (SizeEditor *)w;
  setSize(editor->getSize());
}
//================================================================================
CoordEditor::CoordEditor(const Coord &c, QWidget *parent, const char *name) : QWidget(parent, name),
                                                                          coord(c)
{
  QHBoxLayout *layout = new QHBoxLayout(this, 0, 3, "CoordEditor->Layout");
  QDoubleValidator *validator = new QDoubleValidator(this, "CoordEditor->validator");
  stringstream ss;
  ss << coord.getX() << " " << coord.getY() << " " << coord.getZ();
  for (int i=0; i<3; ++i) {
    string str;
    ss >> str;
    edit[i] = new QLineEdit(str.c_str(), this, "CoordEditor->edit");
    edit[i]->setValidator(validator);
    edit[i]->setFrame(false);
    edit[i]->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));
    layout->addWidget(edit[i]);
  }
  connect(edit[0], SIGNAL(textChanged(const QString &)), this, SLOT(changeX(const QString &)));
  connect(edit[1], SIGNAL(textChanged(const QString &)), this, SLOT(changeY(const QString &)));
  connect(edit[2], SIGNAL(textChanged(const QString &)), this, SLOT(changeZ(const QString &)));
  setFocusPolicy(StrongFocus);
}

CoordEditor::~CoordEditor() {}

Coord CoordEditor::getCoord() const {return coord;}
void CoordEditor::setFocus() {edit[0]->setFocus();}

void CoordEditor::changeX(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  coord.setX(f);
}
void CoordEditor::changeY(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  coord.setY(f);
}
void CoordEditor::changeZ(const QString &s) {
  stringstream ss; ss << s.ascii();
  float f; ss >> f;
  coord.setZ(f);
}
//================================================================================
CoordTableItem::CoordTableItem(QTable *table) : QTableItem(table, OnTyping, "") {}
CoordTableItem::~CoordTableItem() {}
int CoordTableItem::rtti() const {return COORDTABLEITEM_RTTI;}
  
void CoordTableItem::setCoord(const Coord &c) {
  coord = c;
  stringstream ss;
  ss << "(" << coord.getX() << "," << coord.getY() << "," << coord.getZ() << ")";
  setText(ss.str().c_str());
} 
QWidget *CoordTableItem::createEditor() const {
  CoordEditor *w = new CoordEditor(coord, table()->viewport(), "CoordEditor");
  return w;
}
void CoordTableItem::setContentFromEditor(QWidget *w) {
  CoordEditor *editor = (CoordEditor *)w;
  setCoord(editor->getCoord());
}
//================================================================================
GlyphTableItem::GlyphTableItem(QTable *table, bool editable) :
  QComboTableItem(table,QStringList(), editable) {
  QStringList tmp;
  GlGraph::initFactory();
  Iterator<string> *itS=GlGraph::glyphFactory->availablePlugins();
  while (itS->hasNext()) {
    tmp.append(QString(itS->next().c_str()));
  } delete itS;
  setStringList(tmp);
}
GlyphTableItem::~GlyphTableItem() {}
int GlyphTableItem::rtti() const {return GLYPHTABLEITEM_RTTI;}

QString GlyphTableItem::text() const {
  stringstream ss;
  ss << GlGraph::glyphId(this->currentText().ascii());
  return QString(ss.str().c_str());
}
//================================================================================
EdgeShapeTableItem::EdgeShapeTableItem(QTable *table, bool editable) :
  QComboTableItem(table,QStringList(), editable) {
  QStringList tmp;
  for (int i = 0; i < GlGraph::edgeShapesCount; i++)
    tmp.append(QString(GlGraph::edgeShapeName(GlGraph::edgeShapeIds[i]).c_str()));
  setStringList(tmp);
}
EdgeShapeTableItem::~EdgeShapeTableItem() {}
int EdgeShapeTableItem::rtti() const {return EDGESHAPETABLEITEM_RTTI;}

QString EdgeShapeTableItem::text() const {
  stringstream ss;
  ss << GlGraph::edgeShapeId(this->currentText().ascii());
  return QString(ss.str().c_str());
}
//================================================================================
SelectionTableItem::SelectionTableItem(QTable *table, const QString &txt) :
  QCheckTableItem(table, txt) {}
SelectionTableItem::~SelectionTableItem() {}

int SelectionTableItem::rtti() const {return SELECTIONTABLEITEM_RTTI;}

QString SelectionTableItem::text() const {return QString(isChecked() ? "true" : "false");}
//================================================================================
TulipTableWidget::TulipTableWidget(QWidget *parent, const char *name) :
  QTable(parent, name), updateColumnTitle(false) {
  resetBackColor1();
  resetBackColor2();
  setColumnReadOnly(0, true);
  horizontalHeader()->setLabel(0, tr("Undefined"));
}

TulipTableWidget::~TulipTableWidget() {}

bool TulipTableWidget::getUpdateColumnTitle() const {return updateColumnTitle;}
void TulipTableWidget::setUpdateColumnTitle(const bool b) { updateColumnTitle = b;}

QColor TulipTableWidget::getBackColor1() const {return backColor1;}
void TulipTableWidget::setBackColor1(const QColor &c) {backColor1 = c;}
void TulipTableWidget::resetBackColor1() {backColor1 = QColor(236, 245, 255);}

QColor TulipTableWidget::getBackColor2() const {return backColor2;}
void TulipTableWidget::setBackColor2(const QColor &c) {backColor2 = c;}
void TulipTableWidget::resetBackColor2() {backColor2 = QColor(250, 250, 250);}

QColor TulipTableWidget::backgroundColor(const int row) const {
  return (row % 2) ? backColor1 : backColor2;
}

void TulipTableWidget::paintCell(QPainter * p, int row, int col, const QRect &cr, bool selected, const QColorGroup &cg) {
  QColorGroup g(cg);
  g.setColor(QColorGroup::Base, backgroundColor(row));
  QTable::paintCell(p, row, col, cr, selected, g);
}

void TulipTableWidget::setTulipNodeItem(const PropertyInterface *editedProperty, const std::string propertyName, const node &n,
                                    const int row, const int col) {
  QString label;
  if (propertyName == "viewShape") {
    GlyphTableItem *item = new GlyphTableItem(this, false);
    int shapenum = ((IntegerProperty *)editedProperty)->getNodeValue(n);

    label = tr("Shape");
    this->setItem(row, col, item);
    item->setCurrentItem(GlGraph::glyphName(shapenum).c_str());
  }
  else if (propertyName == "viewTexture") {
    FileTableItem *item = new FileTableItem(this);
    item->setText( const_cast<PropertyInterface *>(editedProperty)->getNodeStringValue(n).c_str() );
    label = tr("Texture");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(BooleanProperty)) {
    BooleanProperty *tmpSel = (BooleanProperty *) editedProperty;
    SelectionTableItem *item = new SelectionTableItem(this, "");
    item->setChecked(tmpSel->getNodeValue(n));
    label = tr("Selected");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(ColorProperty)) {
    ColorProperty *tmpCol = (ColorProperty *)editedProperty;
    Color c = tmpCol->getNodeValue(n);
    ColorTableItem *item = new ColorTableItem(this, qRgba(c[0], c[1], c[2], c[3]));
    label = tr("Color");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(SizeProperty)) {
    SizeProperty *tmpSiz = (SizeProperty *) editedProperty;
    Size s = tmpSiz->getNodeValue(n);
    SizeTableItem *item = new SizeTableItem(this);
    item->setSize(s);
    label = tr("Size");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(LayoutProperty)) {
    LayoutProperty *tmpLay = (LayoutProperty *)editedProperty;
    Coord c = tmpLay->getNodeValue(n);
    CoordTableItem *item = new CoordTableItem(this);
    item->setCoord(c);
    label = tr("Coord");
    this->setItem(row, col, item);
  }
  else {
    QTableItem *item = new QTableItem(this, QTableItem::OnTyping,
                                      const_cast<PropertyInterface *>(editedProperty)->getNodeStringValue(n).c_str() );
    label = tr("Value");
    this->setItem(row, col, item);
  }

  if (updateColumnTitle) {
    this->horizontalHeader()->setLabel(col, label);
  }
}

void TulipTableWidget::setTulipEdgeItem(const PropertyInterface *editedProperty, const std::string propertyName, const edge &e,
                                    const int row, const int col) {
  QString label;
  if (typeid(*editedProperty) == typeid(BooleanProperty)) {
    BooleanProperty *tmpSel = (BooleanProperty *)editedProperty;
    SelectionTableItem *item = new SelectionTableItem(this, "");
    item->setChecked(tmpSel->getEdgeValue(e));
    label = tr("Selected");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(ColorProperty)) {
    ColorProperty *tmpCol = (ColorProperty *)editedProperty;
    Color c = tmpCol->getEdgeValue(e);
    ColorTableItem *item = new ColorTableItem(this, qRgba(c[0], c[1], c[2], c[3]));
    label = tr("Color");
    this->setItem(row, col, item);
  }
  else if (typeid(*editedProperty) == typeid(SizeProperty)) {
    SizeProperty *tmpSiz = (SizeProperty *)editedProperty;
    Size s = tmpSiz->getEdgeValue(e);
    SizeTableItem *item = new SizeTableItem(this);
    item->setSize(s);
    label = tr("Size");
    this->setItem(row, col, item);
  } else if (propertyName == "viewShape") {
    EdgeShapeTableItem *item = new EdgeShapeTableItem(this, false);
    int shapenum = ((IntegerProperty *)editedProperty)->getEdgeValue(e);

    label = tr("Shape");
    this->setItem(row, col, item);
    item->setCurrentItem(GlGraph::edgeShapeName(shapenum).c_str());
  }
//   else if (typeid(*editedProperty) == typeid(Layout)) {
//     Layout *tmpLay = getProperty<LayoutProperty>(graph, propertyName);
//     Coord c = tmpLay->getEdgeValue(e);
//     CoordTableItem *item = new CoordTableItem(this);
//     item->setCoord(c);
//     label = tr("Coord");
//     this->setItem(row, col, item);
//   }
  else {
    QTableItem *item = new QTableItem(this, QTableItem::OnTyping,
                                      const_cast<PropertyInterface *>(editedProperty)->getEdgeStringValue(e).c_str() );
    label = tr("Value");
    this->setItem(row, col, item);
  }

  if (updateColumnTitle) {this->horizontalHeader()->setLabel(col, label);}
}
