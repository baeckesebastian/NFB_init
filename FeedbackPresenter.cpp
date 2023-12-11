#include "FeedbackPresenter.h"



FeedbackPresenter::FeedbackPresenter(QElapsedTimer &qetTimer)
{
	setWindowTitle(tr("FeedbackPresenter"));
	setWindowFlags( windowFlags() ^ Qt::WindowStaysOnTopHint );
	setFixedSize( QSize(1024,768) );
	setStyleSheet("color: white; background-color: black");

	myPixLabel = new QLabel;
	myPixLabel->setAlignment(Qt::AlignCenter);   
 	myPixLabel->setFont( QFont("Helvetica [Cronyx]", 70) );

	myTextLabel = new QLabel;
	myTextLabel->setAlignment(Qt::AlignBottom|Qt::AlignRight);

	QVBoxLayout *myLayout = new QVBoxLayout();
	myLayout->addStretch();
	myLayout->addWidget(myPixLabel);
	myLayout->addStretch();
    setLayout(myLayout);

	qTimer = &qetTimer;
}

FeedbackPresenter::~FeedbackPresenter()
{
}

void FeedbackPresenter::updateTextLabel(QString qsText)
{
	myTextLabel->setText( qsText );
}

void FeedbackPresenter::StartBaseline()
{
	myPixLabel->setText( "<font color='gray'>+</font>" );
	emit LogBaseline();
}

void FeedbackPresenter::StartCondition()
{
	myPixLabel->setText( "<font color='white'>+</font>" );
	emit LogCondition();
}

void FeedbackPresenter::StopPresentation()
{
	myPixLabel->clear();
}

void FeedbackPresenter::mouseDoubleClickEvent(QMouseEvent *e)
{
	if( isFullScreen() )
		setWindowState( Qt::WindowMaximized );
	else
		setWindowState( Qt::WindowFullScreen );
}