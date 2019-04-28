#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "Restaurant.h"
#include "..\Events\ArrivalEvent.h"
#include"../Events/CancellationEvent.h"
#include"../Events/PromotionEvent.h"
#include"../InputClass/ReadInput.h"


Restaurant::Restaurant()
{
	pGUI = NULL;
}

void Restaurant::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE	mode = pGUI->getGUIMode();
	//Simulate();

	switch (mode)	//Add a function for each mode in next phases
	{
	case MODE_INTR:
		GUI_mode = 0;
		Simulate(); 
		break;
	case MODE_STEP:
		GUI_mode = 1; 
		Simulate();
		break;
	case MODE_SLNT: 
		// Need to only extract output file without graphing the window
		GUI_mode = 2; 
		Simulate();
		break;
	};

}
void Restaurant::go_without_simulation() {
	// Here We should Write the code for the silent mode and call it in the other two modes to produce the output file . 

}

//////////////////////////////////  Event handling functions   /////////////////////////////
void Restaurant::AddEvent(Event* pE)	//adds a new event to the queue of events
{
	EventsQueue.enqueue(pE);
}

//Executes ALL events that should take place at current timestep
void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
	Event *pE;
	while (EventsQueue.peekFront(pE))	//as long as there are more events
	{
		if (pE->getEventTime() > CurrentTimeStep)	//no more events at current time
			return;

		pE->Execute(this);
		EventsQueue.dequeue(pE);	//remove event from the queue
		delete pE;		//deallocate event object from memory
	}

}


Restaurant::~Restaurant()
{
	DeleteMotorcycles();
	delete pGUI;
}

void Restaurant::DeleteMotorcycles()
{
	for (int i = 0; i < 4; i++) {
		Motorcycle* Pord;
		while (!M_Normal[i].isEmpty()) {
			M_Normal[i].dequeue(Pord);
			delete Pord;
		}
		while (!M_VIP[i].isEmpty()) {
			M_VIP[i].dequeue(Pord);
			delete Pord;
		}
		while (!M_Frozen[i].isEmpty()) {
			M_Frozen[i].dequeue(Pord);
			delete Pord;
		}
	}
}


bool Restaurant::checker() {
	bool temp = false; 
	if (!in_service_Motorcyles[0].isEmpty() || !in_service_Motorcyles[1].isEmpty() || !in_service_Motorcyles[2].isEmpty() || !in_service_Motorcyles[3].isEmpty()) {
		temp = true;
	}
	bool temp2[4] = { 0,0,0,0 };
	for (int i = 0; i < 4; i++){
		if (!vipOrders[i].isEmpty()|| !FrozenOrders[i].isEmpty() || !NormalOrders[i].is_empty()) {
			temp2[i] = true;
	}	
	}
	return (temp || temp2[0] || temp2[1] || temp2[2] || temp2[3]);
}

