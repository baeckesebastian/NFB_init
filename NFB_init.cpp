#include "NFB_init.h"

const float PlugInVersion = 1.0;



NFB_init::NFB_init(QObject *parent)
{	
}

NFB_init::~NFB_init()
{
	//myTriggerThread->terminate();
	myTimerThread->terminate();
	myFeedbackPresenter->close();
	myRTLogTable->close();
}

bool NFB_init::initPlugin() 
{
	return InitTBVAccess();
}

bool NFB_init::executePreRun()
{	
	iMoveBaseWinSize	= 10;
	fMaxPercChange		= 0.0;
	uiTriggerCounter	= 0;

	/* RTLogTable */
	//int iTR = tGetIntegerInput("TR in ms: ", 2000, 1, 5000);
	myRTLogTable = new RTLogTable(qetTimer, 2000, uiTriggerCounter);
    myRTLogTable->show();

	/* TimerThread */
	tGetPluginsPath(cPluginsPath);
	tGetProjectName(cProjectName);
	myTimerThread = new TimerThread(qetTimer, uiTriggerCounter, QString(cPluginsPath), QString(cProjectName));
	myTimerThread->start();

	/* FeedbackPresenter */
	myFeedbackPresenter = new FeedbackPresenter(qetTimer);
	myFeedbackPresenter->show();

	/* TriggerThread */
	//char cTriggerPort[601];
	//if(tGetTextInput("Trigger Memory Address: ", "C011", cTriggerPort))		// 7T: EC00????
	//char cTriggerPort[] = "C011";
	//{
	//	QString qsTriggerPort = cTriggerPort;
	//	myTriggerThread = new TriggerThread( qsTriggerPort.toUInt(0, 16) );
	//	myTriggerThread->start();
	//	if( myTriggerThread->isRunning() )
	//		tLogText("TriggerThread is running.");
	//}
	QShortcut* qTriggerShortcut = new QShortcut(QKeySequence(Qt::Key_T), myFeedbackPresenter);
	
	/* TriggerSimulation */
	QShortcut* qHiddenShortcut1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), myFeedbackPresenter);
	tLogText("  Strg+T: \tTrigger-Simulation Switch");

	/* CONNECTIONS */
	//connect( myTriggerThread,		SIGNAL( TriggerReceived() ),	myTimerThread,			SLOT( TriggerReceived() ));
	connect(qTriggerShortcut,		SIGNAL( activated() ),			myTimerThread,			SLOT( TriggerReceived() ));
	//connect( myTriggerThread,		SIGNAL( TriggerReceived() ),	myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect(qTriggerShortcut,		SIGNAL( activated() ),			myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( this,					SIGNAL( ImageReceived() ),		myTimerThread,			SLOT( LogImgReceived() ));
	connect( this,					SIGNAL( ImageProcessed() ),		myTimerThread,			SLOT( LogImgProcessed() ));
	connect( this,					SIGNAL( ImageReceived() ),		myRTLogTable,			SLOT( ImageReceivedLog() ));
	connect( this,					SIGNAL( ImageProcessed() ),		myRTLogTable,			SLOT( ImageProcessedLog() ));
	connect( this,					SIGNAL( StopPresentation() ),	myFeedbackPresenter,	SLOT( StopPresentation() ));
	connect( myTimerThread,			SIGNAL( StartBaseline() ),		myFeedbackPresenter,	SLOT( StartBaseline() ));
	connect( myTimerThread,			SIGNAL( StartCondition() ),		myFeedbackPresenter,	SLOT( StartCondition() ));
	connect( myTimerThread,			SIGNAL( SimulatedTrigger() ),	myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( myFeedbackPresenter,	SIGNAL( LogBaseline() ),		myTimerThread,			SLOT( LogBaseline() ));
	connect( myFeedbackPresenter,	SIGNAL( LogCondition() ),		myTimerThread,			SLOT( LogCondition() ));
	connect( qHiddenShortcut1,		SIGNAL( activated() ),			myTimerThread,			SLOT( TriggerSimSwitch() ));
	
	tLogText("  DoubleClick: \tFull-Screen Switch");
	tLogText("<br>");
	return true;
}

bool NFB_init::executePreStep()
{
	emit ImageReceived();
	return true;
}

