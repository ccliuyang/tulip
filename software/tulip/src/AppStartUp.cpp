#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <qprogressbar.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <tulip/TlpTools.h>
#include <tulip/PluginLoaderTxt.h>
#include <tulip/GlMainWidget.h>
#include <tulip/Glyph.h>
#include <tulip/GlyphManager.h>
#include <qapplication.h>
#include "AppStartUp.h"

#include "TulipPluginLoader.h"

using namespace std;
using namespace tlp;

static std::string errorMsgs;



void AppStartUp::initTulip(TulipPluginLoader *loader,std::string &errors) {
  setTotalSteps(0);
  setProgress(0);
  setLabel("Tulip");

  loader->appStartUp = this;

  /*#ifndef NDEBUG
  PluginLoaderTxt plug;
  #else
  PluginLoaderQt plug;
  plug.appStartUp = this;
  #endif*/
  
  //tlp::initTulipLib(); already done in Application.cpp
  tlp::loadPlugins(loader);   // library side plugins
  GlyphManager::getInst().loadPlugins(loader);   // software side plugins, i.e. glyphs

  errors = errorMsgs;
  // free memory
  errorMsgs.resize(0);
}

/* 
 *  Constructs a AppStartUp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AppStartUp::AppStartUp( QWidget* parent,  const char* name, bool modal, WFlags fl )
  : AppStartUpForm( parent, name, modal, fl ),
    currentFrame(0){
  string tmp="Tulip V. ";
  tmp+=VERSION;
  tulipVersion->setText(tmp.c_str());
  string dir=TulipLibDir;
  dir += "tlp/bitmaps/";
  movie=new QMovie(string(dir+"startup.gif").c_str());
  PixmapLabel1->setMovie(movie);
  movie->start();
  movie->stop();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AppStartUp::~AppStartUp() {
    // no need to delete child widgets, Qt does it all for us
}

void AppStartUp::setProgress(int progress) {
  progressBar->setProgress(progress);
  if(progress!=0 && currentFrame!=100) {
    if(currentFrame!=(int)(((float)progress/(float)totalSteps)*100)) {
      int nextFrame=(int)(((float)progress/(float)totalSteps)*100);
      for(int i=0;i<nextFrame-currentFrame;++i) {
	movie->jumpToNextFrame();
      }
      currentFrame=nextFrame;
    }
  }
}
 
void AppStartUp::setTotalSteps(int totalSteps) {
  this->totalSteps=totalSteps;
  progressBar->setTotalSteps(totalSteps);
}
 
void AppStartUp::setLabel(string str) {
  QString tmpQString=str.c_str();
  textLabel->setText(tmpQString);
}
