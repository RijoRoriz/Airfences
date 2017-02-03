#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_readInfoWifi;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_process;
pthread_mutex_t * mutex_wifi_list;
pthread_mutex_t * mutex_RF;
pthread_mutex_t * mutex_field;
pthread_mutex_t * mutex_animal;
pthread_mutex_t * mutex_tcp;


/***** SIGNALS *****/
pthread_cond_t *ts_sendInfoRF;
pthread_cond_t *ts_readInfoRF;
pthread_cond_t *ts_readInfoWifi;
pthread_cond_t *ts_process;

/***** SEMAPHORES *****/
sem_t *tsem_sendInfoWifi;

/***** QUEUES *****/
mqd_t mq_rf;
mqd_t mq_wifiReceiver;

/***** Objects *****/
CRFCom *p_rf;
CField *p_field;
SanimalRegist *animalRegists;
CTcpCom *p_tcpcom;
uint16_t _ID_ANIMAL=0;
unsigned char msgRfSender[32];



CThreadsField::CThreadsField()
{
  /***** MUTEX *****/
  mutex_readInfoWifi = new pthread_mutex_t();
  int mutex_readInfoWifi_status=pthread_mutex_init(mutex_readInfoWifi,NULL);
  mutex_sendInfoRF = new pthread_mutex_t();
  int mutex_sendInfoRF_status=pthread_mutex_init(mutex_sendInfoRF,NULL);
  mutex_readInfoRF = new pthread_mutex_t();
  int mutex_readInfoRF_status=pthread_mutex_init(mutex_sendInfoRF,NULL);
  mutex_process = new pthread_mutex_t();
  int mutex_Process_status=pthread_mutex_init(mutex_process,NULL);
  mutex_wifi_list = new pthread_mutex_t();
  int mutex_wifi_list_status=pthread_mutex_init(mutex_wifi_list,NULL);
  mutex_RF = new pthread_mutex_t();
  int mutex_RF_status=pthread_mutex_init(mutex_RF,NULL);
  mutex_field = new pthread_mutex_t();
  int mutex_field_status=pthread_mutex_init(mutex_field,NULL);
  mutex_animal = new pthread_mutex_t();
  int mutex_animal_status=pthread_mutex_init(mutex_animal,NULL);
  mutex_tcp = new pthread_mutex_t();
  int mutex_tcp_status=pthread_mutex_init(mutex_tcp,NULL);

  /***** SIGNALS *****/
  ts_readInfoWifi = new pthread_cond_t();
  int ts_readInfoTcp_status=pthread_cond_init(ts_readInfoWifi, NULL);
  ts_sendInfoRF = new pthread_cond_t();
  int ts_sendInfoRF_status=pthread_cond_init(ts_sendInfoRF, NULL);
  ts_readInfoRF = new pthread_cond_t();
  int ts_readInfoRF_status=pthread_cond_init(ts_readInfoRF, NULL);
  ts_process= new pthread_cond_t();
  int ts_process_status=pthread_cond_init(ts_process, NULL);

  /***** QUEUES *****/
  mq_close(mq_wifiReceiver);
  mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR | O_CREAT /*| O_EXCL*/, S_IRWXU | S_IRWXG, NULL);

  if (mq_wifiReceiver == (mqd_t)-1) {
    perror("CThreadsField::CThreadsField In mq_open()");
    //exit(1);
  }

  /***** SEMAPHORES *****/
  tsem_sendInfoWifi = new sem_t();
  sem_init(tsem_sendInfoWifi,0,0);

  /***** OBJECT *****/
  p_field = new CField();
  p_rf = new CRFCom();
  p_tcpcom = new CTcpCom();
  animalRegists=NULL;


  /***** THREADS *****/
  pthread_attr_t thread_attr=setAttr(60); //
  int rfS_status = pthread_create(&t_RFComSender,&thread_attr,pv_RFComSenderHandler,(void*)p_rf);
  int rfR_status = pthread_create(&t_RFComReceiver, &thread_attr,pv_RFComReceiverHandler, (void*)p_rf);
  int tcpS_status = pthread_create(&t_WIFIComSender,&thread_attr,pv_WIFIComSenderHandler,NULL);
  int tcpR_status = pthread_create(&t_WIFIComReceiver, &thread_attr,pv_WIFIComReceiverHandler, NULL);
  int process_status = pthread_create(&t_processAnimalInfo, &thread_attr,pv_processAnimalInfoHandler, (void*)p_field);

  if(rfS_status || rfR_status || tcpS_status || tcpR_status) {
    cout << "ERROR CREATING THREAD" << endl;
    exit(EXIT_FAILURE);
  }

  /***** OBJECTS CONF *****/
  unsigned char addrR[4] = {0xFF, 0xFF, 0x00, 0x01};

  p_rf->RFComSetAddR(addrR);
  run();
  pthread_mutex_lock(mutex_process);
  pthread_cond_signal(ts_process);
  pthread_mutex_unlock(mutex_process);

}
CThreadsField::~CThreadsField()
{
  /***** Delete QUEUES *****/
  mq_close(mq_wifiReceiver);
  //mq_unlink(MQ_WIFI_R);
  /***** Delete Threads *****/
  /***** Delete Objects *****/


}
pthread_attr_t CThreadsField::setAttr(int prio)
{
  pthread_attr_t thread_attr;
  int thread_policy;
  struct sched_param thread_param;
  pthread_attr_init (&thread_attr);
  pthread_attr_getschedpolicy(&thread_attr, &thread_policy);
  pthread_attr_getschedparam(&thread_attr, &thread_param);
  thread_param.sched_priority = prio;
  pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
  pthread_attr_setschedparam(&thread_attr,&thread_param);
  pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
  return thread_attr;
}

