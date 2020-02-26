#include <iostream>
#include "Seller.h"
#include "DistributionCentre.h"
#include <string>
#include <ctime>

using namespace std;

Seller sellers[3];
DistributionCentre dist;
int numOfSellers = 3;
int salesMissed;

void SetUp()
{
	sellers[0] = Seller(350.0f, 5.0f, 2.0f);
	sellers[1] = Seller(450.0f, 10.0f, 4.0f);
	sellers[2] = Seller(800.0f, 15.0f, 6.0f);
	Lorry lorry;
	lorry.carryLimit = 200.0f;
	lorry.daysUntilArrive = 0.0f;
	lorry.carrying = 0.0f;
	lorry.phase = IDLE;
	dist = DistributionCentre(500.0f, 300.0f, lorry);
}

void MoveStock(int sellerID, int LorryID)
{
	//if (dist.GetCurrentStock() >= dist.lorries[LorryID].carryLimit)
	//{
	//	dist.RemoveStock(dist.lorries[LorryID].carryLimit);
	//	dist.lorries[LorryID].carrying = dist.lorries[LorryID].carryLimit;

	//}
	//else
	//{
	//	dist.lorries[LorryID].carrying = dist.GetCurrentStock();
	//	dist.RemoveStock(dist.GetCurrentStock());
	//}
	
	dist.lorries[LorryID].carrying = dist.lorries[LorryID].carryLimit;


	dist.lorries[LorryID].daysUntilArrive = sellers[sellerID].GetTravelTime();
	dist.lorries[LorryID].sellerID = sellerID;
	dist.lorries[LorryID].phase = MOVING;

	std::cout << ("Truck " + to_string(LorryID + 1) + " has been dispatched to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + " carrying  " + to_string(dist.lorries[LorryID].carrying) + " stock and will arrive in " + to_string(dist.lorries[LorryID].daysUntilArrive) + " days\n");
}

void DeliverStock(int sellerID, int LorryID)
{
	float newStock = sellers[sellerID].GetStock() + dist.lorries[LorryID].carrying;
	if (newStock <= sellers[sellerID].GetMaxStock())
	{
		sellers[sellerID].AddStock(dist.lorries[LorryID].carrying);
		std::cout << ("Truck " + to_string(LorryID + 1) + " has delivered " + to_string(dist.lorries[LorryID].carrying ) + " to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + "\n");
		dist.lorries[LorryID].carrying = 0;

	}
	else
	{
		
		std::cout << ("Truck " + to_string(LorryID + 1) + " has delivered " + to_string((sellers[sellerID].GetMaxStock() - sellers[sellerID].GetStock())) + " to shop " + to_string(dist.lorries[LorryID].sellerID + 1) + "\n");
		sellers[sellerID].SetStock(sellers[sellerID].GetMaxStock());
		dist.lorries[LorryID].carrying = 0;
	}

	
}

void Sell(int sellerID)
{
	cout << ("Shop " + to_string(sellerID+1) + " started with "  +  to_string(sellers[sellerID].GetStock()));

	if (sellers[sellerID].GetStock() >= sellers[sellerID].GetSalesPerDay())
	{
		cout << (" stock, sold " + to_string(sellers[sellerID].GetSalesPerDay()));
		sellers[sellerID].SellStock(sellers[sellerID].GetSalesPerDay());
		cout << (" stock and ended with " + to_string(sellers[sellerID].GetStock()) + "stock, missing 0 sales\n");
	}
	else
	{
		cout << (" stock, sold " + to_string(sellers[sellerID].GetStock()));
		
		int sm = sellers[sellerID].GetSalesPerDay() - sellers[sellerID].GetStock();
		salesMissed += sm;
		sellers[sellerID].SellStock(sellers[sellerID].GetStock());
		cout << (" stock ended with " + to_string(sellers[sellerID].GetStock()) + "stock, missing "+ to_string(sm )+" sales\n");
	}
	
}

void RandomPlaceholder(int lorryID)
{
	
	MoveStock((rand() % 3),lorryID);
	
}

void CheckLorries()
{
	for (int i = 0; i < 2; i++)
	{
		if (dist.lorries[i].phase == IDLE)
		{
			RandomPlaceholder(i);
		}
		else if (dist.lorries[i].phase == MOVING)
		{
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i+1) + " is on its way to shop " + to_string(dist.lorries[i].sellerID+1) + " will arrive in " + to_string(dist.lorries[i].daysUntilArrive)  +" days\n");
			}
			else
			{
				dist.lorries[i].daysUntilArrive -= 1;
				DeliverStock(dist.lorries[i].sellerID,i);
				dist.lorries[i].phase = ARRIVED;
			}
		}
		else if (dist.lorries[i].phase == ARRIVED)
		{
			dist.lorries[i].daysUntilArrive = sellers[dist.lorries[i].sellerID].GetTravelTime();
			std::cout << ("Truck  " + to_string(i + 1) + " is returning from shop " + to_string(dist.lorries[i].sellerID+1) + " and will arrive in " + to_string(dist.lorries[i].daysUntilArrive) + " days\n");
			dist.lorries[i].phase = RETURNING;
		}
		else if (dist.lorries[i].phase == RETURNING)
		{
			if (dist.lorries[i].daysUntilArrive > 1)
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i + 1) + " is returning from shop " + to_string(dist.lorries[i].sellerID+1) + " and will arrive in " + to_string(dist.lorries[i].daysUntilArrive) + " days\n");
			}
			else
			{
				dist.lorries[i].daysUntilArrive -= 1;
				std::cout << ("Truck  " + to_string(i + 1) + " has returned from shop " + to_string(dist.lorries[i].sellerID +1) + " and is waiting to be dispatched\n");
				dist.lorries[i].phase = IDLE;
			}
		}
	} 

}


void ProcessDay()
{
	//CheckLorries
	CheckLorries();
	//Sell
	for (int i = 0; i < numOfSellers; i++)
	{
		Sell(i);
	}
	
}



int main()
{
	srand(std::time(nullptr));

	SetUp();
	int daysToProcess = 365;
	
	//processDays 
	for (int i = 1; i <= daysToProcess; i++)
	{
		std::cout << ("Day " + to_string(i) + ":\n\n") ;
		ProcessDay();
		std::cout << ("\n");
	}

	cout << ("\n TOTAl SALES MISSED: " + to_string(salesMissed) + "\n");
   
}




