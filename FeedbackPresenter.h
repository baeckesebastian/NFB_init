#ifndef FEEDBACKPRESENTER_H
#define FEEDBACKPRESENTER_H

#include <QtWidgets>



class FeedbackPresenter : public QWidget
{
	Q_OBJECT

public:

	FeedbackPresenter(QElapsedTimer &qetTimer);
	~FeedbackPresenter();
	void updateTextLabel(QString qsText);
	
private:

	void	mouseDoubleClickEvent(QMouseEvent *e);

	QLabel			*myPixLabel, *myTextLabel;
	QElapsedTimer	*qTimer;

signals:

	void LogBaseline();
	void LogCondition();

private slots:

	void StartBaseline();
	void StartCondition();
	void StopPresentation();
	

};

#endif