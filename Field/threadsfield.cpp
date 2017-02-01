#include "threadsfield.h"

/***** MUTEX *****/
pthread_mutex_t * mutex_readInfoWifi;
pthread_mutex_t * mutex_sendInfoRF;
pthread_mutex_t * mutex_readInfoRF;
pthread_mutex_t * mutex_process;
pthread_mutex_t * mutex_wifi_list;
pthread_mutex_t * mutex_RF;
pthread_mutex_t * mutex_field;

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
  p_field->setAnimal(1,GREENZONE);
  //p_tcpcom->TcpComOpen();
  run();
  pthread_mutex_lock(mutex_process);
  pthread_cond_signal(ts_process);
  pthread_mutex_unlock(mutex_process);

}
CThreadsField::~CThreadsField()
{
  /***** Delete QUEUES *****/
  mq_close(mq_wifiReceiver);
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
      if(trys < 3) {
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
        if((int)msgRfReceiver[13]>0 && (int)msgRfReceiver[13]<5) //AnimalZone
        {

          memcpy(&id_animal,&msgRfReceiver[2], 2);

          pthread_mutex_lock(mutex_field);
          cout << "*****  set animal ******" << "zona:"<< (int)msgRfReceiver[13] << "id:" << id_animal << endl;
          p_field->setAnimal(id_animal,(int)msgRfReceiver[13]); //ID, ZONE
          pthread_mutex_unlock(mutex_field);

          // pthread_mutex_lock(mutex_wifi_list);
          // //p_field->setAnimalInfo(msgRfReceiver);
          // pthread_mutex_unlock(mutex_wifi_list);
          //
          // //sem_post(tsem_sendInfoWifi);
        }
        else {
          cout << "CThreadsField::pv_RFComReceiverHandler ERROR!" << endl;
        }
        break;
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
  char cReadWifi[32];
  uint16_t idAnimal = 0;
  struct mq_attr attr;
  unsigned int msgqWifi_prio = 1;

  while (1)
  {
    pthread_mutex_lock(mutex_readInfoWifi);
    pthread_cond_wait(ts_readInfoWifi, mutex_readInfoWifi);
    pthread_mutex_unlock(mutex_readInfoWifi);

    memset(cReadWifi, '\0', 32);
    //Get message from wifi
    //Copy to cReadWifi

    mq_wifiReceiver = mq_open(MQ_WIFI_R, O_RDWR);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsField::pv_WIFIComReceiverHandler In mq_open()");
      //exit(1);
    }

    mq_wifiReceiver = mq_send(mq_wifiReceiver, cReadWifi, 33, msgqWifi_prio);
    if (mq_wifiReceiver == (mqd_t)-1) {
      perror("CThreadsField::pv_processingInfoHandler In mq_send()");
      //exit(1);
    }
    mq_close(mq_wifiReceiver);

    // pthread_mutex_lock(mutex_field);
    // idAnimal = p_field->getNextAnimal();
    // pthread_mutex_unlock(mutex_field);

    // if(!idAnimal) { //No animals in the list
    //   cout << "WIFI_RECEIVER NO ANIMALS IN THE LIST" << endl;
    //   pthread_mutex_lock(mutex_process);
    //   pthread_cond_signal(ts_process);
    //   pthread_mutex_unlock(mutex_process);
    // }
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
  float temperature=12.5;
  float battery=0;
  int zone=0;
  float latitude=0;
  float longitude=0;
  bool new_info=false;
  char aux[400];
  while (1)
  {
    sem_wait (tsem_sendInfoWifi);
    pthread_mutex_lock (mutex_wifi_list);
    //p_field->getAnimalInfo(buffer);
    pthread_mutex_unlock (mutex_wifi_list);
    p_tcpcom = new CTcpCom();
    memset(poststring,'\0',150);
    memset(post_send,'\0',400);
    memset(aux,'\0',400);
    p_tcpcom->TcpComOpen();
     snprintf(poststring,300,"animal_id=%d&zone_id=%d&field_id=%d&date=%s&temp=%f&latitude=%f&longitude=%f&bat=%f",idAnimal,zone,idField,"15",temperature,latitude, battery);
    //snprintf(poststring,300,"animal_id=1&zone_id=1&field_id=2&date=1&temp=1&latitude=1&longitude=1&bat=1");
    // cout << "poststring: "<< poststring << "tamanho: "<<strlen(poststring) <<endl << endl;
     snprintf(post_send,400,
       "POST /Airfences/give_data.php HTTP/1.1\r\n"
       "Host: localhost\r\n"
       "Content-Type: application/x-www-form-urlencoded\r\n"
      "Content-Length: %d\r\n\r\n"
      "%s\r\n",strlen(poststring),poststring);
    cout << "enviar:" << post_send << endl << endl;
    p_tcpcom->TcpComTransmite(post_send,strlen(post_send));
    p_tcpcom->TcpComReceive(aux,400);
    //aux[499]='\0';
    cout << "recebido: "<< aux << endl;
    p_tcpcom->TcpComClose();
    delete(p_tcpcom);
    //
    // if(new_info)
    // {
    //   pthread_mutex_lock(mutex_readInfoWifi);
    //   pthread_cond_signal(ts_readInfoWifi);
    //   pthread_mutex_unlock(mutex_readInfoWifi);
    // }

  }
}

void *CThreadsField::pv_processAnimalInfoHandler(void *threadid)
{
  uint16_t idAnimal = 0;
  uint16_t idField = FIELDADDRT;

  char newInfoDatabase[MAX_MSG_LEN];
  unsigned int sender;
  struct mq_attr attr;

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

    mq_getattr(mq_wifiReceiver, &attr);

    //Have a commmand to send by RF ('N' or 'C')
    //if(attr.mq_curmsgs > 0) {
    if(0){
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
      msgRfSender[5] = 1;
      msgRfSender[6] = 1;
      msgRfSender[7] = 1;
      pthread_mutex_unlock(mutex_RF);

      pthread_mutex_lock(mutex_sendInfoRF);
      pthread_cond_signal(ts_sendInfoRF);
      pthread_mutex_unlock(mutex_sendInfoRF);
    }
  }
}
}


void  CThreadsField::run()
{
  pthread_detach(t_RFComReceiver);
  pthread_detach(t_RFComSender);
  // pthread_detach(t_WIFIComReceiver);
  pthread_detach(t_WIFIComSender);
  pthread_detach(t_processAnimalInfo);
}
