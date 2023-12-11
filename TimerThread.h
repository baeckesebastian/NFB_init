#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QtCore>



class TimerThread : public QThread
{
	Q_OBJECT

public:
	
	TimerThread(QElapsedTimer &qetTimer, uint &uiTriggerCounter, QString qsPluginsPath, QString qsProjectName);		
	~TimerThread();	
	QString WriteLogFile();
	bool BaselinePeriod();
	bool isStarted();

protected:
  
	void run();

private:

	bool			bStarted, bBaseline;
	QString			qsPath, qsProject;
	QElapsedTimer	*qTimer;
	QTimer			*qtTriggerSimTimer;
	uint			*iTriggerCounter;
	QList<int>		qlImgReceived, qlImgProcessed, qlBase, qlCond;

private slots:

	void	TriggerSimSwitch();
	void	TriggerReceived();
	void	updatePresentation();
	void	LogImgReceived();
	void	LogImgProcessed();
	void	LogBaseline();
	void	LogCondition();

signals:

	void	StartBaseline();
	void	StartCondition();
	void	SimulatedTrigger();

};
#endif