void * CThreadsField::pv_RFComSenderHandler(void *threadid)
{
  unsigned char animalAddr[4];

  while (1)
  {
    pthread_mutex_lock(mutex_sendInfoRF);
    pthread_cond_wait(ts_sendInfoRF, mutex_sendInfoRF);
    pthread_mutex_unlock(mutex_sendInfoRF);

    animalAddr[0]=0xAA;
    animalAddr[1]=0xAA;
    animalAddr[2]=0x00;
    animalAddr[3]=0x01;
    pthread_mutex_lock(mutex_RF);
    p_rf->RFComSender(animalAddr,msgRfSender);
    p_rf->RFComPrintTPaylo();
    pthread_mutex_unlock(mutex_RF);

    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_signal(ts_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);
    }
}


void  * CThreadsField::pv_RFComReceiverHandler(void *threadid)
{
  unsigned char msgRfReceiver[32];
  bool received = false;
  bool commandMatch = false;
  uint16_t id_animal = 0;
  int trys = 0;
  while (1)
  {
    pthread_mutex_lock(mutex_readInfoRF);
    pthread_cond_wait(ts_readInfoRF, mutex_readInfoRF);
    pthread_mutex_unlock(mutex_readInfoRF);

    memset(msgRfReceiver, '\0', 32);

    pthread_mutex_lock(mutex_RF);
    received=p_rf->RFComReceiver(msgRfReceiver);
    pthread_mutex_unlock(mutex_RF);

    if(!received) { //Message not received
      if(trys < 5) {
        trys++;
        cout << "timeout"<< endl;

        pthread_mutex_lock(mutex_sendInfoRF);
        pthread_cond_signal(ts_sendInfoRF);
        pthread_mutex_unlock(mutex_sendInfoRF);
      }
      else {
        cout << "not found"<< endl;
        trys = 0;
        commandMatch = true;

        memcpy(msgRfReceiver,msgRfSender, 4);
        memcpy(&id_animal,&msgRfSender[2],2);
        memset(&msgRfReceiver[4],0, 25);
        for(int i=0;i<32;i++) cout << hex << (int)msgRfReceiver[i];
        cout << endl;
        pthread_mutex_lock(mutex_field);
        p_field->setAnimal(id_animal,REDZONE); //ID, ZONE
        pthread_mutex_unlock(mutex_field);

        pthread_mutex_lock(mutex_wifi_list);
        p_field->setAnimalInfo(msgRfReceiver);
        pthread_mutex_unlock(mutex_wifi_list);
        sem_post(tsem_sendInfoWifi);
      }
    }
    else { //Message Received
      trys = 0;
      received = false;

      switch (msgRfReceiver[4]) { //Command Type
        case 'R': //Reset
        case 'C': //New conf
        case 'N': //First conf
        //Compare command sent with message received
        if(!memcmp(&msgRfReceiver[0], &msgRfSender[0], 32)) //Are the same
        commandMatch = true;
        else
        commandMatch = false;
        break;

        case 'I':
        if(!memcmp(&msgRfReceiver[0], &msgRfSender[0], 5)) //Are the same
        commandMatch = true;
        else
        commandMatch = false;
        break;
      }
      if(!commandMatch)
      {
        trys++;
        pthread_mutex_lock(mutex_sendInfoRF);
        pthread_cond_signal(ts_sendInfoRF);
        pthread_mutex_unlock(mutex_sendInfoRF);
      }

      else if((int)msgRfReceiver[13]>0 && (int)msgRfReceiver[13]<5 && msgRfReceiver[4]=='I') //AnimalZone
      {

        memcpy(&id_animal,&msgRfReceiver[2], 2);

        pthread_mutex_lock(mutex_field);
        p_field->setAnimal(id_animal,(int)msgRfReceiver[13]); //ID, ZONE
        pthread_mutex_unlock(mutex_field);

        pthread_mutex_lock(mutex_wifi_list);
        p_field->setAnimalInfo(msgRfReceiver);
        pthread_mutex_unlock(mutex_wifi_list);
        sem_post(tsem_sendInfoWifi);
      }
      else {
        cout << "CThreadsField::pv_RFComReceiverHandler ERROR!" << endl;
      }

    }
    if((trys == 0) && commandMatch) {
      delay(5000);
      cout << "Trigger ts_process" << endl;
      pthread_mutex_lock(mutex_process);
      pthread_cond_signal(ts_process);
      pthread_mutex_unlock(mutex_process);
    }
  }
}