void Restaurant::Simulate()
{
	ReadInput pIn;
	string name;
	pIn.read(this,pGUI);
	timeStep = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 3; j++)
		{
			NumberOfActiveOrders[i][j] = 0;
		}

	for (int i = 0; i < 4; i++) 
		for (int j = 0; j < 3; j++) 
			NumberOfMotorcycles[i][j] = pIn.getMotorCyclesInRegion(static_cast<ORD_TYPE>(j), static_cast<REGION>(i));
	
	

	/* ADD MORE INITIALIZATIONS */

	/* READ INPUT FROM pIn */
		// Here i should write the assignment process. 

	bool ch = false;
	
	while (!EventsQueue.isEmpty() || ch)
	{
		Event* tempPtr;

		//remove one order from each type each timeStep
		Order* pOrd;

		//check if there is more than one event at the same timeStep
		while (EventsQueue.peekFront(tempPtr))
		{
			if (tempPtr->getEventTime() > timeStep) break;

			tempPtr->Execute(this);

			Event* junk;
			EventsQueue.dequeue(junk);
		}
		for (int i = 0; i < 4; i++) {// Here i should loop to assign motor cycles to VIP orders (i the iteration for each region ) 
			bool i_could = true; 
			Motorcycle * temp_Motor; 

			while (!vipOrders[i].isEmpty()&&i_could) { // Here add another condition to exit the loop if there is no motor cycles now. 
				if (M_VIP[i].dequeue(temp_Motor)) {
					temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor); 
					i_could = true;
					vipOrders[i].dequeue(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT()); 
					//NumberOfActiveOrders[i][2]--;
					inServicsOrder[i].enqueue(pOrd);
				}
				else if (M_Normal[i].dequeue(temp_Motor)) {
					temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor);
					i_could = true;
					vipOrders[i].dequeue(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT());
					//NumberOfActiveOrders[i][2]--;
					inServicsOrder[i].enqueue(pOrd);
				}
				else if (M_Frozen[i].dequeue(temp_Motor)) {
					temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor);
					i_could = true;
					vipOrders[i].dequeue(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT());
					//NumberOfActiveOrders[i][2]--;
					inServicsOrder[i].enqueue(pOrd);
				}
				else {
					i_could = false;
				}
			}
			// Here trying to serve normal orders after serving VIP orders 
			i_could = true; 
			while (!FrozenOrders[i].isEmpty() && i_could) {
			 if (M_Frozen[i].dequeue(temp_Motor)) {
				 temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor);
					i_could = true;
					FrozenOrders[i].dequeue(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT());
					//NumberOfActiveOrders[i][3]--;
					inServicsOrder[i].enqueue(pOrd);
				}
			 else {
				 i_could = false; 
			 }
			}
			// Here trying to serve normal Orders after serving VIP first and frozen
			i_could = true; 
			while (!NormalOrders[i].is_empty() &&i_could) {
				if (M_Normal[i].dequeue(temp_Motor)) {
					temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor);
					i_could = true;
					NormalOrders[i].removeFront(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT());
					//NumberOfActiveOrders[i][0]--;
					inServicsOrder[i].enqueue(pOrd);
				}
				
				
				else if (M_VIP[i].dequeue(temp_Motor)) {
					temp_Motor->set_status(SERV);
					in_service_Motorcyles[i].enqueue(temp_Motor);
					i_could = true;
					NormalOrders[i].removeFront(pOrd);
					pOrd->set_wait_time(timeStep - pOrd->get_AVT());
					pOrd->set_Service_time(pOrd->GetDistance() / temp_Motor->get_speed());
					pOrd->set_Finish_time(pOrd->get_AVT() + pOrd->get_SVT() + pOrd->get_WT());
					temp_Motor->set_again_use(pOrd->get_FT() + pOrd->get_SVT());
					//NumberOfActiveOrders[i][0]--;
					inServicsOrder[i].enqueue(pOrd);
				}
				else {
					i_could = false; 
				}
			}
			
		}
		// Here i should remove orders which have arrived to the customer as well as the availmotorcyles 
		for (int i = 0; i < 4; i++) {
			Order *ord;
			while (inServicsOrder[i].peekFront(ord)) {
				if (ord->get_FT() <= timeStep) {
					inServicsOrder[i].dequeue(ord); 
					Delivered_orders[i].enqueue(ord);
					if (ord->GetType() == TYPE_NRM) {
						NumberOfActiveOrders[i][0]--;
					}
					else if (ord->GetType() == TYPE_FROZ) {
						NumberOfActiveOrders[i][1]--;
					}
					else {
						NumberOfActiveOrders[i][2]--;
					}
				}
				else {
					break; 
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			Motorcycle *ord;
			while (in_service_Motorcyles[i].peekFront(ord)) {
				if (ord->get_again_use()<= timeStep) {
					in_service_Motorcyles[i].dequeue(ord);
					ord->set_status(IDLE);
					if (ord->GetType() == TYPE_NRM) {
						M_Normal[i].enqueue(ord);
					}
					else if (ord->GetType() == TYPE_FROZ) {
						M_Frozen[i].enqueue(ord);
					}
					else {
						M_VIP[i].enqueue(ord);
					}
				}
				else {
					break;
				}
			}
		}
	
		pGUI->ResetDrawingList();
		
		for (int i = 0; i < 4; i++) {
			PriorityQueue<Order*,less_ptrs<Order*>> tempVip = inServicsOrder[i];
			while (tempVip.dequeue(pOrd)) {
				pGUI->AddOrderForDrawing(pOrd);
			}	
		}
		updateRestaurantsInfo();
		pGUI->UpdateInterface();
		if (GUI_mode == 0) {
			pGUI->waitForClick();
			timeStep++;
			ch = checker();
		}
		else if (GUI_mode == 1) {
			Sleep(1000);
			timeStep++; 
			ch = checker(); 
		}
		else {
			timeStep++;
			ch = checker();
		}
	}
	output_file(); 
	pGUI->PrintMessage("Simulation Finished Thanks for watching");
	//cout << "Ended successfully" << endl;
}

