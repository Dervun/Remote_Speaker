#include "sendermanager.h"

SenderManager::SenderManager(QGridLayout* newLayout)
{
    mainLayout = newLayout;
    audioMode = QAudio::Mode::AudioInput;

    soundSender = new SoundSender();
    connect(soundSender, SIGNAL(connected()), this, SLOT(connected()));
    connect(soundSender, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // initialization of widgets, connections and addind of widgets to mainLayout
    initAllWidgets();

    // set correct device widget's content
    deviceLabel->setText("Input device:");
}

SenderManager::~SenderManager()
{
    mainLayout->removeWidget(setPreferredFormatButton);
    mainLayout->removeWidget(ipLabel);
    mainLayout->removeWidget(portLabel);
    mainLayout->removeWidget(ipLineEdit);
    mainLayout->removeWidget(portLineEdit);
    mainLayout->removeWidget(startButton);
    mainLayout->removeWidget(stopButton);
    mainLayout->removeWidget(infoLabel);

    delete setPreferredFormatButton;
    delete ipLabel;
    delete portLabel;
    delete ipLineEdit;
    delete portLineEdit;
    delete startButton;
    delete stopButton;
    delete infoLabel;
}

void SenderManager::initSpecificWidgets()
{
    setPreferredFormatButton = new QPushButton("Set preferred format for this device");
    ipLabel = new QLabel("your ip:");
    portLabel = new QLabel("your port:");
    ipLineEdit = new QLineEdit;
    portLineEdit = new QLineEdit;
    startButton = new QPushButton("Start sending");
    stopButton = new QPushButton("Stop sending");
    infoLabel = new QLabel("Wait for connections");

    mainLayout->addWidget(setPreferredFormatButton, 7, 0, 1, 2);
    mainLayout->addWidget(ipLabel, 8, 0);
    mainLayout->addWidget(ipLineEdit, 8, 1);
    mainLayout->addWidget(portLabel, 9, 0);
    mainLayout->addWidget(portLineEdit, 9, 1);
    mainLayout->addWidget(startButton, 10, 0);
    mainLayout->addWidget(stopButton, 10, 1);
    mainLayout->addWidget(infoLabel, 11, 0, 1, 2);

    ipLineEdit->setReadOnly(true);
    portLineEdit->setReadOnly(true);
    startButton->setEnabled(false);
    stopButton->setEnabled(false);
    infoLabel->setAlignment(Qt::AlignCenter);

    ipLineEdit->setText(soundSender->getHost().toString());
    portLineEdit->setText(QString::number(soundSender->getPort()));

    connect(setPreferredFormatButton, SIGNAL(clicked(bool)), this, SLOT(setPreferredFormat()));
    connect(startButton, SIGNAL(clicked(bool)), this, SLOT(handleStartButtonClicked()));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(handleStopButtonClicked()));
}

void SenderManager::connected()
{
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    infoLabel->setText("Receiver connected :)");
    handleStartButtonClicked();
}

void SenderManager::disconnected()
{
    changeLabelsToBoxes();
    startButton->setEnabled(false);
    stopButton->setEnabled(false);

    soundSender->stopSending();
    infoLabel->setText("Receiver disconnected");
}

void SenderManager::handleStartButtonClicked()
{
    QAudioFormat format = currentAudioFormat;
    while (!currentDeviceInfo->isFormatSupported(format))
    {
        qDebug() << "currentAudioFormat not supported, looking for the nearest";
        format = currentDeviceInfo->nearestFormat(format);
    }
    soundSender->updateInfo(*currentDeviceInfo, format);
    if (soundSender->startSending())
    {
        changeBoxesToLabels();
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        infoLabel->setText("Sending data");
    }
    else
        infoLabel->setText("Can not start with current settings :(");
}

void SenderManager::handleStopButtonClicked()
{
    changeLabelsToBoxes();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    soundSender->stopSending();
    infoLabel->setText("Sending data is stopped");
}