void  * CThreadsField::pv_WIFIComReceiverHandler(void *threadid)
{
  unsigned char buffer[32];
  char cReadWifi[32];
  char poststring[300];
  char post_send[500];
  uint16_t idField = FIELDADDRT;
  struct mq_attr attr;
  unsigned int msgqWifi_prio = 1;
  char aux[500];
  char command_to_send[100];
  double latitude=0;
  double latitude2=0;
  double longitude=0;
  double longitude2=0;
  uint16_t idAnimal=0;
  bool empty=false;

  while (1)
  {
    pthread_mutex_lock(mutex_readInfoWifi);
    pthread_cond_wait(ts_readInfoWifi, mutex_readInfoWifi);
    pthread_mutex_unlock(mutex_readInfoWifi);
     memset(poststring,'\0',300);
     memset(post_send,'\0',400);
     memset(aux,'\0',500);
     memset(cReadWifi, '\0', 32);
     mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR  /* |O_CREAT*/);
     if (mq_wifiReceiver == (mqd_t)-1) {
       perror("CThreadsField::pv_WIFIComReceiverHandler In mq_open()");
       //exit(1);
      }
      else
      {
        if(p_tcpcom)
        {
          pthread_mutex_lock(mutex_field);
          empty=(p_field->checkNULL(GREENZONE) && p_field->checkNULL(YELLOWZONE) && p_field->checkNULL(REDZONE));
          pthread_mutex_unlock(mutex_field);

           if(!empty){
             snprintf(poststring,300,"field_id=%d",idField);
            snprintf(post_send,500,
            "POST /Airfences/give_data_rec.php HTTP/1.1\r\n"
           "Host: localhost\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n\r\n"
             "%s\r\n",strlen(poststring),poststring);
             pthread_mutex_lock (mutex_tcp);
             p_tcpcom->TcpComOpen();
             p_tcpcom->TcpComTransmite(post_send,strlen(post_send));
             p_tcpcom->TcpComReceive(aux,500);
             p_tcpcom->TcpComClose();
             pthread_mutex_unlock (mutex_tcp);
             pthread_mutex_lock(mutex_animal);
             idAnimal=_ID_ANIMAL;
             pthread_mutex_unlock(mutex_animal);
             memcpy(command_to_send,&aux[333],100);
             sscanf(command_to_send,"%lf;%lf;%lf;%lf",&latitude,&longitude,&latitude2,&longitude2);
             memcpy(&cReadWifi[0],&idField,2);
             memcpy(&cReadWifi[2],&idAnimal,2);
             cReadWifi[4]='C';
             memcpy(&cReadWifi[5],&latitude,4);
             memcpy(&cReadWifi[9],&longitude,4);
             memcpy(&cReadWifi[13],&latitude2,4);
             memcpy(&cReadWifi[17],&longitude2,4);
             mq_wifiReceiver = mq_send(mq_wifiReceiver, cReadWifi, 33, msgqWifi_prio);
             if (mq_wifiReceiver == (mqd_t)-1) {
               perror("CThreadsField::pv_processingInfoHandler In mq_send()");
               //exit(1);
             }
            }
            else
            {
              cout << "first time "<< endl;
              snprintf(poststring,300,"field_id=%d",idField);
              snprintf(post_send,500,
              "POST /Airfences/give_full_data.php HTTP/1.1\r\n"
              "Host: localhost\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "Content-Length: %d\r\n\r\n"
              "%s\r\n",strlen(poststring),poststring);
              pthread_mutex_lock (mutex_tcp);
              p_tcpcom->TcpComOpen();
              p_tcpcom->TcpComTransmite(post_send,strlen(post_send));
              p_tcpcom->TcpComReceive(aux,500);
              p_tcpcom->TcpComClose();
              pthread_mutex_unlock(mutex_tcp);
              int ids=0;
              for(int j=0;j<25;j++)
              {
                cout << aux[333+j] << " ";
              }
              sscanf(&aux[333],"%d#",&ids);
              cout << "valor::::::" << ids << endl;
              if(ids>0)
              {
                int i=0;
                while(i!=ids)
                {
                  i++;
                  pthread_mutex_lock(mutex_field);
                  p_field->setAnimal(i,REDZONE);
                  pthread_mutex_unlock(mutex_field);
                }
              }
              pthread_mutex_lock(mutex_process);
              pthread_cond_signal(ts_process);
              pthread_mutex_unlock(mutex_process);
            }
         }
       }
      mq_close(mq_wifiReceiver);
}
}

