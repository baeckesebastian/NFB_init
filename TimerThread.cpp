#include "TimerThread.h"



TimerThread::TimerThread(QElapsedTimer &qetTimer, uint &uiTriggerCounter, QString qsPluginsPath, QString qsProjectName)
{
	qsPath				= qsPluginsPath;
	qsProject			= qsProjectName;
	bStarted			= false;
	bBaseline			= false;
	qtTriggerSimTimer	= new QTimer(this);
	iTriggerCounter		= &uiTriggerCounter;
	qTimer				= &qetTimer;
	connect(qtTriggerSimTimer, SIGNAL( timeout() ), this, SLOT( TriggerReceived() ));
}

TimerThread::~TimerThread()
{
}

void TimerThread::run()
{
}

void TimerThread::TriggerSimSwitch()
{
	if( !bStarted )
	{
		qtTriggerSimTimer->start( 2000-3 );
		TriggerReceived();
	}
	else
	{
		if( qtTriggerSimTimer->isActive() )
		{
			qtTriggerSimTimer->stop();
		}
	}
}

void TimerThread::TriggerReceived()
{
	*iTriggerCounter = *iTriggerCounter + 1;
	if( !bStarted )
	{
		qTimer->start();
		bStarted = true;
	}
	updatePresentation();
	if( qtTriggerSimTimer->isActive() )
	{
		emit SimulatedTrigger();
	}
}

void TimerThread::updatePresentation()
{
	switch( *iTriggerCounter )
	{
		case 1:	case 16: case 31: case 46: case 61: case 76: case 91: case 106: case 121: case 136: case 151: 
			emit StartBaseline();
			bBaseline = true;
			break;
		case 11: case 26: case 41: case 56: case 71: case 86: case 101: case 116: case 131: case 146: 
			emit StartCondition();
			bBaseline = false;
			break;
		case 161:
			if( qtTriggerSimTimer->isActive() )
				qtTriggerSimTimer->stop();
			break;
	}
}


void TimerThread::LogImgReceived()
{
	qlImgReceived.append( qTimer->elapsed() );
}

void TimerThread::LogImgProcessed()
{
	qlImgProcessed.append( qTimer->elapsed() );
}

void TimerThread::LogBaseline()
{
	qlBase.append( qTimer->elapsed() );
}

void TimerThread::LogCondition()
{
	qlCond.append( qTimer->elapsed() ); 
}

QString TimerThread::WriteLogFile()
{
	QString qsFilename = qsPath + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmm") + "_" + qsProject + ".txt";
	QFile qfOut(qsFilename);
	qfOut.open(QIODevice::WriteOnly);
	if( qfOut.isOpen() )
	{
		QTextStream qtStream(&qfOut);

		qtStream << "ImgReceived: " << "\t";
		for(int i=0; i<qlImgReceived.size(); i++)
		{
			qtStream << qlImgReceived[i] << "\t";
		}
		qtStream << "\n";

		qtStream << "ImgProcessed: ";
		for(int i=0; i<qlImgProcessed.size(); i++)
		{
			qtStream << qlImgProcessed[i] << "\t";
		}
		qtStream << "\n";

		qtStream << "Baseline: " << "\t\t";
		for(int i=0; i<qlBase.size(); i++)
		{
			qtStream << qlBase[i] << "\t";
		}
		qtStream << "\n";

		qtStream << "Condition: " << "\t";
		for(int i=0; i<qlCond.size(); i++)
		{
			qtStream << qlCond[i] << "\t";
		}
		qtStream << "\n";

 		qfOut.close();
	}
	return qsFilename;
}

bool TimerThread::BaselinePeriod()
{
	return bBaseline;
}

bool TimerThread::isStarted()
{
	return bStarted;
}


