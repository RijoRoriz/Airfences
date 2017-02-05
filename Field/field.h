/*
	field.h - Library to handle the field on Airfences project
	Created by Ricardo Roriz and Pedro Costa, 12 December 2016.
	Last Update by Ricardo Roriz, 3 February 2017
*/

/************************************************************************************
	Class to handle all the information of the Field Module

	Members:
		struct Sanimal *mL_greenList -> linked list to save all animals in greenzone
		struct Sanimal *mL_yellowList -> linked list to save all animals  in yellowzone
		struct Sanimal *mL_redList -> linked list to save all animals  in redzone
		struct SanimalRegist *mL_sendInfoList -> linked list of mensages to send
		struct SanimalInfo *mL_wifiInfoList linked list of information to send to DB
		uint16_t idField -> self id
		int mi_count_Red -> counter of animals in RedZone
	  int mi_count_Yellow -> counter of animals in Yellowzone
	  int mi_count_Green; -> counter of animals in Greenzone

	Functions:
		void setAnimal(uint16_t id, int list) -> Set the animal in the linked list
			input:  uint16_t id -> id or the animal
							int list -> right list of animals (Ex:REDLIST)

		bool checkNULL(int zone)-> check if the a list is empty
			input:  int zone -> animals linked list zone (Ex:REDLIST)
			output: true -> if NULL
							false -> if not NULL

		uint16_t getAnimal(int list) -> get an Animal of "list" linked list
			input:  int list ->  right list of animals (Ex:REDLIST)
			output: uint16_t -> Animals id

		uint16_t getNextAnimal() -> get the next Animal to request
			output: uint16_t -> Animals id

		void setAnimalInfo(unsigned char *msg) -> set the information to send to data base
			input: unsigned char *msg) -> mensage

		void getAnimalInfo(unsigned char* returnedMsg) -> get the information to send to database
			input: unsigned char* returnedMsg -> mensage to send to the database


************************************************************************************/

#ifndef FIELD_H_
#define FIELD_H_

#include "fieldmap.h"
#include "defines.h"

/*
	Linked list to handle animals to request
*/
struct Sanimal
{
	uint16_t id;
	Sanimal * pointer;
};

/*
	Linked list to handle the response mensage of the request
*/

struct SanimalInfo
{
	unsigned char msg[32];
	SanimalInfo * pointer;
};

class CField
{
	private:
		CFieldMap fieldmap;

		struct Sanimal *mL_greenList;
		struct Sanimal *mL_yellowList;
		struct Sanimal *mL_redList;
		struct SanimalRegist *mL_sendInfoList;
		struct SanimalInfo *mL_wifiInfoList;
		uint16_t idField;

		int mi_count_Red;
	  int mi_count_Yellow;
	  int mi_count_Green;

	public:
		CField();
		~CField();
		void setAnimal(uint16_t id, int list);
		bool checkNULL(int zone);
	 	uint16_t getAnimal(int list);
		uint16_t getNextAnimal();
		void setAnimalInfo(unsigned char *msg);
		void getAnimalInfo(unsigned char* returnedMsg);
};

#endif /*FIELD_H_*/