void Restaurant::Just_A_Demo()
{

	//
	// THIS IS JUST A DEMO FUNCTION
	// IT SHOULD BE REMOVED IN PHASE 1 AND PHASE 2

	int EventCnt;
	Order* pOrd;
	Event* pEv;
	srand(time(NULL));

	pGUI->PrintMessage("Just a Demo. Enter EVENTS Count(next phases should read I/P filename):");
	EventCnt = atoi(pGUI->GetString().c_str());	//get user input as a string then convert to integer

	pGUI->UpdateInterface();

	pGUI->PrintMessage("Generating orders randomly... In next phases, orders should be loaded from a file");

	int EvTime = 0;

	//Create Random events
	//All generated event will be "ArrivalEvents" for the demo
	for (int i = 0; i < EventCnt; i++)
	{
		int O_id = i + 1;

		//Rendomize order type
		int OType;
		if (i < EventCnt*0.2)	//let 1st 20% of orders be VIP (just for sake of demo)
			OType = TYPE_VIP;
		else if (i < EventCnt*0.5)
			OType = TYPE_FROZ;	//let next 30% be Frozen
		else
			OType = TYPE_NRM;	//let the rest be normal


		int reg = rand() % REG_CNT;	//randomize region


		//Randomize event time
		EvTime += rand() % 4;
		pEv = new ArrivalEvent(EvTime, O_id, (ORD_TYPE)OType, (REGION)reg);
		AddEvent(pEv);

	}

	int CurrentTimeStep = 1;
	//as long as events queue is not empty yet
	while (!EventsQueue.isEmpty())
	{
		//print current timestep
		char timestep[10];
		itoa(CurrentTimeStep, timestep, 10);
		pGUI->PrintMessage(timestep);


		ExecuteEvents(CurrentTimeStep);	//execute all events at current time step
		//The above line may add new orders to the DEMO_Queue

		//Let's draw all arrived orders by passing them to the GUI to draw

		while (DEMO_Queue.dequeue(pOrd))
		{
			pGUI->AddOrderForDrawing(pOrd);
			pGUI->UpdateInterface();
		}
		Sleep(1000);
		CurrentTimeStep++;	//advance timestep
	}


	pGUI->PrintMessage("generation done, click to END program");
	pGUI->waitForClick();


}
////////////////

void Restaurant::AddtoDemoQueue(Order *pOrd)
{
	DEMO_Queue.enqueue(pOrd);
}

Order* Restaurant::getDemoOrder()
{
	Order* pOrd;
	DEMO_Queue.dequeue(pOrd);
	return pOrd;

}

void Restaurant::AddVipOrder(Order * o)
{
	NumberOfActiveOrders[o->GetRegion()][o->GetType()]++;
	vipOrders[o->GetRegion()].enqueue(o);
}

void Restaurant::AddFrozenOrder(Order * o)
{
	NumberOfActiveOrders[o->GetRegion()][o->GetType()]++;
	FrozenOrders[o->GetRegion()].enqueue(o);
}

void Restaurant::AddNormalOrder(Order * o)
{
	NumberOfActiveOrders[o->GetRegion()][o->GetType()]++;
	NormalOrders[o->GetRegion()].insert_at_end(o);
}

Order* Restaurant::getOrderById(int orderID)
{
	for (int i = 0; i < 4; i++)
	{
		Node<Order*>* temp = NormalOrders[i].getHead();
		while (temp) {
			if ((temp->getItem())->GetID() == orderID) return temp->getItem();
			temp = temp->getNext();
		}
	}
	return NULL;
}