void *CThreadsField::pv_WIFIComSenderHandler(void *threadid)
{
  unsigned char buffer[32];
  char poststring[150];
  char post_send[400];
  uint16_t idAnimal=0;
  uint16_t idField=0;
  char command=0;
  float temperature=0;
  float battery=0;
  char zone=0;
  float latitude=0;
  float longitude=0;
  bool new_info=false;
  char aux[400];
  bool empty=false;
  uint16_t new_animal_to_add =0;
  while (1)
  {
    sem_wait (tsem_sendInfoWifi);
    if(p_tcpcom)
    {
      pthread_mutex_lock (mutex_wifi_list);
      p_field->getAnimalInfo(buffer);
      pthread_mutex_unlock (mutex_wifi_list);
      memset(poststring,'\0',150);
      memset(post_send,'\0',400);
      memset(aux,'\0',400);
          memcpy(&idField,&buffer[0],2);
          memcpy(&idAnimal,&buffer[2],2);
          memcpy(&temperature,&buffer[5],4);
          memcpy(&battery,&buffer[9],4);
          zone = buffer[13];
          memcpy(&latitude,&buffer[14],4);
          memcpy(&longitude,&buffer[18],4);

           snprintf(poststring,150,"animal_id=%d&zone_id=%d&field_id=%d&date=%s&temp=%f&latitude=%f&longitude=%f&bat=%f",idAnimal,zone,idField,"15",temperature,latitude, longitude, battery);
           snprintf(post_send,400,
             "POST /Airfences/give_data.php HTTP/1.1\r\n"
             "Host: localhost\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s\r\n",strlen(poststring),poststring);
            pthread_mutex_lock (mutex_tcp);
            p_tcpcom->TcpComOpen();
            p_tcpcom->TcpComTransmite(post_send,strlen(post_send));
            p_tcpcom->TcpComReceive(aux,400);
            p_tcpcom->TcpComClose();
            pthread_mutex_unlock (mutex_tcp);
            new_animal_to_add=0;
            char temp [30];
            memcpy(temp,&aux[334],30);
            sscanf(temp,"#%d#",&new_animal_to_add);

          if(aux[333]!='1')
          {
            pthread_mutex_lock(mutex_animal);
            _ID_ANIMAL=idAnimal;
            pthread_mutex_unlock(mutex_animal);
            pthread_mutex_lock(mutex_readInfoWifi);
            pthread_cond_signal(ts_readInfoWifi);
            cout << "new configuration" << endl;
            pthread_mutex_unlock(mutex_readInfoWifi);
          }
          if (new_animal_to_add!=0)
          {
            pthread_mutex_lock(mutex_animal);
            _ID_ANIMAL=idAnimal;
            pthread_mutex_unlock(mutex_animal);
            pthread_mutex_lock(mutex_field);
            p_field->setAnimal(new_animal_to_add,REDZONE); //ID, ZONE
            pthread_mutex_unlock(mutex_field);
          }
  }

}
}