bool NFB_init::executePostStep()
{
	emit ImageProcessed();

	int iCurrentTR		= tGetCurrentTimePoint(); // access time point just finished processing in TBV: 1-based, for access of 0-based data, subtract 1
	int iExpectedNrOfTR = tGetExpectedNrOfTimePoints();
	int iNrOfROIs		= tGetNrOfROIs(); 

	float fValueOfDesignMatrix = tGetValueOfDesignMatrix(0, iCurrentTR-1);

	if(fValueOfDesignMatrix < 0.1)
	{
		qviBaseline.append( iCurrentTR-1 );
	}
	
	sprintf_s(cLogString, "<b>%04i</b>/%04i (NrOfROIs=%02i): ", iCurrentTR, iExpectedNrOfTR, iNrOfROIs);
	
	if(iNrOfROIs > 0)
	{
		int iNrOfVoxelsOfROI = tGetNrOfVoxelsOfROI(0);
		float fMeanOfROI = tGetMeanOfROI(0);
		sprintf_s(cLogString, "%s [%04i: %4.3f]", cLogString, iNrOfVoxelsOfROI, fMeanOfROI);

		fBaseline = calcBaseline();

		char cTMP[601];
		sprintf_s(cTMP, "Baseline: %3.3f   ROI: %3.3f", fBaseline, fMeanOfROI);

		if(fValueOfDesignMatrix < 0.1)
		{
			sprintf_s(cLogString, "%s <b>%4.3f</b>", cLogString, fBaseline);
		}
		else
		{
			float fPercSignChange = ((fMeanOfROI * 100) / fBaseline) - 100;
			if( myTimerThread->isStarted() && !myTimerThread->BaselinePeriod() )
			{
				fMaxPercChange = qMax(fMaxPercChange, fPercSignChange);
				sprintf_s(cTMP, "Baseline: %3.3f   ROI: %3.3f   ->   %2.2f%%", fBaseline, fMeanOfROI, fPercSignChange);
				sprintf_s(cLogString, "%s <b><font color='red'>%2.2f%%</font></b>", cLogString, fPercSignChange);
			}
		}

		myFeedbackPresenter->updateTextLabel( cTMP );
	}
	tLogText(cLogString);

	return true;
}

bool NFB_init::executePostRun()
{
	emit StopPresentation();

	sprintf_s(cLogString, "<br>fMaxPercChange = %2.2f%%", fMaxPercChange);	
	tLogText(cLogString);

	tLogText( "<br><b>LOGFILE:</b>" );
	QString qsFilename = myTimerThread->WriteLogFile();
	tLogText( qsFilename.toStdString().c_str() );
	tLogText("<br>Plugin completed processing.");

	myRTLogTable->finalize();
	myTimerThread->terminate();
	//myTriggerThread->terminate();

	return true;
}

float NFB_init::calcBaseline()
{
	int iTimepoints		= iMoveBaseWinSize;
	int iNrVoxels		= tGetNrOfVoxelsOfROI(0);
	if( iTimepoints > qviBaseline.size() )
		iTimepoints		= qviBaseline.size();
	float fMeanSignal	= 0.0;

	for(int vx=0; vx<iNrVoxels; vx++)
	{
		int vx_x, vx_y, vx_z;
		tGetCoordsOfVoxelOfROI(0, vx, vx_x, vx_y, vx_z);
		
		for(int t=qviBaseline.size()-1; t>=qviBaseline.size()-iTimepoints; t--)
		{
			fMeanSignal += tGetValueOfVoxelAtTime(vx_x, vx_y, vx_z, qviBaseline[t]);
		}
	}
	return fMeanSignal / (iTimepoints * iNrVoxels);
}



PLUGIN_ACCESS TBVPluginInterface *createPlugin() 
{
	return new NFB_init();
}

PLUGIN_ACCESS const char *getPluginName() 
{
	char *PluginName = new char[100];
	sprintf_s(PluginName, 100, "NFB [init 10x(10+5)+10=160Sc] v%1.1f", PlugInVersion);
	return PluginName;
}

static const char *PluginDescription = "This simple plugin shows ...";
PLUGIN_ACCESS const char *getPluginDescription() 
{
	return PluginDescription;
}

PLUGIN_ACCESS const char *getAuthor()
{
	return "<i>Sebastian Baecke</i>, Otto-von-Guericke-University Magdeburg";
}

PLUGIN_ACCESS const char *getHelpFile()
{
	return ""; // "NFB_initHelp/index.html";
}

PLUGIN_ACCESS const char *getPluginVersion()
{
	char *PluginVersionString = new char[6];
	sprintf_s(PluginVersionString, 6, "%1.1f", PlugInVersion);
	return PluginVersionString;
}