void Restaurant::cancelOrder(Order * o)
{
	if (o == NULL || ( o->GetType() != 0)) return;
	NumberOfActiveOrders[o->GetRegion()][0]--;
	NormalOrders[o->GetRegion()].remove(o);
	delete o;
}

void Restaurant::promoteOrder(Order * o)
{
	if (o == NULL) return;
	o->setType(TYPE_VIP);
	vipOrders[o->GetRegion()].enqueue(o);
	NormalOrders[o->GetRegion()].remove(o);
	NumberOfActiveOrders[o->GetRegion()][0]--;
	NumberOfActiveOrders[o->GetRegion()][2]++;
}
void Restaurant::creat_motor_cycles(int *speed, int *regA, int *regB, int *regC, int *regD) {
	int i = 0;
	for (i = 0; i < regA[0]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(i);  M->set_REG(A_REG);  M->set_speed(speed[0]);  M->set_type(TYPE_NRM); M_Normal[0].enqueue(M);
	}
	int k = i;
	for (i = 0; i < regB[0]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(B_REG);  M->set_speed(speed[0]);  M->set_type(TYPE_NRM); M_Normal[1].enqueue(M); k++;
	}
	for (i = 0; i < regC[0]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(C_REG);  M->set_speed(speed[0]);  M->set_type(TYPE_NRM); M_Normal[2].enqueue(M);  k++;
	}
	for (i = 0; i < regD[0]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(D_REG);  M->set_speed(speed[0]);  M->set_type(TYPE_NRM); M_Normal[3].enqueue(M);  k++;
	}
	for (i = 0; i < regA[1]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(A_REG);  M->set_speed(speed[1]);  M->set_type(TYPE_FROZ); M_Frozen[0].enqueue(M);   k++;
	}
	for (i = 0; i < regB[1]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(B_REG);  M->set_speed(speed[1]);  M->set_type(TYPE_FROZ); M_Frozen[1].enqueue(M);   k++;
	}
	for (i = 0; i < regC[1]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(C_REG);  M->set_speed(speed[1]);  M->set_type(TYPE_FROZ); M_Frozen[2].enqueue(M);   k++;
	}
	for (i = 0; i < regD[1]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(D_REG);  M->set_speed(speed[1]);  M->set_type(TYPE_FROZ); M_Frozen[3].enqueue(M);   k++;
	}
	for (i = 0; i < regA[2]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(i);  M->set_REG(A_REG);  M->set_speed(speed[2]);  M->set_type(TYPE_VIP); M_VIP[0].enqueue(M);   k++;
	}
	for (i = 0; i < regB[2]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(B_REG);  M->set_speed(speed[2]);  M->set_type(TYPE_VIP); M_VIP[1].enqueue(M);   k++;
	}
	for (i = 0; i < regC[2]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(C_REG);  M->set_speed(speed[2]);  M->set_type(TYPE_VIP); M_VIP[2].enqueue(M);   k++;
	}
	for (i = 0; i < regD[2]; i++) {
		Motorcycle * M = new Motorcycle;
		M->set_id(k);  M->set_REG(D_REG);  M->set_speed(speed[2]);  M->set_type(TYPE_VIP); M_VIP[3].enqueue(M);   k++;
	}

}

void Restaurant::updateRestaurantsInfo()
{
	string s, s1, s2, s3, s4, s5, s6;

	updateStringsInfo(s, s1, s2, s3, s4, s5, s6, timeStep);
	pGUI->print_msg_multi(s, s1, s2, s3, s4, s5, s6);
}


