#ifndef NFB_INIT_H
#define NFB_INIT_H

#include "TBVPluginInterface.h"
//#include "TriggerThread.h"
#include "FeedbackPresenter.h"
#include "TimerThread.h"
#include "RTLogTable.h"



class NFB_init : public QObject, public TBVPluginInterface
{
	Q_OBJECT
		Q_INTERFACES(TBVPluginInterface)

public:

	PLUGIN_ACCESS NFB_init(QObject* parent = 0);
	PLUGIN_ACCESS ~NFB_init();

	PLUGIN_ACCESS bool initPlugin();
	PLUGIN_ACCESS bool executePreRun();
	PLUGIN_ACCESS bool executePreStep();
	PLUGIN_ACCESS bool executePostStep();
	PLUGIN_ACCESS bool executePostRun();

private:

	float				calcBaseline();

	FeedbackPresenter*	myFeedbackPresenter;
	//TriggerThread*		myTriggerThread;
	TimerThread*		myTimerThread;
	RTLogTable*			myRTLogTable;

	int					iMoveBaseWinSize;
	float				fMaxPercChange, fBaseline;
	char				cLogString[601], cPluginsPath[101], cProjectName[101];

	QVector<int>		qviBaseline;
	QElapsedTimer		qetTimer;
	uint				uiTriggerCounter;

signals:

	void	ImageReceived();
	void	ImageProcessed();
	void	StopPresentation();

};

// To avoid C++ name mangling problems, we need to bootstrap access of plugin via C functions
extern "C" {
	PLUGIN_ACCESS TBVPluginInterface* createPlugin();
	PLUGIN_ACCESS const char* getPluginName();
	PLUGIN_ACCESS const char* getPluginDescription();
	PLUGIN_ACCESS const char* getAuthor();
	PLUGIN_ACCESS const char* getHelpFile();
	PLUGIN_ACCESS const char* getPluginVersion();
}

#endif