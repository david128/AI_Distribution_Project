#include "DistributionCentre.h"

DistributionCentre::DistributionCentre()
{
}

DistributionCentre::DistributionCentre(float max, float startingStock, Lorry l, int numOfLorries)
{

	//create lorries
	for (int i = 0; i < numOfLorries; i++)
	{
		lorries.push_back(l);
	}
	
	//stock, may use later
	maxStock = max;
	currentStock = startingStock;

}