void *CThreadsField::pv_processAnimalInfoHandler(void *threadid)
{
  uint16_t idAnimal = 0;
  uint16_t idField = FIELDADDRT;

  char newInfoDatabase[MAX_MSG_LEN];
  unsigned int sender;
  struct mq_attr attr;
  bool empty=false;
  while (1)
  {
    pthread_mutex_lock(mutex_process);
    pthread_cond_wait(ts_process, mutex_process);
    pthread_mutex_unlock(mutex_process);

    memset(newInfoDatabase, '\0', MAX_MSG_LEN);

    mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsField::pv_processAnimalInfoHandler In mq_open()");
      //exit(1);
    }
    pthread_mutex_lock(mutex_field);
    empty=(p_field->checkNULL(GREENZONE) && p_field->checkNULL(YELLOWZONE) && p_field->checkNULL(REDZONE));
    pthread_mutex_unlock(mutex_field);

      if(!empty){
        mq_getattr(mq_wifiReceiver, &attr);

      //Have a commmand to send by RF ('N' or 'C')
      if(attr.mq_curmsgs > 0) {
        cout << "MQUEUE WIFI_RECEIVER IS NOT EMPTY" << endl;
        int msgsz = mq_receive(mq_wifiReceiver, newInfoDatabase, MAX_MSG_LEN, &sender);
        if (msgsz == -1) {
          perror("CThreadsField::pv_processAnimalInfoHandler In mq_receive()");
          //exit(1);
        }
        mq_close(mq_wifiReceiver);

        //Receive command to send by RF
        pthread_mutex_lock(mutex_RF);
        for(int i=0; i < 32; i++) {
          msgRfSender[i] = static_cast<unsigned char>(newInfoDatabase[i]);
        }
        pthread_mutex_unlock(mutex_RF);

        pthread_mutex_lock(mutex_sendInfoRF);
        pthread_cond_signal(ts_sendInfoRF);
        pthread_mutex_unlock(mutex_sendInfoRF);
      }
      else { //Normal request to animal 'I'
      mq_close(mq_wifiReceiver);
      cout << "NORMAL REQUEST 'I'" << endl;

      pthread_mutex_lock(mutex_field);
      idAnimal = p_field->getNextAnimal();
      cout << "ID ANIMAL: " << idAnimal << endl;
      pthread_mutex_unlock(mutex_field);

      if(!idAnimal) { //No animals
        //Get info from database
        cout << "NO ANIMALS" << endl;
        pthread_mutex_lock(mutex_readInfoWifi);
        pthread_cond_signal(ts_readInfoWifi);
        pthread_mutex_unlock(mutex_readInfoWifi);
      }
      else { //If have an animal
        cout << "SELECT AN ANIMAL" << endl;
        pthread_mutex_lock(mutex_RF);
        // memset(msgRfSender, '\0', 32);

        memcpy(&msgRfSender[0], &idField, 2);
        memcpy(&msgRfSender[2], &idAnimal, 2);
        msgRfSender[4] = 'I';
        memset(&msgRfSender[5],1,25);
        pthread_mutex_unlock(mutex_RF);

        pthread_mutex_lock(mutex_sendInfoRF);
        pthread_cond_signal(ts_sendInfoRF);
        pthread_mutex_unlock(mutex_sendInfoRF);
      }
    }
  }
  else
  {
    cout << "null" << endl;
    pthread_mutex_lock(mutex_readInfoWifi);
    pthread_cond_signal(ts_readInfoWifi);
    pthread_mutex_unlock(mutex_readInfoWifi);
  }
}
}


void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  pthread_detach(t_WIFIComReceiver);
  pthread_detach(t_WIFIComSender);
  pthread_detach(t_processAnimalInfo);
}
