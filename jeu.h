#ifndef JEU_H
#define JEU_H

#include "iplugins.h"
#include <QJsonObject>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <wiringPi.h>

//class Blink:public QObject
//{
//    Q_OBJECT

//public :
//   Blink()= default;
//   int _Gpio;
//public Q_SLOTS:
//   void Blinker() {digitalWrite(_Gpio,digitalRead(_Gpio));}
//};

class Jeu : public IPlugins
{
    Q_OBJECT

public:
    Jeu(IPluginsReport* report);

    // QThread interface
    //bool checkVictoryCondition();
    bool Game_Success();
    bool Game_Fail();
    bool Game_Socketio();
    bool _Ventilation = false;
    bool _FlagPlayOnceAudioV = true;
    bool _FlagPlayOnceAudioWFV = true;
    void Setup();
    int _OutputRelayFum;
    int _OutputRelayLed;
    int _Serum;
    int _LedR;
    int _LedV;
    int _Ventil;
    int _BlinkT=0;







    QString _AudioPathVentil;
    QString _AudioPath;
    QTime _Time;
    QTime _Debounce;

    //Blink _BlinkLedR;
    void Clignotement(int Led);
    QSerialPort _serialPort;
    void loadConfigGameTech(QString config_file);
    void loadConfigGameFonct(QString config_file);
public Q_SLOTS:
    void Slot_RCon(QJsonObject packet);

protected:
    virtual void main_game_callback() override;
    int _score;

Q_SIGNALS:
    void Signal_RCon_AudioControler(QJsonObject);
    void Signal_RCon_NetworkIO(QJsonObject);
    void Signal_RCon_Network(QJsonObject);
    void Signal_RCon_Serial(QJsonObject);
};


#endif // JEU_H
