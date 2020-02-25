#include <iostream>
#include "Seller.h"
#include "DistributionCentre.h"

Seller sellers[3];
DistributionCentre dist;
int salesMissed;

void SetUp()
{
	sellers[0] = Seller(250.0f, 20.0f, 3.0f);
	sellers[1] = Seller(350.0f, 40.0f, 5.0f);
	sellers[2] = Seller(400.0f, 50.0f, 10.0f);
	Lorry lorry;
	lorry.carryLimit = 50.0f;
	lorry.daysUntilReturn = 0.0f;
	lorry.carrying = 0.0f;
	dist = DistributionCentre(500.0f, 300.0f, lorry);
}

void moveStock(int sellerID, int LorryID)
{
	if (dist.GetCurrentStock() >= dist.lorries[LorryID].carryLimit)
	{
		dist.RemoveStock(dist.lorries[LorryID].carryLimit);
		dist.lorries[LorryID].carrying = dist.lorries[LorryID].carryLimit;

	}
	else
	{
		dist.lorries[LorryID].carrying = dist.GetCurrentStock();
		dist.RemoveStock(dist.GetCurrentStock());
	}
	
	dist.lorries[LorryID].daysUntilReturn = sellers[sellerID].GetTravelTime() * 2;
}

void deliverStock(int sellerID, int LorryID)
{
	float newStock = sellers[sellerID].GetStock() + dist.lorries[LorryID].carrying;
	if (newStock <= sellers[sellerID].GetMaxStock())
	{
		sellers[sellerID].AddStock(dist.lorries[LorryID].carrying);
		dist.lorries[LorryID].carrying = 0;

	}
	else
	{
		sellers[sellerID].SetStock(sellers[sellerID].GetMaxStock());
		dist.lorries[LorryID].carrying = 0;
	}
}

void sell(int sellerID)
{
	if (sellers[sellerID].GetStock() >= sellers[sellerID].GetSalesPerDay())
	{
		sellers[sellerID].SellStock(sellers[sellerID].GetSalesPerDay());
	}
	else
	{
		salesMissed = sellers[sellerID].GetSalesPerDay() - sellers[sellerID].GetStock();
	}
	
}

int main()
{
	SetUp();

    std::cout << "Hello World!\n";
}