void Restaurant::updateStringsInfo(string& s,string & s1, string & s2, string & s3, string & s4, string & s5,string &s6,int timeStep)
{
	s = "                                   Orders                Motorcycles";
	s1 = "                                   N   F   V                    N   F   V";

	s2 = "Region A                    ";
	s2 += to_string(NumberOfActiveOrders[0][0]);
	s2 += "   " + to_string(NumberOfActiveOrders[0][1]);
	s2 += "   " + to_string(NumberOfActiveOrders[0][2]);
	s2 += "                     " + to_string(NumberOfMotorcycles[0][0]);
	s2 += "   " + to_string(NumberOfMotorcycles[0][1]);
	s2 += "   " + to_string(NumberOfMotorcycles[0][2]);

	s3 = "Region B                   ";
	s3 += to_string(NumberOfActiveOrders[1][0]);
	s3 += "   " + to_string(NumberOfActiveOrders[1][1]);
	s3 += "   " + to_string(NumberOfActiveOrders[1][2]);
	s3 += "                     " + to_string(NumberOfMotorcycles[1][0]);
	s3 += "   " + to_string(NumberOfMotorcycles[1][1]);
	s3 += "   " + to_string(NumberOfMotorcycles[1][2]);

	s4 = "Region C                   " + to_string(NumberOfActiveOrders[2][0]);
	s4 += "   " + to_string(NumberOfActiveOrders[2][1]);
	s4 += "   " + to_string(NumberOfActiveOrders[2][2]);
	s4 += "                     " + to_string(NumberOfMotorcycles[2][0]);
	s4 += "   " + to_string(NumberOfMotorcycles[2][1]);
	s4 += "   " + to_string(NumberOfMotorcycles[2][2]);

	s5 = "Region D                   " + to_string(NumberOfActiveOrders[3][0]);
	s5 += "   " + to_string(NumberOfActiveOrders[3][1]);
	s5 += "   " + to_string(NumberOfActiveOrders[3][2]);
	s5 += "                     " + to_string(NumberOfMotorcycles[3][0]);
	s5 += "   " + to_string(NumberOfMotorcycles[3][1]);
	s5 += "   " + to_string(NumberOfMotorcycles[3][2]);
	
	s6 = "Timestep: " + to_string(timeStep);
}
void Restaurant::output_file() {
	ofstream ofile;
	ofile.open("OUTPUT.txt");
	int types[4][3];
	int total_sum_of_MC = 0; 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			types[i][j] = 0;
			total_sum_of_MC += NumberOfMotorcycles[i][j];
		}
	}
	double avgwait[4] = { 0,0,0,0 };
	double avgser[4] = { 0,0,0,0 };
	int orders[4] = { 0,0,0,0 }; // for each region; 
	for (int i = 0; i < 4; i++) {
		Queue<Order*> temp = Delivered_orders[i];
		ofile << "FT" << "      " << "ID" << "     " << "AT" << "     " << "WT" << "      " << "ST" << endl;
		while (!temp.isEmpty()) {
			orders[i]++;
			Order*ord;
			temp.dequeue(ord);
			avgwait[i] += ord->get_WT();
			avgser[i] += ord->get_SVT();
			if (ord->GetType() == TYPE_NRM) {
				types[i][0]++;
			}
			else if (ord->GetType() == TYPE_FROZ) {
				types[i][1]++;
			}
			else {
				types[i][2]++;
			}
			avgwait[i] /= orders[i];
			avgser[i] /= orders[i];
			//Delivered_orders[i].dequeue(ord);
			ofile << ord->get_FT() << "      " << ord->GetID() << "      " << ord->get_AVT() << "     " << ord->get_SVT() << endl; 
		}
		ofile << "......................................................................................................" << endl; 
		ofile << "......................................................................................................" << endl;
		if (i == 0) {
			ofile << "REGION A" << endl;
			ofile << "Orders" << "  :" << orders[i] << "[" << "NORM: " << types[i][0] << "   " << ", FROZ: " << types[i][0] << "  VIP: " << types[i][2] <<"]"<< endl;
			ofile<<"MOROTC:  "<<NumberOfMotorcycles[0][0]+ NumberOfMotorcycles[0][1]+ NumberOfMotorcycles[0][2]<<"   " << "[" << "NORM: " << NumberOfMotorcycles[0][0] << "   " << ", FROZ: " << NumberOfMotorcycles[0][1] << "  VIP: " << NumberOfMotorcycles[0][2] <<"]"<< endl;
			ofile << "AVG Wait" << "      " << avgwait[i] << "           " << "AVG SERVICE TIME     " << avgser[i] << endl; 
		}
		else if (i == 1) {
			ofile << "REGION B" << endl;
			ofile << "Orders" << "  :" << orders[i] << "[" << "NORM: " << types[i][0] << "   " << ", FROZ: " << types[i][0] << "  VIP: " << types[i][2] <<"]"<< endl;
			ofile << "MOROTC:  " << NumberOfMotorcycles[1][0] + NumberOfMotorcycles[1][1] + NumberOfMotorcycles[1][2] << "   " << "[" << "NORM: " << NumberOfMotorcycles[1][0] << "   " << ", FROZ: " << NumberOfMotorcycles[1][1] << "  VIP: " << NumberOfMotorcycles[1][2] <<"]"<< endl;
			ofile << "AVG Wait" << "      " << avgwait[i] << "           " << "AVG SERVICE TIME     " << avgser[i] << endl;
		}
		else if (i == 2) {
			ofile << "REGION C" << endl;
			ofile << "Orders" << "  :" << orders[i] << "[" << "NORM: " << types[i][0] << "   " << ", FROZ: " << types[i][0] << "  VIP: " << types[i][2] <<"]"<< endl;
			ofile << "MOROTC:  " << NumberOfMotorcycles[2][0] + NumberOfMotorcycles[2][1] + NumberOfMotorcycles[2][2] << "   " << "[" << "NORM: " << NumberOfMotorcycles[2][0] << "   " << ", FROZ: " << NumberOfMotorcycles[2][1] << "  VIP: " << NumberOfMotorcycles[2][2] <<"]"<< endl;
			ofile << "AVG Wait" << "      " << avgwait[i] << "           " << "AVG SERVICE TIME     " << avgser[i] << endl;
		}
		else {
			ofile << "REGION D" << endl;
			ofile << "Orders" << "  :" << orders[i] << "[" << "NORM: " << types[i][0] << "   " << ", FROZ: " << types[i][0] << "  VIP: " << types[i][2] <<"]"<< endl;
			ofile << "MOROTC:  " << NumberOfMotorcycles[3][0] + NumberOfMotorcycles[3][1] + NumberOfMotorcycles[3][2] << "   " << "[" << "NORM: " << NumberOfMotorcycles[3][0] << "   " << ", FROZ: " << NumberOfMotorcycles[3][1] << "  VIP: " << NumberOfMotorcycles[3][2] << "]" << endl; ;
			ofile << "AVG Wait" << "      " << avgwait[i] << "           " << "AVG SERVICE TIME     " << avgser[i] << endl;
		}
	}
	ofile << "For the Hole Restaurant " << endl;
	ofile << "FT" << "      " << "ID" << "     " << "AT" << "     " << "WT" << "      " << "ST" << endl;
	for (int i = 0; i < 4; i++) {
		while (!Delivered_orders[i].isEmpty()) {
			Order*ord;
			Delivered_orders[i].dequeue(ord);
			ofile << ord->get_FT() << "      " << ord->GetID() << "      " << ord->get_AVT() << "     " << ord->get_SVT() << endl;
		}
	}
	
	ofile << "Orders" << "  :" << orders[0]+ orders[1]+ orders[2]+ orders[3] << "[" << "NORM: " << types[0][0]+ types[1][0]+ types[2][0]+ types[3][0] << "   " << ", FROZ: " << types[1][1]+ types[2][1]+ types[3][1]+ types[0][1] << "  VIP: " << types[0][2]+ types[1][2]+ types[2][2]+ types[3][2] <<"]"<< endl;
	ofile << "MOROTC:  " << total_sum_of_MC << "   " << "[" << "NORM: " << NumberOfMotorcycles[3][0]+ NumberOfMotorcycles[0][0] + NumberOfMotorcycles[1][0] + NumberOfMotorcycles[2][0] << "   " << ", FROZ: " << NumberOfMotorcycles[3][1]+ NumberOfMotorcycles[2][1] + NumberOfMotorcycles[1][1] + NumberOfMotorcycles[1][0] << "  VIP: " << NumberOfMotorcycles[3][2] + NumberOfMotorcycles[2][2] + NumberOfMotorcycles[1][2] + NumberOfMotorcycles[0][2] <<"]"<< endl;
	ofile << "AVG Wait" << "      " << (avgwait[0]+ avgwait[1]+ avgwait[2]+ avgwait[3])/4 << "           " << "AVG SERVICE TIME     " << (avgser[0] + avgser[1] + avgser[2] + avgser[3]) / 4 << endl;
	ofile.close();
}


/// ==> end of DEMO-related function