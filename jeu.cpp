#include "jeu.h"
#include <iostream>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonObject>
#include <audiocontroller.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <wiringPi.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>





Jeu::Jeu(IPluginsReport *report) : IPlugins(report)
{

}



bool Jeu::Game_Fail()
{
    QJsonObject play_sound
    {
        {"command", "stop"},
        {"id", "file:///home/pi/Downloads/test.mp3"}
    };

    std::cout << "YOU LOOSE !!!" << std::endl;
    Signal_RCon_AudioControler(play_sound);
    return (true);
}

void Jeu::loadConfigGameTech(QString config_file)
{
    QJsonDocument docGameTech;
    QDir dirGameTech;
    qDebug() << dirGameTech.path();
    QFile fileGameTech(config_file);
    if (!fileGameTech.open(QIODevice::ReadOnly | QIODevice::Text))
    {
          qDebug("error FFFIILLLEEEUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
          return;
    }
    qDebug() << "Opening config file" << config_file;
    docGameTech = QJsonDocument::fromJson(fileGameTech.readAll());
    QJsonObject object = docGameTech.object();
    qDebug()<< docGameTech;
    qDebug("\n\n###\t start json file\t###");

    _OutputRelayLed = object.value("OutputRelayLed").toString().toInt();
    _OutputRelayFum = object.value("OutputRelayFum").toString().toInt();
    _LedV = object.value("ControlerLedV").toString().toInt();
    _LedR = object.value("ControlerLedR").toString().toInt();
    _Serum = object.value("DigitSerum").toString().toInt();
    _Ventil = object.value("Ventil").toString().toInt();
    qDebug()<<_Ventil ;
    qDebug("###\t end json file###\n\n");

}
void Jeu::loadConfigGameFonct(QString config_file)
{
    QJsonDocument docGame;
    QDir dirGame;
    qDebug() << dirGame.path();
    QFile fileGame(config_file);
    if (!fileGame.open(QIODevice::ReadOnly | QIODevice::Text))
    {
          qDebug("error FFFIILLLEEEUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
          return;
    }
    qDebug() << "Opening config file" << config_file;

    docGame = QJsonDocument::fromJson(fileGame.readAll());
    qDebug()<<docGame;
    QJsonObject object = docGame.object();


    qDebug("\n\n###\t start json file\t###");
    _AudioPathVentil= object.value("AudioPathVentil").toString();
    if (object.contains("AudioPathWaitFVentil"))
    _AudioPath = object.value("AudioPathWaitFVentil").toString();

    qDebug("###\t end json file###\n\n");

}



void Jeu::Setup()
{
    wiringPiSetupGpio();
    pinMode(_OutputRelayFum,OUTPUT);
    pinMode(_OutputRelayLed,OUTPUT);
    pinMode(_LedR,OUTPUT);
    pinMode(_LedV,OUTPUT);
    pinMode(_Ventil,OUTPUT);
    pinMode(_Serum,INPUT);
    pullUpDnControl(_Serum,PUD_DOWN);
    digitalWrite(_OutputRelayFum,LOW);
    digitalWrite(_LedR,LOW);
    digitalWrite(_LedV,LOW);
    digitalWrite(_OutputRelayLed,LOW);
    _Time = QTime::currentTime();
    _Debounce = QTime::currentTime();
    loadConfigGameFonct("/home/pi/Dev/CodeSequenceur/test_du_soir4/Code_brief/build-generik-Desktop-Debug/GameFonct.json");
    loadConfigGameTech("/home/pi/Dev/CodeSequenceur/test_du_soir4/Code_brief/build-generik-Desktop-Debug/GameTech.json");
    //_BlinkLedR = new Blink();
    //connect(&_Clignotement, &QTimer::timeout, this->_BlinkLedR, &Blink::Blinker());
    //_BlinkLedR->_Gpio = _LedR;
}

bool Jeu::Game_Socketio()
{
    QJsonObject packet
    {
        {"io_socket", "iterate_and_pulse"},
        {"command", "{\"color\":\"0xFAFAFA\",\"delay\":\"1\",\"brightness_max\":\"255\",\"brightness_min\":\"32\",\"brightness_scale\":\"5\"pos_start\":\"0\",\"pos_end\":\"144\",\"scale\":\"i\"}"}
    };

    std::cout << "YOU Socket iO !!!" << std::endl;
    Signal_RCon_NetworkIO(packet);
    return (true);
}

void Jeu::Slot_RCon( QJsonObject packet )
{
   qDebug()<<">IPlugins::Slot_RCon<"<< "\n" << packet;

        if (packet.contains("param"))
        {
            if (packet.value("param")=="VentilationONOFF")
            {
              _Ventilation = packet.value("State").toBool();
            }

        }
}
void Jeu::Clignotement(int Led)
{
    digitalWrite(Led,!digitalRead(Led));
}


//GameFlow
void Jeu::main_game_callback()
{
        if(digitalRead(_Ventil))
        {
            _Ventilation = true;
        }
        else
        {
            _Ventilation = false;
        }


      if(_Ventilation)
      {
          if (_FlagPlayOnceAudioV)
          {
          _FlagPlayOnceAudioV = false;
          QJsonObject AudioListenV;
          AudioListenV.insert("command","play");
          AudioListenV.insert("recast",true);
          AudioListenV.insert("path",_AudioPathVentil);
          Signal_RCon_AudioControler(AudioListenV);
          qDebug ()<<"je lance la musique "<< AudioListenV;
          }
          if (digitalRead(_Serum))
          {
              digitalWrite(_LedV,HIGH);
              digitalWrite(_LedR,LOW);
              digitalWrite(_OutputRelayFum, HIGH);
              digitalWrite(_OutputRelayLed, HIGH);
              qDebug()<<"Serum OK";
          }
          else
          {
              digitalWrite(_LedR,HIGH);
              digitalWrite(_LedV,LOW);
          }
      }
      else
      {
         // qDebug()<<digitalRead(_testVentil)<< _Ventilation << _testVentil;
          QJsonObject AudioStop;
          AudioStop.insert("command","stop");
          AudioStop.insert("force","true");
          AudioStop.insert("id",_AudioPathVentil);

          _FlagPlayOnceAudioV=true;
          if (digitalRead(_Serum))
          {
              if (QTime::currentTime()>_Time)
              {
                  _Time = QTime::currentTime().addMSecs(_BlinkT);
                  Clignotement(_LedR);
              }

              if((!_AudioPath.isEmpty())&&(_FlagPlayOnceAudioWFV)&&(QTime::currentTime()>_Debounce))
              {
                _FlagPlayOnceAudioWFV=false;
                QJsonObject AudioListenWFV;
                AudioListenWFV.insert("command","play");
                AudioListenWFV.insert("path",_AudioPath);
                Signal_RCon_AudioControler(AudioListenWFV);
                _Debounce = QTime::currentTime().addMSecs(500);
              }
          }
          else
          {
              digitalWrite(_LedR,HIGH);
              _FlagPlayOnceAudioWFV = true;
          }
      }


}
